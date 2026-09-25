# REDOT XY6 — LFO + pattern OS

Teensy 4.0 firmware for the XY6, a controller for the Elektron Monomachine:

- six Monomachine-style LFOs, phase-locked to the machine's MIDI clock
- a melodic pattern generator with scenes, p-locks and ratchets
- a MIDI joystick that drives any mix of the six tracks
- TurboMIDI at 8x over the standard DIN cable (type `turbo` on the console, or SET → TURBO → ENGAGE)
- a 256×64 OLED UI, and presets on a 24LC512 EEPROM

The code is one sketch, [`XY6_LFO/XY6_LFO.ino`](XY6_LFO/XY6_LFO.ino), plus
[`XY6_LFO/mnm_params.h`](XY6_LFO/mnm_params.h): every Monomachine machine, page
and parameter with its range, default and value list, from a survey of a real
machine. Keep the two files in the same folder. The sketch's header has the
wiring, the controls, and a "WHAT CHANGED" entry for every version.

## Build and flash

1. Install the Arduino IDE and the Teensy board package (Teensyduino).
2. Open `XY6_LFO/XY6_LFO.ino`.
3. Tools → Board: **Teensy 4.0** · USB Type: **Serial** · CPU Speed: **600 MHz**
4. Upload. The serial console (115200) prints a command list at boot; type `?` for it again.

## Tests (no hardware needed)

```sh
test/host/run.sh
```

This builds the sketch on a PC with g++ against a small model of the Teensy API
(`test/host/mock`) and runs:

| test | what it checks |
| --- | --- |
| `midi_latency_test` | how late notes and stick CCs reach the MIDI wire under a full stage load (pattern, six LFOs, stick on six tracks) |
| `joystick_filter_test` | stick step response, and that a resting or held stick sends nothing through ADC noise and spikes |
| `regression_test` | one check for each bug fixed in v1.18, and that a saved preset loads (v1.22) |
| `kit_test` | the parameter map (every list complete, every default in range, the FM+DYN ratios), names and values per machine, `mach`, `kit`, `kit watch`, and old presets' machine numbers - built twice, for both answers to the map's open question |
| `turbo_test <scenario>` | TurboMIDI against a model of the Elektron machine, both ways round: the XY6 leading (byte for byte, keepalive, no caps, no ACK, bad echo, a machine slow to switch, machine power-cycled, `turbo off`, silent machine, a MIDI IN too slow for 10x) and the machine leading as a real one does (plain, during back-to-back display pushes, refused after `turbo off`), a MIDI IN that smears edges above 1x (the real test board's symptom), `turbo max`, and the `turbo loop` self-test |

CI ([`.github/workflows/build.yml`](.github/workflows/build.yml)) runs these and a
real Teensy 4.0 compile on every push.

## Making a change

1. Edit the sketch, flash it, and try it on the hardware.
2. Bump `XY6_VERSION` and add a "WHAT CHANGED" entry at the top of the sketch.
3. Run `test/host/run.sh`, then commit and push. CI must be green.
