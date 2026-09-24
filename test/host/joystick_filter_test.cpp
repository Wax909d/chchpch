// The joystick's smoothing, on the host: how fast it follows a real throw, and
// how quiet it stays at rest. Drives JoyAxis::step() directly at the sketch's
// own 125 Hz (JOY_INTERVAL_US), exactly as joyService() does.
//
//   step response  stick thrown from rest to near full travel in one pass;
//                  how long until the smoothed reading is 90% of the way there
//   rest           60 s of a stick left alone: +/-3 counts of ADC noise and a
//                  60-count spike (a worn wiper) every 2 s; how many times the
//                  0..127 value changes - each change is a CC the stick would
//                  owe every track under it
//   held           the same noise and spikes with the stick held off centre,
//                  outside the deadzone, where nothing hides a spike
//
// Exit code 0 when both are inside the limits below. Build and run: ./run.sh
#ifndef SKETCH
#define SKETCH "../../XY6_LFO/XY6_LFO.ino"
#endif
#include SKETCH

namespace {
const double kStepLimitMs = 32.0;   // 90% of a throw within 4 passes
const int    kRestLimit   = 0;      // a resting stick sends nothing, spikes or not
const int    kHeldLimit   = 0;      // nor does a held one: noise is not a move

JoyAxis freshAxis() {
  JoyAxis a;
  a.centre = a.bootCentre = 512;
  a.lo = 512 - JOY_SPAN_INIT; a.hi = 512 + JOY_SPAN_INIT;
  return a;
}
}  // namespace

int main() {
  const uint32_t passMs = JOY_INTERVAL_US / 1000u;

  // ---- step response ----
  JoyAxis a = freshAxis();
  uint32_t ms = 0;
  for (int i = 0; i < 50; ++i, ms += passMs) a.step(512, ms);   // settled at rest
  const int target = 850, start = (int)(a.filt >> 4);
  const int goal = start + (target - start) * 9 / 10;
  int passes = 0;
  while ((int)(a.filt >> 4) < goal && passes < 100) { ms += passMs; a.step(target, ms); ++passes; }
  const double stepMs = passes * (double)passMs;

  // ---- rest and held: noise and spikes, at centre and off it ----
  auto quiet = [](int where) {
    JoyAxis r = freshAxis();
    const uint32_t passMs = JOY_INTERVAL_US / 1000u;
    uint32_t seed = 12345, ms = 0;
    int changes = 0;
    uint8_t last = 0xFF;
    for (int i = 0; i < 60 * 125; ++i, ms += passMs) {
      seed = seed * 1103515245u + 12345u;
      int raw = where + (int)((seed >> 16) % 7) - 3;   // +/-3 counts
      if (i % 250 == 125) raw += 60;                   // one spike every 2 s
      r.step(raw, ms);
      if (i > 250 && r.pend != last) ++changes;        // after it has settled
      last = r.pend;
    }
    return changes;
  };
  const int changes = quiet(512), held = quiet(700);

  printf("sketch        %s (v%s)\n", SKETCH, XY6_VERSION);
  printf("step response %.0f ms to 90%% of a throw (%d passes of %u ms)\n",
         stepMs, passes, (unsigned)passMs);
  printf("rest          %d value changes in 60 s of noise + spikes, at centre\n", changes);
  printf("held          %d value changes in 60 s of noise + spikes, held off centre\n", held);
  bool ok = true;
  if (stepMs > kStepLimitMs) { printf("FAIL  step response over %.0f ms\n", kStepLimitMs); ok = false; }
  if (changes > kRestLimit)  { printf("FAIL  resting stick not quiet\n"); ok = false; }
  if (held > kHeldLimit)     { printf("FAIL  held stick not quiet\n"); ok = false; }
  printf(ok ? "PASS\n" : "FAIL\n");
  return ok ? 0 : 1;
}
