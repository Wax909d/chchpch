// The TurboMIDI initiator against a model of the machine on the other end of
// the cable. Runs the real sketch against ./mock; the peer below plays the
// Elektron side of the handshake exactly as the captured exchange shows it:
//
//   <- 10                     -> 11 7F 01 0F 00
//   <- 12 08 07               -> 13, then its UART goes to SPEED1 (0x08, 10x)
//   <- 00 x16, 14 55x4 00x4   -> 15 55x4 00x4
//   <- 16                     -> 17, then its UART goes to SPEED2 (0x07, 8x)
//
// and, like the real thing, it drops back to 31250 if our FE keepalive stops
// for 300 ms, sends its own FE every 150 ms while the link is up, and sends
// MIDI clock. A byte that reaches either end while the two are at different
// speeds is misframed: the peer counts it, and the XY6 receives junk.
//
// With `leads` set it is the machine as a real Monomachine with TURBO on
// behaves: it sends 10 by itself every 2 s and drives the same exchange from
// the other side, switching its own UART on the F7 of our 13 and 17.
//
//   ./turbo_test <scenario>     one of the kScenarios below; exit 0 = pass
#ifndef SKETCH
#define SKETCH "../../XY6_LFO/XY6_LFO.ino"
#endif
#include SKETCH
#include <stdlib.h>

namespace {

// The spec's numbers, written out here rather than read from the sketch's
// #defines - a test that takes its limits from the code under test passes
// whatever the code says.
const uint64_t kSpecSettleUs    = 10000;   // step 11: 10 ms before any payload
const uint64_t kSpecKeepaliveUs = 150000;  // FE every 150 ms

int failures = 0;
void check(bool ok, const char* what) {
  printf("%s  %s\n", ok ? "pass" : "FAIL", what);
  if (!ok) failures++;
}

// ---- the machine --------------------------------------------------------------
// What the XY6's MIDI IN hands its UART for a byte sent at `baud`.
bool xyDeafGlobal = false; uint32_t xySmearGlobal = 0;
uint8_t throughXyMidiIn(uint8_t b, uint32_t baud) {
  if (baud != Serial1.baud()) return 0x00;                  // wrong speed: junk
  if (xyDeafGlobal && baud > 31250) return 0x00;            // nothing usable at all
  if (xySmearGlobal && baud > xySmearGlobal) {              // smeared edges
    uint8_t ones = 0xFF;                                    // ones-then-zeros only
    for (int k = 0; k <= 8; ++k, ones = (uint8_t)(ones << 1)) if (b == ones) return b;
    return 0x00;
  }
  return b;
}

struct Peer {
  // How it behaves.
  bool answersCaps = true, acks = true, echoIntact = true, answersTest2 = true;
  bool ownKeepalive = true, clock = true;
  bool leads = false;                  // it starts the handshake itself
                                       //   (first probe at nextProbe)
  uint32_t lateSwitchUs = 0;           // extra delay before its switch to SPEED1
  bool xyDeaf = false;                 // the XY6's MIDI IN fails above 1x
  // A MIDI IN whose optocoupler smears edges above this baud: a byte survives
  // only if it has a single low stretch (F0 FE 00 ...), everything else comes
  // out mangled - what the real XY6 test board showed at 10x.
  uint32_t xySmearAbove = 0;
  // Where it is.
  uint32_t baud = 31250;
  bool     turbo = false;              // at SPEED2 with the link up
  uint8_t  speed1 = 0, speed2 = 0;
  uint64_t speed1Since = 0, lastXyFe = 0, nextOwnFe = 0, nextClock = 0;
  uint64_t switchAt = 0;               // its own UART change, once its reply is out
  uint32_t switchBaud = 0;
  bool     switchToTurbo = false;
  uint64_t ackDoneUs = 0, resultDoneUs = 0;   // when its 13 / 17 finished arriving
  uint32_t misframed = 0, reverts = 0;
  // As the leader.
  enum { M_IDLE, M_WAIT_CAPS, M_WAIT_ACK, M_WAIT_ECHO, M_WAIT_RES } m = M_IDLE;
  uint64_t nextProbe = ~0ull, mDeadline = 0, xyAckDoneUs = 0, xyResultDoneUs = 0;
  std::vector<uint8_t> afterSwitch;    // sent the moment its UART changes
  uint32_t probes = 0;
  // Its receiver.
  size_t rd = 0;
  bool inSx = false;
  std::vector<uint8_t> sx;
  // Its transmitter: every byte with the time it finishes arriving here.
  struct Out { uint64_t us; uint8_t b; uint32_t baud; };
  std::deque<Out> out;
  uint64_t txFree = 0;

  void send(const uint8_t* m, size_t n, uint64_t at) {
    const uint64_t byteUs = 10000000ull / baud;
    uint64_t t = std::max(at, txFree);
    for (size_t i = 0; i < n; ++i) { t += byteUs; out.push_back({t, m[i], baud}); }
    txFree = t;
  }
  // One Elektron message, after 200 us to think. The capability answer
  // carries a clock byte in the middle, as real gear may: Real-Time is legal
  // anywhere, SysEx included.
  void reply(uint8_t cmd, const uint8_t* d, size_t n, uint64_t at) {
    uint8_t m[32] = {0xF0, 0x00, 0x20, 0x3C, 0x00, 0x00, cmd};
    size_t k = 7;
    for (size_t i = 0; i < n; ++i) {
      if (cmd == 0x11 && i == 2) m[k++] = 0xF8;
      m[k++] = d[i];
    }
    m[k++] = 0xF7;
    send(m, k, at + 200);
  }
  void revert() { baud = 31250; turbo = false; switchAt = 0; reverts++; }

  void onSysex(uint64_t t) {
    if (sx.size() < 8 || sx[1] != 0x00 || sx[2] != 0x20 || sx[3] != 0x3C) return;
    const uint8_t cmd = sx[6];
    const uint8_t* d = sx.data() + 7;
    const size_t n = sx.size() - 8;
    switch (cmd) {
      case 0x10:
        if (answersCaps) { const uint8_t c[4] = {0x7F, 0x01, 0x0F, 0x00}; reply(0x11, c, 4, t); }
        break;
      case 0x12:
        if (n < 2 || !acks) break;
        speed1 = d[0]; speed2 = d[1];
        reply(0x13, nullptr, 0, t);
        ackDoneUs = txFree;
        switchAt = txFree + lateSwitchUs; switchBaud = kTmSpeeds[speed1]; switchToTurbo = false;
        break;
      case 0x14: {
        uint8_t e[8] = {0};
        memcpy(e, d, std::min<size_t>(n, 8));
        if (!echoIntact) e[1] ^= 0x10;               // one bit wrong
        reply(0x15, e, 8, t);
        break;
      }
      // ---- as the leader: our answers ----
      case 0x11: {
        if (m != M_WAIT_CAPS || n < 2) break;
        // Like the XY6's own initiator: test at the fastest speed offered, run
        // one step below it. Our default 7F 01 gives 08 07.
        const uint16_t mask = (uint16_t)(d[0] | (d[1] << 7));
        uint8_t top = 0;
        for (uint8_t c = 1; c <= 11; ++c) if (mask & (1u << (c - 1))) top = c;
        speed1 = top; speed2 = top > 2 ? (uint8_t)(top - 1) : top;
        const uint8_t sp[2] = {speed1, speed2};
        reply(0x12, sp, 2, t);
      }
        m = M_WAIT_ACK; mDeadline = t + 500000;
        break;
      case 0x13:
        if (m != M_WAIT_ACK) break;
        xyAckDoneUs = t;
        switchAt = t + 20; switchBaud = kTmSpeeds[speed1]; switchToTurbo = false;
        afterSwitch.assign(16, 0x00);                  // its pad, at SPEED1
        { const uint8_t tm[16] = {0xF0, 0x00, 0x20, 0x3C, 0x00, 0x00, 0x14,
                                  0x55, 0x55, 0x55, 0x55, 0, 0, 0, 0, 0xF7};
          afterSwitch.insert(afterSwitch.end(), tm, tm + 16); }
        m = M_WAIT_ECHO; mDeadline = t + 300000;
        break;
      case 0x15: {
        if (m != M_WAIT_ECHO) break;
        const uint8_t want[8] = {0x55, 0x55, 0x55, 0x55, 0, 0, 0, 0};
        if (n != 8 || memcmp(d, want, 8) != 0) { revert(); m = M_IDLE; break; }
        reply(0x16, nullptr, 0, t);
        m = M_WAIT_RES; mDeadline = t + 300000;
        break;
      }
      case 0x17:
        if (m != M_WAIT_RES) break;
        xyResultDoneUs = t;
        switchAt = t + 20; switchBaud = kTmSpeeds[speed2]; switchToTurbo = true;
        m = M_IDLE;
        break;
      case 0x16:
        if (!answersTest2) break;
        reply(0x17, nullptr, 0, t);
        resultDoneUs = txFree;
        switchAt = txFree; switchBaud = kTmSpeeds[speed2]; switchToTurbo = true;
        break;
      default: break;
    }
  }
  void rx(uint8_t b, uint64_t t) {
    if (b == 0xFE) { lastXyFe = t; return; }
    if (b >= 0xF8) return;
    if (b == 0xF0) { inSx = true; sx.assign(1, b); return; }
    if (!inSx) return;
    if (b & 0x80) { if (b == 0xF7) { sx.push_back(b); onSysex(t); } inSx = false; return; }
    sx.push_back(b);
  }

  void tick(uint64_t now) {
    xyDeafGlobal = xyDeaf; xySmearGlobal = xySmearAbove;
    // Our bytes arriving, and its own UART change, in time order.
    for (;;) {
      const bool haveByte = rd < sim::wire.size() && sim::wire[rd].doneUs <= now;
      const bool haveSwitch = switchAt && switchAt <= now;
      if (!haveByte && !haveSwitch) break;
      if (haveSwitch && (!haveByte || switchAt <= sim::wire[rd].doneUs)) {
        const uint64_t at = switchAt;
        baud = switchBaud; turbo = switchToTurbo; switchAt = 0;
        if (turbo) { lastXyFe = now; nextOwnFe = now; } else speed1Since = now;
        if (!afterSwitch.empty()) { send(afterSwitch.data(), afterSwitch.size(), at);
                                    afterSwitch.clear(); }
        continue;
      }
      const sim::WireByte& w = sim::wire[rd++];
      if (w.baud != baud) { misframed++; inSx = false; continue; }
      rx(w.b, w.doneUs);
    }
    // As the leader: probe every 2 s at 1x, give up on a step after its deadline.
    if (leads && m == M_IDLE && !turbo && baud == 31250 && !switchAt && now >= nextProbe) {
      reply(0x10, nullptr, 0, now);
      probes++;
      m = M_WAIT_CAPS; mDeadline = now + 500000; nextProbe = now + 2000000;
    }
    if (leads && m != M_IDLE && now > mDeadline) { if (baud != 31250) revert(); m = M_IDLE; }
    // Its timeouts: active sensing, and a handshake that stalls at SPEED1.
    if (turbo && now - lastXyFe > 300000) revert();
    if (!turbo && baud != 31250 && !switchAt && now - speed1Since > 500000) revert();
    // Its own traffic - never while its UART is changing speed.
    const bool steady = !switchAt && (turbo || baud == 31250);
    if (turbo && ownKeepalive && now >= nextOwnFe) {
      const uint8_t fe = 0xFE; send(&fe, 1, now); nextOwnFe += 150000;
    }
    if (clock && steady && now >= nextClock) {
      const uint8_t f8 = 0xF8; send(&f8, 1, now); nextClock = now + 20833;
    }
    // Delivered to the XY6: intact at a matching speed, junk otherwise.
    while (!out.empty() && out.front().us <= now) {
      Serial1.inject(throughXyMidiIn(out.front().b, out.front().baud));
      out.pop_front();
    }
  }
};

Peer peer;
bool load = true;          // pattern + LFOs + stick running throughout
bool sysexStress = false;  // our own SysEx on the wire, some of it in pieces
bool displayStorm = false; // a full 8 KB panel push after every loop pass
bool loopback = false;     // OUT 1 patched to IN 1, no machine at all
size_t loopRd = 0;

// One loop pass: the peer, the stick, the sketch, then 20 us of other work.
void pass() {
  const uint64_t now = sim::nowUs();
  if (loopback) {
    xyDeafGlobal = peer.xyDeaf; xySmearGlobal = peer.xySmearAbove;
    while (loopRd < sim::wire.size() && sim::wire[loopRd].doneUs <= now) {
      const sim::WireByte& w = sim::wire[loopRd++];
      Serial1.inject(throughXyMidiIn(w.b, w.baud));
    }
  } else {
    peer.tick(now);
  }
  if (load) {
    const double ts = (double)now * 1e-6;
    sim::adc[JOY_PIN_X] = (uint16_t)(512 + 330 * sin(2 * M_PI * 1.5 * ts));
    sim::adc[JOY_PIN_Y] = (uint16_t)(512 + 330 * cos(2 * M_PI * 1.1 * ts));
  }
  if (sysexStress) {
    // Every 7 ms: one SysEx whole, and one in three pieces (F0 .. | .. | .. F7)
    // - 90 bytes, more than the UART's lead, so the pieces straddle passes of
    // midiTxService() and the keepalive has a chance to fall between them.
    static uint64_t next = 0;
    if (now >= next) {
      next = now + 7000;
      const uint8_t whole[8] = {0xF0, 0x7D, 0x01, 0x02, 0x03, 0x04, 0x05, 0xF7};
      qNote.push(whole, sizeof whole);
      uint8_t a[30], b[30], c[30];
      memset(a, 0x11, 30); memset(b, 0x22, 30); memset(c, 0x33, 30);
      a[0] = 0xF0; a[1] = 0x7D; c[29] = 0xF7;
      qNote.push(a, 30); qNote.push(b, 30); qNote.push(c, 30);
    }
  }
  loop();
  if (displayStorm) { g_shadowValid = false; flushAll(); }   // ~5 ms, MIDI pumped inside
  sim::nowNs += 20000;
}
void run(uint32_t ms) {
  const uint64_t end = sim::nowUs() + (uint64_t)ms * 1000u;
  while (sim::nowUs() < end) pass();
}
template <class F> bool runUntil(uint32_t ms, F done) {
  const uint64_t end = sim::nowUs() + (uint64_t)ms * 1000u;
  while (sim::nowUs() < end) { pass(); if (done()) return true; }
  return false;
}

void boot() {
  if (getenv("XY6_ECHO")) Serial.echo = true;       // show the console
  sim::adc[JOY_PIN_X] = sim::adc[JOY_PIN_Y] = 512;
  setup();
  bootActive = false;
  sysState = SYS_RUN;
  ui.page = UPAGE_PERF;
  g_joyMask = 0x3F;
  pat.engineOn = true;
  patGenerate(PAT_TECHNO, 1);
  const uint8_t start = 0xFA;
  peer.send(&start, 1, sim::nowUs());
  run(500);                                         // playing, at 1x, before turbo
}

// Our Elektron messages on the wire since `from`: command, payload, baud,
// index of the F0.
struct TmMsg { uint8_t cmd; std::vector<uint8_t> d; uint32_t baud; size_t at; };
std::vector<TmMsg> turboSentSince(size_t from) {
  std::vector<TmMsg> v;
  const auto& w = sim::wire;
  for (size_t i = from; i + 7 < w.size(); ++i) {
    if (w[i].b != 0xF0 || w[i + 1].b != 0x00 || w[i + 2].b != 0x20 || w[i + 3].b != 0x3C) continue;
    TmMsg m{w[i + 6].b, {}, w[i].baud, i};
    size_t j = i + 7;
    while (j < w.size() && w[j].b != 0xF7) m.d.push_back(w[j++].b);
    v.push_back(m);
  }
  return v;
}
size_t count(size_t from, uint8_t b, uint32_t baud) {
  size_t n = 0;
  for (size_t i = from; i < sim::wire.size(); ++i) n += (sim::wire[i].b == b && sim::wire[i].baud == baud);
  return n;
}
bool noSwitchAfter(size_t fromChange) { return sim::baudChanges.size() == fromChange; }
void common() {
  check(sim::txTruncations == 0, "no baud change ever cut a byte off mid-frame");
  check(sim::txOverruns == 0, "no write into a full UART ring (nothing blocked)");
}

// ---- scenarios -------------------------------------------------------------------

// The whole handshake, byte for byte, then three seconds of a live link.
void scenarioHappy() {
  boot();
  const size_t mark = sim::wire.size(), bc = sim::baudChanges.size();
  handleCommand("turbo");
  check(runUntil(1000, [] { return turbo.locked(); }), "handshake reaches LOCKED");
  check(turbo.baud() == 250000 && peer.baud == 250000 && peer.turbo,
        "both ends at SPEED2, 250000 baud");

  const std::vector<TmMsg> m = turboSentSince(mark);
  const uint8_t pat[8] = {0x55, 0x55, 0x55, 0x55, 0, 0, 0, 0};
  check(m.size() == 4, "exactly four turbo messages sent");
  if (m.size() == 4) {
    check(m[0].cmd == 0x10 && m[0].d.empty() && m[0].baud == 31250, "step 2: 10 at 31250");
    check(m[1].cmd == 0x12 && m[1].d == std::vector<uint8_t>({0x08, 0x07}) && m[1].baud == 31250,
          "step 4: 12 08 07 at 31250");
    check(m[2].cmd == 0x14 && m[2].d == std::vector<uint8_t>(pat, pat + 8) && m[2].baud == 312500,
          "step 7: 14 55 55 55 55 00 00 00 00 at 312500");
    check(m[3].cmd == 0x16 && m[3].d.empty() && m[3].baud == 312500, "step 9: 16 at 312500");
    // Step 6: the first thing at 10x is the pad, and the test follows it.
    size_t first10x = 0;
    while (first10x < sim::wire.size() && sim::wire[first10x].baud != 312500) first10x++;
    bool padOk = (first10x + 16 == m[2].at);
    for (size_t i = first10x; padOk && i < m[2].at; ++i) padOk = (sim::wire[i].b == 0x00);
    check(padOk, "step 6: 16 raw 00 bytes at 312500, straight before the test");
    // Steps 4-11 hold the wire: from the 12 to the switch to 8x, only the
    // handshake itself - 12 (10 bytes), pad (16), 14 (16), 16 (8).
    size_t first8x = first10x;
    while (first8x < sim::wire.size() && sim::wire[first8x].baud != 250000) first8x++;
    check(first8x - m[1].at == 10 + 16 + 16 + 8, "nothing else on the wire during the handshake");
  }
  // The switches: SPEED1 on the ACK's F7, SPEED2 on the result's F7.
  uint64_t t10 = 0, t8 = 0;
  for (size_t i = bc; i < sim::baudChanges.size(); ++i) {
    if (sim::baudChanges[i].baud == 312500 && !t10) t10 = sim::baudChanges[i].us;
    if (sim::baudChanges[i].baud == 250000 && !t8)  t8 = sim::baudChanges[i].us;
  }
  check(t10 && t10 >= peer.ackDoneUs && t10 - peer.ackDoneUs < 1000,
        "step 6: UART at 312500 within 1 ms of the ACK's F7");
  check(t8 && t8 >= peer.resultDoneUs && t8 - peer.resultDoneUs < 1000,
        "step 11: UART at 250000 within 1 ms of the result's F7");
  bool quiet = true;
  for (const auto& w : sim::wire) if (w.baud == 250000 && w.writeUs < t8 + kSpecSettleUs) quiet = false;
  check(quiet, "step 11: nothing transmitted for 10 ms after the switch to 8x");

  // Three seconds of a live link under load, with SysEx in pieces.
  sysexStress = true;
  const size_t live = sim::wire.size();
  run(3000);
  sysexStress = false;
  check(turbo.locked() && peer.turbo && peer.reverts == 0, "link stays up for 3 s (keepalive heard)");
  check(peer.misframed == 0, "the machine never received a byte at the wrong speed");
  std::vector<uint64_t> fe;
  for (const auto& w : sim::wire) if (w.b == 0xFE && w.baud == 250000) fe.push_back(w.doneUs);
  uint64_t gap = 0;
  for (size_t i = 1; i < fe.size(); ++i) gap = std::max(gap, fe[i] - fe[i - 1]);
  printf("      keepalive: %zu FE, longest gap %.1f ms, first %.1f ms after the settle\n",
         fe.size(), (double)gap / 1000.0,
         fe.empty() ? -1.0 : (double)(fe[0] - (t8 + kSpecSettleUs)) / 1000.0);
  check(!fe.empty() && fe[0] >= t8 + kSpecSettleUs && fe[0] - (t8 + kSpecSettleUs) < 2000,
        "first FE right after the settle");
  check(fe.size() > 1 && gap <= kSpecKeepaliveUs + 5000, "FE every 150 ms (5 ms slack)");
  // No FE ever inside a SysEx: open from F0 until F7 or any other status byte.
  bool open = false; size_t inside = 0, pieces = 0;
  for (size_t i = live; i < sim::wire.size(); ++i) {
    const uint8_t b = sim::wire[i].b;
    if (b == 0xFE) { inside += open; continue; }
    if (b == 0xF0) { open = true; pieces++; }
    else if (b == 0xF7 || (b >= 0x80 && b < 0xF8)) open = false;
  }
  printf("      %zu SysEx messages sent while the keepalive ran\n", pieces);
  check(pieces > 100 && inside == 0, "no FE ever lands inside an open SysEx");
  size_t notes = 0;
  for (size_t i = live; i < sim::wire.size(); ++i)
    notes += ((sim::wire[i].b & 0xF0) == 0x90 && sim::wire[i].baud == 250000);
  check(notes > 20, "the pattern plays over the turbo link");
  common();
}

// Step 3 never answered: nothing may change.
void scenarioNoCaps() {
  peer.answersCaps = false;
  boot();
  const size_t bc = sim::baudChanges.size(), mark = sim::wire.size();
  handleCommand("turbo");
  run(TURBO_STEP_TIMEOUT_MS + 300);
  check(!turbo.negotiating() && turbo.baud() == 31250, "gives up, back to OFF at 1X");
  check(noSwitchAfter(bc), "the UART was never touched");
  check(count(mark, 0xFE, 31250) == 0 && !g_tmKeepalive, "no keepalive");
  size_t after = 0;
  for (size_t i = mark; i < sim::wire.size(); ++i)
    after += ((sim::wire[i].b & 0xF0) == 0x90 && sim::wire[i].writeUs > sim::nowUs() - 200000);
  check(after > 0, "ordinary MIDI carries on at 1X");
  common();
}

// Step 5 never answered: back to 1X with nothing switched, wire released.
void scenarioNoAck() {
  peer.acks = false;
  boot();
  const size_t bc = sim::baudChanges.size(), mark = sim::wire.size();
  handleCommand("turbo");
  check(runUntil(500, [] { return turbo.holdsWire(); }), "holds the wire once 12 is sent");
  run(TURBO_STEP_TIMEOUT_MS + 300);
  check(!turbo.negotiating() && turbo.baud() == 31250 && noSwitchAfter(bc),
        "no ACK: back to OFF at 1X, UART never touched");
  const std::vector<TmMsg> m = turboSentSince(mark);
  check(m.size() == 2 && m[1].cmd == 0x12, "sent 10 and 12, nothing more");
  size_t after = 0;
  for (size_t i = mark; i < sim::wire.size(); ++i)
    after += ((sim::wire[i].b & 0xF0) == 0x90 && sim::wire[i].writeUs > sim::nowUs() - 200000);
  check(after > 0, "ordinary MIDI carries on at 1X");
  common();
}

// Step 8 answered wrong: REVERT - 31250, no keepalive, All Notes Off.
void scenarioBadEcho() {
  peer.echoIntact = false;
  boot();
  const size_t bc = sim::baudChanges.size(), mark = sim::wire.size();
  handleCommand("turbo");
  check(runUntil(1000, [] { return turbo.stateName()[0] == 'R'; }), "a bad echo starts REVERT");
  run(TURBO_REVERT_HOLD_MS + 300);
  check(!turbo.negotiating() && turbo.baud() == 31250, "back to OFF at 1X");
  bool saw10 = false, saw8 = false;
  for (size_t i = bc; i < sim::baudChanges.size(); ++i) {
    saw10 |= sim::baudChanges[i].baud == 312500; saw8 |= sim::baudChanges[i].baud == 250000;
  }
  check(saw10 && !saw8 && sim::baudChanges.back().baud == 31250, "went to 10x, never 8x, back to 1X");
  check(count(mark, 0xFE, 312500) + count(mark, 0xFE, 250000) + count(mark, 0xFE, 31250) == 0,
        "the keepalive never started");
  check(turboSentSince(mark).size() == 3, "stopped after the test: no 16 sent");
  size_t ano = 0;
  for (size_t i = mark; i + 1 < sim::wire.size(); ++i)
    ano += ((sim::wire[i].b & 0xF0) == 0xB0 && sim::wire[i + 1].b == 123 && sim::wire[i].baud == 31250);
  check(ano == 6, "All Notes Off on all six channels, at 1X");
  common();
}

// A live link that dies: the machine is power-cycled, back at 1x, silent at
// 8x. REVERT within TURBO_PEER_SILENT_MS, keepalive stopped.
void scenarioDrop() {
  boot();
  handleCommand("turbo");
  check(runUntil(1000, [] { return turbo.locked(); }), "handshake reaches LOCKED");
  run(1000);
  check(turbo.peerKeepalive(), "the machine's own keepalive was seen");
  peer.revert();                                     // power-cycled: 1x, no FE
  const uint64_t dropUs = sim::nowUs();
  check(runUntil(1000, [] { return turbo.baud() == 31250; }), "the XY6 falls back to 31250");
  const uint64_t tookMs = (sim::nowUs() - dropUs) / 1000u;
  printf("      detected in %lu ms\n", (unsigned long)tookMs);
  check(tookMs <= TURBO_PEER_SILENT_MS + 50, "within TURBO_PEER_SILENT_MS of the drop");
  const size_t mark = sim::wire.size();
  const uint32_t mis = peer.misframed;
  run(TURBO_REVERT_HOLD_MS + 1000);
  check(count(mark, 0xFE, 31250) == 0 && !g_tmKeepalive, "keepalive stopped");
  check(!turbo.negotiating() && !turbo.locked(), "OFF");
  check(peer.misframed == mis, "everything after the revert reaches the machine intact");
  common();
}

// 'turbo off' from a live link: the keepalive stops, the machine times out
// and returns to 1x, and nothing is sent to it until it has.
void scenarioOff() {
  boot();
  handleCommand("turbo");
  check(runUntil(1000, [] { return turbo.locked(); }), "handshake reaches LOCKED");
  run(500);
  // Off with the UART mid-burst, so the revert has bytes to wait for.
  sysexStress = true;
  check(runUntil(100, [] { return Serial1.availableForWrite() < g_txCap - 20; }),
        "UART busy when turbo off is typed");
  sysexStress = false;
  handleCommand("turbo off");
  check(runUntil(100, [] { return turbo.baud() == 31250; }), "UART back to 31250");
  const size_t mark = sim::wire.size();
  const uint32_t mis = peer.misframed;
  run(TURBO_REVERT_HOLD_MS + 1000);
  check(peer.baud == 31250 && !peer.turbo && peer.reverts == 1,
        "the machine times out on the missing keepalive and returns to 1x");
  check(count(mark, 0xFE, 31250) == 0, "no FE after turbo off");
  check(peer.misframed == mis, "nothing reached the machine at the wrong speed");
  check(!turbo.negotiating() && !turbo.locked(), "OFF");
  common();
}

// A machine that sends nothing at all once the link is up. Silence is not a
// fault: the link must stay up, carried by our keepalive.
void scenarioSilentPeer() {
  peer.ownKeepalive = false;
  boot();
  peer.clock = false;
  handleCommand("turbo");
  check(runUntil(1000, [] { return turbo.locked(); }), "handshake reaches LOCKED");
  run(3000);
  check(turbo.locked() && peer.turbo && peer.reverts == 0,
        "a silent machine does not tear the link down");
  common();
}

// v1.20: the machine leads, as the real one does with TURBO enabled. The XY6
// answers, follows it to 10x within the machine's 0.5 ms pad, and locks at 8x.
void scenarioMachineLeads() {
  peer.leads = true;
  boot();                                            // idle at 1x; answering is on
  const size_t mark = sim::wire.size(), bc = sim::baudChanges.size();
  peer.nextProbe = sim::nowUs() + 100000;
  check(runUntil(2000, [] { return turbo.locked(); }), "the machine's handshake reaches LOCKED");
  check(peer.turbo && peer.baud == 250000 && turbo.baud() == 250000, "both ends at 8x");
  check(peer.probes == 1, "on the machine's first probe");
  const std::vector<TmMsg> m = turboSentSince(mark);
  const uint8_t pat[8] = {0x55, 0x55, 0x55, 0x55, 0, 0, 0, 0};
  check(m.size() == 4, "exactly four answers sent");
  if (m.size() == 4) {
    check(m[0].cmd == 0x11 && m[0].d == std::vector<uint8_t>({0x7F, 0x01, 0x0F, 0x00}) &&
          m[0].baud == 31250, "-> 11 7F 01 0F 00 at 31250");
    check(m[1].cmd == 0x13 && m[1].d.empty() && m[1].baud == 31250, "-> 13 at 31250");
    check(m[2].cmd == 0x15 && m[2].d == std::vector<uint8_t>(pat, pat + 8) && m[2].baud == 312500,
          "-> 15 echo at 312500");
    check(m[3].cmd == 0x17 && m[3].d.empty() && m[3].baud == 312500, "-> 17 at 312500");
  }
  uint64_t t10 = 0, t8 = 0;
  for (size_t i = bc; i < sim::baudChanges.size(); ++i) {
    if (sim::baudChanges[i].baud == 312500 && !t10) t10 = sim::baudChanges[i].us;
    if (sim::baudChanges[i].baud == 250000 && !t8)  t8 = sim::baudChanges[i].us;
  }
  printf("      to 10x %.0f us after our 13 left, to 8x %.0f us after our 17\n",
         (double)(t10 - peer.xyAckDoneUs), (double)(t8 - peer.xyResultDoneUs));
  check(t10 >= peer.xyAckDoneUs && t10 - peer.xyAckDoneUs < 500,
        "at 10x before the machine's 0.5 ms pad runs out");
  bool quiet = true;
  for (const auto& w : sim::wire) if (w.baud == 250000 && w.writeUs < t8 + kSpecSettleUs) quiet = false;
  check(quiet, "10 ms of quiet after the switch to 8x");
  run(2000);
  check(turbo.locked() && peer.turbo && peer.reverts == 0, "the link stays up (keepalive heard)");
  check(peer.misframed == 0, "the machine never received a byte at the wrong speed");
  common();
}

// The same, with the XY6 inside a full-panel push nearly all the time: the
// switch to 10x has to come from the pump inside the push, not from loop().
void scenarioMachineBusy() {
  peer.leads = true;
  boot();
  displayStorm = true;
  peer.nextProbe = sim::nowUs() + 100000;
  check(runUntil(2000, [] { return turbo.locked(); }),
        "the machine's handshake reaches LOCKED with the display pushing frames");
  check(peer.misframed == 0 && peer.reverts == 0, "no byte at the wrong speed, no retry needed");
  displayStorm = false;
  common();
}

// After 'turbo off' the machine's probes go unanswered - and say so once.
void scenarioRefused() {
  peer.leads = true;
  boot();
  handleCommand("turbo off");
  const size_t mark = sim::wire.size(), bc = sim::baudChanges.size();
  const size_t logFrom = Serial.captured.size();
  peer.nextProbe = sim::nowUs() + 100000;
  run(4500);                                         // three probes
  check(peer.probes >= 3, "the machine kept asking");
  check(turboSentSince(mark).empty() && noSwitchAfter(bc), "no answer, no switch");
  const std::string log = Serial.captured.substr(logFrom);
  size_t said = 0;
  for (size_t at = 0; (at = log.find("not answering", at)) != std::string::npos; ++at) said++;
  check(said == 1, "the refusal is logged once, not every probe");
  handleCommand("turbo");                            // allowed again: our own attempt
  check(runUntil(2500, [] { return turbo.locked(); }), "'turbo' brings it up again");
  common();
}

// v1.20: a machine slower to change speed than the pad allows misses our
// first test. The resend has to rescue it.
void scenarioLatePeer() {
  peer.lateSwitchUs = 3000;
  boot();
  const size_t mark = sim::wire.size();
  handleCommand("turbo");
  check(runUntil(1000, [] { return turbo.locked(); }), "reaches LOCKED despite the late switch");
  size_t tests = 0;
  for (const TmMsg& m : turboSentSince(mark)) tests += (m.cmd == 0x14);
  printf("      %zu speed tests sent\n", tests);
  check(tests >= 2, "the first test was missed and sent again");
  common();
}

// v1.20: an XY6 whose MIDI IN cannot receive TurboMIDI speeds (a slow
// optocoupler). The failure must say what arrived, and fall back cleanly.
void scenarioDeaf() {
  peer.xyDeaf = true;
  boot();
  const size_t logFrom = Serial.captured.size();
  handleCommand("turbo");
  run(TURBO_STEP_TIMEOUT_MS + TURBO_REVERT_HOLD_MS + 500);
  check(!turbo.negotiating() && turbo.baud() == 31250, "falls back to 1X");
  const std::string log = Serial.captured.substr(logFrom);
  check(log.find("at 10X since the switch") != std::string::npos &&
        log.find("no SysEx starts") != std::string::npos,
        "the report says bytes arrived at 10X with no SysEx in them");
  common();
}

// v1.21: the real test board. The machine leads and gets to 10x; our MIDI IN
// passes F0 FE 00 but mangles every edge-dense byte. The report must say the
// machine IS talking and nothing arrives whole - not blame its timing.
void scenarioGarbled() {
  peer.leads = true;
  peer.xySmearAbove = 31250;
  boot();
  const size_t logFrom = Serial.captured.size();
  peer.nextProbe = sim::nowUs() + 100000;
  run(1500);
  check(!turbo.locked() && !turbo.negotiating() && turbo.baud() == 31250, "no link, back at 1X");
  const std::string log = Serial.captured.substr(logFrom);
  check(log.find("at 10X since the switch") != std::string::npos &&
        log.find("0 arrived whole") != std::string::npos &&
        log.find("IS talking at this speed") != std::string::npos,
        "the report says the machine is talking at 10X and nothing arrives whole");
  check(log.find("did not answer") == std::string::npos, "and does not blame the machine");
  common();
}

// v1.21: the same MIDI IN, clean up to 4x. 'turbo max 4' has to bring the link
// up - whichever end leads.
void scenarioCeiling() {
  peer.xySmearAbove = 125000;
  boot();
  handleCommand("turbo max 4");
  check(turbo.speed1Code() == 4 && turbo.speed2Code() == 3, "ceiling 4X: test 4X, run 3.3X");
  const size_t mark = sim::wire.size();
  handleCommand("turbo");
  check(runUntil(1000, [] { return turbo.locked(); }), "XY6 leading: locks under the ceiling");
  const std::vector<TmMsg> m = turboSentSince(mark);
  check(m.size() >= 2 && m[1].cmd == 0x12 && m[1].d == std::vector<uint8_t>({0x04, 0x03}),
        "asks 12 04 03");
  check(turbo.baud() == 104062 && peer.baud == 104062, "both at 3.3X");
  handleCommand("turbo off");
  run(TURBO_REVERT_HOLD_MS + 600);
  // Now the machine leads, and has to pick from what we offer.
  handleCommand("turbo max 4");                      // (the ceiling survives 'turbo off')
  turbo.start(millis()); turbo.stop(millis());       // answering on again, via 'turbo'...
  handleCommand("turbo");                            // ...and our own attempt racing it
  peer.leads = true; peer.nextProbe = sim::nowUs();
  check(runUntil(2000, [] { return turbo.locked(); }), "machine leading: locks under the ceiling");
  check(turbo.baud() <= 125000 && peer.baud == turbo.baud(), "at or below 4X, both ends");
  common();
}

// v1.21: 'turbo loop' with OUT 1 patched to IN 1 and a MIDI IN clean to 8x.
void scenarioLoop() {
  loopback = true;
  peer.clock = false;
  peer.xySmearAbove = 250000;
  boot();
  loopRd = sim::wire.size();
  const size_t logFrom = Serial.captured.size();
  handleCommand("turbo loop");
  check(runUntil(2000, [] { return !turbo.looping(); }), "the self-test finishes");
  run(100);
  const std::string log = Serial.captured.substr(logFrom);
  if (getenv("XY6_ECHO")) printf("%s\n", log.c_str());
  check(log.find("8X   250000 baud: 32 of 32 back intact") != std::string::npos, "8X passes");
  check(log.find("10X  312500 baud: 32 of 32") == std::string::npos &&
        log.find("10X  312500 baud:") != std::string::npos, "10X fails");
  check(log.find("passes up to 8X cleanly") != std::string::npos &&
        log.find("'turbo max 8'") != std::string::npos, "the verdict: up to 8X, 'turbo max 8'");
  check(!turbo.negotiating() && turbo.baud() == 31250, "back at 1X afterwards");
  common();
}

struct Scenario { const char* name; void (*fn)(); };
const Scenario kScenarios[] = {
  {"happy", scenarioHappy},   {"nocaps", scenarioNoCaps}, {"noack", scenarioNoAck},
  {"badecho", scenarioBadEcho}, {"drop", scenarioDrop},   {"off", scenarioOff},
  {"silent", scenarioSilentPeer},
  {"machine", scenarioMachineLeads}, {"busy", scenarioMachineBusy},
  {"refused", scenarioRefused},
  {"latepeer", scenarioLatePeer},    {"deaf", scenarioDeaf},
  {"garbled", scenarioGarbled},      {"ceiling", scenarioCeiling},
  {"loop", scenarioLoop},
};

}  // namespace

int main(int argc, char** argv) {
  for (const Scenario& s : kScenarios) {
    if (argc < 2 || strcmp(argv[1], s.name) != 0) continue;
    s.fn();
    printf(failures ? "FAIL (%d)\n" : "PASS\n", failures);
    return failures ? 1 : 0;
  }
  printf("usage: turbo_test <");
  const size_t n = sizeof kScenarios / sizeof kScenarios[0];
  for (size_t i = 0; i < n; ++i) printf("%s%s", kScenarios[i].name, i + 1 == n ? ">\n" : "|");
  return 2;
}
