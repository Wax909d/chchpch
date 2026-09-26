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
for t in midi_latency_test joystick_filter_test regression_test turbo_test kit_test; do
  g++ "${flags[@]}" "$t.cpp" -o "$out/$t"
done
# The parameter map's open question: list CC values as indexes (built) or spread.
g++ "${flags[@]}" -DMNM_LIST_SPREAD=1 kit_test.cpp -o "$out/kit_test_spread"
g++ "${flags[@]}" render_pages.cpp -o "$out/render_pages"
status=0
echo "== MIDI OUT latency, typical load"; "$out/midi_latency_test" typical || status=1
echo; echo "== MIDI OUT latency, heavy load"; "$out/midi_latency_test" heavy || status=1
echo; echo "== joystick smoothing"; "$out/joystick_filter_test" || status=1
echo; echo "== regressions"; "$out/regression_test" || status=1
echo; echo "== Monomachine parameter map, lists as indexes"; "$out/kit_test" || status=1
echo; echo "== Monomachine parameter map, lists spread"; "$out/kit_test_spread" || status=1
echo; echo "== every page drawn (smoke test; the PNG is for looking at)"
"$out/render_pages" "$out/pages.png" || status=1
for s in happy nocaps noack badecho drop off silent machine busy refused latepeer deaf garbled ceiling loop; do
  echo; echo "== TurboMIDI: $s"; "$out/turbo_test" "$s" || status=1
done
exit $status
