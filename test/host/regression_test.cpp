// One check per bug fixed in v1.18, plus the turbo speed change under load
// (the TX path it depends on changed). Runs the real sketch against ./mock.
// Exit code 0 when every check passes. Build and run: ./run.sh
#ifndef SKETCH
#define SKETCH "../../XY6_LFO/XY6_LFO.ino"
#endif
#include SKETCH

namespace {

int failures = 0;
void check(bool ok, const char* what) {
  printf("%s  %s\n", ok ? "pass" : "FAIL", what);
  if (!ok) failures++;
}

// Run the main loop for `ms` of simulated time, 20 us of other work a pass,
// with MIDI clock at 120 BPM once `clock` is set.
bool clockOn = false;
uint64_t nextClk = 0;
void run(uint32_t ms) {
  const uint64_t end = sim::nowUs() + (uint64_t)ms * 1000u;
  while (sim::nowUs() < end) {
    if (clockOn && sim::nowUs() >= nextClk) { Serial1.inject(0xF8); nextClk += 20833; }
    loop();
    sim::nowNs += 20000;
  }
}

void boot() {
  sim::adc[JOY_PIN_X] = sim::adc[JOY_PIN_Y] = 512;
  setup();
  bootActive = false;
  sysState = SYS_RUN;
  ui.page = UPAGE_PERF;
  ui.inSub = false;
}

// Wire bytes after index `from`, as (status, d1, d2) triples, running status
// expanded. Real-time bytes are skipped.
struct Msg { uint8_t st, d1, d2; };
std::vector<Msg> messagesSince(size_t from) {
  std::vector<Msg> out;
  uint8_t st = 0, d[2]; int n = 0;
  for (size_t i = from; i < sim::wire.size(); ++i) {
    const uint8_t b = sim::wire[i].b;
    if (b >= 0xF8) continue;
    if (b & 0x80) { st = b; n = 0; continue; }
    if (st < 0x80 || st >= 0xF0) continue;
    d[n++] = b;
    if (n == 2) { out.push_back({st, d[0], d[1]}); n = 0; }
  }
  return out;
}

}  // namespace

int main() {
  boot();

  // ---- 1. BASE CH moved while notes sound: every note-off on the OLD channel.
  {
    pat.engineOn = true;
    patGenerate(PAT_TECHNO, 1);
    clockOn = true; nextClk = sim::nowUs();
    Serial1.inject(0xFA);                              // START
    // Run until a note is sounding, then move the channel under it.
    int sounding = -1;
    for (int k = 0; k < 400 && sounding < 0; ++k) {
      run(5);
      for (uint8_t t = 0; t < PAT_TRACKS; ++t) if (patActive[t].active) { sounding = t; break; }
    }
    check(sounding >= 0, "pattern is playing notes");
    const uint8_t oldCh = patChan((uint8_t)sounding), note = patActive[sounding].note;
    const size_t mark = sim::wire.size();
    setBaseChannel(5);
    run(50);
    bool offOld = false, offNew = false;
    for (const Msg& m : messagesSince(mark)) {
      if ((m.st & 0xF0) != 0x80 || m.d1 != note) continue;
      if ((m.st & 0x0F) == oldCh) offOld = true;
      if ((m.st & 0x0F) == ((4 + sounding) & 0x0F)) offNew = true;
    }
    check(offOld, "channel change: the sounding note is released on its OWN channel");
    check(!offNew, "channel change: no stray note-off on the new channel");
    check(txChannel == 5, "channel change: base channel is now 5");
    setBaseChannel(1);
    Serial1.inject(0xFC);                              // STOP
    pat.engineOn = false;
    clockOn = false;
    run(50);
  }

  // ---- 2. Turbo fallback's All Notes Off follows the live base channel.
  {
    setBaseChannel(9);
    const size_t mark = sim::wire.size();
    turbo.forceSpeed(7, millis());                     // 8x, no handshake
    run(300);
    check(turbo.locked() && turbo.baud() == 250000, "turbo: forced 8x locks under the new TX path");
    turbo.forceSpeed(1, millis());                     // back to 1x: All Notes Off
    run(300);
    check(!turbo.negotiating() && turbo.baud() == 31250, "turbo: back to 31250");
    bool live = false, stale = false;
    for (const Msg& m : messagesSince(mark)) {
      if ((m.st & 0xF0) != 0xB0 || m.d1 != 123) continue;
      if ((m.st & 0x0F) == 8) live = true;             // channel 9, track 1
      if ((m.st & 0x0F) == 0) stale = true;            // channel 1: compile-time base
    }
    check(live, "turbo revert: All Notes Off on the live base channel");
    check(!stale, "turbo revert: none on the compile-time channel");
    setBaseChannel(1);
    run(50);
  }

  // ---- 3. A track taken off the stick and put back is not sent the rest value.
  {
    g_joyMask = 0x01;
    sim::adc[JOY_PIN_X] = 150;                         // push the stick (inverted axis)
    run(300);
    const uint8_t pushed = g_joySentX[0];
    check(pushed != 0xFF && pushed > 100, "stick: track 1 follows a push");
    g_joyMask = 0x00;                                  // take track 1 off
    run(50);
    sim::adc[JOY_PIN_X] = 512;                         // let the stick go
    run(1500);
    const size_t mark = sim::wire.size();
    g_joyMask = 0x01;                                  // put track 1 back, stick at rest
    run(300);
    bool sentRest = false;
    const uint8_t ccX = joyDestCC(g_joyDestX);
    for (const Msg& m : messagesSince(mark))
      if (m.st == 0xB0 && m.d1 == ccX) sentRest = true;
    check(!sentRest, "stick: re-adding a track sends nothing until the stick moves");
  }

  // ---- 4. The stick's echo through a THRU does not throttle the LFOs.
  {
    g_joyMask = 0x01;
    sim::adc[JOY_PIN_X] = 200;
    run(200);
    const uint8_t v = g_joySentX[0];
    mnmOut.begin();                                    // clear any back-off
    const uint8_t ccX = joyDestCC(g_joyDestX);
    Serial1.inject(0xB0); Serial1.inject(ccX); Serial1.inject((uint8_t)(v ^ 1));   // echo, a step behind
    run(5);
    // With the back-off on, the per-LFO interval is 66 ms, so in 40 ms at most
    // one message per LFO could go out. Count what LFO 1's slot sends.
    const size_t mark = sim::wire.size();
    lfo.p[0].enabled = true; lfo.p[0].trig = TRIG_FREE; lfo.p[0].spd = 127; lfo.p[0].mult = 3;
    lfo.p[0].depth = 60;
    run(40);
    int lfoMsgs = 0;
    const uint8_t cc0 = mnmCC(lfo.p[0].page, lfo.p[0].dest);
    for (const Msg& m : messagesSince(mark)) if ((m.st & 0xF0) == 0xB0 && m.d1 == cc0) lfoMsgs++;
    check(lfoMsgs >= 2, "echo: a THRU echo of the stick does not trigger the LFO back-off");
    sim::adc[JOY_PIN_X] = 512;
    lfo.p[0].trig = TRIG_TRIG;
    run(1500);
  }

  // ---- 5. SAVE during a save keeps the save's own message.
  {
    check(Store::saveTo(0), "store: a save starts");
    run(2);
    check(!Store::saveTo(0), "store: a second save is refused while busy");
    check(strcmp(storeMsg, "NO EEPROM") != 0, "store: ...without claiming there is no EEPROM");
    for (int k = 0; k < 200 && Store::busy(); ++k) run(10);
    check(!Store::busy() && strcmp(storeMsg, "SAVED") == 0, "store: the first save completes");
  }

  printf(failures ? "FAIL (%d)\n" : "PASS\n", failures);
  return failures ? 1 : 0;
}
