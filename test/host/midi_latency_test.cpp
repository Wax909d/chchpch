// MIDI OUT latency under a realistic stage load, on the host.
//
// Runs the real sketch (setup() + loop()) against the mocks in ./mock for ten
// simulated seconds at standard MIDI speed (31250 baud), with everything that
// transmits going at once:
//   * the Monomachine's clock at 120 BPM, and START, so all six LFOs drive
//   * the pattern engine playing on all six tracks
//   * the joystick swept continuously, routed to all six tracks
// Two loads: "typical" keeps the six boot LFOs (slow sweeps, ~75% of the
// wire in total), "heavy" runs all six LFOs fast, so together with the stick
// the box asks for MORE than the wire can carry - the case where anything
// that queues in the wrong place shows up as latency.
// It reports how late things reached the wire:
//   note latency   a note-on queued by the pattern engine -> its last byte out
//   stick latency  the stick reaching a value -> that value's CC byte out
//   UART backlog   bytes sitting in Serial1's TX ring (what nothing can reorder)
//
// Exit code 0 when the latency limits below hold, 1 otherwise, so CI can gate
// on it. Build and run with ./run.sh.
#include <vector>
#include <algorithm>

#ifndef SKETCH
#define SKETCH "../../XY6_LFO/XY6_LFO.ino"
#endif
#include SKETCH

namespace {

// What "responsive" means here, at 31250 baud. A note must reach the wire
// within 10 ms of being queued (the ear notices ~10 ms against a hi-hat) and
// a stick value within 25 ms of the stick getting there. The stick shares the
// wire with six LFOs and the pattern and its own budget is capped, so its
// bound is about never building a backlog. v1.18 measures ~8.6 / ~12.5 ms.
const double kNoteP99LimitMs  = 10.0;
const double kStickP99LimitMs = 25.0;

double pct(std::vector<double> v, double p) {
  if (v.empty()) return 0;
  std::sort(v.begin(), v.end());
  size_t i = (size_t)(p / 100.0 * (double)(v.size() - 1) + 0.5);
  return v[std::min(i, v.size() - 1)];
}

struct Change { uint64_t us; uint8_t v; };

}  // namespace

int main(int argc, char** argv) {
  const bool heavy = (argc > 1 && !strcmp(argv[1], "heavy"));
  // The stick rests at centre through boot, as the calibration expects.
  sim::adc[JOY_PIN_X] = sim::adc[JOY_PIN_Y] = 512;
  setup();
  // Skip the splash and the EEPROM route: straight onto the PERF page.
  bootActive = false;
  sysState = SYS_RUN;
  ui.page = UPAGE_PERF;
  ui.inSub = false;
  g_joyMask = 0x3F;                      // the stick drives all six tracks
  pat.engineOn = true;
  for (uint8_t t = 0; t < PAT_TRACKS; ++t) pat.trackOn[t] = true;
  patGenerate(PAT_TECHNO, 1);
  if (heavy)
    for (uint8_t i = 0; i < LFO_COUNT; ++i) {   // fast, and never clipped (a clipped
      lfo.p[i].spd = 127; lfo.p[i].mult = 3;     // LFO sits still and sends nothing)
      lfo.p[i].depth = 60;
    }

  const uint64_t t0 = sim::nowUs();
  const uint64_t runUs = 10000000;       // ten seconds
  const uint64_t clockUs = 20833;        // 24 ppqn at 120 BPM
  uint64_t nextClk = t0 + 1000;
  bool started = false;
  uint32_t notesQueued = patNotesSent;
  std::vector<uint64_t> noteQueuedUs;    // one entry per note-on, FIFO order
  std::vector<Change> stickX;            // every change of the stick's X value
  uint8_t lastX = 0xFF;
  size_t peakRing = 0;

  while (sim::nowUs() - t0 < runUs) {
    const uint64_t now = sim::nowUs();
    if (now >= nextClk) {
      if (!started) { Serial1.inject(0xFA); started = true; }
      Serial1.inject(0xF8);
      nextClk += clockUs;
    }
    // Stick: a hand working both axes, ~1.5 Hz, most of the travel.
    const double ts = (double)(now - t0) * 1e-6;
    sim::adc[JOY_PIN_X] = (uint16_t)(512 + 330 * sin(2 * M_PI * 1.5 * ts));
    sim::adc[JOY_PIN_Y] = (uint16_t)(512 + 330 * cos(2 * M_PI * 1.1 * ts));

    loop();
    sim::nowNs += 20000;                 // the rest of a typical loop pass

    // Joystick values are inverted in the sketch (JOY_INVERT_X), so track the
    // value the sketch computed, not the ADC.
    if (joyX.pend != lastX) { lastX = joyX.pend; stickX.push_back({sim::nowUs(), lastX}); }
    while (notesQueued != patNotesSent) { noteQueuedUs.push_back(sim::nowUs()); notesQueued++; }
    const int inRing = Serial1.txSize() - 1 - Serial1.availableForWrite();
    peakRing = std::max(peakRing, (size_t)std::max(inRing, 0));
  }

  // ---- note latency: the i-th note-on queued is the i-th 0x9n on the wire.
  // Running status is never used for notes, so every note-on has its status.
  std::vector<double> noteMs;
  size_t qi = 0;
  for (size_t i = 0; i + 2 < sim::wire.size(); ++i) {
    if ((sim::wire[i].b & 0xF0) != 0x90) continue;
    if (qi < noteQueuedUs.size())
      noteMs.push_back((double)(sim::wire[i + 2].doneUs - noteQueuedUs[qi++]) / 1000.0);
  }

  // ---- stick latency: parse CC 72 (FILT BASE, the stick's X) on channels
  // 1-6, running status included, and date each value from the moment the
  // stick first reached it.
  const uint8_t ccX = joyDestCC(g_joyDestX);
  std::vector<double> stickMs;
  uint8_t status = 0;
  for (size_t i = 0; i < sim::wire.size(); ++i) {
    const uint8_t b = sim::wire[i].b;
    if (b >= 0xF8) continue;
    if (b & 0x80) { status = b; continue; }
    if ((status & 0xF0) != 0xB0 || i + 1 >= sim::wire.size()) continue;
    if (b == ccX && !(sim::wire[i + 1].b & 0x80)) {
      const uint8_t v = sim::wire[i + 1].b;
      const uint64_t done = sim::wire[i + 1].doneUs;
      for (size_t k = stickX.size(); k-- > 0;) {
        if (stickX[k].us > done) continue;
        if (stickX[k].v == v) { stickMs.push_back((double)(done - stickX[k].us) / 1000.0); break; }
      }
    }
    ++i;                                   // skip the data byte we just read
  }

  size_t noteOns = 0;
  for (const auto& w : sim::wire) noteOns += ((w.b & 0xF0) == 0x90);
  const double wireLoad = (double)sim::wire.size() / 3125.0 / ((double)runUs * 1e-6) * 100.0;

  printf("sketch        %s (v%s), %s load\n", SKETCH, XY6_VERSION, heavy ? "heavy" : "typical");
  printf("wire          %zu bytes in %.0f s = %.0f%% of 31250 baud\n",
         sim::wire.size(), (double)runUs * 1e-6, wireLoad);
  printf("notes         %zu note-ons on the wire, %lu dropped by the queue\n",
         noteOns, (unsigned long)qNote.drops());
  printf("note latency  median %6.1f ms   p99 %6.1f ms   max %6.1f ms\n",
         pct(noteMs, 50), pct(noteMs, 99), pct(noteMs, 100));
  printf("stick latency median %6.1f ms   p99 %6.1f ms   max %6.1f ms   (%zu CCs)\n",
         pct(stickMs, 50), pct(stickMs, 99), pct(stickMs, 100), stickMs.size());
  printf("UART backlog  peak %zu bytes = %.1f ms of wire time\n",
         peakRing, (double)peakRing * 0.32);
  printf("ring overruns %lu (writes that would have blocked the loop)\n",
         (unsigned long)sim::txOverruns);

  bool ok = true;
  if (pct(noteMs, 99) > kNoteP99LimitMs)   { printf("FAIL  note p99 over %.0f ms\n", kNoteP99LimitMs); ok = false; }
  if (pct(stickMs, 99) > kStickP99LimitMs) { printf("FAIL  stick p99 over %.0f ms\n", kStickP99LimitMs); ok = false; }
  if (noteMs.size() < 100)                 { printf("FAIL  pattern barely played\n"); ok = false; }
  if (stickMs.size() < 100)                { printf("FAIL  stick barely sent\n"); ok = false; }
  if (sim::txOverruns)                     { printf("FAIL  UART ring overrun\n"); ok = false; }
  printf(ok ? "PASS\n" : "FAIL\n");
  return ok ? 0 : 1;
}
