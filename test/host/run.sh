#!/usr/bin/env bash
# Builds XY6_LFO.ino on a PC against the mocks in ./mock and runs the host
# tests. Needs only g++ with C++17 - no Teensy toolchain, no hardware.
#   ./run.sh            build with -Wall -Wextra -Werror, run everything
# Exit code is non-zero if any test fails, so CI can gate on it.
set -euo pipefail
cd "$(dirname "$0")"
out="${TMPDIR:-/tmp}/xy6-host-tests"
mkdir -p "$out"
flags=(-std=gnu++17 -O2 -Wall -Wextra -Werror -Imock)
for t in midi_latency_test joystick_filter_test regression_test turbo_test; do
  g++ "${flags[@]}" "$t.cpp" -o "$out/$t"
done
status=0
echo "== MIDI OUT latency, typical load"; "$out/midi_latency_test" typical || status=1
echo; echo "== MIDI OUT latency, heavy load"; "$out/midi_latency_test" heavy || status=1
echo; echo "== joystick smoothing"; "$out/joystick_filter_test" || status=1
echo; echo "== v1.18 regressions"; "$out/regression_test" || status=1
for s in happy nocaps noack badecho drop off silent machine busy refused latepeer deaf; do
  echo; echo "== TurboMIDI: $s"; "$out/turbo_test" "$s" || status=1
done
exit $status
