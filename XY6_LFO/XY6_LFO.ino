// =============================================================================
// REDOT XY6 — LFO + pattern OS                                    single file
// =============================================================================
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.24 - THE MONOMACHINE GRID
// -----------------------------------------------------------------------------
// Every page redrawn to sit beside the Monomachine: a rigid grid of 1px rules,
// sharp pixels, and one inverted block for whatever is in focus. The rules are
// in THE STYLE LAYER; each page's geometry is in the comment above it.
//  * FIVE SHADES, ONE JOB EACH: WHITE primary data and the focus block, LIGHT
//    secondary data, MID labels / frames / hints, DARK rules and empty bar
//    space (never text), BLACK the ground. Text never goes below MID.
//  * SHARP: no anti-aliasing anywhere - the scope trace, fader tops and
//    meters used to split a pixel's intensity across two. Faders, cursors and
//    meters snap instead of gliding, and page changes are CUT by default
//    (DISSOLVE and the rest are still in SET > TRANSITION).
//  * ONE FOCUS BLOCK: labels are plain 3x5 text now; only the selected
//    parameter, row or track is a solid WHITE block with its text knocked
//    out. Headers are bold 5x7 over a DARK rule; footers are 3x5 MID hints.
//  * VALUES in the 5x7 LCD face, dropping to the 3x5 only when a value will
//    not fit its cell (stText).
//  * BARS: DARK empty space, WHITE value, no outline. Bipolar parameters (PAN,
//    DIST, BOFS...) fill from the centre, lists (SID WAVE, TRIG, MULT...) show
//    a block at their entry.
//  * LFO EDIT is laid out as the Monomachine's LFO page: PAGE DEST TRIG WAVE /
//    MULT SPD INTL DPTH as a 2 x 4 grid of cells, E1 walking them in reading
//    order. Step probability is a solid fill height (was a checkerboard).
//  * PERF is a 3 x 2 grid of cells on rules - name, value, a 6 x 106 bar - with
//    the joystick as 1px meters at the two edges. The capsules, number chips
//    and triangle cursor are gone.
//  * 'style 0' draws the focus as a WHITE outline instead; 'invert 1' turns the
//    whole OS dark-on-light with the hierarchy intact.
//  * SET > FONT and 'font' are retired: the face is chosen by role on every
//    page now, so the global switch no longer changed anything.
//  * ONE FILE AGAIN: mnm_params.h is now the MONOMACHINE PARAMETER MAP section
//    just below the #includes. A folder holding only the .ino failed with
//    "mnm_params.h: No such file or directory"; nothing can go missing now.
//  * test/host/render_pages draws every page into one PNG.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.23 - PRESETS LOAD; THE STICK AND LOCKS FEED THE KIT MODEL
// -----------------------------------------------------------------------------
// v1.22 below, plus:
//  * FIX: LOAD never worked - every preset said CRC FAIL. The CRC's range
//    begins on the crc field itself and SAVE computes it with that field at
//    0; LOAD checked it with the stored value in place. Now checked the way
//    it was computed, so presets saved by any earlier version load too.
//  * The joystick and pattern locks tell the kit model what they sent, as
//    PERF and scenes already did - after a stick move, 'kit' and an LFO
//    pointed there start from what the machine really holds.
//  * test/host/kit_test: the parameter map, the kit model, 'mach', 'kit' and
//    'kit watch', echoes, ranges, PERF, and presets from v1.21 - built for
//    both answers to MNM_LIST_SPREAD.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.22 - THE MONOMACHINE'S OWN VALUES, FROM A HARDWARE SURVEY
// -----------------------------------------------------------------------------
// Every parameter on every page and every machine was read off the real
// Monomachine: its range, its value after loading a default kit, and every
// value list. That map is mnm_params.h, beside this file (a header, so the
// IDE's prototype hoisting cannot trip over its types).
//  * KIT MODEL. The XY6 now keeps what each track's 56 parameters hold,
//    starting from the init kit and learning from every CC the machine sends
//    and every value we send. 'kit <t>' prints it; 'kit watch' logs learning.
//  * LFO CENTRE. An LFO with nothing captured swings around its parameter's
//    init value (ATCK 0, WDTH 127, PHASER FB -19...), not a blanket 64. And an
//    LFO pointed at a new destination drops the old one's capture - it used
//    to keep swinging around the previous parameter's value.
//  * RANGES. Output, PERF knobs and fader fills stop at a parameter's last
//    value: a 5-entry list like SID WAVE is 5 values, not 128.
//  * DISPLAY. Values read as the machine shows them: DIST -12, SID WAVE SAW,
//    FM ratio 1/2, DENS PCH +07, DPRO WAVE S01. SYNT names come from the
//    track's machine (SET > MACHINES, the wizard, or 'mach').
//  * MACHINES. The 22 real machines replace a guessed list of 12. Old presets
//    map across (SIDLEAD / SIDBASS become the SID).
//  * PERF PUSH resets a slot to its init value instead of raw 0 (which on PAN
//    was hard left). The DEST subpage shows that value for unknown slots.
//  * LFO WAVE in the machine's order: TRI ITRI SAW ISAW SQR ISQR EXP IEXP RMP
//    IRMP RND (EXP used to sit after RMP). MULT stops at 64X, as the machine
//    does. Presets saved earlier are translated on load, never refused.
//  * EFFX slots 7/8 are DBAS / DWID, the machine's names.
//  * OPEN: whether a list's CC value is its index (assumed) or spread over
//    0..127 - see MNM_LIST_SPREAD in mnm_params.h and 'kit watch'.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.21 - WHY 10X FAILS ON THE TEST BOARD, AND WHAT TO DO ABOUT IT
// -----------------------------------------------------------------------------
// v1.20 on the real Monomachine: the machine led, we answered, both ends went
// to 10x - and at 10x we received  10 00 00 FE F0 F0 F0 F0 F0 00 00 00 ...
// Five F0s are the machine's test SysEx, sent five times: it WAS at 10x. Only
// bytes with one long low stretch (F0 FE 00) survived; every edge-dense byte
// of the message was mangled. That is a MIDI IN too slow for the speed.
//  * FIX: the timeout report said "clock parses, the link works, the machine
//    did not answer" - on the strength of one FE, a byte that survives almost
//    any line. It now counts SysEx STARTED against SysEx ARRIVED WHOLE and
//    says the machine is talking and the MIDI IN is mangling it.
//  * 'turbo loop': OUT 1 patched to IN 1, machine unplugged - the same 32
//    bytes (the real test SysEx and more) out and back at every speed from
//    1x to 10x, and the fastest this MIDI IN passes cleanly.
//  * 'turbo max <n>': a ceiling on both sides - the speeds we ask for when we
//    lead (test at the ceiling, run one step below) and the speeds we OFFER
//    in our 11 when the machine leads. 10 by default, which is exactly v1.20;
//    'save' keeps it (it lives in the globals' unused turboSpeedIdx byte).
//  * FIX: our SWITCH1 always went to 10x, whatever our 12 had asked for -
//    harmless while the request was always 10x, fatal to any ceiling.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.20 - TURBOMIDI RESPONDER, TEST RETRIES, A REASON FOR FAILURE
// -----------------------------------------------------------------------------
// From v1.19 on a real Monomachine: it sends 10 by itself every few seconds
// (TURBO enabled, looking for a TM-1), and our own handshake got 11 and 13
// back, switched to 10x, sent the test - and heard nothing.
//  * RESPONDER: the XY6 now answers the machine's own handshake, as a TM-1
//    does - 11, 13, switch to its SPEED1, echo its test, 17, switch to its
//    SPEED2. On from boot; 'turbo off' stops answering until 'turbo'.
//  * The switch after our 13 or 17 is taken from pumpMidi() as well as
//    loop(), so it lands within ~0.2 ms of the UART draining even mid-frame -
//    the machine starts at the new speed behind only a 0.5 ms pad.
//  * INITIATOR: no echo within 60 ms, the test goes again, pad and all, up to
//    4 times inside the 250 ms step.
//  * A step at 10x or 8x that times out now says what arrived since the
//    switch: nothing, bytes that do not parse (speed mismatch, or a MIDI IN
//    optocoupler too slow for TurboMIDI), or clock that parses (the link
//    works; the machine did not answer). The raw first bytes are printed.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.19 - TURBOMIDI INITIATOR, REBUILT TO THE CAPTURED PROTOCOL
// -----------------------------------------------------------------------------
//  * The negotiator is rebuilt around the handshake as captured from Elektron
//    gear: 10 / 11, then 12 SPEED1 SPEED2, 13, a switch to SPEED1 (10x), 16
//    raw 00s, test 14 / 15, test 16 / 17, a switch to SPEED2 (8x), 10 ms of
//    quiet, and the link is up. One state per step, all non-blocking - see
//    the table at the top of the Turbo MIDI section.
//  * v1.18 read 12 08 07 as "multiplier 8, index 7" and switched straight to
//    8x after the ACK. 08 / 07 are SPEED1 / SPEED2: the link is proven at 10x
//    first, then run at 8x. The second test (16 / 17) was not implemented.
//  * KEEPALIVE: FE every 150 ms for as long as the link is up (was compiled
//    out). Sent by midiTxService(), so every pump keeps it on time, and only
//    between messages - never inside a SysEx, even one sent in pieces.
//  * The handshake holds the WHOLE wire from the 12 on - notes and note-offs
//    wait too (v1.18 let the note queue through). Its own messages ride a new
//    queue, qTurbo, ahead of everything.
//  * Every UART switch waits for the LPUART's transmit-complete flag, so no
//    byte is cut off; each is made on the pass that parses the reply's F7.
//  * Link loss: a step that times out, a wrong echo, traffic that stops
//    parsing, or - once it has shown it sends FE - the machine going quiet
//    for 600 ms. From 10x or 8x that means REVERT: 31250, keepalive off, the
//    wire held 400 ms so the machine's own active-sensing timeout puts it
//    back at 1x too, then All Notes Off.
//  * Initiator only: a handshake the machine starts itself is logged and not
//    answered (the old responder followed it to the wrong speed). Removed
//    with it: 'turbo ack', 'turbo a', 'turbo b', the IDX BIAS row, the
//    identity-request probe and the legacy 0x20 / 0x21 commands. The settings
//    SPEED row is now 1X / 8X.
//  * test/host/turbo_test.cpp runs the handshake against a model of the
//    machine: the happy path byte for byte, no caps, no ACK, a bad echo, a
//    machine power-cycled mid-link, 'turbo off', and a silent machine.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.18 - MIDI OUT LATENCY, JOYSTICK FEEL, FIVE FIXES
// -----------------------------------------------------------------------------
// Numbers are from test/host, which builds this file on a PC against a model
// of the Teensy UART and runs it under a full stage load: pattern on all six
// tracks, six LFOs, the stick on all six tracks, 31250 baud.
//  * NOTES FIRST, FOR REAL. midiTxService() filled Serial1's 576-byte TX ring
//    whenever it had room, so the backlog sat in the UART where nothing can
//    reorder it, and a note-on waited behind every CC already handed over.
//    The UART now holds at most ~3 ms of wire (MIDI_TX_LEAD_US); the rest waits
//    in qNote / qCtrl, where notes do go first and JOY_Q_CAP does bite.
//    Note latency p99 12-15 ms -> 8.6 ms; worst 15 -> 9.9 ms.
//  * JOYSTICK SMOOTHING (JOY_FAST_CNT): a median of three, then the same
//    one-pole, which follows real motion at 3/4 per pass instead of 1/4. A
//    throw reaches 90% in 24 ms, was 64. A spike (a worn wiper) is removed
//    instead of averaged in: a stick held off centre with one spike every 2 s
//    sent 105 CCs a minute per track, now none. At rest it is as quiet as
//    before; JOY_FAST_CNT 0 restores the v1.17 filter exactly.
//  * FIX: moving BASE CH while notes sounded sent their note-offs - and the
//    p-lock restores - to the NEW channel: stuck notes, stuck locks. Every
//    channel change now goes through setBaseChannel(), which releases the old.
//  * FIX: the turbo fallback's All Notes Off used the compile-time channel.
//  * FIX: a track taken off the stick and put back later was sent the
//    resting 64 at once, overwriting its patch value.
//  * FIX: the stick's own CCs echoed through a THRU / merge counted as someone
//    at the machine and held the LFOs at the 15 Hz back-off.
//  * FIX: SAVE pressed during a save showed NO EEPROM over a save that was
//    working; 'wipe' during a save printed "cleared" and wrote nothing.
//  * uiNextPage() is declared instead of relying on the IDE's generated
//    prototypes, so the sketch also builds as plain C++. Clean at -Wall
//    -Wextra. Out-of-date comments and help text corrected (LFO 1 is TRIG,
//    presets are wired, rmap default 41, turbo ack default 1); two unused
//    #defines removed. The sketch now lives in XY6_LFO/XY6_LFO.ino.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.17 - JOYSTICK TO ONE, SOME OR ALL TRACKS
// -----------------------------------------------------------------------------
//  * The stick plays any set of Monomachine tracks: one, all six, or any mix,
//    each on its own channel (base + track). New PERF sub-page JOY - hold on
//    PERF, then click past DEST:
//      push E1..E6   track 1..6 under the stick, on / off
//      turn E3       quick pick: T1 .. T6 alone, then ALL
//      turn E1 / E2  what X / Y move - any of the 57 CC destinations
//  * FIX: X / Y went out as CC 16 / 17, which the Monomachine maps to nothing,
//    so the stick moved the meters and no sound. Default now: X = FILT BASE
//    (CC 72), Y = FILT WDTH (CC 73).
//  * Made for all six at once. Every track remembers what it was last sent and
//    only ever gets the LATEST value - never a backlog of stale ones - and X+Y
//    for one track go as one running-status message (5 bytes, not 6). The
//    stick tops the CC queue up to JOY_Q_CAP bytes and no further, and uses at
//    most JOY_BUDGET_BPS of the wire, so LFOs and notes keep their timing
//    however hard it is thrown; a track that has to wait goes first next pass.
//  * A PERF slot on the same track + parameter follows the stick, and an LFO
//    running there has its centre moved (like the PERF knobs) instead of being
//    fought over.
//  * Saved with the globals. Console 'joy': status, all, off, 1-6, tog N,
//    x N, y N, and 'joy sweep S' - a dry-run stress test that transmits nothing,
//    then watches the real resting stick for 2 s and reports any would-be send.
//  * DEST's footer said CLICK / BACK; since v1.16 a click is NEXT.
//  * FIX: LFO 1's power-up default was FREE (a bring-up aid), so whenever no
//    preset loaded it swept AMP ATTACK on track 1 - and the PERF ATK fader -
//    with the sequencer stopped. It is TRIG like the other five: it waits for
//    play.
//  * Two transition settings, SET > TRANSITION: PAGES for moving between the
//    four main pages (default CUT - a plain instant switch) and SUB for
//    everything that starts or ends on a sub-page - in, out and along the
//    ring (default DISSOLVE).
//    Console: 'trans N' and 'trans sub N'.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.16 - NAVIGATION, BOOT, TRANSITIONS
// -----------------------------------------------------------------------------
//  * Board button, two layers. MAIN: click = next page, hold 0.5 s = enter this
//    page's sub-pages. SUB: click = NEXT SUB-PAGE (it used to drop you back to
//    the main page), hold 0.5 s = back to MAIN on the page you are on. The
//    sub-pages form one ring: DEST > EDIT L1 > ... > EDIT L6 > DEST.
//  * A thin hold-progress line on the bottom row of every page whose hold does
//    something, so the hold is visible everywhere, not only in setup.
//  * The hold is 0.5 s (BTN_HOLD_MS), down from 1 s; a click is still anything
//    released before that.
//  * Boot lands on PERF, the performance page.
//  * Page transitions are a setting (SET > DISPLAY > TRANS, or 'trans N'):
//    SLIDE, WIPE, DISSOLVE (default), FLASH, CUT. Saved with the globals.
//    DISSOLVE is the soft kind: 8x8 dither order, each pixel crossfading
//    through the grey levels, smoothstep-paced over 240 ms. All transitions
//    run 1.5x quicker than first shipped: SLIDE 160, sub-page fade 113,
//    WIPE 120, DISSOLVE 240, FLASH 47 ms.
//  * FIX: no page transition had EVER played (since v1.08). The start time came
//    from micros() but the first frame used the older loop timestamp, so the
//    elapsed time was negative - as unsigned, "already finished". Transitions
//    also run at 60 Hz now (30 Hz otherwise), ~14 frames for a dissolve.
//    'trans' reports how many in-between frames the last one drew.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.15 - STABILITY AUDIT
// -----------------------------------------------------------------------------
//  H1  Globals CRC ran over an UNCHECKED length from EEPROM: a record with a good
//      magic and a corrupt size read up to 64 KB past a 32-byte struct.
//  H2  Presets accepted any size <= the struct, so a short size meant a short
//      CRC and the rest of the record loaded unverified. Size is now exact.
//  H3  The first-run wizard could never see a kit dump: the byte count and the
//      end-of-dump test only ran while the 8-byte header was being captured.
//      Counting is separate now, and the wizard advances when a dump lands.
//  H4  A full note queue silently dropped NOTE-OFFs: stuck notes. Note-ons and
//      locks now leave headroom, and a note-off that cannot queue is retried.
//  H5  MIDI clock bytes processed back to back (after any stall) were thrown
//      away as "runt edges", so the tick count - and every LFO - fell behind the
//      Monomachine until the next START. Late ticks count; they just do not
//      steer the tempo estimate.
//  H6  A faulty I2C bus blocked the loop up to ~66 ms per MCP23017 call, 200
//      times a second. Now: exponential back-off, the expander is reconfigured
//      when it answers again, and the LED shadow only updates on success.
//  H7  Hardware watchdog (RTWDOG, 4 s). A hang now reboots into the splash and
//      rejoins the clock instead of freezing on stage. 'crash' shows why the
//      last reset happened.
//  M1  Incoming CCs were matched against an UNWRAPPED channel (base+track can
//      pass 16), so with a high base channel nothing was ever learned.
//  M2  A status byte arriving inside Song Position Pointer was eaten as data.
//  M3  DELETE went through the save path and made the deleted slot the one to
//      load at the next boot.
//  M4  LFO SPD/INTL are range-clamped on preset load like every other field.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.14 - JOYSTICK METERS HIDE WHEN IDLE
// -----------------------------------------------------------------------------
//  * The PERF page's X/Y meters fade out after JOY_HIDE_MS at rest and fade
//    back in the moment the stick moves (they start hidden at power-up).
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.13 - PERF JOYSTICK METERS, USER DESIGN
// -----------------------------------------------------------------------------
//  * Built from the user's pixel drawing: 1-px meters on a dashed track, X on
//    the left and Y on the right, lettered on the split between the banks.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.12 - PERF FOLLOWS THE LFOs
// -----------------------------------------------------------------------------
//  * A PERF slot pointed at the same track + parameter as a running LFO now
//    SHOWS the LFO: the fader and the number move with what is actually being
//    sent. (Centre notches were tried and removed - they read as stray lines.)
//  * Turning that encoder moves the centre (the LFO's base), the way the
//    Monomachine's own knob does under an LFO, instead of sending a CC the LFO
//    overwrites a millisecond later. An LFO that is not running yet still gets
//    its centre from the knob, so it starts where the fader is.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.11 - SCENES
// -----------------------------------------------------------------------------
//  * Three new pattern genres that are complete SCENES: FROST (ambient), FOG
//    (Skee Mask broken techno) and DRILL (Aphex braindance). Notes as scale
//    degrees, seed-varied; each brings its own key, scale and length.
//  * Per-step PARAMETER LOCKS over MIDI: the CC goes out just before the note,
//    the base value comes back before that track's next note - a p-lock.
//  * RATCHETS: 2-4 hits inside one 16th, timed by the XY6 itself.
//  * Scene SETUP on E6 push (PAT page) or 'scene': shared-page sound for all six
//    tracks plus a configuration for the six XY6 LFOs. 'scene info' prints the
//    kit sheet (machine per track) and tempo - the two things MIDI cannot set.
//  * Pattern notes now follow the live BASE CH, like everything else.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v1.09 / v1.10 - DISPLAY FIX AND POLISH PASS
// -----------------------------------------------------------------------------
// v1.09 (display):
//  * Panel re-map is 0x41, not 0x43. The wrong nibble order swapped every pair
//    of adjacent pixels - invisible on lines and big text, fatal to 1-px text.
//  * No flicker: a frame identical to what the panel holds is not sent at all
//    (v1.08 pushed 8 KB at 60 Hz regardless). Redraw is 30 Hz.
//  * Stress-test bus timing and single-stream frame push; reset timing and the
//    geometry registers re-asserted every push. 'fbdump' dumps the framebuffer.
// v1.10 (polish):
//  * JOYSTICK ON. It was compiled out in every earlier build. Now also: no CC
//    at boot (it used to send 64 and overwrite the patch), outer edge margin so
//    worn pots reach 0/127, implausible boot centre falls back to 512, and it
//    follows the live BASE CH instead of the compile-time one.
//  * Joystick X/Y meters on the PERF page, in the side margins.
//  * Switch debounce is per switch and needs 3 stable scans (10-15 ms); the old
//    whole-port two-sample rule was shorter than a PEC16 push bounce.
//  * Encoders step on arrival at the detent, so one missed transition can no
//    longer leave every later click half a detent out.
//  * Removed the 8 KB LUT bus driver and two diagnostic switches (bus, fquiet).
//  * 's' prints a JOY line: position, centre, CCs sent.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v7 - THE ARCHITECTURE PASS
// -----------------------------------------------------------------------------
// Nothing in the turbo layer, the pattern generator or the LFO maths was
// touched. Everything here is the specification's own structure, finally built:
//
//  1. PRESETS ARE REAL. The settings page used to say SAVE and LOAD were not
//     wired, because they were not. There is now a packed, versioned, CRC'd
//     format and a NON-BLOCKING 24LC512 driver behind those rows - a 2.2 KB
//     preset writes in ~340 ms of device time spread across ~340 passes of the
//     main loop, and not one of them blocks. The LFOs keep sweeping while you
//     hit SAVE, which is the entire point.
//
//     The chunk size is 32 bytes, NOT the part's full 128-byte page, and that
//     is the one number worth arguing about: a 128-byte page write is 2.9 ms of
//     blocking I2C inside a loop whose budget is 1 ms. It would have satisfied
//     "non-blocking save" on paper while breaking the rule this file exists to
//     enforce. Four times the write cycles is the price; a save you asked for
//     taking a third of a second is invisible, a third of a millisecond of MIDI
//     jitter is not.
//
//  2. THE FIRST-RUN WIZARD. Spec: "All these preset should show while booting
//     if on device is no presets." Cold boot now routes REDOT splash ->
//     machine select -> kit SysEx wait -> save -> Page 1. Warm boot reads the
//     globals BEHIND the splash animation and loads the last preset, so the
//     3.6 s logo is doing real work rather than being a stall with a picture
//     over it.
//
//  3. ENCODER COARSE MODE. Spec: "pressing encoder and rotating should be 7
//     values +-". One global interceptor between the quadrature decoder and the
//     UI, so no page can forget it or implement it differently. The half that
//     is easy to miss is the SUPPRESSION: push-and-turn and push-to-click are
//     the same gesture up to the instant you turn, so encoder pushes now fire
//     on RELEASE and only when the knob did not move. Without that, every
//     coarse sweep ended in an accidental button press.
//
//  4. THE HOLD GESTURE IS A CLASS. The inline b6DownMs / b6Done block became
//     ButtonGesture, which adds an explicit `valid` input - a dropped I2C read
//     can no longer be mistaken for a release - and a progress read-out, so the
//     hold stops feeling like a dead button while you wait it out. The hold is
//     1 s, half the spec's 2 - see the note by BTN_HOLD_MS.
//
//  5. PER-STEP PROBABILITY. Spec, LFO subpage, fourth encoder: "changes step
//     into Active 0, 10%, 20%, 33%, 50%, 66%, 75, 90, 100% activation". Nine
//     detents, stored as the ladder INDEX so re-tuning the ladder later keeps
//     old presets meaning the notch the user chose. The grid draws each step as
//     a fill HEIGHT, so a pattern that thins toward the end of the bar is a
//     shape you can see rather than sixteen numbers to decode.
//
//  6. SPEC ROUTING ON THE LIST PAGES. The encoder subpage and the LFO subpage
//     are scroll lists now, which is what the sketch asks for:
//       encoder subpage   E1 scroll, E2 destination, E3 track
//       LFO page          E1 scroll, E3 depth, E4 amount
//       LFO subpage       E1 parameter row, E2 value, E3 step, E4 probability
//     This is a real change to muscle memory and it is deliberate. E6 keeps its
//     old depth binding on the LFO page so the two cannot disagree.
//
//  7. AMOUNT. The LFO page's fourth encoder needed something to hold. Spec says
//     only "Depth and ammount control by 3-4 Encoder" and never defines the
//     difference, so: depth is the swing, amount is how much of that swing
//     reaches the destination. The scope trace scales by both, so the picture
//     stays the truth.
//
//  8. THE RENDERER YIELDS TO MIDI. Rendering is the only task in the loop with
//     no deadline at all - a dropped frame is a frame nobody saw, a dropped
//     clock byte is audible. When the receive buffer backs up past 96 bytes the
//     renderer stands down for one frame and hands the slice to MIDI. That
//     matters most during exactly the operation the wizard waits on: a kit dump.
//
//  9. LOOP TELEMETRY. Worst-case pass time is on the settings page next to the
//     RX buffer high-water mark. Those two numbers together are the whole
//     health picture of an architecture like this one; under ~1500 us is
//     healthy, and the pass that sets the maximum should be a display flush.
//
// Still deliberately not done, and the pages say so rather than pretending:
// the kit SysEx PARSER (the wizard counts the dump's bytes, it does not decode
// engine placements yet) and MIDI_HandleDirectPlayCapture, which is on the
// schedule with an empty body.
//
// Arduino IDE + Teensyduino.  Tools > Board: Teensy 4.0
//                             Tools > USB Type: Serial
//                             Tools > CPU Speed: 600 MHz
//
// Six Monomachine-exact LFOs phase-locked to the Monomachine's sequencer over
// MIDI clock, a melodic pattern generator on the same wire, and a rebuilt OLED
// UI. The Arduino IDE wants the sketch in a folder of the same name, so this
// file lives in XY6_LFO/ - open XY6_LFO/XY6_LFO.ino.
//
// -----------------------------------------------------------------------------
// TURBO MIDI  (new in v4)
// -----------------------------------------------------------------------------
// Elektron's TurboMIDI, the protocol the TM-1 speaks, negotiated with the
// Monomachine over the DIN link and then run at up to 10x standard MIDI.
//
// Type  turbo  on the console, or SET > TURBO > ENGAGE. The XY6 runs the
// handshake (tested at 10x, run at 8x) and keeps the link alive with FE every
// 150 ms; 'turbo off' goes back to 1x. v1.19 rebuilt the negotiator - see the
// Turbo MIDI section for the protocol step by step.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN v6 - THE TURBO HANDSHAKE AUDIT
// -----------------------------------------------------------------------------
// Nothing outside the turbo layer was touched. Ten fixes, in the order they are
// worth checking:
//
//  1. THE DEVICE-ID FIELD. SysexRx demanded the header be exactly
//     F0 00 20 3C 00 00. Bytes 4 and 5 are a product / device id, and Elektron
//     instruments stamp their own there - 03 00 on a Monomachine. If this
//     machine does that, EVERY turbo message it sent was classified FOREIGN and
//     the negotiator never saw one: it sat waiting for a reply it had already
//     discarded, then changed speed on its own. Those two bytes are now learned
//     and echoed back, with a command whitelist so kit dumps stay out.
//     >>> Read them off the DIAG page (page 5, LAST SYSEX IN) first. <<<
//
//  2. THE SPEED TABLE INDEX. kTmSpeeds[] has twelve entries with 31250 twice,
//     so 8x is index 7. MegaCommand's table has eleven and puts 8x at index 6.
//     If the machine uses the other convention we ask for index 7 meaning 8x
//     and it hears index 7 meaning 10x - it goes to 312500, we go to 250000,
//     and the link is dead from the first byte while both ends believe they
//     negotiated. The old two-byte cross-check could not catch this because
//     both bytes came from the same table. Inbound 0x12 now resolves by
//     MULTIPLIER, which has no table behind it, and PRINTS the bias to use.
//     Set it live with  turbo b <-2..2>  - no reflash.
//
//  3. LOGGING FROM THE RX PATH. onMessage() is reached from handleMidiByte(),
//     which runs from inside flushAll() with /CS asserted. Every Serial.print
//     there is the 120 ms usb_serial_write stall this file warns about at the
//     top - 3000 received bytes at 250000 baud, three times the RX buffer,
//     destroyed in the middle of the negotiation. That is the mechanism by
//     which turbo killed the LFOs, and it was still present. The whole turbo
//     layer now writes to a ring that loop() drains into free USB space only.
//
//  4. rxSeen_ WAS A uint8_t compared against 0xFFFF, so it wrapped silently at
//     256. At 160 BPM the sequencer puts exactly 256 clock bytes in the 4000 ms
//     watchdog window, rxSeen_ lands on zero, and the watchdog tears down a
//     perfectly healthy link. That is the random drop-back to 31250.
//
//  5. THE ACKNOWLEDGEMENT WAS IGNORED. It arrives during SEND/DRAIN/GUARD and
//     was only tested in VERIFY, a state it never arrives in. So the port was
//     reprogrammed on a timer with no confirmation at all. The switch now waits
//     for it, and aborts rather than going one-sided ('turbo f' still forces).
//
//  6. SILENCE MEANT YES. An unanswered capability query used to "set the speed
//     anyway" - with zero evidence the machine speaks the protocol. Now a NO.
//
//  7. VERIFY PROBED WITH 0x10 - which the watchdog comment further down already
//     explains can never be answered, because the machine is the initiator of
//     0x10, not a responder to it. Verification silently collapsed onto
//     counting clock bytes, so with the transport stopped it locked an entirely
//     unverified link. It now probes with the Universal Identity Request
//     (F0 7E 7F 06 01 F7), which every Elektron instrument answers and which
//     tests the only thing that matters: whether both ends still frame bytes
//     the same way.
//
//  8. availableForWrite() DOES NOT SEE THE HARDWARE FIFO. Serial1 is LPUART6;
//     when the software ring reports empty there are still up to four bytes in
//     the TX FIFO and one in the shift register - five byte times, 1600 us at
//     31250 - and the guard was four. begin() disables the transmitter, so it
//     truncated a byte still on the pin. Now gated on LPUART_STAT_TC.
//
//  9. THE GUARD USED A STALE TIMESTAMP. loop() captures nowUs before
//     pollSerial(), and a console 's' runs four usbWait() calls of up to 50 ms
//     each - so the guard could be computed from a 150 ms old clock and expire
//     instantly. It reads micros() itself now.
//
// 10. begin() LEFT THE RX FIFO AND ERROR FLAGS ALONE, so four bytes captured at
//     the OLD baud were handed to the parser as valid MIDI at the new one, and
//     half-assembled running-status / SPP state survived the change too.
//
// -----------------------------------------------------------------------------
// REVIEW PASS - 16 DEFECTS FOUND AND FIXED AFTER THE UI RESTRUCTURE
// -----------------------------------------------------------------------------
//  1  DRAIN was the only turbo state with no deadline. It is the state that
//     holds turboHoldsWire() true, so a port that never went idle blocked the
//     CC queue FOREVER: silent box, skipped notes, no indication, no recovery
//     short of a power cycle. 600 ms timeout that releases the wire either way.
//  2  The '*** INDEX MISMATCH ***' message is 203 chars of format string and
//     the log buffer was 200, so vsnprintf beheaded the line that names the
//     'turbo b' value to set - the single most useful output in a bring-up.
//  3  sendCmd() had no bound on n. 1+3+2+1+n+1+16 into uint8_t m[32] overflows
//     above n == 8; every caller passes 2, so it was safe by luck.
//  4  tmLogPut() copied until the ring filled and then returned, leaving HALF
//     A LINE - a corrupt fragment welded to the next message. All or nothing.
//  5  setActivate() used a bare Serial.printf, the one UI print bypassing the
//     deferred log: up to 120 ms of blocked loop, ~3750 bytes at 250000 baud.
//  6  mcpRead() returned 0xFF on a bus error, indistinguishable from "nothing
//     pressed". Harmless while only press edges mattered - but the board button
//     now acts on RELEASE, so one dropped I2C read silently advanced the page
//     or killed a hold. Explicit failure return, plus a two-sample filter.
//  7  A PERF slot kept its old VALUE after being re-pointed at a new
//     destination, so the fader confidently displayed a number the machine had
//     never been given. Same defect as the fixed `label` field. Slots are now
//     `known`/unknown and draw "--" until there is something true to show.
//  8  joyMap() divided by zero at centre == 29 (low) and 994 (high). UB in C.
//  9  No deadzone hysteresis: a stick parked on the boundary dithered 64<->65
//     and streamed a CC every 8 ms. Measured 375 B/s before, 0.1 B/s after.
// 10  midiRxReset() left an in-progress SysEx header capture running across a
//     baud change, splicing before-and-after bytes into one bogus LAST SYSEX IN
//     - on the very row you read to diagnose turbo.
// 11  setValue() passed an argument to a runtime-selected format string.
// 12  The SI_HDR branch ignored the `cap` it was handed.
// 13  The settings page was the only page that could never idle: 25 snprintf
//     per frame at 30 Hz for counters nobody reads that fast. Now ticked at 4 Hz.
// 14  Pushing E1 on a value row bumps it - undocumented until the hint existed.
// 15  Dead forward declaration of mnmCC().
// 16  Found by testing, not by reading: the joystick could not reach 0 or 127.
//     The "+ 1" in both divisors - there to dodge the division in defect 8, and
//     failing at that too - cost both ends of the range, so the stick could
//     never fully close a filter or hit maximum. Clamped instead of fudged.
//
// Both build configurations are compiled and warning-clean, JOYSTICK_ENABLED 0
// and 1: defects 8, 9 and 16 live in a block that is normally compiled out, and
// shipping them unverified would have been the same mistake twice.
//
// NOT a firmware issue, and worth ruling out before any of the above: the DIN
// input opto. A 6N138 has microseconds of propagation delay against a 4 us bit
// at 8x - it cannot work above about 2x no matter what this file does. Elektron
// gear and the TM-1 use a 6N137 or H11L1 (~50 ns). The baud GENERATOR is not a
// suspect: Teensyduino clocks LPUART6 from 24 MHz and hits 250000 and 500000
// exactly, 312500 within 0.26%, worst case in the table 1.05%.
//
// WHY THE LAST ATTEMPT KILLED THE LFOs, AND WHAT IS DIFFERENT NOW
//
// The MIDI spec allows a System Real-Time byte — 0xF8 Clock above all — to
// appear ANYWHERE in the stream, including between two data bytes of a SysEx
// message. A receiver is required to act on it immediately and then carry on
// with whatever it was assembling. The previous implementation read the
// handshake with a blocking loop that swallowed every byte until 0xF7, so for
// the length of the negotiation every clock tick the Monomachine sent was
// eaten. The LFOs here are phase-locked to that clock, so they froze; and
// because a blocked loop also stops servicing the UART, the RX buffer overran
// and the tick count never recovered.
//
// In this version the byte handler dispatches Real-Time FIRST, before any
// other test, and returns — see handleMidiByte(). A Real-Time byte therefore
// never reaches the SysEx parser at all, so it cannot advance, corrupt or
// terminate the SysEx state, and the parser has no idea it happened. Nothing
// in the turbo path blocks: no delay(), no readBytes(), no wait-for-byte loop.
// The negotiation is a state machine ticked from loop() alongside everything
// else, and even the wait for the UART to drain before the baud change is a
// polled condition rather than flush().
//
// Belt and braces on top of that: the link is VERIFIED after the switch by
// probing the machine at the new speed, and a watchdog reverts to 31250 baud
// if the link ever goes quiet. Losing clock is the thing that breaks the LFOs,
// so losing clock is precisely the condition that undoes the change.
//
// -----------------------------------------------------------------------------
// WHAT CHANGED IN THE PREVIOUS REVISION
// -----------------------------------------------------------------------------
// Three areas: readability, smoothness, and eleven real bugs.
//
// READABILITY. The old pages were built out of inverted chips — a solid white
// block with the label knocked out of it. On a 4bpp OLED at 5x7 that is the
// worst possible way to set small text: the lit block blooms into the glyph
// strokes and the word fills in solid. Every chip is gone. Text is now bright
// strokes on black, the secondary shade was lifted from 5/15 to 9/15 (a third
// of full brightness is invisible under any room light), headings and live
// values are drawn in a real bold — the same 5x7 glyph struck twice one pixel
// apart, so strokes are 2px and survive the panel's bloom — and every page was
// relaid out with more air: 11px row pitch instead of 9, values right-aligned
// against a knocked-out margin so a moving trace can never cross a digit.
//
// Solid white also costs current. Dropping the chips bought back enough of the
// 3V3 budget to raise the default contrast from 0x80 to 0xA0, which is worth
// more for legibility than anything else in this list.
//
// SMOOTHNESS. Every MIDI write now goes through a queue. Before, the pattern
// generator wrote notes straight to Serial1 behind a one-byte space check, so
// a full TX buffer blocked the whole main loop mid-note — the LFOs stuttered
// exactly when the pattern was busiest. There are two queues, notes and CC,
// notes drain first, and nothing ever blocks. Also: the per-loop 64-bit divide
// for the free-running clock is now a cached multiply, the output scheduler is
// rate-limited instead of running its 36-entry scan at full loop speed, the CC
// rate cap adapts to how many LFOs are actually driving (a lone LFO now sweeps
// at 80 Hz instead of 50), 'ramp' no longer blocks for six seconds, and a
// captured SysEx dump prints from the main loop rather than from inside the
// display flush.
//
// BUGS FIXED
//   1. drawPattern's key buffer was 8 bytes for a string that reaches 10
//      ("C#" + "-1" + " " + "hmin"). Stack smash on every frame of the
//      pattern page. The seed buffer was one byte short of a 10-digit seed.
//   2. LfoEngine::held_ and trigPos_ were never initialized. A HOLD-mode LFO
//      read garbage until its first trig.
//   3. stepSeq divided by p.bars * 16 without checking for zero.
//   4. patSendNoteOn checked availableForWrite() for non-zero, not for the
//      three bytes it was about to write, so it blocked with 1 or 2 free.
//   5. Any incoming CC — including our own output echoed back through a THRU
//      or a merge — triggered the "user is touching the machine" backoff and
//      held the LFO update rate at 15 Hz forever.
//   6. At boot every LFO transmitted before anything had been captured, so
//      six AMP parameters were overwritten with 64 before you touched a knob.
//      LFOs now transmit nothing until they are genuinely driving.
//   7. Changing PAGE or DEST from the encoders did not re-address the output.
//   8. setParam with an unmapped destination (MIDI/MENV pages have no CC) left
//      the previous address live and kept writing to it.
//   9. The pattern miniature's last step column was drawn one pixel off-screen.
//  10. Trig conditions consumed the generator's own RNG at playback time, so
//      what you heard drifted from what 'pat show' printed.
//  11. clip3() rendered "TRI" and "TRI'" identically — a mirrored waveform was
//      indistinguishable from its base on any narrow layout. Mirrors are now
//      named ITR / ISW / ISQ / IRM / IEX.
//
// -----------------------------------------------------------------------------
// WIRING
// -----------------------------------------------------------------------------
//   OLED D0..D7   2 3 4 5 6 9 10 11      /WR 12  D/C 16  /CS 17  /RES 20
//   MIDI port 1   RX 0   TX 1   (Serial1)   <- connect this one to the MnM
//   MIDI port 2   RX 7   TX 8   (Serial2)
//   Joystick      X 14 (A0)  Y 15 (A1)
//   I2C           SDA 18  SCL 19    MCP23017 @0x20, EEPROM @0x50
//   Encoders A/B  21/22  23/24  25/26  27/28  29/30  31/32
//   MCP23017      GPA0-5 encoder switches, GPA6 button, GPB0-4 LEDs
//
// -----------------------------------------------------------------------------
// CONTROLS
// -----------------------------------------------------------------------------
//   BOARD BTN  SHORT PRESS  next page:  PERF -> LFO -> PAT -> SET -> PERF
//              HOLD 0.5 s   enter or leave this page's subpage
//
//   The short press fires when you RELEASE, because that is the only way to
//   tell it apart from the first second of a hold. Pages 3 and 4 have no
//   subpage and a hold there does nothing.
//
//   1 PERF         six faders, live. enc N turn : slot N value
//                  enc N push : slot N back to its init-kit value
//     > DEST       enc N turn : slot N destination, through all 57 CC-backed
//                               parameters     enc N push : slot N track
//
//   2 LFO overview enc1 turn : select LFO       enc6 turn : selected LFO depth
//                  enc1 push : open LFO edit (same as the 0.5 s hold)
//
//   3 PAT          the pattern generator, as below
//
//   4 SET          enc1 turn : move cursor      enc6 turn : change value
//                  enc1 push : fire an action row (SAVE / LOAD / ENGAGE)
//                  presets, MIDI, MACHINES (T1-T6), turbo, display, and the
//                  live LINK block that
//                  used to be the DIAG page - RX/TX counters, buffer peak,
//                  turbo state and baud, and the last SysEx header received.
//                  SAVE / LOAD / DELETE write the 24LC512 at 0x50 through the
//                  non-blocking driver (see PRESET STORE).
//
//   LFO edit       enc1 PAGE   enc2 DEST   enc3 WAVE   enc4 MULT   enc5 SPD
//                  enc6 DPTH
//                  enc1 push : back      enc2 push : LFO on/off
//                  enc3 push : TRIG mode enc4 push : INTL   enc5 push : bars
//                  enc6 push : manual trig
//
//   PATTERN        enc1 GENRE   enc2 SEED nudge   enc3 BARS
//                  enc4 ROOT    enc5 SCALE        enc6 track cursor
//                  enc6 push : on a scene (FRST / FOG / DRIL), send its sound
//                              and load its six LFOs - footer shows "6 SCN"
//                  enc1 push : regenerate (new seed)
//                  enc2 push : engine ON/OFF (sends note-offs on the way out)
//                  enc3 push : view next bar in the grid
//                  enc4 push : mute/unmute cursor track
//                  enc5 push : cycle scale
//
//   OUT OF THE BOX: all six LFOs are TRIG (v1.17), so they start on play and
//   lock to the bar. None of them transmit until they are actually driving, so
//   a cold boot never disturbs your patch. 'clk i' runs them on the internal
//   clock to check the chain with no Monomachine attached.
//
//   At boot all five LEDs light in turn, GPB0..GPB4 = D2 D3 D4 D5 D1. The last
//   one in that sweep is the tempo LED; after the sweep it blinks the beat.
//   D2..D5 follow the four pages in order, and the page LED BLINKS while you
//   are in a subpage - so the panel tells you which level you are on without
//   spending a fifth LED on it.
//
// =============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include <math.h>          // sqrtf, used by the boot animation
#include <stdarg.h>        // vsnprintf, used by the deferred turbo log

// =============================================================================
// MONOMACHINE PARAMETER MAP                          (mnm_params.h until v1.24)
// =============================================================================
// It lived in a header beside this file, and a sketch folder holding only
// the .ino failed to compile: "mnm_params.h: No such file or directory".
// Inside the sketch, it cannot go missing.
//
// It has to stay HERE, above the first function in the file. The Arduino IDE
// inserts the prototypes it generates just before the first function
// definition, and several of them name MnmParam - so every type in this
// block is declared before its first function (mnmListIndex, at the end).
//
// Transcribed from Redot's hardware survey (Freeform board "Xy6", box
// "Claude", Sep 2026): every parameter on every page and machine, its
// display range, its init-kit default, and every value list, read off a
// real Monomachine. Survey notation: "(x)" = value after loading a default
// kit, "0-127" / "-64-63" / "on/off" = the parameter's range.
//
// Everything is prefixed kMnm / Mnm / mnm so it cannot collide with the
// sketch, which owns page numbering (PAGE_*), CC numbers and names.
//
// Wire convention (confirmed by the survey itself): a bipolar -64..63
// parameter is raw-64 on the wire. THRU/REVERB show INP as "(64) 0-127" and
// CHORUS/DYNAMIX show it as "(0) -64..63" — the same raw 64.
//
// ONE OPEN QUESTION: for list parameters (SID WAVE, FM ratios, intervals,
// phonemes, LFO WAVE/TRIG/MULT...) it is not yet known whether the CC value
// is the list index (0,1,2,...) or spread across 0..127. Default here is
// index, clamped. To check: 'kit watch' on the XY6 console, then turn SID
// WAVE TRI->NOISE on the MnM. Raw 0,1,2,3,4 = leave as is. Big jumps = set
// MNM_LIST_SPREAD 1. (FM+DYN FRQ has 128 entries, so it is exact either way.)

#ifndef MNM_LIST_SPREAD
#define MNM_LIST_SPREAD 0
#endif

// ---------------------------------------------------------------- kinds
enum : uint8_t {
  MNK_NONE = 0,   // unused slot ("X" on the survey)
  MNK_U7,         // 0..127, shown as is
  MNK_S7,         // -64..63, shown as raw-64
  MNK_LIST,       // arg = MnmListId
  MNK_SLOT,       // arg = MnmSlotId  (S01-S32, D01-D64)
};

typedef char MnmItem[6];                    // up to 5 chars + NUL

// ---------------------------------------------------------------- lists
static const MnmItem kMnmOnOff[]    = {"OFF","ON"};
static const MnmItem kMnmSidWave[]  = {"TRI","SAW","PULS","MIX","NOIS"};
static const MnmItem kMnmSidMod[]   = {"OFF","RING","SYNC","R+S"};
static const MnmItem kMnmSidMsrc[]  = {"MFRQ","PTCH"};
static const MnmItem kMnmDproSync[] = {"OFF","SFRQ","PTCH"};

// FM+ STAT / PAR operator ratios. Entry 9 is written "5/32" on the survey but
// sits between 3/8 and 7/16, so it is almost certainly 13/32 — check on the MnM.
static const MnmItem kMnmFmRatio[] = {
  "1/64","1/32","1/16","3/32","1/8","3/16","1/4","5/16","3/8","5/32",
  "7/16","1/2","5/8","3/4","7/8","1","1.25","1.5","1.75","2",
  "2.5","3","3.5","4"};

// FM+ DYN ratios: exactly one entry per raw value — raw/64, truncated,
// except raw 127 which reads 2.0. Verbatim from the survey.
static const MnmItem kMnmDynRatio[128] = {
  "0.0","1/64","1/32",".046","1/16",".078","3/32",".109","1/8",".140",
  "5/32",".171","3/16",".203",".218",".234","1/4",".265",".281",".296",
  "5/16",".328",".343",".359","3/8",".390",".406",".421","7/16",".453",
  ".468",".484","1/2",".515",".531",".546","9/16",".578",".593",".609",
  "5/8",".640",".656",".671",".687",".703",".718",".734","3/4",".765",
  ".781",".796",".812",".828",".843",".859","7/8",".890",".906",".921",
  ".937",".953",".968",".984","1.0","1.01","1.03","1.04","1.06","1.07",
  "1.09","1.10","1.12","1.14","1.15","1.17","1.18","1.20","1.21","1.23",
  "1.25","1.26","1.28","1.29","1.31","1.32","1.34","1.35","1.37","1.39",
  "1.40","1.42","1.43","1.45","1.46","1.48","1.5","1.51","1.53","1.54",
  "1.56","1.57","1.59","1.60","1.62","1.64","1.65","1.67","1.68","1.70",
  "1.71","1.73","1.75","1.76","1.78","1.79","1.81","1.82","1.84","1.85",
  "1.87","1.89","1.90","1.92","1.93","1.95","1.96","2.0"};

// DPRO DENS (and SWAVE ENS) chord intervals, OFF in the middle.
static const MnmItem kMnmInterval[] = {
  "-12","-11","-10","-09","-08","2/3","-07","-06","-05","3/4","-04",
  "4/5","5/6","-03","-02","-01","OFF","+01","+02","+03","6/5","5/4",
  "+04","4/3","+05","+06","+07","3/2","+08","+09","+10","+11","+12"};
#define MNM_INTERVAL_OFF 16

static const MnmItem kMnmPhoneme[] = {
  "-","B","D","F","G","H","J","K","L","M","N","P","R","RR","S","SJ",
  "T","TH","TJ","V","Z"};

// LFO — hardware order, which is the CC value order.
static const MnmItem kMnmLfoPageName[] = {"PTCH","SYNT","AMP","FILT","EFFX",
                                          "LFO1","LFO2","LFO3","MIDI"};
static const MnmItem kMnmLfoTrig[] = {"FREE","TRIG","HOLD","ONE","HALF"};
static const MnmItem kMnmLfoWave[] = {"TRI","ITRI","SAW","ISAW","SQR","ISQR",
                                      "EXP","IEXP","RMP","IRMP","RND"};
static const MnmItem kMnmLfoMult[] = {"1X","2X","4X","8X","16X","32X","64X"};
// DEST's meaning depends on the same LFO's PAGE; the sketch names it. These
// placeholders only give the list its size (eight destinations per page).
static const MnmItem kMnmLfoDest[] = {"1","2","3","4","5","6","7","8"};
static const MnmItem kMnmPtchDest[] = {"1/12","2/12","7/12","10CT","20CT",
                                       "40CT","80CT","160CT"};
static const MnmItem kMnmMidiDest[] = {"LEN","VEL","PB","PCHG",
                                       "CC1","CC2","CC3","CC4"};

enum MnmListId : uint8_t {
  ML_ONOFF, ML_SIDWAVE, ML_SIDMOD, ML_SIDMSRC, ML_DPROSYNC, ML_FMRATIO,
  ML_DYNRATIO, ML_INTERVAL, ML_PHONEME, ML_LFOPAGE, ML_LFOTRIG, ML_LFOWAVE,
  ML_LFOMULT, ML_LFODEST, ML_PTCHDEST, ML_MIDIDEST, ML_COUNT };

struct MnmList { const MnmItem* items; uint8_t n; };
#define MNM_L(a) { a, (uint8_t)(sizeof(a) / sizeof(a[0])) }
static const MnmList kMnmLists[ML_COUNT] = {
  MNM_L(kMnmOnOff), MNM_L(kMnmSidWave), MNM_L(kMnmSidMod), MNM_L(kMnmSidMsrc),
  MNM_L(kMnmDproSync), MNM_L(kMnmFmRatio), MNM_L(kMnmDynRatio),
  MNM_L(kMnmInterval), MNM_L(kMnmPhoneme), MNM_L(kMnmLfoPageName),
  MNM_L(kMnmLfoTrig), MNM_L(kMnmLfoWave), MNM_L(kMnmLfoMult),
  MNM_L(kMnmLfoDest), MNM_L(kMnmPtchDest), MNM_L(kMnmMidiDest) };
#undef MNM_L

enum MnmSlotId : uint8_t { MS_S32, MS_D64 };        // S01-S32, D01-D64
static const char    kMnmSlotPrefix[] = {'S', 'D'};
static const uint8_t kMnmSlotCount[]  = {32, 64};

// ---------------------------------------------------------------- params
// def is the init-kit default: the raw wire value for U7/S7 (bipolar 0 ->
// 64), the list INDEX for LIST/SLOT. mnmDefaultRaw() gives what to send.
struct MnmParam { char name[5]; uint8_t kind; uint8_t def; uint8_t arg; };

#define P_NONE           {"-",  MNK_NONE, 0,        0}
#define P_U(n, d)        {n,    MNK_U7,   d,        0}
#define P_S(n, d)        {n,    MNK_S7,   (uint8_t)((d) + 64), 0}
#define P_L(n, l, d)     {n,    MNK_LIST, d,        l}
#define P_ON(n, d)       {n,    MNK_LIST, d,        ML_ONOFF}
#define P_SLOT(n, s)     {n,    MNK_SLOT, 0,        s}
#define P_TUNE           P_S("TUNE", 0)

// Track pages, slot order = encoder order = CC order.
static const MnmParam kMnmAmpP[8] = {
  P_U("ATCK", 0), P_U("HOLD", 0), P_U("DEC", 64), P_U("REL", 64),
  P_S("DIST", 0), P_U("VOL", 64), P_S("PAN", 0),  P_U("PORT", 0) };

static const MnmParam kMnmFiltP[8] = {
  P_U("BASE", 0), P_U("WDTH", 127), P_U("HPQ", 0), P_U("LPQ", 0),
  P_U("ATCK", 0), P_U("DEC", 32),   P_S("BOFS", 0), P_S("WOFS", 0) };

// EQG is written 0-127 with default 0 on the survey, kept as written.
static const MnmParam kMnmEffxP[8] = {
  P_U("EQF", 64), P_U("EQG", 0),   P_U("SRR", 0),  P_U("DTIM", 64),
  P_S("DSND", 0), P_U("DFB", 28),  P_U("DBAS", 0), P_U("DWID", 127) };

// All three Monomachine LFOs share this layout (CC 88+i / 104+i / 112+i).
// PAGE/DEST/TRIG/WAVE/MULT defaults were not on the survey: index 0.
static const MnmParam kMnmLfoP[8] = {
  P_L("PAGE", ML_LFOPAGE, 0), P_L("DEST", ML_LFODEST, 0),
  P_L("TRIG", ML_LFOTRIG, 0), P_L("WAVE", ML_LFOWAVE, 0),
  P_L("MULT", ML_LFOMULT, 0), P_U("SPD", 64), P_U("INTL", 0), P_U("DPTH", 0) };

// ---------------------------------------------------------------- machines
// Machine id in the sketch = index here + 1; 0 means "no machine chosen".
struct MnmMachine { char family[6]; char name[5]; char label[9]; MnmParam p[8]; };

static const MnmMachine kMnmMachines[] = {
  {"GND", "GND", "GND-GND", {P_NONE, P_NONE, P_NONE, P_NONE,
                             P_NONE, P_NONE, P_NONE, P_NONE}},
  {"GND", "SIN", "GND-SIN", {P_NONE, P_NONE, P_NONE, P_NONE,
                             P_NONE, P_NONE, P_NONE, P_TUNE}},
  {"GND", "NOIS","GND-NOIS",{P_U("ST", 0), P_U("RED", 0), P_ON("STON", 1), P_NONE,
                             P_NONE, P_NONE, P_NONE, P_TUNE}},

  {"SID", "6581","SID-6581",{P_U("PW", 0), P_U("PWAD", 0), P_ON("PWRS", 1),
                             P_L("WAVE", ML_SIDWAVE, 0),
                             P_L("MOD", ML_SIDMOD, 0), P_L("MSRC", ML_SIDMSRC, 0),
                             P_S("MFRQ", 0), P_TUNE}},

  {"SWAVE","SAW", "SW-SAW", {P_U("UNIL", 0), P_U("UNIW", 0), P_U("UNIX", 0), P_NONE,
                             P_U("SUBX", 0), P_U("SUB1", 0), P_U("SUB2", 0), P_TUNE}},
  {"SWAVE","PULS","SW-PULSE",{P_U("UNIL", 0), P_U("UNIW", 0), P_U("SUB1", 0), P_U("SUB2", 0),
                             P_S("PW", 0), P_U("PWAD", 0), P_ON("PWRS", 0), P_TUNE}},
  // PCH2-4 values were not written for ENS; assumed to match DPRO DENS.
  {"SWAVE","ENS", "SW-ENS", {P_L("PCH2", ML_INTERVAL, MNM_INTERVAL_OFF),
                             P_L("PCH3", ML_INTERVAL, MNM_INTERVAL_OFF),
                             P_L("PCH4", ML_INTERVAL, MNM_INTERVAL_OFF), P_U("WAVE", 0),
                             P_S("PW", 0), P_U("CHM", 0), P_U("CHRW", 127), P_TUNE}},

  // FM+ ratio defaults: 1/2 = idx 11, 1 = idx 15, 2 = idx 19.
  {"FM+", "STAT","FM+STAT", {P_L("1FRQ", ML_FMRATIO, 11), P_S("1FIN", 0),
                             P_U("1ENV", 80), P_U("1FB", 30),
                             P_L("2FRQ", ML_FMRATIO, 15), P_U("2VOL", 64),
                             P_U("TONE", 98), P_TUNE}},
  {"FM+", "PAR", "FM+PAR",  {P_L("1FRQ", ML_FMRATIO, 11), P_U("1FIN", 64),
                             P_L("2FRQ", ML_FMRATIO, 15), P_U("2ENV", 64),
                             P_L("3FRQ", ML_FMRATIO, 19), P_U("3ENV", 80),
                             P_U("TONE", 98), P_TUNE}},
  // 2FRQ written (1.33) — not in the list (1.32 / 1.34); raw 85 = "1.32".
  // 2FB written (30) or (32), unclear; 32 used.
  {"FM+", "DYN", "FM+DYN",  {P_L("1FRQ", ML_DYNRATIO, 64), P_S("1FEN", 0),
                             P_U("1VOL", 64), P_S("1VEN", 0),
                             P_L("2FRQ", ML_DYNRATIO, 85), P_U("2ENV", 80),
                             P_U("2FB", 32), P_TUNE}},

  {"VO-6", "VO6", "VO-6",   {P_U("VOC1", 64), P_U("VOC2", 64), P_ON("V-SW", 1),
                             P_U("VOIC", 0), P_L("CONS", ML_PHONEME, 0),
                             P_U("CLEN", 64), P_U("CVOL", 64), P_TUNE}},

  {"DPRO", "WAVE","DP-WAVE",{P_SLOT("WAVE", MS_S32), P_U("WP", 0), P_U("WPM", 0),
                             P_ON("WPRS", 1), P_L("SYNC", ML_DPROSYNC, 0),
                             P_U("SFRQ", 0), P_NONE, P_TUNE}},
  {"DPRO", "BBOX","DP-BBOX",{P_U("PTCH", 64), P_U("STRT", 0), P_NONE, P_NONE,
                             P_U("RTGR", 0), P_U("RTIM", 0), P_NONE, P_NONE}},
  {"DPRO", "DDRW","DP-DDRW",{P_SLOT("WAV1", MS_D64), P_S("MIX", 0),
                             P_SLOT("WAV2", MS_D64), P_U("TIME", 0),
                             P_U("BR1", 0), P_U("WID", 0), P_U("BR2", 0), P_TUNE}},
  {"DPRO", "DENS","DP-DENS",{P_L("PCH2", ML_INTERVAL, MNM_INTERVAL_OFF),
                             P_L("PCH3", ML_INTERVAL, MNM_INTERVAL_OFF),
                             P_L("PCH4", ML_INTERVAL, MNM_INTERVAL_OFF),
                             P_SLOT("WAVE", MS_D64),
                             P_NONE, P_U("CHM", 0), P_U("CHRW", 0), P_TUNE}},

  {"FX", "THRU", "FX-THRU", {P_NONE, P_NONE, P_NONE, P_NONE,
                             P_NONE, P_NONE, P_NONE, P_U("INP", 64)}},
  {"FX", "REV",  "FX-REV",  {P_U("DEC", 64), P_U("DAMP", 0), P_U("GATE", 127), P_U("MIX", 32),
                             P_U("HP", 0), P_U("LP", 127), P_NONE, P_U("INP", 64)}},
  {"FX", "CHOR", "FX-CHOR", {P_U("DEL", 64), P_U("DEP", 64), P_U("SPD", 64), P_U("MIX", 127),
                             P_U("FB", 0), P_U("WID", 0), P_U("LP", 127), P_S("INP", 0)}},
  {"FX", "DYNX", "FX-DYNX", {P_U("ATK", 64), P_U("REL", 64), P_U("THRS", 64), P_U("MIX", 127),
                             P_U("RAT", 0), P_U("GAIN", 0), P_U("RMS", 0), P_S("INP", 0)}},
  {"FX", "RING", "FX-RING", {P_U("WAVE", 0), P_U("EXT", 0), P_NONE, P_U("MIX", 0),
                             P_NONE, P_NONE, P_NONE, P_S("INP", 0)}},
  {"FX", "PHAS", "FX-PHAS", {P_S("CNTR", 0), P_U("DEP", 64), P_U("SPD", 64), P_U("MIX", 127),
                             P_S("FB", -19), P_U("WID", 0), P_NONE, P_S("INP", 0)}},
  {"FX", "FLNG", "FX-FLNG", {P_U("DEL", 64), P_U("DEP", 64), P_U("SPD", 64), P_U("MIX", 127),
                             P_S("FB", -19), P_U("WID", 0), P_NONE, P_S("INP", 0)}},
};
#define MNM_MACHINE_COUNT ((uint8_t)(sizeof(kMnmMachines) / sizeof(kMnmMachines[0])))

#undef P_NONE
#undef P_U
#undef P_S
#undef P_L
#undef P_ON
#undef P_SLOT
#undef P_TUNE

// ---------------------------------------------------------------- helpers

// raw CC value -> list index (see MNM_LIST_SPREAD at the top).
static inline uint8_t mnmListIndex(uint8_t raw, uint8_t n) {
  raw &= 0x7F;
#if MNM_LIST_SPREAD
  if (n >= 128) return raw;
  return (uint8_t)(((uint16_t)raw * n) >> 7);
#else
  return raw < n ? raw : (uint8_t)(n - 1);
#endif
}

// list index -> raw CC value to send (inverse of the above).
static inline uint8_t mnmListRaw(uint8_t idx, uint8_t n) {
  if (idx >= n) idx = (uint8_t)(n - 1);
#if MNM_LIST_SPREAD
  if (n >= 128) return idx;
  return (uint8_t)((((uint16_t)idx << 7) + n - 1) / n);      // first raw of the bin
#else
  return idx;
#endif
}

// Number of entries behind a list/slot parameter, 0 for the plain kinds.
static inline uint8_t mnmParamCount(const MnmParam& p) {
  if (p.kind == MNK_LIST) return kMnmLists[p.arg].n;
  if (p.kind == MNK_SLOT) return kMnmSlotCount[p.arg];
  return 0;
}

// Highest raw value that means something for this parameter.
static inline uint8_t mnmRawMax(const MnmParam& p) {
  const uint8_t n = mnmParamCount(p);
  return n ? mnmListRaw((uint8_t)(n - 1), n) : (uint8_t)127;
}

// Init-kit default as the raw CC value to send.
static inline uint8_t mnmDefaultRaw(const MnmParam& p) {
  const uint8_t n = mnmParamCount(p);
  return n ? mnmListRaw(p.def, n) : p.def;
}

// Display text for one parameter value: at most 5 characters (list items
// are at most 4 apart from the PTCH-page LFO destinations). out >= 6 bytes.
static inline void mnmFormat(const MnmParam& p, uint8_t raw, char* out) {
  raw &= 0x7F;
  switch (p.kind) {
    case MNK_U7: case MNK_S7: {
      int v = (p.kind == MNK_S7) ? (int)raw - 64 : (int)raw;
      if (v < 0) { *out++ = '-'; v = -v; }
      char t[3]; uint8_t i = 0;
      do { t[i++] = (char)('0' + v % 10); v /= 10; } while (v);
      while (i) *out++ = t[--i];
      *out = 0;
      return;
    }
    case MNK_LIST: {
      const MnmList& l = kMnmLists[p.arg];
      memcpy(out, l.items[mnmListIndex(raw, l.n)], sizeof(MnmItem));
      return;
    }
    case MNK_SLOT: {
      const uint8_t s = (uint8_t)(mnmListIndex(raw, kMnmSlotCount[p.arg]) + 1);
      out[0] = kMnmSlotPrefix[p.arg];
      out[1] = (char)('0' + s / 10);
      out[2] = (char)('0' + s % 10);
      out[3] = 0;
      return;
    }
    default:
      out[0] = '-'; out[1] = 0;
      return;
  }
}

// Machine by id (1-based; 0 or out of range = none).
static inline const MnmMachine* mnmMachine(uint8_t id) {
  return (id >= 1 && id <= MNM_MACHINE_COUNT) ? &kMnmMachines[id - 1] : 0;
}
static inline const char* mnmMachineLabel(uint8_t id) {
  const MnmMachine* m = mnmMachine(id);
  return m ? m->label : "----";
}

// Machine id by family + name (e.g. "FM+", "DYN"), family may be null.
// Returns 0 when not found.
static inline uint8_t mnmFindMachine(const char* family, const char* name) {
  for (uint8_t i = 0; i < MNM_MACHINE_COUNT; i++) {
    const MnmMachine& m = kMnmMachines[i];
    if (family && strcmp(family, m.family)) continue;
    if (!strcmp(name, m.name)) return (uint8_t)(i + 1);
  }
  return 0;
}

// Forward declarations for functions defined further down. Only primitives in
// the signatures, so the Arduino IDE's prototype-hoist quirk cannot bite us:
// the hoisted prototypes land above every type this sketch defines.
static void patTick(uint64_t pos, uint32_t nowUs, uint32_t beatUs, bool running);
static void patSilenceAll();
static bool patHandleCommand(const char* c);
// v1.11 scenes. The scene tables need the Monomachine page enums and the LFO
// engine, both of which live well below the pattern generator, so the two
// halves meet through these.
static void patGenScene(uint8_t genre);
static bool patSceneBase(uint8_t t, uint8_t page, uint8_t slot, uint8_t* out);
static bool sceneDefaults(uint8_t genre, uint8_t* root, uint8_t* scale, uint8_t* bars);
static bool sceneApply();
static void scenePrintSheet();
static uint8_t mnmCC(uint8_t page, uint8_t slot);
static const char* mnmParamName(uint8_t page, uint8_t slot);
// v1.22 value map (see "MONOMACHINE VALUE MAP"). The LFO engine clamps its
// output to a parameter's real range and is compiled before the map.
static uint8_t paramRawMax(uint8_t page, uint8_t slot, uint8_t track);
// v1.23: the pattern generator's locks tell the kit model what they sent.
static void kitSet(uint8_t track, uint8_t page, uint8_t slot, uint8_t val);
static void patPrintHelp();
static void pumpMidi();
static void midiTxService();
static bool turboHoldsWire();
static void turboNoteLiveByte(uint8_t b);
// Clears the channel-voice / SPP parser state. Called after a baud change,
// where a half-assembled message from the old speed is pure poison.
static void midiRxReset();
// Declared here so the Turbo MIDI state machine, which is defined well above
// the UI layer, can mark the screen stale the instant a speed changes.
static void uiTouch();
// v1.10: the joystick's current position (0..127, 64 = centre) for the PERF
// page, which is drawn long before the joystick code is defined. 0xFF = no
// joystick compiled in, so the page can leave the meters out entirely.
static uint8_t g_joyUiX = 0xFF, g_joyUiY = 0xFF;
static bool    g_joyShow = false;        // v1.14: meters wanted on screen?
static uint16_t g_joyCx = 0, g_joyCy = 0;       // calibrated centres, for 's'
static uint32_t g_joySent = 0;                  // CCs the stick has queued
static int16_t  g_joyRng[4] = {0, 0, 0, 0};     // learned lo/hi X, lo/hi Y

// ---------------------------------------------------------------------------
// USB SERIAL IS NOT FREE, AND IT IS NOT NON-BLOCKING
// ---------------------------------------------------------------------------
// usb_serial_write() in the Teensy core spins in `while (!tx_available)` for up
// to TX_TIMEOUT_MSEC - 120 milliseconds - whenever the host has stopped
// draining the port. A Serial Monitor that is open but scrolled, paused, or
// merely busy is enough. 120 ms at 10x turbo is 3750 received bytes, roughly
// four times the whole RX buffer, so ONE careless diagnostic print is a far
// bigger threat to the MIDI stream than anything else in this sketch. For
// scale: the two joystick analogReads below cost single-digit microseconds.
//
// So: never write to USB without checking there is room (usbReady), and where
// a long dump genuinely has to go out, wait for room while still servicing
// MIDI (usbWait) rather than handing the whole loop to the USB stack.
static inline bool usbReady(int n) { return Serial.availableForWrite() >= n; }
static inline void dbgPump() { pumpMidi(); midiTxService(); }
// v1.15 - HARDWARE WATCHDOG (RTWDOG / WDOG3).
// Clocked from the 32 kHz LPO through the /256 prescaler: 125 counts a second,
// so TOVAL 500 is 4 s. Enabled at the END of setup() (the boot delays are
// before it) and fed every pass of loop() and inside the only loops that can
// legitimately run long (usbWait, printDump). The worst normal loop pass is a
// display flush of ~9 ms, so 4 s is pure hang detection, never a false trip.
static bool g_wdtOn = false;
// Why THIS boot happened: SRC_SRSR captured and cleared first thing in setup().
// The register is sticky, so without the clear every cause since power-on
// would pile up and a watchdog recovery would be reported forever after.
static uint32_t g_resetSrsr = 0;
static const char* resetCause(uint32_t r) {
  if (r & SRC_SRSR_WDOG3_RST_B)        return "WATCHDOG - the firmware hung and recovered";
  if (r & SRC_SRSR_LOCKUP_SYSRESETREQ) return "software reset (firmware upload, or a CPU lockup)";
  if (r & SRC_SRSR_IPP_RESET_B)        return "power-on";
  return "other / unknown";
}
static inline void wdtFeed() { if (g_wdtOn) WDOG3_CNT = 0xB480A602u; }
static void wdtBegin(uint16_t timeoutMs) {
  const uint32_t counts = (uint32_t)timeoutMs * 125u / 1000u;
  CCM_CCGR5 |= CCM_CCGR5_WDOG3(3);
  __disable_irq();
  WDOG3_CNT = 0xD928C520u;                        // unlock
  while (!(WDOG3_CS & (1u << 11))) {}             // ULK
  WDOG3_TOVAL = counts > 0xFFFFu ? 0xFFFFu : counts;
  WDOG3_WIN   = 0;
  // UPDATE | EN | CLK=01 (LPO) | PRES /256 | CMD32EN
  WDOG3_CS = (1u << 5) | (1u << 7) | (1u << 8) | (1u << 12) | (1u << 13);
  __enable_irq();
  const uint32_t t0 = micros();
  while (!(WDOG3_CS & (1u << 10)) && (micros() - t0) < 2000u) {}   // RCS
  g_wdtOn = true;
  wdtFeed();
}

static void usbWait(int n) {
  const uint32_t t0 = millis();
  while (!usbReady(n)) {
    wdtFeed();
    dbgPump();                       // MIDI keeps flowing while we wait
    if ((uint32_t)(millis() - t0) > 50u) return;   // never wedge on a dead host
  }
}

// =============================================================================
// DEFERRED TURBO LOG  -  because the turbo path is not a safe place to print
// =============================================================================
//
// TurboMidi::onMessage() is reached from handleMidiByte(), which is called from
// inside flushAll() with /CS asserted. Every bare Serial.print on that path is
// exactly the 120 ms usb_serial_write stall this file warns about above: at
// 250000 baud that is three thousand received bytes, nearly three times the RX
// buffer, thrown away in the middle of the negotiation we are trying to log.
// It is also the precise mechanism by which "turbo killed the LFOs" - the clock
// stream dies while the handshake is being narrated.
//
// So the whole turbo layer now writes to tmSerial, which only APPENDS to this
// ring. loop() drains it through tmLogService(), and only ever writes as many
// bytes as Serial.availableForWrite() has already promised to take - so it can
// never enter the core's spin loop. A log line is worth exactly nothing next to
// a MIDI clock byte, so when the ring is full the line is dropped and counted.
static char     tmLog[2048];
static uint16_t tmLogHead = 0, tmLogTail = 0;
static uint32_t tmLogDrops = 0;

static void tmLogPut(const char* s) {
  if (!s) return;
  // Measure first. The old version copied until it ran out of room and then
  // returned, which leaves HALF A LINE in the ring - so the console shows a
  // truncated fragment welded to whatever is logged next, which is worse than
  // a clean gap because it reads as a real message. It also counted one drop
  // per partial write, under-reporting how much was actually lost.
  uint16_t len = 0;
  while (s[len]) len++;
  const uint16_t used = (uint16_t)((tmLogHead - tmLogTail) & 2047);
  if ((uint16_t)(2047 - used) < len) { tmLogDrops++; return; }   // all or nothing
  for (uint16_t i = 0; i < len; ++i) {
    tmLog[tmLogHead] = s[i];
    tmLogHead = (uint16_t)((tmLogHead + 1) & 2047);
  }
}

// Same call shapes as Serial, so the turbo layer reads unchanged. Swapping the
// object rather than every call site is what keeps this reviewable.
struct TmLogPort {
  // 320, not 200. The '*** INDEX MISMATCH ***' message is 203 characters of
  // format string BEFORE substitution, so at 200 vsnprintf silently cut it -
  // and the part it cut was the last line, the one that names the 'turbo b'
  // value to set. That message is the single most useful thing this firmware
  // can tell you during a turbo bring-up, and it was arriving beheaded.
  //
  // vsnprintf returns what it WANTED to write, so overflow is now visible
  // rather than silent.
  void printf(const char* fmt, ...) {
    char b[320];
    va_list ap; va_start(ap, fmt);
    const int want = vsnprintf(b, sizeof b, fmt, ap);
    va_end(ap);
    tmLogPut(b);
    if (want >= (int)sizeof b) tmLogPut("  [log line truncated]\n");
  }
  void print(const char* s)   { tmLogPut(s); }
  void println(const char* s) { tmLogPut(s); tmLogPut("\n"); }
  void println()              { tmLogPut("\n"); }
  // F() on a Teensy 4 is a plain pointer to the literal - there is no separate
  // program-memory address space on ARM - so this cast is safe here and lets
  // every existing F("...") call site stay exactly as it was.
  void print(const __FlashStringHelper* f)   { tmLogPut((const char*)f); }
  void println(const __FlashStringHelper* f) { tmLogPut((const char*)f);
                                               tmLogPut("\n"); }
};
static TmLogPort tmSerial;

// Drained from loop(). Writes only into space the USB stack has already
// reported, so this never blocks however dead the host is.
static void tmLogService() {
  while (tmLogTail != tmLogHead) {
    int room = Serial.availableForWrite();
    if (room < 16) return;
    while (room-- > 0 && tmLogTail != tmLogHead) {
      Serial.write(tmLog[tmLogTail]);
      tmLogTail = (uint16_t)((tmLogTail + 1) & 2047);
    }
  }
}

// =============================================================================
// TEENSY 4 UART FACTS THE PORTABLE API HIDES
// =============================================================================
//
// Serial1 on a Teensy 4.0 is LPUART6 (pins 0/1).
//
// 1. availableForWrite() reports space in the SOFTWARE ring ONLY. When it says
//    the buffer is empty there can still be four bytes in the LPUART's hardware
//    TX FIFO plus one more in the shift register - five byte times, 1600 us at
//    31250 baud. The previous code waited a fixed FOUR byte times (1280 us)
//    after that check and then called begin(), which disables the transmitter
//    and truncates whatever was still on the pin.
//
//    LPUART_STAT_TC is the hardware's own "FIFO and shifter are both drained"
//    flag. Reading it is exact and costs nothing, and polling it does not block
//    the way flush() does.
//
// 2. begin() resets the software ring indices but leaves the four-deep hardware
//    RX FIFO and the sticky error flags alone, so up to four bytes captured at
//    the OLD baud are handed to the parser afterwards as though they were valid
//    MIDI at the new one. Flush them explicitly.
//
// Note for the record: the baud GENERATOR is not a suspect. Teensyduino clocks
// the LPUART from 24 MHz and searches OSR 4..32 x SBR 1..8191, which lands
// 250000 and 500000 exactly, 312500 within 0.26%, and the worst case in the
// whole turbo table (625000) within 1.05%. All are far inside 8N1 tolerance.
#if defined(__IMXRT1062__)
  #define TM_LPUART IMXRT_LPUART6
  static inline bool tmTxIdle() { return (TM_LPUART.STAT & LPUART_STAT_TC) != 0; }
  static inline void tmPortRestart(uint32_t baud) {
    Serial1.begin(baud);
    TM_LPUART.FIFO |= LPUART_FIFO_RXFLUSH | LPUART_FIFO_TXFLUSH;
    // STAT bits 25..29 are configuration, not flags. Preserve them and write a
    // one only into the error flags we actually mean to clear.
    const uint32_t cfg = TM_LPUART.STAT & 0x3E000000u;
    TM_LPUART.STAT = cfg | LPUART_STAT_OR | LPUART_STAT_FE |
                           LPUART_STAT_NF | LPUART_STAT_PF;
    while (Serial1.available()) (void)Serial1.read();
  }
#else
  static inline bool tmTxIdle() { return true; }
  static inline void tmPortRestart(uint32_t baud) {
    Serial1.begin(baud);
    while (Serial1.available()) (void)Serial1.read();
  }
#endif

// ============================== configuration ================================

// Usable width of the panel, in logical pixels (the short axis, max 64).
//
// A RUNTIME value, not a #define, because it is the one number in the whole OS
// you have to find by looking at the panel. The pages lay themselves out from
// it every frame, so you can dial it in live:
//
//     disp            draw the ruler, read off the largest number still visible
//     uiw <n>         set it (8..64) and go straight back to the page
//
// It starts at 64 — the correct value for a healthy ER-OLEDM3.12. If yours
// truncates, work through the display-init commands (mux / stl / offs / rmap /
// rows) before accepting a smaller number; a panel lighting only 34 of 64 rows
// is almost always an init or wiring fault, and every pixel you give up here is
// a pixel of UI you never get back.
static int16_t UI_W = 64;

#define ACTIVE_LFOS               6
// 1 = rotate the image 180 degrees. This is a true rotation, not a mirror:
// px() maps logical (x,y) to panel (y, 63-x) normally and to (255-y, x) here,
// and rotating the logical plane by 180 - (x,y) -> (63-x, 255-y) - then applying
// the normal map gives exactly (255-y, x). So text stays readable rather than
// coming out backwards, which a horizontal flip would do.
//
// Only the DISPLAY turns. The encoders, the joystick and the LEDs are physical
// objects that did not move, so encoder 1 is still encoder 1. If you flipped the
// panel because the whole enclosure is the other way up, you probably also want
// the knob order and the joystick axes reversed - say so and that is a separate
// change, not this one.
#define ROTATE_FLIP               1   // 1 = image rotated 180 in the enclosure

// 0xA0 rather than 0x80. The pages no longer paint solid white chips, which
// were both the least legible thing on the panel and the largest single draw
// on the 3V3 rail, so there is headroom to run the strokes brighter. Still far
// inside the 250 mA budget: a typical page lights well under 8% of the pixels.
#define OLED_CONTRAST          0xA0
// THE SPLASH USED TO BE THE LARGEST ELECTRICAL LOAD THIS BOX EVER PRESENTED.
// Measured against the host renderer: every normal page lights 15-30% of the
// panel's total possible output, and the old white-field splash lit 96-100% of
// it - every one of the 16384 pixels at full shade - and held that for 2.7
// seconds. The contrast above was raised from 0x80 to 0xA0 on the stated
// grounds that "a typical page lights well under 8% of the pixels", which is an
// assumption that splash broke by more than an order of magnitude, on a unit
// whose 3V3 LDO is the binding limit rather than USB.
//
// Reversing the splash retired that problem rather than managing it: it now
// lights about 4%, which is a quarter of the quietest ordinary page. There is
// no boot-time contrast crutch any more because there is nothing left to
// protect against - see drawBoot.
// -----------------------------------------------------------------------------
// REVERSED VIDEO - black ink on a lit field, for the whole OS
// -----------------------------------------------------------------------------
// Applied ONCE, at the flush, as a single XOR on the way to the panel. The
// framebuffer stays in its normal light-on-dark space, so every draw function,
// every shade constant and the row diff in flushAll() are all completely
// unaware of it - which is the only version of this worth having. Doing it in
// px() instead would mean auditing every SH_* use in eight thousand lines and
// would still leave get() reading back the wrong sense.
//
// The XOR is exact rather than approximate: the framebuffer is two 4-bit pixels
// per byte, and for nibbles a,b the byte ((15-a)<<4)|(15-b) IS ((a<<4)|b) ^ 0xFF,
// because 15-x == x^15 for any 4-bit x. One instruction per byte, ~8 KB a frame.
//
// OFF. The pages are lit ink on black, which is what they were designed as and
// what the shade ramp above is tuned for. The switch stays because it is one
// XOR and it answers a question no screenshot can ("which way round does this
// panel actually read?"), but it is not the shipped look - only the SPLASH is
// reversed now, and that is done in drawBoot rather than here. See the note at
// the top of drawBoot for why the splash could not simply be run through this.
//
// READ THE POWER NOTE BY OLED_CONTRAST_FIELD BEFORE TURNING THIS ON.
static bool uiInvert = false;

// Contrast to use whenever THE BACKGROUND IS THE LIT THING - reversed video on
// an ordinary page, or the un-reversed boot splash. Measured on the host
// renderer, a normal page lights 15-30% of the panel's total possible output
// and its reverse lights 70-85%: roughly four times the load, permanently, on a
// unit whose 3V3 LDO is the binding limit rather than USB.
//
// A lit field does not need anything like the drive that sparse lit strokes do,
// though - there is nothing darker on screen to be read against - so reversed
// video runs at this instead of uiContrast and lands back near the ordinary
// page's current draw. Raise it toward OLED_CONTRAST if your rail turns out to
// have the headroom; that is a measurement, not a preference.
#define OLED_CONTRAST_FIELD    0x50

// The live value, so the settings page can move it. INIT_SEQ still uses the
// macro because a const array initialiser cannot read a variable.
static uint8_t uiContrast = OLED_CONTRAST;

// ---- Turbo MIDI (v1.19: the initiator - see the Turbo MIDI section) ---------
// The two speeds the 0x12 request carries, as codes into kTmSpeeds[]:
//   1 = 1x 31250   7 = 8x 250000   8 = 10x 312500
// SPEED1 is where the link is TESTED, SPEED2 where it RUNS - tested faster
// than it runs, so it goes live with margin.
#define TURBO_SPEED1_CODE         8   // 10x
#define TURBO_SPEED2_CODE         7   //  8x
// Longest wait for any one reply, and for the UART to drain before a switch.
#define TURBO_STEP_TIMEOUT_MS   250
// v1.20: no echo to the first speed test within this long, send the test again
// (pad and all) - up to this many tests inside TURBO_STEP_TIMEOUT_MS. A
// machine that is slower to change speed than the 16-byte pad allows misses
// the first one.
#define TURBO_TEST_RETRY_MS      60
#define TURBO_TEST_TRIES          4
// After the final switch to SPEED2, this long with nothing transmitted before
// ordinary MIDI may go out.
#define TURBO_SETTLE_US       10000
// Active Sensing (FE) is the TurboMIDI keepalive: sent every this many ms for
// as long as the link is up, and stopping it is how the machine learns the
// link is gone.
#define TURBO_KEEPALIVE_MS      150
// Once locked: revert if received traffic stops parsing for a whole window
// of this long (the machine is at another speed)...
#define TURBO_WATCHDOG_MS      4000
// ...or if the machine, having sent its own FE keepalive, goes silent this
// long (four of its keepalives missed).
#define TURBO_PEER_SILENT_MS    600
// After reverting to 31250, hold the wire this long before ordinary MIDI
// resumes - past the machine's own active-sensing timeout (300 ms), so it is
// back at 1x before we speak to it at 1x.
#define TURBO_REVERT_HOLD_MS    400

// ---- PERF page switches -----------------------------------------------------
// Hoisted here from the page body because drawTurboBadgeBig() is defined ~350
// lines ABOVE the page and is now conditional on them. A #define that sits
// below its own #if is not a switch, it is a silent default.
// 1 = capsules run the full 24..127 / 129..231 of the reference render.
// 0 = the old 70-row capsules with the TURBO badge in the gap between banks.
#define PERF_FULL_FADERS   1

// Shown on the boot screen and by the console. One place, so it cannot drift.
#define XY6_VERSION              "1.24"

// ---- Joystick (A0 / A1) -----------------------------------------------------
// v1.17: the stick sends X and Y to every track picked on the JOY page (PERF
// sub-page), each on its own channel - the live base channel + track.
#define JOYSTICK_ENABLED   1        // v1.10: on - it is a MIDI joystick
// Axis mapping. Both pots read DOWNWARD as the stick is pushed toward its
// positive end, so both are inverted. Which pot is called X and which Y is set
// here and nowhere else - the meters, the CC numbers and the calibration all
// follow these four lines. Change these, not the code below, if a different
// stick or mounting ever disagrees.
#define JOY_PIN_X         15        // A1  - physical left/right
#define JOY_PIN_Y         14        // A0  - physical up/down
#define JOY_INVERT_X       1        // 1 = reading falls as the stick goes right
#define JOY_INVERT_Y       1        // 1 = reading falls as the stick goes up
// First-boot routing; after that the JOY page and the globals own it. The
// destinations are entries of the flat list DEST uses: 16 / 17 are FILT BASE
// and FILT WDTH (CC 72 / 73) - a filter sweep on one hand.
#define JOY_DEST_X_DEF    16
#define JOY_DEST_Y_DEF    17
#define JOY_MASK_DEF    0x01        // bit n = track n+1 under the stick; 0x3F all
// Most bytes the stick may have waiting in the CC queue. It only tops the queue
// up to here, so anything queued behind it (LFOs, PERF, scenes) never waits
// more than ~15 ms of stick traffic, even with all six tracks moving.
#define JOY_Q_CAP         48
// ...unless a track has been waiting this long: then it goes regardless (the
// queue's own limit still holds), so an oversubscribed wire cannot starve it.
#define JOY_LATE_US    40000
// And a byte budget: the stick may use at most JOY_BUDGET_BPS of the wire
// (3125 B/s at standard MIDI), in bursts of at most JOY_BURST_B. One to three
// tracks never reach it (125 updates a second each); all six share it at
// ~65 a second each, and the UART never holds more than a few ms of stick
// traffic in front of a pattern note.
#define JOY_BUDGET_BPS  2000
#define JOY_BURST_B       20
#define JOY_ACT_MS       120        // JOY page: a track's send light stays on
#define JOY_DEADZONE      30        // in raw ADC counts, as in your v1.0 sketch
// Schmitt hysteresis on the deadzone edge. You leave the centre at
// JOY_DEADZONE and only fall back into it at JOY_DEADZONE - JOY_HYST, so a
// stick resting exactly on the boundary cannot dither across it.
#define JOY_HYST           8
// v1.10: outer deadzone. A real pot rarely reaches 0 or 1023 - wear, the
// stick's mechanical stop and the ADC reference all eat the last few counts -
// so the old mapping could leave the ends of the range unreachable. The last
// JOY_EDGE counts of travel now clamp to 0 / 127.
#define JOY_EDGE          16
// v1.10: travel is LEARNED per axis. A stick's mechanical stop usually arrives
// long before the pot's end - full throw is often only 150..870, not 0..1023 -
// so a mapping that assumes the whole ADC range never reaches 0 or 127 at the
// stop. Each axis starts believing its travel is centre +/- JOY_SPAN_INIT and
// widens that whenever the stick goes further, so after one push to each end
// the full throw maps exactly onto 0..127. Start SMALL: learning only widens,
// so an initial span bigger than the real travel could never be corrected.
#define JOY_SPAN_INIT    150
// v1.17: WHERE THE STICK RESTS. A cheap spring does not return to one spot:
// this stick comes back up to ~40 counts from where it sat at boot - outside
// JOY_DEADZONE - so "at rest" read 62/63 and, on a step boundary, flickered
// between two values, each flicker a CC to every track. Two cures:
//  * a stick held within JOY_REST_BAND of centre, still to JOY_STILL_CNT for
//    JOY_STILL_MS, IS at rest: that spot becomes the centre (never more than
//    JOY_DRIFT_MAX from the boot centre). Only positions within ~3 steps of
//    64 can be taken for rest, so a deliberate hold is never snapped back.
//  * JOY_RAW_HYST: the smoothed reading must move this many counts before the
//    mapping sees it - under half a step, so no resolution is lost, but ADC
//    noise on a step boundary no longer turns into a stream of 63/64/63.
#define JOY_REST_BAND     48
#define JOY_STILL_CNT      4
#define JOY_STILL_MS     300
#define JOY_DRIFT_MAX     80
#define JOY_RAW_HYST       3
// v1.18: SMOOTHING THAT DOES NOT LAG A REAL MOVE. The v1.17 filter was a plain
// one-pole at 1/4 per 8 ms pass, so a fast throw took ~64 ms to reach 90% of
// where the stick already was. Now each reading goes through a median of the
// last three first - a single spike (a worn wiper lifting, a noisy ground) is
// removed outright instead of being smeared into the average - and a median
// more than JOY_FAST_CNT counts from the filter is motion, not noise, so the
// filter follows it at 3/4 per pass: ~24 ms to 90%. Noise never gets near
// JOY_FAST_CNT, so a resting stick still sees the same 1/4 one-pole - only
// fed a median, which makes rest quieter, never busier. The rest centre, the
// deadzone and the send threshold are untouched. 0 = the v1.17 filter.
#define JOY_FAST_CNT      16
#define JOY_INTERVAL_US 8000        // 125 Hz ceiling
// v1.14: the PERF page's X/Y meters hide themselves when the stick is idle.
// Shown while the stick is off centre or has moved in the last JOY_HIDE_MS.
// (v1.24: they appear and go at once - the fades went with the Monomachine
// grid, whose screen snaps.)
#define JOY_HIDE_MS     3000

// v1.17: where the stick goes. Shared by the JOY page, the globals, the
// console and the joystick code, which is why it lives up here.
static uint8_t  g_joyMask  = JOY_MASK_DEF;      // bit n = track n+1
static uint8_t  g_joyDestX = JOY_DEST_X_DEF;    // flat destination index
static uint8_t  g_joyDestY = JOY_DEST_Y_DEF;
// What each track was last actually sent, per axis. 0xFF = nothing yet, and
// nothing goes until the stick really moves - so adding a track, or pointing
// an axis somewhere new, never overwrites the machine with a resting 64.
static uint8_t  g_joySentX[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t  g_joySentY[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint32_t g_joyTrkN[6]  = {0, 0, 0, 0, 0, 0};   // messages per track
static uint32_t g_joyActMs[6] = {0, 0, 0, 0, 0, 0};   // last send, for the page
static uint32_t g_joyWaits    = 0;   // times the queue cap made a track wait
static uint32_t g_joyLagMaxUs = 0;   // worst "due" -> "queued" delay seen
static uint32_t g_joySweepReq = 0;   // console: dry-run sweep of this many ms
// Result of the last dry-run sweep, for 'joy'.
static uint32_t g_joySwMs = 0, g_joySwBytes = 0, g_joySwWaits = 0, g_joySwLagUs = 0;
static uint32_t g_joySwN[6] = {0, 0, 0, 0, 0, 0};
static uint8_t  g_joySwMask = 0;
static int32_t  g_joySwTail = -1;    // would-be sends from the resting stick after it
static uint8_t  g_perfSub = 0;       // PERF sub-page shown: 0 DEST, 1 JOY

static void joyForget(bool x, bool y) {
  for (uint8_t t = 0; t < 6; ++t) {
    if (x) g_joySentX[t] = 0xFF;
    if (y) g_joySentY[t] = 0xFF;
  }
}

#define MNM_BASE_CHANNEL          1   // Monomachine base channel, 1..16
// The LIVE base channel (settings page / preset). Defined here, not with the
// rest of the MIDI state, because the pattern generator far above needs it.
static uint8_t txChannel = MNM_BASE_CHANNEL;
#define DEBUG_PRINT               1   // one-line USB status every 2 s

// ================================ pin map ====================================

static const uint8_t D_BUS[8] = {2, 3, 4, 5, 6, 9, 10, 11};
static const uint8_t PIN_WR = 12, PIN_DC = 16, PIN_CS = 17, PIN_RES = 20;
// OLED /RD is tied to 3.3 V on the board and is never driven.

static const int PANEL_W = 256, PANEL_H = 64;   // physical
static const int SCR_W   = 64,  SCR_H   = 256;  // logical, panel mounted rotated

// /WR low-pulse width in nops. 40 is the value that works on this panel. Lower
// gives speckle and torn bands.
static uint8_t WR_NOPS = 40;

static const uint8_t I2C_ADDR_MCP = 0x20;

struct EncPins { uint8_t a, b; };
static const EncPins PIN_ENC[6] = {
    {21, 22}, {23, 24}, {25, 26}, {27, 28}, {29, 30}, {31, 32}};

static const uint8_t I2C_ADDR_EE  = 0x50;   // 24LC512

// =============================================================================
// SECTION: KERNEL PRIMITIVES  (new in v7)
// =============================================================================
//
// The scheduler in loop() was already priority-ordered and non-blocking; these
// three pieces are what it was missing.
//
// Deadline replaces the ad-hoc `static uint32_t xNext` gates. Two differences
// that matter: it advances from the DEADLINE rather than from now, so a task
// delayed by a long frame does not permanently shift its own phase - and if it
// has fallen more than one period behind it RESYNCHRONISES instead of firing a
// catch-up burst. A stack of back-to-back LFO ticks after a stall is worse than
// one skipped tick, because the burst lands as a step on every destination.
struct Deadline {
  uint32_t next = 0;
  inline bool due(uint32_t now, uint32_t period) {
    if ((int32_t)(now - next) < 0) return false;
    next += period;
    if ((int32_t)(now - next) > (int32_t)period) next = now + period;
    return true;
  }
  inline void arm(uint32_t now, uint32_t period) { next = now + period; }
};

// -----------------------------------------------------------------------------
// THE HOLD / CLICK GESTURE DETECTOR      spec: "switch, pres 2 sec"
// -----------------------------------------------------------------------------
// This replaces the inline b6DownMs / b6Done block that used to live in loop().
// Same behaviour, plus a progress read-out for the on-screen hold bar.
//
// HALF A SECOND, NOT THE SPEC'S TWO (v1.16: was 1 s, halved on request). The
// quote in the heading is what the original
// sketch asked for and it is left there deliberately, because this is a
// considered deviation rather than a transcription error: two seconds is a long
// time to stand on a button you press dozens of times a session, and the hold
// bar means the gesture no longer has to be slow to be discoverable. Everything
// that reads a hold reads it from here - the detector, the progress bar, and
// the on-screen hints - so this number is the only thing to move if you want
// the spec's timing back.
//
// The rules, which are all about what must NOT happen:
//   * no CLICK on the press edge - a click is only knowable on release
//   * no CLICK on the release that ENDS a hold (fired_ is that latch; without
//     it every subpage you open advances the page the moment you let go)
//   * no edge at all from an I2C fault. buttonsScan() already suppresses edges
//     on a bad read; `valid` carries that through so a dropped transaction
//     cannot be read as a release.
//   * no edge from one glitched sample: two agreeing reads are required.
#define BTN_HOLD_MS     500u
#define BTN_DEBOUNCE_MS 5u
enum BtnGesture : uint8_t { BTN_NONE = 0, BTN_CLICK, BTN_HOLD };

class ButtonGesture {
 public:
  void begin() { stable_ = pending_ = fired_ = false; }
  uint8_t update(bool rawDown, bool valid, uint32_t nowMs) {
    if (!valid) return BTN_NONE;
    if (rawDown != pending_) { pending_ = rawDown; pendMs_ = nowMs; return BTN_NONE; }
    if (pending_ != stable_ && (uint32_t)(nowMs - pendMs_) >= BTN_DEBOUNCE_MS) {
      stable_ = pending_;
      if (stable_) { downMs_ = nowMs; fired_ = false; }
      else {
        const bool wasHold = fired_;
        fired_ = false;
        if (!wasHold) return BTN_CLICK;
        return BTN_NONE;                 // release that ended a hold: swallowed
      }
    }
    if (stable_ && !fired_ && (uint32_t)(nowMs - downMs_) >= BTN_HOLD_MS) {
      fired_ = true;                     // fires WHILE held, exactly once
      return BTN_HOLD;
    }
    return BTN_NONE;
  }
  bool isDown() const { return stable_; }
  uint8_t holdProgress(uint32_t nowMs) const {
    if (!stable_) return 0;
    if (fired_) return 255;
    const uint32_t d = nowMs - downMs_;
    return (d >= BTN_HOLD_MS) ? 255 : (uint8_t)((d * 255u) / BTN_HOLD_MS);
  }
 private:
  bool     stable_ = false, pending_ = false, fired_ = false;
  uint32_t downMs_ = 0, pendMs_ = 0;
};

// Worst-case pass time is the number that says whether the whole non-blocking
// argument is actually holding. It belongs next to the RX buffer high-water
// mark on the settings page: the two together are the entire health picture.
struct LoopStats {
  uint32_t passUsMax = 0, passUsAvg = 0, frameSkipsBacklog = 0;
  inline void note(uint32_t us) {
    if (us > passUsMax) passUsMax = us;
    passUsAvg += ((int32_t)us - (int32_t)passUsAvg) >> 6;
  }
};
static LoopStats gStats;

// Above this many bytes waiting in the Serial1 RX buffer the renderer stands
// down for a frame. 96 bytes is 30 ms of slack at 31250 and 3 ms at 10x - it
// trips early enough at turbo speeds to matter and late enough at base speed
// never to fire on ordinary clock traffic. Costs 33 ms of animation nobody was
// going to notice; buys the whole slice to a SysEx dump that everybody would.
#define MIDI_BACKLOG_YIELD 96

// -----------------------------------------------------------------------------
// ENCODER COARSE MODE      spec: "pressing encoder and rotating should be 7 values +-"
// -----------------------------------------------------------------------------
#define ENC_STEP_FINE    1
#define ENC_STEP_COARSE  7

// =============================================================================
// SECTION: PERSISTENT FORMAT  (24LC512, 64 KB, 128-byte pages)     new in v7
// =============================================================================
//
// This is what finally makes the settings page's SAVE and LOAD rows tell the
// truth instead of printing an apology.
//
// -----------------------------------------------------------------------------
// WHY THESE ARE NOT THE RUNTIME STRUCTS
// -----------------------------------------------------------------------------
// It is tempting to write LfoParams straight to EEPROM and be done. Do not. The
// runtime struct is free to grow a cached field, a scratch bool, something that
// means one thing only this session - and the moment it does, sizeof and member
// order change underneath a format that has no idea it is a format, and every
// preset ever saved is silently misparsed.
//
// So there are two families and exactly one function converts between them. The
// cost is a hundred lines of transcription. The return is that a preset saved
// by v7 still loads on v9, and that a change to working state can never corrupt
// storage.
//
// Everything here is `packed` for layout certainty AND ordered largest-first so
// the members land naturally aligned anyway - on a Cortex-M7 `packed` makes GCC
// emit byte-wise access for anything unaligned, so both, not one or the other.
//
// -----------------------------------------------------------------------------
// WHAT THE 24LC512 IMPOSES
// -----------------------------------------------------------------------------
//  1. A page write is at most 128 bytes AND MAY NOT CROSS A 128-BYTE BOUNDARY.
//     A crossing write does not fail - it WRAPS, silently overwriting the start
//     of the same page. The driver refuses a crossing write rather than
//     trusting the caller.
//  2. A write cycle takes up to 5 ms, during which the device NAKs everything.
//     That cannot be a blocking loop, so the driver is a state machine ticked
//     from loop() that polls for ACK between chunks.
//  3. No wear levelling and no ECC - hence a CRC on every record and a save
//     counter. A half-written preset is DETECTED and refused, not loaded as
//     garbage into six LFOs pointed at a filter cutoff.

static const uint32_t XY6_MAGIC_GLOBAL = 0x36595847UL;   // 'XY6G'
static const uint32_t XY6_MAGIC_PRESET = 0x36595850UL;   // 'XY6P'
static const uint16_t XY6_FORMAT_VER   = 0x0001;

static const uint16_t EE_PAGE_SIZE     = 128;
static const uint32_t EE_SIZE          = 65536UL;
static const uint16_t EE_ADDR_GLOBAL   = 0x0000;
static const uint16_t EE_PRESET_BASE   = 0x1000;
static const uint16_t EE_PRESET_STRIDE = 0x1000;         // 4096
static const uint8_t  EE_PRESET_SLOTS  = 15;             // 0x1000..0xFFFF

// Mirrors of the runtime dimensions. Literals here on purpose: the persistent
// layout must not silently change size because a runtime constant moved. The
// static_asserts further down are what keep the two in step.
static const uint8_t EE_TRACKS      = 6;
static const uint8_t EE_PARAM_SLOTS = 56;   // 7 pages x 8, per Appendix B
static const uint8_t EE_LFOS        = ACTIVE_LFOS;
static const uint8_t EE_LFO_STEPS   = 64;
static const uint8_t EE_PAT_TRACKS  = 6;
static const uint8_t EE_PAT_STEPS   = 64;

// Spec, wizard step 3: "sysex recieve, we have Each Engine placement and
// values" - per track, which machine is loaded and every parameter value. That
// is exactly the "before" state the LFO engine needs so it can swing AROUND the
// patch value instead of overwriting it.
struct __attribute__((packed)) TrackKitP {
  uint8_t machineId, level, flags, rsv;
  uint8_t param[EE_PARAM_SLOTS];
};
static_assert(sizeof(TrackKitP) == 60, "TrackKitP layout changed");

struct __attribute__((packed)) EncSlotP {
  uint8_t page, slot, track, value, flags, rsv;   // flags bit0: value is known
};
static_assert(sizeof(EncSlotP) == 6, "EncSlotP layout changed");

// One byte per step. Four bits carry the probability ladder index; four are
// held back deliberately, because the step grid is the part of this format most
// likely to grow and widening it inside the existing byte costs no slot space.
struct __attribute__((packed)) LfoStepP { uint8_t prob; };

struct __attribute__((packed)) LfoPresetP {
  uint8_t destTrack, destPage, destSlot, wave, trig, mult, spd, intl;
  uint8_t depth;        // spec: E3 on the LFO page
  uint8_t amount;       // spec: E4 on the LFO page
  uint8_t lo, hi, bars, baseValue;
  uint8_t stepCount;    // 8 or 16.  TODO: [UX DECISION] per-LFO, not global
  uint8_t flags;        // bit0: enabled (not bypassed)
  LfoStepP steps[EE_LFO_STEPS];
};
static_assert(sizeof(LfoPresetP) == 16 + EE_LFO_STEPS, "LfoPresetP layout changed");

// The generator is deterministic from (genre, seed, bars, root, scale), so the
// seed alone would restore it. The full grid is stored anyway, because Direct
// Play writes notes the generator did not produce - and a format that can
// represent a generated pattern but not a played one would quietly lose the
// take the moment someone used the feature.
struct __attribute__((packed)) PatStepP {
  uint8_t note, vel, len, flags;   // flags: b0 on, b1 accent, b2 slide, b3 ghost, b4-7 cond
};

struct __attribute__((packed)) PatternP {
  uint32_t seed;
  uint8_t  genre, bars, root, scale, trackOn, flags, rsv[2];
  PatStepP step[EE_PAT_TRACKS][EE_PAT_STEPS];
};

// crc covers everything AFTER the crc field. It is written LAST by the save
// state machine, so a preset interrupted by a power cut fails verification and
// is refused - the failure is "slot 3 did not save", never "slot 3 loaded
// something that was never a preset".
struct __attribute__((packed)) PresetHdrP {
  uint32_t magic;
  uint16_t version, size;
  uint32_t saveCount;
  uint16_t crc, rsv;
  char     name[12];
};
static_assert(sizeof(PresetHdrP) == 28, "PresetHdrP layout changed");

struct __attribute__((packed)) PresetP {
  PresetHdrP hdr;
  TrackKitP  kit[EE_TRACKS];
  EncSlotP   enc[6];
  LfoPresetP lfo[EE_LFOS];
  PatternP   pat;
};
static_assert(sizeof(PresetP) <= EE_PRESET_STRIDE, "PresetP no longer fits a slot");

struct __attribute__((packed)) GlobalCfgP {
  uint32_t magic;
  uint16_t version, size;
  uint32_t saveCount;
  uint16_t crc;
  uint8_t  lastPreset, baseChannel, txMode, perTrack, contrast, uiWidth, font;
  uint8_t  turboSpeedIdx;
  int8_t   turboBias;       // v1.19: unused (the speed codes are fixed), always 0
  uint8_t  flags;          // b0 wizard done, b1 reversed video, b2 chip style,
                           // b3 bits 1-2 were actually written by this firmware
  uint8_t  rsv[8];
};
static_assert(sizeof(GlobalCfgP) == 32, "GlobalCfgP layout changed");

// CRC-16/CCITT. Small, fast, and adequate for the only failure it guards
// against: a page torn by a power cut mid-save.
static uint16_t xy6Crc16(const uint8_t* p, uint16_t n) {
  uint16_t c = 0xFFFF;
  while (n--) {
    c ^= (uint16_t)(*p++) << 8;
    for (uint8_t i = 0; i < 8; ++i)
      c = (c & 0x8000) ? (uint16_t)((c << 1) ^ 0x1021) : (uint16_t)(c << 1);
  }
  return c;
}

// =============================================================================
// SECTION: 24LC512 DRIVER, NON-BLOCKING                            new in v7
// =============================================================================
//
// THE CHUNK SIZE IS 32 BYTES, NOT THE FULL 128-BYTE PAGE, and that is the one
// number here worth arguing about. A 128-byte page write is 130 bytes on the
// wire, and at 400 kHz that is 2.9 ms of BLOCKING I2C inside a loop whose whole
// budget is 1 ms - it would meet the letter of "non-blocking save" while
// breaking the rule this firmware exists to enforce. At 32 bytes a transfer is
// ~0.77 ms, safely inside budget, and 32 divides 128 so a chunk can never
// straddle a page boundary.
//
// The price is four times the write cycles: ~340 ms per preset instead of
// ~85 ms. A save you asked for taking a third of a second is invisible; a third
// of a millisecond of MIDI jitter is not.
static const uint16_t EE_CHUNK = 32;

enum EeState : uint8_t { EE_IDLE = 0, EE_BUSY, EE_DONE,
                         EE_ERR_NAK, EE_ERR_RANGE, EE_ERR_ABSENT };

struct HalEeprom {
  static uint8_t  st;
  static bool     writing, devicePresent;
  static uint16_t addr, len, done;
  static uint8_t* buf;
  static const uint8_t* src;
  static uint32_t waitUntilUs, ackDeadlineUs;
  static bool     waitingAck;

  // Probed once at boot. A box that cannot reach its storage must SAY so on the
  // panel rather than booting into a UI whose SAVE key silently does nothing.
  static void begin() {
    Wire.beginTransmission(I2C_ADDR_EE);
    Wire.write((uint8_t)0); Wire.write((uint8_t)0);
    devicePresent = (Wire.endTransmission() == 0);
    st = devicePresent ? EE_IDLE : EE_ERR_ABSENT;
  }
  static uint8_t  state()    { return st; }
  static bool     present()  { return devicePresent; }
  static uint16_t progress() { return done; }
  static bool     busy()     { return st == EE_BUSY; }
  static void     clear()    { if (st != EE_BUSY) st = devicePresent ? EE_IDLE
                                                                    : EE_ERR_ABSENT; }

  static bool beginRead(uint16_t a, uint8_t* dst, uint16_t n) {
    if (!devicePresent) { st = EE_ERR_ABSENT; return false; }
    if (st == EE_BUSY) return false;
    if ((uint32_t)a + n > EE_SIZE) { st = EE_ERR_RANGE; return false; }
    addr = a; buf = dst; src = nullptr; len = n; done = 0;
    writing = false; waitingAck = false; st = EE_BUSY;
    return true;
  }
  static bool beginWrite(uint16_t a, const uint8_t* s2, uint16_t n) {
    if (!devicePresent) { st = EE_ERR_ABSENT; return false; }
    if (st == EE_BUSY) return false;
    if ((uint32_t)a + n > EE_SIZE) { st = EE_ERR_RANGE; return false; }
    addr = a; src = s2; buf = nullptr; len = n; done = 0;
    writing = true; waitingAck = false; st = EE_BUSY;
    return true;
  }

  static void service(uint32_t nowUs) {
    if (st != EE_BUSY) return;

    if (waitingAck) {
      // Poll for the end of the internal write cycle. Each poll is one address
      // byte pair, ~25 us, and the cycle is up to 5 ms - about six polls spread
      // across six passes of the main loop, none of them blocking.
      if ((int32_t)(nowUs - waitUntilUs) < 0) return;
      Wire.beginTransmission(I2C_ADDR_EE);
      if (Wire.endTransmission() != 0) {
        // A write cycle on this part is 5 ms at worst. If it still has not
        // acknowledged after 50 ms it is not coming back - and polling it
        // forever is not a stall, it is a HANG: st stays EE_BUSY, so
        // Store::busy() stays true, so the first-run wizard sits on its SAVING
        // screen for ever and bootFinish() keeps turning round at the door.
        // Ten times the worst legitimate cycle, then call it what it is.
        if ((int32_t)(nowUs - ackDeadlineUs) >= 0) { st = EE_ERR_NAK; return; }
        waitUntilUs = nowUs + 500;
        return;
      }
      waitingAck = false;
      if (done >= len) { st = EE_DONE; return; }
    }

    const uint16_t a = (uint16_t)(addr + done);
    uint16_t n = (uint16_t)(len - done);
    if (n > EE_CHUNK) n = EE_CHUNK;
    // Never straddle a page boundary. 32 divides 128 so this only ever trims an
    // unaligned first chunk - but it is here because relying on the caller to
    // align is exactly how the silent wrap gets shipped.
    const uint16_t toPageEnd = (uint16_t)(EE_PAGE_SIZE - (a % EE_PAGE_SIZE));
    if (writing && n > toPageEnd) n = toPageEnd;

    if (writing) {
      Wire.beginTransmission(I2C_ADDR_EE);
      Wire.write((uint8_t)(a >> 8)); Wire.write((uint8_t)(a & 0xFF));
      for (uint16_t i = 0; i < n; ++i) Wire.write(src[done + i]);
      if (Wire.endTransmission() != 0) { st = EE_ERR_NAK; return; }
      done = (uint16_t)(done + n);
      waitingAck = true;
      waitUntilUs   = nowUs + 1000;      // do not poll before the cycle can end
      ackDeadlineUs = nowUs + 50000;     // and give up on it long before a user would
    } else {
      Wire.beginTransmission(I2C_ADDR_EE);
      Wire.write((uint8_t)(a >> 8)); Wire.write((uint8_t)(a & 0xFF));
      if (Wire.endTransmission(false) != 0) { st = EE_ERR_NAK; return; }
      const uint8_t got = (uint8_t)Wire.requestFrom((int)I2C_ADDR_EE, (int)n);
      if (got != n) { st = EE_ERR_NAK; return; }
      for (uint16_t i = 0; i < n; ++i) buf[done + i] = (uint8_t)Wire.read();
      done = (uint16_t)(done + n);
      if (done >= len) st = EE_DONE;
    }
    // A 32-byte transfer is ~0.77 ms of blocking I2C, and a full preset is 77
    // of them. Pumping on the way out costs nothing and keeps the receive path
    // serviced across a save - the same discipline flushAll() already follows.
    pumpMidi();
    midiTxService();
  }
};
uint8_t  HalEeprom::st = EE_IDLE;
bool     HalEeprom::writing = false;
bool     HalEeprom::devicePresent = false;
uint16_t HalEeprom::addr = 0, HalEeprom::len = 0, HalEeprom::done = 0;
uint8_t* HalEeprom::buf = nullptr;
const uint8_t* HalEeprom::src = nullptr;
uint32_t HalEeprom::waitUntilUs = 0, HalEeprom::ackDeadlineUs = 0;
bool     HalEeprom::waitingAck = false;

// =============================================================================
// SECTION: SYSTEM STATE MACHINE                                    new in v7
// =============================================================================
// Spec boot flow:
//   power on -> REDOT splash (dot getting bigger)
//            -> EEPROM has presets?  no  -> first-run wizard -> save -> Page 1
//                                     yes -> load last preset        -> Page 1
//
// SYS_FAULT is not in the specification and is here because a box that cannot
// reach its storage must say so rather than booting into a UI whose SAVE key
// silently does nothing.
enum SysState : uint8_t { SYS_SPLASH = 0, SYS_WIZARD, SYS_LOADING, SYS_RUN, SYS_FAULT };

// Spec: "1 REDOT" -> "2 machine select" -> "3 KIT sysex recieve, Animated Page"
//       -> "Preset SAVE".  WIZ_SYSEX_WAIT is the one state with an external
// dependency, so it needs a visible escape: a wizard you can only leave by
// pulling the USB cable is not a wizard.
enum WizState : uint8_t {
  WIZ_SPLASH = 0, WIZ_MACHINE_SELECT, WIZ_SYSEX_WAIT, WIZ_SYSEX_OK, WIZ_SAVE, WIZ_DONE
};

// Spec, LFO subpage, fourth encoder: "rotation changes step into Active 0, 10%,
// 20%, 33%, 50%, 66%, 75, 90, 100% activation". Nine detents, not a 0..100
// continuum - which is what lets a step cell be drawn as nine fill heights and
// what puts the encoder on musically useful ratios (33 and 66 are thirds, which
// a linear sweep makes you hunt for).
//
// Stored as the INDEX, never the percentage: if the ladder is ever re-tuned,
// old presets keep meaning "the notch the user chose".
enum StepProb : uint8_t {
  PROB_OFF = 0, PROB_10, PROB_20, PROB_33, PROB_50, PROB_66, PROB_75, PROB_90,
  PROB_100, PROB_COUNT
};
static const uint8_t kStepProbPct[PROB_COUNT] = {0, 10, 20, 33, 50, 66, 75, 90, 100};

// Machine ids for the wizard's machine-select page and the SET page's
// MACHINES block: 0 = none, 1..22 = the surveyed machines in the parameter map,
// named by mnmMachineLabel(). v1.22 replaced a guessed list of 12 names
// (SIDLEAD, SIDBASS... do not exist on this machine) with the real one.
static const uint8_t MACHINE_COUNT = (uint8_t)(MNM_MACHINE_COUNT + 1);

// =============================== 5x7 font ====================================
// REDOT SQUARED 5x7 - derived from kFont35 below, not from a stock ASCII set.
// One byte per column, bit 0 = top row. Six pixels of advance gives ten
// characters across the 64-pixel width; the bold renderer below strikes the
// same glyph twice one pixel apart and advances seven, which gives nine
// characters of genuinely legible text.
//
// WHY IT CHANGED: the two fonts used to belong to different families. The 3x5
// is Redot's own squared/LED face - C, E, S, A, N, P and R all carry FULL
// width top and bottom bars, and 1 wears its flag on the top row. The 5x7 was
// stock Bell-style ASCII with cut corners and a curved C. Any page mixing the
// two read as two devices, which is exactly what a header bar sitting above a
// value field does on every page of this OS.
//
// So every letter and digit here is generated FROM the 3x5 by a fixed rule:
//     columns  c0 c1 c2  ->  per-row pattern map, so a bar widens but a stem
//                            never becomes three pixels thick
//     rows     r0 r1 r2 r3 r4  ->  r0 r1 r1 r2 r3 r3 r4, stretching the two
//                            body bands and leaving every bar exactly 1px
// Twelve glyphs that the rule cannot express - the diagonals K M W X V Z Q J,
// plus 1 O G Y whose stems or counters would come out wrong - are authored by
// hand in the same idiom. Punctuation is untouched: it is not part of the
// letterform system and the stock shapes are already geometric.
//
// Regenerating: the rule is mechanical, so if the 3x5 is ever re-cut this table
// should be re-derived rather than hand-edited, or the two will drift apart
// again.
static const uint8_t kFont[64][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // space
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x14,0x08,0x3E,0x08,0x14}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x50,0x30,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /
    {0x7F,0x41,0x41,0x41,0x7F}, // 0
    {0x40,0x41,0x7F,0x40,0x40}, // 1
    {0x79,0x49,0x49,0x49,0x4F}, // 2
    {0x49,0x49,0x49,0x49,0x7F}, // 3
    {0x0F,0x08,0x08,0x08,0x7F}, // 4
    {0x4F,0x49,0x49,0x49,0x79}, // 5
    {0x7F,0x49,0x49,0x49,0x79}, // 6
    {0x01,0x01,0x01,0x01,0x7F}, // 7
    {0x7F,0x49,0x49,0x49,0x7F}, // 8
    {0x4F,0x49,0x49,0x49,0x7F}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x32,0x49,0x79,0x41,0x3E}, // @
    {0x7F,0x09,0x09,0x09,0x7F}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x7F,0x41,0x41,0x41,0x41}, // C
    {0x7F,0x41,0x41,0x41,0x3E}, // D
    {0x7F,0x49,0x49,0x49,0x49}, // E
    {0x7F,0x09,0x09,0x09,0x09}, // F
    {0x7F,0x41,0x41,0x49,0x79}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x41,0x41,0x7F,0x41,0x41}, // I
    {0x70,0x40,0x40,0x40,0x7F}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x01,0x01,0x01,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x0F}, // P
    {0x0F,0x09,0x19,0x29,0x4F}, // Q
    {0x7F,0x09,0x09,0x09,0x77}, // R
    {0x4F,0x49,0x49,0x49,0x79}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x7F,0x40,0x40,0x40,0x7F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x7F,0x20,0x18,0x20,0x7F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x0F,0x08,0x78,0x08,0x0F}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
    {0x00,0x7F,0x41,0x41,0x00}, // [
    {0x02,0x04,0x08,0x10,0x20}, // backslash
    {0x00,0x41,0x41,0x7F,0x00}, // ]
    {0x04,0x02,0x01,0x02,0x04}, // ^
    {0x40,0x40,0x40,0x40,0x40}, // _
};


// =============================== 3x5 font ====================================
// Lifted verbatim from XY6_DEMO.ino - the sketch the UI mockups were rendered
// from - so pages ported from those designs land on the same pixels rather than
// on a retyped approximation. Three pixels wide, five tall, in a four-pixel
// cell: SIXTEEN characters across the 64px panel where the 5x7 font manages ten.
// That difference is the whole reason the mockup can say "RUN DIAGNOSTICS" and
// "MOVE STICK TO ALL 4 CORNERS".
//
// One byte per row, bit 2 leftmost.
static const uint8_t kFont35[64][5] = {
    {0,0,0,0,0}, {2,2,2,0,2}, {5,5,0,0,0}, {5,7,5,7,5},
    {3,6,3,6,2}, {5,1,2,4,5}, {6,4,7,5,7}, {2,2,0,0,0},
    {3,4,4,4,3}, {6,1,1,1,6}, {5,2,7,2,5}, {0,2,7,2,0},
    {0,0,0,2,4}, {0,0,7,0,0}, {0,0,0,0,2}, {1,1,2,4,4},
    {7,5,5,5,7}, {6,2,2,2,7}, {7,1,7,4,7}, {7,1,7,1,7},
    {5,5,7,1,1}, {7,4,7,1,7}, {7,4,7,5,7}, {7,1,1,1,1},
    {7,5,7,5,7}, {7,5,7,1,7}, {0,2,0,2,0}, {0,2,0,2,4},
    {1,3,7,3,1}, {0,7,0,7,0}, {4,6,7,6,4}, {7,1,2,0,2},
    {7,5,7,4,7}, {7,5,7,5,5}, {6,5,6,5,6}, {7,4,4,4,7},
    {6,5,5,5,6}, {7,4,7,4,7}, {7,4,7,4,4}, {7,4,5,5,7},
    {5,5,7,5,5}, {7,2,2,2,7}, {1,1,1,5,7}, {5,5,6,5,5},
    {4,4,4,4,7}, {5,7,7,5,5}, {7,5,5,5,5}, {7,5,5,5,7},
    {7,5,7,4,4}, {7,5,5,7,3}, {7,5,6,5,5}, {7,4,7,1,7},
    {7,2,2,2,2}, {5,5,5,5,7}, {5,5,5,5,2}, {5,5,7,7,5},
    {5,5,2,5,5}, {5,5,7,1,7}, {7,1,2,4,7}, {7,4,4,4,7},
    {4,6,7,6,4}, {7,1,1,1,7}, {2,7,5,0,0}, {0,0,0,0,7},
};

// Which font the PORTED pages draw in. The three original pages keep their
// explicit 5x7 calls, because their layouts were built around that metric.
//
// You told me the old small text was unreadable and I fixed it by going bigger;
// the mockup goes smaller. Neither of us can settle that from a screenshot, so
// both are compiled in and `font 3` / `font 5` swaps them live on the panel.
enum UiFont : uint8_t { UIFONT_3X5 = 0, UIFONT_5X7 = 1 };
static uint8_t uiFont = UIFONT_3X5;      // mockup-accurate by default

// ================================ canvas =====================================
//
// The framebuffer is held in PANEL orientation, 4 bpp, two pixels per byte —
// 8192 bytes, ready to blast straight out of the bus with no packing step.
// Drawing addresses it in logical portrait coordinates and px() does the
// rotation, exactly as in the brightness test that works on this panel.
//
// One thing worth knowing before you add a primitive: px() maps logical y to
// the panel's x and logical x to the panel's y, so a logical VERTICAL line
// walks consecutive nibbles inside one panel row and a logical HORIZONTAL line
// strides 128 bytes per pixel. Vertical spans are therefore several times
// cheaper than horizontal ones, which is why fillRect below is column-major
// and why the scope traces are drawn as spans rather than Bresenham lines.

static const int16_t GFX_W = 64;
static const int16_t GFX_H = 256;

// v1.24 - FIVE SHADES, ONE JOB EACH. The Monomachine's screen is two-tone; this
// panel has sixteen greys, and five of them, each with a fixed role, give the
// pages depth without any softness. Every pixel the UI draws is one of these
// five at full strength - no blending, no anti-aliasing, no partial pixel
// anywhere. (The one exception is the DISSOLVE page transition, which is a
// setting and off by default.)
//
//   G_WHITE  15  primary data: values, the focus block, bar fills, the trace
//   G_LIGHT  12  secondary data: the name beside a value, the machine, idle traces
//   G_MID     9  labels out of focus, frames, hints, inactive track numbers
//   G_DARK    4  region rules, empty bar space, guides - NEVER text
//   G_BLACK   0  the ground
//
// Text never goes below G_MID. 5/15 was measured unreadable on this panel with
// any light in the room (the reason SH_DIM became 9), so the darkest grey only
// ever draws lines and empty space - an inactive track number is MID.
static const uint8_t G_BLACK = 0, G_DARK = 4, G_MID = 9, G_LIGHT = 12, G_WHITE = 15;
// The names the rest of the file grew up with, as aliases of the roles.
static const uint8_t SH_OFF   = G_BLACK;
static const uint8_t SH_FAINT = G_DARK;
static const uint8_t SH_DIM   = G_MID;
static const uint8_t SH_MID   = G_LIGHT;
static const uint8_t SH_ON    = G_WHITE;

static uint8_t g_fb[PANEL_W * PANEL_H / 2];

class Gfx {
 public:
  inline void px(int16_t x, int16_t y, uint8_t s) {
    if ((uint16_t)x >= (uint16_t)SCR_W || (uint16_t)y >= (uint16_t)SCR_H) return;
    s &= 0x0F;
#if ROTATE_FLIP
    int pxx = PANEL_W - 1 - y, pyy = x;
#else
    int pxx = y, pyy = PANEL_H - 1 - x;
#endif
    uint8_t* p = &g_fb[pyy * (PANEL_W / 2) + (pxx >> 1)];
    if (pxx & 1) *p = (uint8_t)((*p & 0xF0) | s);
    else         *p = (uint8_t)((*p & 0x0F) | (s << 4));
  }
  inline uint8_t get(int16_t x, int16_t y) const {
    if ((uint16_t)x >= (uint16_t)SCR_W || (uint16_t)y >= (uint16_t)SCR_H) return 0;
#if ROTATE_FLIP
    int pxx = PANEL_W - 1 - y, pyy = x;
#else
    int pxx = y, pyy = PANEL_H - 1 - x;
#endif
    uint8_t v = g_fb[pyy * (PANEL_W / 2) + (pxx >> 1)];
    return (pxx & 1) ? (uint8_t)(v & 0x0F) : (uint8_t)(v >> 4);
  }

  void clear() { memset(g_fb, 0, sizeof(g_fb)); }

  void hLine(int16_t x, int16_t y, int16_t w, uint8_t s) {
    for (int16_t i = 0; i < w; ++i) px(x + i, y, s);
  }
  void vLine(int16_t x, int16_t y, int16_t h, uint8_t s) {
    for (int16_t i = 0; i < h; ++i) px(x, y + i, s);
  }
  // Span between two logical y values, endpoints included, either order. This
  // is the primitive every trace is drawn from.
  void vSpan(int16_t x, int16_t y0, int16_t y1, uint8_t s) {
    if (y1 < y0) { int16_t t = y0; y0 = y1; y1 = t; }
    for (int16_t y = y0; y <= y1; ++y) px(x, y, s);
  }
  void dotHLine(int16_t x, int16_t y, int16_t w, uint8_t s, uint8_t period) {
    for (int16_t i = 0; i < w; i += period) px(x + i, y, s);
  }
  // Column-major: see the note above about which axis is cheap.
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t s) {
    for (int16_t i = 0; i < w; ++i) vLine(x + i, y, h, s);
  }
  void rect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t s) {
    if (w <= 0 || h <= 0) return;
    hLine(x, y, w, s); hLine(x, y + h - 1, w, s);
    vLine(x, y, h, s); vLine(x + w - 1, y, h, s);
  }
  void line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t s) {
    int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int16_t dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int16_t sx = (x0 < x1) ? 1 : -1, sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    for (;;) {
      px(x0, y0, s);
      if (x0 == x1 && y0 == y1) break;
      int16_t e2 = (int16_t)(err * 2);
      if (e2 > -dy) { err -= dy; x0 += sx; }
      if (e2 <  dx) { err += dx; y0 += sy; }
    }
  }

  static const int16_t CH_W  = 6, CH_H = 7;   // 5x7 glyph in a 6x8 cell
  static const int16_t CHB_W = 7;             // bold: 6px glyph in a 7px cell

  int16_t textWidth(const char* s) const {
    int16_t n = 0; while (*s++) n++;
    return n ? (int16_t)(n * CH_W - 1) : 0;
  }
  int16_t textBoldWidth(const char* s) const {
    int16_t n = 0; while (*s++) n++;
    return n ? (int16_t)(n * CHB_W - 1) : 0;
  }

  void text(int16_t x, int16_t y, const char* s, uint8_t shade) {
    while (*s) {
      uint8_t c = (uint8_t)*s++;
      if (c >= 'a' && c <= 'z') c -= 32;
      if (c < 32 || c > 95) c = 32;
      const uint8_t* g = kFont[c - 32];
      for (int16_t col = 0; col < 5; ++col) {
        const uint8_t bits = g[col];
        if (!bits) continue;
        for (int16_t row = 0; row < 7; ++row)
          if (bits & (1u << row)) px(x + col, y + row, shade);
      }
      x += CH_W;
    }
  }

  // The same glyph struck twice, one pixel apart, so every stroke is 2px wide.
  // This is the single biggest readability win on this panel: a 1px stroke at
  // 5x7 disappears into the OLED's own bloom at arm's length, a 2px one does
  // not. Costs one extra column of advance per character.
  void textBold(int16_t x, int16_t y, const char* s, uint8_t shade) {
    while (*s) {
      uint8_t c = (uint8_t)*s++;
      if (c >= 'a' && c <= 'z') c -= 32;
      if (c < 32 || c > 95) c = 32;
      const uint8_t* g = kFont[c - 32];
      for (int16_t col = 0; col < 6; ++col) {
        // Column `col` of the bold glyph is the OR of source columns col and
        // col-1, which is exactly "draw it at x and again at x+1".
        uint8_t bits = 0;
        if (col < 5)  bits |= g[col];
        if (col > 0)  bits |= g[col - 1];
        if (!bits) continue;
        for (int16_t row = 0; row < 7; ++row)
          if (bits & (1u << row)) px(x + col, y + row, shade);
      }
      x += CHB_W;
    }
  }

  void textRight(int16_t rx, int16_t y, const char* s, uint8_t shade) {
    text((int16_t)(rx - textWidth(s)), y, s, shade);
  }
  void textBoldRight(int16_t rx, int16_t y, const char* s, uint8_t shade) {
    textBold((int16_t)(rx - textBoldWidth(s)), y, s, shade);
  }
  void textCentre(int16_t cx, int16_t y, const char* s, uint8_t shade) {
    text((int16_t)(cx - textWidth(s) / 2), y, s, shade);
  }

  // Same glyphs, each source pixel drawn as a block. `w` is extra weight: the
  // block is (sc+w) square while the step stays sc, so every stroke grows by w
  // device pixels while the glyph's overall size barely changes.
  //
  // This is a better way to embolden than scaling up. Scaling thickens the
  // strokes AND the counters - the holes inside R, D and O - so the letter gets
  // bigger without looking heavier. Overlapping the blocks thickens only the
  // ink, which is what weight actually is. At sc=2, w=1 turns 2px strokes into
  // 3px and costs one pixel of glyph width.
  void textScaledWeight(int16_t x, int16_t y, const char* s, uint8_t shade,
                        uint8_t sc, uint8_t w) {
    const int16_t blk = (int16_t)(sc + w);
    while (*s) {
      uint8_t c = (uint8_t)*s++;
      if (c >= 'a' && c <= 'z') c -= 32;
      if (c < 32 || c > 95) c = 32;
      const uint8_t* g = kFont[c - 32];
      for (int16_t col = 0; col < 5; ++col) {
        if (!g[col]) continue;
        for (int16_t row = 0; row < 7; ++row)
          if (g[col] & (1u << row))
            fillRect((int16_t)(x + col * sc), (int16_t)(y + row * sc),
                     blk, blk, shade);
      }
      x = (int16_t)(x + CH_W * sc);
    }
  }
  // Plain scaled text is just weight 0, so every existing caller is unchanged.
  void textScaled(int16_t x, int16_t y, const char* s, uint8_t shade, uint8_t sc) {
    textScaledWeight(x, y, s, shade, sc, 0);
  }
  // Width of scaled text: n cells of CH_W*sc, less the trailing gap, plus the
  // extra pixel the weight adds to the final glyph.
  int16_t scaledWidth(const char* s, uint8_t sc, uint8_t w) const {
    int16_t n = 0; while (*s++) n++;
    if (!n) return 0;
    // The last glyph's rightmost lit column is col 4, drawn at x + 4*sc as a
    // block (sc+w) wide, so it runs to 5*sc + w. The old 4*sc + 1 + w was only
    // right at sc == 1 and under-measured every scaled string by sc-1, which
    // biased every centre-on-scaled-text by half that.
    return (int16_t)((n - 1) * CH_W * sc + 5 * sc + w);
  }

  // ---- 3x5 renderer -------------------------------------------------------
  void text35(int16_t x, int16_t y, const char* s, uint8_t shade) {
    while (*s) {
      uint8_t c = (uint8_t)*s++;
      if (c >= 'a' && c <= 'z') c -= 32;
      if (c < 32 || c > 95) c = '?';
      const uint8_t* g = kFont35[c - 32];
      for (int16_t j = 0; j < 5; ++j) {
        const uint8_t r = g[j];
        if (!r) continue;
        if (r & 4) px(x,          (int16_t)(y + j), shade);
        if (r & 2) px((int16_t)(x + 1), (int16_t)(y + j), shade);
        if (r & 1) px((int16_t)(x + 2), (int16_t)(y + j), shade);
      }
      x += 4;
    }
  }
  void text35R(int16_t rx, int16_t y, const char* s, uint8_t shade) {
    text35((int16_t)(rx - width35(s)), y, s, shade);
  }
  void text35Centre(int16_t cx, int16_t y, const char* s, uint8_t shade) {
    text35((int16_t)(cx - width35(s) / 2), y, s, shade);
  }
  int16_t width35(const char* s) const {
    int16_t n = 0; while (*s++) n++;
    return n ? (int16_t)(n * 4 - 1) : 0;
  }

  // ---- font-dispatching layer, used by every ported page ------------------
  // f* == "whichever font is selected". Pages written against these lay out
  // from fCellW()/fCellH() so switching fonts reflows rather than overprints.
  int16_t fCellW() const { return uiFont == UIFONT_3X5 ? 4 : CH_W; }
  int16_t fCellH() const { return uiFont == UIFONT_3X5 ? 5 : CH_H; }
  int16_t fWidth(const char* s) const {
    return uiFont == UIFONT_3X5 ? width35(s) : textWidth(s);
  }
  void fText(int16_t x, int16_t y, const char* s, uint8_t shade) {
    if (uiFont == UIFONT_3X5) text35(x, y, s, shade);
    else                      text(x, y, s, shade);
  }
  void fTextRight(int16_t rx, int16_t y, const char* s, uint8_t shade) {
    fText((int16_t)(rx - fWidth(s)), y, s, shade);
  }
  void fTextCentre(int16_t cx, int16_t y, const char* s, uint8_t shade) {
    fText((int16_t)(cx - fWidth(s) / 2), y, s, shade);
  }

  // ---- the mockup's component vocabulary ----------------------------------
  // Ported from XY6_DEMO.ino. These are the only shapes those designs are made
  // of, so having them here means a ported page is a transcription of its
  // original rather than a reinterpretation.
  //
  // chip() is the inverted label block: a filled rectangle with the text
  // knocked out of it. It is exactly what I removed from the three original
  // pages for legibility, and exactly what the mockup is built from - which is
  // why the font switch above exists.
  void chipD(int16_t x, int16_t y, int16_t w, const char* s) {
    fillRect(x, y, w, 7, SH_ON);
    fTextCentre((int16_t)(x + w / 2), (int16_t)(y + 1), s, SH_OFF);
  }
  void numChip(int16_t x, int16_t y, int16_t side, char d) {
    fillRect(x, y, side, side, SH_ON);
    const char t[2] = {d, 0};
    fText((int16_t)(x + (side - 3) / 2), (int16_t)(y + (side - 5) / 2), t, SH_OFF);
  }
  void meterD(int16_t x, int16_t y, int16_t w, int16_t h, int32_t num, int32_t den) {
    rect(x, y, w, h, SH_ON);
    const int16_t f = den > 0 ? (int16_t)(((int32_t)(w - 2) * num) / den) : 0;
    if (f > 0) fillRect((int16_t)(x + 1), (int16_t)(y + 1), f, (int16_t)(h - 2), SH_ON);
  }
  // Solid triangle, 5 tall. dir < 0 points left, dir > 0 points right.
  void triD(int16_t x, int16_t y, int8_t dir) {
    for (int16_t j = 0; j < 5; ++j) {
      const int16_t w = (j < 3) ? (int16_t)(j + 1) : (int16_t)(5 - j);
      for (int16_t i = 0; i < w; ++i)
        px(dir < 0 ? (int16_t)(x + 2 - i) : (int16_t)(x + i), (int16_t)(y + j), SH_ON);
    }
  }

  // ---- the vertical fader, FW=13 wide, with its rounded caps --------------
  static const int16_t FW = 13;
  void capCap(int16_t x0, int16_t y, int8_t down) {
    static const int16_t wt[5] = {9, 11, 11, 13, 13};
    for (int16_t k = 0; k < 5; ++k) {
      const int16_t w = wt[k];
      hLine((int16_t)(x0 + (FW - w) / 2), down > 0 ? (int16_t)(y + k) : (int16_t)(y - k),
            w, SH_ON);
    }
  }
  void faderShell(int16_t x0, int16_t a, int16_t b) {
    capCap(x0, a, +1); capCap(x0, b, -1);
    const int16_t t = (int16_t)(a + 5), n = (int16_t)((b - 5) - t + 1);
    vLine(x0, t, n, SH_ON);
    vLine((int16_t)(x0 + 1), t, n, SH_ON);
    vLine((int16_t)(x0 + FW - 2), t, n, SH_ON);
    vLine((int16_t)(x0 + FW - 1), t, n, SH_ON);
  }
  // Track runs a+7..b-7: the cap is five solid rows, then two rows of channel
  // above the bar so a full-scale fill still reads as a bar inside a shell
  // rather than as a filled capsule. Section across the 13px column is
  // wall 2 | gap 2 | bar 5 | gap 2 | wall 2.
  void faderFill(int16_t x0, int16_t a, int16_t b, int16_t v) {
    const int16_t iT = (int16_t)(a + 7), iB = (int16_t)(b - 7);
    const int16_t span = (int16_t)(iB - iT + 1);
    if (span <= 0) return;
    if (v < 0)   v = 0;
    if (v > 127) v = 127;
    // Rounded, not truncated: at span 90 the old truncation lost most of a
    // pixel per step and made the top of travel look like it stopped short.
    int16_t h = (int16_t)(((int32_t)span * v + 63) / 127);
    if (v && h < 1) h = 1;          // a non-zero value is never invisible
    if (h <= 0) return;             // and zero draws nothing, which is the truth
    fillRect((int16_t)(x0 + 4), (int16_t)(iB - h + 1), 5, h, SH_ON);
  }

  // Filled circle, drawn as vertical spans because a logical vertical run is
  // consecutive bytes in the framebuffer and a horizontal one strides 128.
  void fillCircle(int16_t cx, int16_t cy, int16_t r, uint8_t shade) {
    if (r <= 0) return;
    const int32_t r2 = (int32_t)r * r;
    int16_t x0 = (int16_t)(cx - r), x1 = (int16_t)(cx + r);
    if (x0 < 0) x0 = 0;
    if (x1 > SCR_W - 1) x1 = SCR_W - 1;
    for (int16_t x = x0; x <= x1; ++x) {
      const int32_t dx = (int32_t)(x - cx);
      int32_t h2 = r2 - dx * dx;
      if (h2 < 0) continue;
      int16_t h = 0; while ((int32_t)(h + 1) * (h + 1) <= h2) ++h;   // isqrt
      vSpan(x, (int16_t)(cy - h), (int16_t)(cy + h), shade);
    }
  }
  // Text that flips whatever is under it. The boot logo needs exactly this:
  // REDOT reads white while the background is black and turns black as the
  // expanding disc swallows it, which is what the filmstrip shows happening.
  void textScaledXor(int16_t x, int16_t y, const char* s, uint8_t sc) {
    while (*s) {
      uint8_t c = (uint8_t)*s++;
      if (c >= 'a' && c <= 'z') c -= 32;
      if (c < 32 || c > 95) c = 32;
      const uint8_t* g = kFont[c - 32];
      for (int16_t col = 0; col < 5; ++col)
        for (int16_t row = 0; row < 7; ++row)
          if (g[col] & (1u << row))
            for (int16_t j = 0; j < sc; ++j)
              for (int16_t i = 0; i < sc; ++i) {
                const int16_t px_ = (int16_t)(x + col * sc + i);
                const int16_t py_ = (int16_t)(y + row * sc + j);
                px(px_, py_, get(px_, py_) >= 8 ? SH_OFF : SH_ON);
              }
      x = (int16_t)(x + CH_W * sc);
    }
  }

  // A hairline rule. Used instead of the old inverted title chips: it separates
  // sections without lighting a solid block of pixels next to small text.
  void hrule(int16_t x, int16_t y, int16_t w, uint8_t shade) { hLine(x, y, w, shade); }

  // Horizontal meter. Reads at a glance from across a room in a way a 25-pixel
  // waveform trace never will, so every page that shows a 0..127 value shows
  // one of these next to the number.
  void meter(int16_t x, int16_t y, int16_t w, int16_t h,
             int16_t v, int16_t vmax, uint8_t shade) {
    if (w < 3 || h < 3) return;
    rect(x, y, w, h, SH_FAINT);
    if (vmax <= 0) return;
    int16_t fw = (int16_t)(((int32_t)v * (int32_t)(w - 2)) / vmax);
    if (fw < 0) fw = 0;
    if (fw > w - 2) fw = w - 2;
    if (fw) fillRect((int16_t)(x + 1), (int16_t)(y + 1), fw, (int16_t)(h - 2), shade);
  }

  void valueBox(int16_t x, int16_t y, int16_t w, int16_t h, const char* v) {
    rect(x, y, w, h, SH_ON);
    text((int16_t)(x + (w - textWidth(v)) / 2), (int16_t)(y + (h - CH_H) / 2),
         v, SH_ON);
  }
  // fill: 0 empty, 1 checkered (conditional trig), 2 solid
  void stepSquare(int16_t x, int16_t y, int16_t size, uint8_t fill, bool cur) {
    rect(x, y, size, size, cur ? SH_ON : SH_DIM);
    if (fill == 2) {
      fillRect((int16_t)(x + 2), (int16_t)(y + 2),
               (int16_t)(size - 4), (int16_t)(size - 4), SH_ON);
    } else if (fill == 1) {
      for (int16_t j = 2; j < size - 2; ++j)
        for (int16_t i = 2; i < size - 2; ++i)
          if (((i + j) & 1) == 0) px(x + i, y + j, SH_ON);
    }
    if (cur) rect((int16_t)(x - 2), (int16_t)(y - 2),
                  (int16_t)(size + 4), (int16_t)(size + 4), SH_MID);
  }
  void cursorTri(int16_t x, int16_t y, uint8_t shade) {  // points right
    for (int16_t i = 0; i < 3; ++i)
      vLine((int16_t)(x + i), (int16_t)(y + i), (int16_t)(5 - i * 2), shade);
  }
};

static Gfx gfx;

// ============================ MIDI transmit queue ============================
//
// Everything the XY6 sends to the Monomachine goes through here. Two queues,
// because the two streams have completely different deadlines: a note-on is
// musical timing and being 3 ms late is audible, whereas a CC update is a
// smooth ramp and dropping one frame of it is invisible. (v1.19: and a third,
// qTurbo, which only the TurboMIDI negotiator uses and which goes first.)
//
// Before this existed the pattern generator wrote notes straight to the port
// behind a check that the buffer had *some* room, then wrote three bytes into
// it. With one byte free that call blocks until the UART drains — 320 us per
// byte at 31250 baud — inside the main loop, which stalls the LFO tick, the
// encoder scan and the display. It was worst exactly when the pattern was
// busiest, which is why the LFOs stuttered under load.
//
// Messages are stored length-prefixed so a message is never split across the
// UART boundary, and nothing here ever blocks: if a queue is full the message
// is dropped and counted. A dropped CC is a frame of a ramp. A dropped note is
// reported in the status block so you know the wire is oversubscribed.

// v1.19: does what has gone into the UART so far leave a SysEx open (an F0
// with no F7 yet)? The TurboMIDI keepalive waits for a boundary so it never
// lands inside one of ours. Every message in these queues is complete today,
// so this reads false between messages - it is here so that a SysEx sent in
// pieces later (a sample dump, say) cannot be broken by the keepalive.
static bool g_txInSysex = false;
static inline void midiTxTrack(uint8_t b) {
  if (b == 0xF0) g_txInSysex = true;
  else if (b == 0xF7 || (b >= 0x80 && b < 0xF8)) g_txInSysex = false;   // EOX, or
}                                                  // any status that ends a SysEx

class MidiQueue {
 public:
  void reset() { head_ = tail_ = 0; count_ = 0; drops_ = 0; }

  // Up to 31 bytes per message; every message goes into the UART whole.
  bool push(const uint8_t* m, uint8_t n) {
    if (n == 0 || n > 31) return false;
    if (freeBytes() < (uint16_t)(n + 1)) { drops_++; return false; }
    buf_[head_] = n; head_ = nextIdx(head_);
    for (uint8_t i = 0; i < n; ++i) { buf_[head_] = m[i]; head_ = nextIdx(head_); }
    count_ = (uint16_t)(count_ + n + 1);
    return true;
  }
  // Length of the message at the head of the queue, 0 if empty.
  uint8_t peekLen() const { return count_ ? buf_[tail_] : 0; }
  // Only call when the port has room for peekLen() bytes.
  void popTo(HardwareSerial* p) {
    const uint8_t n = buf_[tail_]; tail_ = nextIdx(tail_);
    for (uint8_t i = 0; i < n; ++i) {
      p->write(buf_[tail_]); midiTxTrack(buf_[tail_]); tail_ = nextIdx(tail_);
    }
    count_ = (uint16_t)(count_ - n - 1);
  }
  uint16_t pending() const { return count_; }
  uint32_t drops()   const { return drops_; }
  // v1.17: drop the head message unsent - the joystick's dry-run wire.
  void discard() {
    if (!count_) return;
    const uint8_t n = buf_[tail_];
    tail_ = (uint16_t)((tail_ + n + 1) & MASK);
    count_ = (uint16_t)(count_ - n - 1);
  }
  // v1.15: push only if `reserve` bytes stay free afterwards. Note-ons and
  // locks use this so the last bytes of the note queue always belong to
  // NOTE-OFFs - a dropped note-off is a note that never stops.
  bool pushReserve(const uint8_t* m, uint8_t n, uint16_t reserve) {
    if (freeBytes() < (uint16_t)(n + 1 + reserve)) { drops_++; return false; }
    return push(m, n);
  }

 private:
  static const uint16_t SIZE = 256, MASK = SIZE - 1;
  static uint16_t nextIdx(uint16_t i) { return (uint16_t)((i + 1) & MASK); }
  uint16_t freeBytes() const { return (uint16_t)(SIZE - 1 - count_); }
  uint8_t  buf_[SIZE];
  uint16_t head_ = 0, tail_ = 0, count_ = 0;
  uint32_t drops_ = 0;
};

static MidiQueue qTurbo;  // v1.19: TurboMIDI handshake - ahead of everything
static MidiQueue qNote;   // drained first — musical timing
static MidiQueue qCtrl;   // drained with whatever room is left
static uint32_t  txBytesOut = 0;

// v1.19: THE TURBOMIDI KEEPALIVE. While a TurboMIDI link is up the machine
// must hear Active Sensing (FE) every TURBO_KEEPALIVE_MS, or it drops back to
// 1x. The negotiator switches it on at LOCKED and off the moment the link is
// abandoned; midiTxService() sends it, not the negotiator, because
// midiTxService() runs from every pump - each loop pass, every 256 bytes of a
// display push, the EEPROM driver, usbWait - so a long frame or a console dump
// cannot stretch the gap. Scheduled on a Deadline (the loop's own timer
// primitive): it keeps its 150 ms grid, and after a stall it sends one FE and
// resyncs rather than a burst.
static bool     g_tmKeepalive = false;
static Deadline g_tmKeepaliveDl;
static uint32_t g_tmFeSent = 0;            // for 'turbo ?'

// -----------------------------------------------------------------------------
// v1.18: THE UART ONLY GETS A FEW MILLISECONDS AHEAD
// -----------------------------------------------------------------------------
// Serial1's software TX ring is 576 bytes (64 in the core + 512 added in
// setup()) - 184 ms of wire at 31250 baud. midiTxService() used to fill it
// whenever it had room, so the queues were nearly always empty and the real
// backlog sat in the UART, where nothing can jump the line. "Notes drain
// first" only decided the order bytes ENTERED the ring: a note-on queued
// behind a stick sweep and six LFOs still waited for every CC byte already in
// there. Measured on the host model (test/host) under a full stage load:
// notes 6.5-8.5 ms late typically, 15 ms at worst. JOY_Q_CAP could not work
// either - it caps the stick's share of qCtrl, and qCtrl read empty.
//
// So the UART is handed at most MIDI_TX_LEAD_US of wire time; everything else
// waits in the queues, where notes really do go first and the stick's cap
// really does apply. midiTxService() runs on every loop pass and every 256
// bytes of a display push, far more often than the lead takes to drain, so
// the wire never goes idle for it.
#define MIDI_TX_LEAD_US   3000    // wire time the UART may hold ahead of us
#define MIDI_TX_LEAD_MIN    12    // ...but never fewer bytes than this
static int g_txCap  = 0;          // Serial1 availableForWrite() when idle (setup)
static int g_txLead = MIDI_TX_LEAD_MIN;

// Called for every baud the port is given. 3 ms is 12 bytes at 31250 (the
// floor) and 75 at 8x turbo.
static void midiTxSetBaud(uint32_t baud) {
  int lead = (int)((baud / 10u) * MIDI_TX_LEAD_US / 1000000u);
  if (lead < MIDI_TX_LEAD_MIN) lead = MIDI_TX_LEAD_MIN;
  g_txLead = lead;
}

// May an n-byte message go into the UART now? It needs the room, and it must
// not take the ring past the lead - unless the ring is empty, so a message
// longer than the lead still goes out whole.
static inline bool midiTxFits(int n, int room, int inRing) {
  if (room < n) return false;
  return inRing <= 0 || inRing + n <= g_txLead;
}

// Push whole messages into the port while it has room for them. Called from the
// main loop and from inside the display flush, so a frame push can never leave
// a note sitting in the queue for a millisecond.
static void midiTxService() {
  int room = Serial1.availableForWrite();
  // Bytes in the ring not yet on the wire. Before setup() has measured the
  // ring (g_txCap 0) there is no lead limit - the pre-v1.18 behaviour.
  int inRing = g_txCap ? g_txCap - room : 0;
  // v1.19: the TurboMIDI keepalive, first - one byte, between messages, never
  // inside an open SysEx. It skips the lead check: the lead is ~3 ms, the
  // keepalive's deadline 150 ms, and it must not queue behind anything.
  if (g_tmKeepalive && !g_txInSysex && room >= 1 &&
      g_tmKeepaliveDl.due(millis(), TURBO_KEEPALIVE_MS)) {
    Serial1.write((uint8_t)0xFE);
    room--; inRing++; txBytesOut++; g_tmFeSent++;
  }
  // v1.19: the negotiator's own messages, ahead of everything.
  for (;;) {
    const uint8_t n = qTurbo.peekLen();
    if (!n || !midiTxFits(n, room, inRing)) break;
    qTurbo.popTo(&Serial1); room -= n; inRing += n; txBytesOut += n;
  }
  // While a TurboMIDI handshake holds the wire NOTHING else moves - notes and
  // note-offs included. From the speed request on, the machine may already be
  // at another baud, and a byte from us at the wrong one is garbage to it; the
  // negotiator also has to see the UART go completely idle before each switch.
  // Everything waits in its queue and goes out at the new speed. (v1.18 let
  // the note queue through here - the old turbo blob rode in it.)
  if (qTurbo.pending() || turboHoldsWire()) return;
  for (;;) {
    const uint8_t n = qNote.peekLen();
    if (!n || !midiTxFits(n, room, inRing)) break;
    qNote.popTo(&Serial1); room -= n; inRing += n; txBytesOut += n;
  }
  // v1.18: notes first, strictly. A note-queue message still waiting means the
  // lead is full; a CC slipped in now would take its place - and could keep a
  // message longer than the lead waiting for an empty ring indefinitely.
  if (qNote.pending()) return;
  for (;;) {
    const uint8_t n = qCtrl.peekLen();
    if (!n || !midiTxFits(n, room, inRing)) break;
    qCtrl.popTo(&Serial1); room -= n; inRing += n; txBytesOut += n;
  }
}

// =============================================================================
// MELODIC PATTERN GENERATOR
// =============================================================================
//
// The Monomachine has ~12 pitched machines plus the BBOX drums. This generator
// writes notes for the PITCHED tracks — bass, chords, arpeggios, lead lines,
// pad drones — and leaves drums to the BBOX or the machine's own sequencer
// (mute any track you want to drive from the MnM instead).
//
// Six tracks, six fixed melodic roles:
//
//   T1 BASS   T2 CHORD   T3 ARP   T4 LEAD   T5 PAD   T6 ACCENT
//
// The generator picks a chord progression for the pattern (four chords, one per
// bar, keyed to the chosen scale) and every track plays notes that stay inside
// the CURRENT bar's chord. That is how the whole pattern hangs together instead
// of six unrelated lines fighting for the same key.
//
// Notes go out on the track's own MIDI channel (base + track - 1), monophonic
// per track. To CAPTURE the output:
//
//   1. As a Monomachine pattern — put the MnM in play + record, `pat on`,
//      generate. Every note you hear is written into its own sequencer.
//   2. As a .mid file — route MIDI to your DAW, arm a track per channel, record.
//
// =============================================================================

// ---- genre + scale --------------------------------------------------------

// v1.11: FROST, FOG and DRILL are SCENES, not just genres - see the SCENES
// block below the LFO engine. Each writes notes AND per-step parameter locks
// and ratchets, and carries a sound + LFO setup that E6 on the PAT page sends.
enum PatGenre : uint8_t {
  PAT_TECHNO = 0, PAT_HOUSE, PAT_AMBIENT, PAT_ACID, PAT_SYNTHWAVE,
  PAT_FROST, PAT_FOG, PAT_DRILL,
  PAT_GENRE_COUNT
};
static inline bool patIsScene(uint8_t g) { return g >= PAT_FROST && g < PAT_GENRE_COUNT; }
static const char* const kPatGenreName[PAT_GENRE_COUNT] = {
    "TECHNO", "HOUSE", "AMBIENT", "ACID", "SYNTHWAVE", "FROST", "FOG", "DRILL"};
// Four characters, because that is what fits bold in the pattern page header
// and "TECHN" truncated mid-word reads worse than a deliberate short form.
static const char* const kPatGenreShort[PAT_GENRE_COUNT] = {
    "TECH", "HOUS", "AMBI", "ACID", "WAVE", "FRST", "FOG", "DRIL"};

// What each track IS in each genre. The five original genres share the fixed
// melodic roles; a scene puts drums, hats and FX where it needs them, and the
// PAT page and 'pat show' must say so or the grid labels lie.
static const char* const kRole2[4][6] = {
    {"BS","CH","AR","LD","PD","AC"},        // the five melodic genres
    {"PD","BL","SB","WN","VO","FX"},        // FROST
    {"DR","HH","BS","ST","PD","VO"},        // FOG
    {"DR","HH","EP","CT","SB","PD"}};       // DRILL
static const char* const kRole4[4][6] = {
    {"BASS","CHRD","ARP","LEAD","PAD","ACC"},
    {"PAD","BELL","SUB","WIND","VOX","FX"},
    {"DRUM","HATS","BASS","STAB","PAD","VOX"},
    {"DRUM","HATS","EPNO","CNTR","SUB","PAD"}};

enum PatScale : uint8_t {
  PSC_MINOR = 0, PSC_DORIAN, PSC_PHRYG, PSC_PENTA, PSC_HMINOR, PSC_SCALE_COUNT
};
static const char* const kPatScaleName[PSC_SCALE_COUNT] = {
    "min", "dor", "phr", "pnt", "hmin"};
static const uint8_t kPatScMinor[7]   = {0, 2, 3, 5, 7, 8, 10};
static const uint8_t kPatScDorian[7]  = {0, 2, 3, 5, 7, 9, 10};
static const uint8_t kPatScPhryg[7]   = {0, 1, 3, 5, 7, 8, 10};
static const uint8_t kPatScPenta[5]   = {0, 3, 5, 7, 10};
static const uint8_t kPatScHminor[7]  = {0, 2, 3, 5, 7, 8, 11};   // harmonic

static const char* const kNoteLetter[12] = {
    "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

// ---- chord progressions --------------------------------------------------
// One per genre, four chords, each a scale degree (0 = i, 2 = iii, 5 = VI...).
// The chord's own root/3rd/5th/7th come from stacking further scale degrees at
// generation time, so a progression written in natural minor still works after
// a `gen sc hmin` — the leading tone lands on any chord that stacks a 7th.

struct ChordProg { int8_t deg[4]; bool add7; };
static const ChordProg kProg[PAT_GENRE_COUNT] = {
    {{0, 5, 2, 6}, false},   // TECHNO    i - VI - III - VII
    {{0, 3, 6, 2}, true },   // HOUSE     i - iv - VII - III
    {{0, 2, 5, 3}, false},   // AMBIENT   i - iii - VI - iv
    {{0, 0, 6, 6}, false},   // ACID      i - i - VII - VII
    {{5, 2, 6, 0}, false},   // SYNTHWAVE VI - III - VII - i
    {{0, 0, 2, 2}, true },   // FROST     i - i - III - III
    {{0, 5, 0, 5}, true },   // FOG       i - VI (two-bar loop)
    {{0, 0, 5, 3}, true },   // DRILL     i - i - VI - iv
};

// ---- trig conditions ------------------------------------------------------

enum PatCond : uint8_t {
  PC_NONE = 0, PC_FIRST, PC_NOT_FIRST, PC_FILL_4,
  PC_PCT_25, PC_PCT_50, PC_PCT_75
};

// ---- step + globals -------------------------------------------------------

struct PatStep {
  uint8_t on     : 1;
  uint8_t accent : 1;
  uint8_t slide  : 1;
  uint8_t ghost  : 1;
  uint8_t cond   : 4;
  uint8_t note;                 // 0..127
  uint8_t vel;                  // 1..127
  uint8_t len;                  // 24ths of a beat; 6 = one 16th, 96 = one bar
  // v1.11: ratchet. 0/1 = a single hit; 2..4 = that many evenly spaced hits
  // inside the 16th (32nds, 16th triplets, 64ths), played by patTick itself.
  uint8_t roll;
};

static const uint8_t PAT_TRACKS = 6;
static const uint8_t PAT_STEPS = 64;
static const uint8_t PAT_SPB = 16;   // 16 steps per bar (1/16 note grid)

struct PatState {
  PatStep  step[PAT_TRACKS][PAT_STEPS];
  bool     trackOn[PAT_TRACKS] = {true, true, true, true, true, true};
  bool     engineOn = false;    // off by default so a boot into the LFO page
                                //   does not surprise anyone with a bass line
  uint8_t  bars   = 4;
  uint8_t  genre  = PAT_TECHNO;
  uint8_t  root   = 36;         // C1
  uint8_t  scale  = PSC_MINOR;
  uint32_t seed   = 1;
  uint32_t rng    = 1;          // GENERATION rng, reseeded on every generate
  uint32_t liveRng = 0x2545F491u;  // PLAYBACK rng, never touches the above
};
static PatState pat;

struct PatActive { uint8_t note; uint8_t vel; uint32_t offUs; bool active;
                   // v1.11 ratchet state: hits still to play inside this step
                   uint8_t rollLeft, rollNote, rollVel;
                   uint32_t rollNextUs, rollGapUs; };
static PatActive patActive[PAT_TRACKS] = {};

static inline uint8_t patRoleSet() {
  return patIsScene(pat.genre) ? (uint8_t)(1 + pat.genre - PAT_FROST) : 0;
}

// ---- parameter locks (v1.11) ------------------------------------------------
// A lock is the Monomachine's p-lock, done over MIDI: on its step the CC goes
// out immediately BEFORE the note (through the note queue, so it can never
// arrive after it), and the parameter goes back to its base value just before
// that track's NEXT note - which is exactly how long a p-lock lasts on the
// machine. The base comes from the scene's sound table, so a lock can only be
// placed on a parameter the scene defines; patLock() refuses anything else.
struct PatLock { uint8_t t, k, page, slot, val; };
static const uint8_t PAT_MAX_LOCKS = 40;
static PatLock patLocks[PAT_MAX_LOCKS];
static uint8_t patLockN = 0;
// What each track currently has locked and must restore: (page, slot, base).
static uint8_t patPend[PAT_TRACKS][8][3];
static uint8_t patPendN[PAT_TRACKS];

// ---- rng ------------------------------------------------------------------
// Two streams on purpose. The generator's stream is reseeded from `seed` on
// every patGenerate() so a pattern is exactly reproducible; the playback stream
// is what trig conditions roll against. They used to be the same variable,
// which meant every probabilistic trig that fired advanced the generator's
// state, so a regenerate mid-take produced a different pattern from the one
// `pat show` had just printed and the same seed did not give the same result
// twice. Deterministic pattern, non-deterministic performance — as intended.

static inline uint32_t patRand() {
  uint32_t x = pat.rng;
  x ^= x << 13; x ^= x >> 17; x ^= x << 5;
  pat.rng = x;
  return x;
}
static inline uint32_t patRandN(uint32_t n) { return n ? (patRand() % n) : 0; }
static inline bool     patRandPct(uint8_t p) { return (patRand() % 100u) < p; }

static inline uint32_t patLiveRand() {
  uint32_t x = pat.liveRng;
  x ^= x << 13; x ^= x >> 17; x ^= x << 5;
  pat.liveRng = x;
  return x;
}

// ---- scales + chord notes -------------------------------------------------

static uint8_t patScaleNote(int8_t degree, int8_t octave) {
  const uint8_t* sc; uint8_t len;
  switch (pat.scale) {
    case PSC_DORIAN: sc = kPatScDorian; len = 7; break;
    case PSC_PHRYG:  sc = kPatScPhryg;  len = 7; break;
    case PSC_PENTA:  sc = kPatScPenta;  len = 5; break;
    case PSC_HMINOR: sc = kPatScHminor; len = 7; break;
    default:         sc = kPatScMinor;  len = 7; break;
  }
  int8_t oct = octave, deg = degree;
  while (deg < 0) { deg += len; oct--; }
  int32_t n = (int32_t)pat.root + 12 * oct + sc[deg % len] + 12 * (deg / len);
  if (n < 0)   n = 0;
  if (n > 127) n = 127;
  return (uint8_t)n;
}

// One voice of the CURRENT chord. voice: 0 root, 1 third, 2 fifth, 3 seventh.
static uint8_t chordNote(uint8_t bar, uint8_t voice, int8_t octave) {
  static const int8_t kVoiceOffset[4] = {0, 2, 4, 6};
  const ChordProg& cp = kProg[pat.genre];
  const int8_t rootDeg = cp.deg[bar % 4];
  const int8_t offset  = kVoiceOffset[voice & 3];
  return patScaleNote((int8_t)(rootDeg + offset), octave);
}

// Note name + octave into a caller buffer of at least 5 bytes ("A#-1" + NUL).
static void patNoteName(uint8_t midi, char* out) {
  const char* n = kNoteLetter[midi % 12];
  const int   oct = (int)(midi / 12) - 1;
  uint8_t k = 0;
  while (*n) out[k++] = *n++;
  if (oct < 0) { out[k++] = '-'; out[k++] = (char)('0' - oct); }
  else         { out[k++] = (char)('0' + (oct > 9 ? 9 : oct)); }
  out[k] = 0;
}

// ---- trig-condition test --------------------------------------------------

static bool patCondFire(uint8_t cond, uint16_t pass) {
  switch (cond) {
    case PC_FIRST:      return pass == 0;
    case PC_NOT_FIRST:  return pass != 0;
    case PC_FILL_4:     return (pass & 3) == 3;
    case PC_PCT_25:     return (patLiveRand() % 100u) < 25u;
    case PC_PCT_50:     return (patLiveRand() % 100u) < 50u;
    case PC_PCT_75:     return (patLiveRand() % 100u) < 75u;
    default:            return true;
  }
}

// ---- primitives -----------------------------------------------------------

static inline void patSet(uint8_t t, uint8_t k, uint8_t note, uint8_t vel,
                          uint8_t len, uint8_t flags = 0, uint8_t cond = PC_NONE) {
  if (t >= PAT_TRACKS || k >= PAT_STEPS) return;
  PatStep& s = pat.step[t][k];
  s.on     = 1;
  s.accent = (flags & 1) ? 1 : 0;
  s.slide  = (flags & 2) ? 1 : 0;
  s.ghost  = (flags & 4) ? 1 : 0;
  s.cond   = (uint8_t)(cond & 0x0F);
  s.note   = note; s.vel = vel; s.len = len;
  s.roll   = 0;
}
static inline void patClear() { memset(pat.step, 0, sizeof pat.step); patLockN = 0; }

// Ratchet a step that is already set: n hits inside the 16th (2..4).
static inline void patRoll(uint8_t t, uint8_t k, uint8_t n) {
  if (t >= PAT_TRACKS || k >= PAT_STEPS || !pat.step[t][k].on) return;
  pat.step[t][k].roll = (uint8_t)(n > 4 ? 4 : n);
}
// Lock a parameter on a step. Refused unless the current scene gives that
// parameter a base value - a lock with nothing to return to would leave the
// Monomachine stuck on the locked value for good.
static void patLock(uint8_t t, uint8_t k, uint8_t page, uint8_t slot, uint8_t val) {
  uint8_t base;
  if (t >= PAT_TRACKS || k >= PAT_STEPS || patLockN >= PAT_MAX_LOCKS) return;
  if (!patSceneBase(t, page, slot, &base)) return;
  PatLock& L = patLocks[patLockN++];
  L.t = t; L.k = k; L.page = page; L.slot = slot; L.val = (uint8_t)(val & 0x7F);
}
static bool patLockAt(uint8_t t, uint16_t k) {
  for (uint8_t i = 0; i < patLockN; ++i)
    if (patLocks[i].t == t && patLocks[i].k == k) return true;
  return false;
}

// =============================================================================
// GENRE 1 — TECHNO   (driving, dark, minor)
// =============================================================================
static void patGenTechno() {
  for (uint8_t bar = 0; bar < pat.bars; ++bar) {
    const uint8_t b = (uint8_t)(bar * PAT_SPB);
    // T1 BASS — 8ths on the root, one variation per bar
    for (uint8_t k = 0; k < 16; k += 2) {
      const bool fifth = (k == 14) && patRandPct(35);
      patSet(0, b + k, chordNote(bar, fifth ? 2 : 0, 0),
             (uint8_t)(105 - (k & 1) * 10), 5);
    }
    // T2 CHORD — beat 1, and-of-3
    patSet(1, b + 0,  chordNote(bar, 0, 1), 105, 10);
    patSet(1, b + 10, chordNote(bar, 0, 1), 90,  6);
    // T3 ARP — cascade up-down on 16ths through r/3/5/o
    static const uint8_t arpUpDn[16] = {0,1,2,3, 2,1,0,1, 2,3,2,1, 0,1,2,3};
    for (uint8_t k = 0; k < 16; ++k)
      patSet(2, b + k, chordNote(bar, arpUpDn[k], 1), 75, 3);
    // T4 LEAD — one held 5th per bar, dropped some bars
    if (patRandPct(65)) patSet(3, b + 4, chordNote(bar, 2, 2), 100, 12);
    // T5 PAD — whole-bar drone on the root
    patSet(4, b, chordNote(bar, 0, 1), 65, 96);
    // T6 ACCENT — high 3rd on the last 16th, every other bar
    if (bar & 1) patSet(5, b + 15, chordNote(bar, 1, 2), 85, 3, 1);
  }
}

// =============================================================================
// GENRE 2 — HOUSE   (warm, jazz-inflected, 7ths on)
// =============================================================================
static void patGenHouse() {
  for (uint8_t bar = 0; bar < pat.bars; ++bar) {
    const uint8_t b = (uint8_t)(bar * PAT_SPB);
    // T1 BASS — walking outline
    patSet(0, b + 0,  chordNote(bar, 0, 0), 108, 6);
    patSet(0, b + 6,  chordNote(bar, 2, 0),  95, 4);
    patSet(0, b + 8,  chordNote(bar, 0, 0), 100, 6);
    patSet(0, b + 12, chordNote(bar, 3, 0),  92, 6);
    // T2 CHORD — every offbeat 8th
    for (uint8_t k = 2; k < 16; k += 4) {
      const uint8_t voice = (k == 6 || k == 14) ? 1 : 0;
      patSet(1, b + k, chordNote(bar, voice, 1), 95, 3);
    }
    // T3 ARP — 8ths, walking r-3-5-7-7-5-3-r
    static const uint8_t arpV[8] = {0, 1, 2, 3, 3, 2, 1, 0};
    for (uint8_t i = 0; i < 8; ++i)
      patSet(2, b + i * 2, chordNote(bar, arpV[i], 1), 78, 4);
    // T4 LEAD — dotted phrase once per 2 bars
    if ((bar & 1) == 0 && patRandPct(75)) {
      patSet(3, b + 3,  chordNote(bar, 2, 2), 100, 4);
      patSet(3, b + 6,  chordNote(bar, 1, 2),  95, 5);
      patSet(3, b + 11, chordNote(bar, 3, 2), 105, 6, 1);
    }
    // T5 PAD — sustained 7th
    patSet(4, b, chordNote(bar, 3, 1), 55, 96);
    // T6 ACCENT — end-of-bar fill every other bar
    if (bar & 1) {
      patSet(5, b + 13, chordNote(bar, 2, 2), 80, 2);
      patSet(5, b + 14, chordNote(bar, 3, 2), 85, 2);
      patSet(5, b + 15, chordNote((uint8_t)((bar + 1) % pat.bars), 0, 2), 90, 3, 1);
    }
  }
}

// =============================================================================
// GENRE 3 — AMBIENT   (slow, sparse, evolving)
// =============================================================================
static void patGenAmbient() {
  for (uint8_t bar = 0; bar < pat.bars; ++bar) {
    const uint8_t b = (uint8_t)(bar * PAT_SPB);
    // T1 BASS — half notes, root, low
    patSet(0, b + 0, chordNote(bar, 0, -1), 90, 48);
    patSet(0, b + 8, chordNote(bar, 0, -1), 85, 48);
    // T2 CHORD — whole bar
    patSet(1, b, chordNote(bar, 0, 1), 70, 96);
    // T3 ARP — quarter notes through r-5-3-o
    static const uint8_t arpV[4] = {0, 2, 1, 3};
    for (uint8_t i = 0; i < 4; ++i)
      patSet(2, b + i * 4, chordNote(bar, arpV[i], 1), 72, 20);
    // T4 LEAD — 1 or 2 sparse notes per bar
    const uint8_t leadPos1 = (uint8_t)(2 + patRandN(4));
    patSet(3, b + leadPos1, chordNote(bar, (uint8_t)(1 + patRandN(2)), 2),
           95, (uint8_t)(24 + patRandN(24)));
    if (patRandPct(40)) {
      const uint8_t leadPos2 = (uint8_t)(10 + patRandN(4));
      patSet(3, b + leadPos2, chordNote(bar, (uint8_t)patRandN(4), 2),
             85, (uint8_t)(12 + patRandN(20)));
    }
    // T5 PAD — 5th stacked above the bass
    patSet(4, b, chordNote(bar, 2, 1), 60, 96);
    // T6 ACCENT — sparse chimes, no more than 2 a bar
    for (uint8_t i = 0; i < 2; ++i)
      if (patRandPct(45)) {
        const uint8_t k = (uint8_t)patRandN(PAT_SPB);
        patSet(5, b + k, chordNote(bar, (uint8_t)patRandN(4), 3),
               (uint8_t)(55 + patRandN(30)), 4);
      }
  }
}

// =============================================================================
// GENRE 4 — ACID   (T1 IS the show; everything else stays out of its way)
// =============================================================================
static void patGenAcid() {
  for (uint8_t bar = 0; bar < pat.bars; ++bar) {
    const uint8_t b = (uint8_t)(bar * PAT_SPB);
    bool prevSlide = false;
    uint8_t prevNote = chordNote(bar, 0, 0);
    for (uint8_t k = 0; k < 16; ++k) {
      if (!prevSlide && !patRandPct(70)) continue;
      uint8_t note = prevNote;
      if (!prevSlide) {
        const uint32_t r = patRandN(100);
        if      (r < 65) note = chordNote(bar, 0, 0);       // root, most
        else if (r < 80) note = chordNote(bar, 1, 0);       // 3rd
        else if (r < 90) note = chordNote(bar, 2, 0);       // 5th
        else if (r < 96) note = chordNote(bar, 0, 1);       // octave up
        else             note = chordNote(bar, 3, 0);       // 7th (colour)
      }
      const bool accent = patRandPct(22);
      const bool slide  = patRandPct(30);
      patSet(0, b + k, note, (uint8_t)(accent ? 122 : 92), (uint8_t)(slide ? 6 : 4),
             (uint8_t)((accent ? 1 : 0) | (slide ? 2 : 0)));
      prevSlide = slide; prevNote = note;
    }
    // T2 CHORD — one long chord tone per bar, up high, out of the 303's way
    patSet(1, b, chordNote(bar, (uint8_t)patRandN(3), 2), 70, 84);
    // T5 PAD — sub root, deep, keeps a floor
    patSet(4, b, chordNote(bar, 0, -1), 55, 96);
    // T3 ARP, T4 LEAD, T6 ACCENT — deliberately empty
  }
}

// =============================================================================
// GENRE 5 — SYNTHWAVE   (pulsing 8ths, ascending arp, dotted lead)
// =============================================================================
static void patGenSynthwave() {
  for (uint8_t bar = 0; bar < pat.bars; ++bar) {
    const uint8_t b = (uint8_t)(bar * PAT_SPB);
    for (uint8_t k = 0; k < 16; k += 2)
      patSet(0, b + k, chordNote(bar, 0, 0), 102, 4);
    patSet(1, b, chordNote(bar, 0, 1), 90, 96);
    static const uint8_t asc[4] = {0, 1, 2, 3};
    for (uint8_t k = 0; k < 16; ++k)
      patSet(2, b + k, chordNote(bar, asc[k & 3], 1), 78, 3);
    static const uint8_t leadK[] = {0, 3, 6, 10, 14};
    static const uint8_t leadV[] = {0, 2, 1, 2, 3};
    for (uint8_t i = 0; i < 5; ++i)
      patSet(3, b + leadK[i], chordNote(bar, leadV[i], 2),
             (uint8_t)(i == 0 ? 115 : 100),
             (uint8_t)(i == 4 ? 8 : 4), (uint8_t)(i == 0 ? 1 : 0));
    patSet(4, b, chordNote(bar, 2, 2), 55, 96);
    if (patRandPct(80)) patSet(5, b + 15, chordNote(bar, 3, 2), 65, 3, 4);
  }
}

// ---- public generate --------------------------------------------------------

static void patGenerate(uint8_t genre, uint32_t seed) {
  pat.genre = genre;
  pat.seed  = seed;
  pat.rng   = seed ? seed : 1;
  patClear();
  switch (genre) {
    case PAT_TECHNO:    patGenTechno();    break;
    case PAT_HOUSE:     patGenHouse();     break;
    case PAT_AMBIENT:   patGenAmbient();   break;
    case PAT_ACID:      patGenAcid();      break;
    case PAT_SYNTHWAVE: patGenSynthwave(); break;
    default: if (patIsScene(genre)) patGenScene(genre); break;
  }
}

// Changing TO a scene also brings its key, scale and length - the blueprint's
// melody is written for them. Regenerating (a new seed) keeps whatever you have
// since changed them to, so transposing a scene sticks.
static void patSelectGenre(uint8_t genre, uint32_t seed) {
  if (genre >= PAT_GENRE_COUNT) return;
  if (genre != pat.genre) {
    uint8_t r, sc, b;
    if (sceneDefaults(genre, &r, &sc, &b)) { pat.root = r; pat.scale = sc; pat.bars = b; }
  }
  patGenerate(genre, seed);
}

// ---- MIDI note out --------------------------------------------------------
// Queued, never written straight to the port. See MidiQueue for why.

static uint32_t patNotesSent = 0;

// v1.11: the LIVE base channel. Notes used the compile-time MNM_BASE_CHANNEL, so
// changing BASE CH on the settings page moved the LFOs and the PERF page but
// left every pattern note on the old channel.
static inline uint8_t patChan(uint8_t track) {
  return (uint8_t)((txChannel - 1 + track) & 0x0F);
}
static const uint16_t PAT_OFF_RESERVE = 48;   // bytes kept free for note-offs
static bool patSendNoteOn(uint8_t track, uint8_t note, uint8_t vel) {
  const uint8_t msg[3] = {
      (uint8_t)(0x90 | patChan(track)),
      (uint8_t)(note & 0x7F), (uint8_t)(vel & 0x7F)};
  if (!qNote.pushReserve(msg, 3, PAT_OFF_RESERVE)) return false;
  patNotesSent++;
  return true;
}
static bool patSendNoteOff(uint8_t track, uint8_t note) {
  const uint8_t msg[3] = {
      (uint8_t)(0x80 | patChan(track)),
      (uint8_t)(note & 0x7F), 0};
  return qNote.push(msg, 3);
}

// v1.15: a note is only marked silent once its note-off is actually queued. If
// the queue is full the note stays active, its off time has passed, and the
// sweep at the top of patTick retries on the very next loop pass.
static bool patSilenceTrack(uint8_t t) {
  if (t < PAT_TRACKS && patActive[t].active) {
    if (!patSendNoteOff(t, patActive[t].note)) return false;
    patActive[t].active = false;
  }
  return true;
}
// A lock or its restore, through the NOTE queue so it stays in order with the
// note it belongs to (the CC queue drains after notes and would land late).
static void patSendParam(uint8_t t, uint8_t page, uint8_t slot, uint8_t val) {
  const uint8_t cc = mnmCC(page, slot);
  if (cc == 0xFF) return;
  const uint8_t msg[3] = {(uint8_t)(0xB0 | patChan(t)), cc, (uint8_t)(val & 0x7F)};
  // Never at a note-off's expense. v1.23: what went out is what the machine
  // holds - a lock, then its restore, so the kit model ends on the base.
  if (qNote.pushReserve(msg, 3, PAT_OFF_RESERVE)) kitSet(t, page, slot, val);
}
// Before a track's note: put back whatever its last lock changed (unless this
// step locks the same parameter again), then apply this step's locks.
static void patApplyLocks(uint8_t t, uint16_t k) {
  for (uint8_t i = 0; i < patPendN[t]; ++i) {
    bool relocked = false;
    for (uint8_t j = 0; j < patLockN; ++j)
      if (patLocks[j].t == t && patLocks[j].k == k &&
          patLocks[j].page == patPend[t][i][0] && patLocks[j].slot == patPend[t][i][1])
        { relocked = true; break; }
    if (!relocked) patSendParam(t, patPend[t][i][0], patPend[t][i][1], patPend[t][i][2]);
  }
  patPendN[t] = 0;
  for (uint8_t j = 0; j < patLockN; ++j) {
    const PatLock& L = patLocks[j];
    if (L.t != t || L.k != k) continue;
    uint8_t base;
    if (!patSceneBase(t, L.page, L.slot, &base)) continue;
    patSendParam(t, L.page, L.slot, L.val);
    if (patPendN[t] < 8) {
      patPend[t][patPendN[t]][0] = L.page; patPend[t][patPendN[t]][1] = L.slot;
      patPend[t][patPendN[t]][2] = base;   patPendN[t]++;
    }
  }
}
// Nothing may stay locked once the engine stops: every pending restore goes out.
static void patRestoreAllLocks() {
  for (uint8_t t = 0; t < PAT_TRACKS; ++t) {
    for (uint8_t i = 0; i < patPendN[t]; ++i)
      patSendParam(t, patPend[t][i][0], patPend[t][i][1], patPend[t][i][2]);
    patPendN[t] = 0;
  }
}

static void patSilenceAll() {
  for (uint8_t t = 0; t < PAT_TRACKS; ++t) { patSilenceTrack(t); patActive[t].rollLeft = 0; }
  patRestoreAllLocks();
}

// ---- tick -----------------------------------------------------------------

static int32_t patLastStep = -1;

static void patTick(uint64_t pos, uint32_t nowUs, uint32_t beatUs, bool running) {
  for (uint8_t t = 0; t < PAT_TRACKS; ++t)
    if (patActive[t].active && (int32_t)(nowUs - patActive[t].offUs) >= 0)
      patSilenceTrack(t);

  if (!pat.engineOn || !running) {
    // The transport (or the engine) just stopped: nothing may stay locked, and
    // no ratchet may keep firing into the silence.
    if (patLastStep != -1) patRestoreAllLocks();
    for (uint8_t t = 0; t < PAT_TRACKS; ++t) patActive[t].rollLeft = 0;
    patLastStep = -1;
    return;
  }

  // ---- ratchets: the remaining hits of a rolled step, on their own clock ---
  // Checked on every pass, not only on step edges - the hits fall BETWEEN 16ths.
  for (uint8_t t = 0; t < PAT_TRACKS; ++t) {
    PatActive& a = patActive[t];
    if (!a.rollLeft || (int32_t)(nowUs - a.rollNextUs) < 0) continue;
    if (turboHoldsWire()) { a.rollLeft = 0; continue; }
    if (!patSilenceTrack(t)) continue;              // retry next pass
    if (!patSendNoteOn(t, a.rollNote, a.rollVel)) { a.rollLeft = 0; continue; }
    a.note = a.rollNote; a.vel = a.rollVel; a.active = true;
    a.offUs = nowUs + (a.rollGapUs * 6u) / 10u;
    a.rollVel = (uint8_t)((a.rollVel * 88u) / 100u);   // each hit a touch softer
    if (!a.rollVel) a.rollVel = 1;
    a.rollLeft--;
    a.rollNextUs += a.rollGapUs;
  }

  const uint32_t stepAbs = (uint32_t)(pos >> 30);
  if ((int32_t)stepAbs == patLastStep) return;
  patLastStep = (int32_t)stepAbs;

  // A speed change is physically on the wire. New notes would sit behind the
  // turbo blob and push back the moment the port is safe to reprogram, so this
  // step is skipped rather than queued. Note-OFFs above still go out - a stuck
  // note is worse than a missed one. The window is a few tens of milliseconds.
  if (turboHoldsWire()) return;

  const uint16_t len = (uint16_t)pat.bars * PAT_SPB;
  if (!len) return;
  const uint16_t idx  = (uint16_t)(stepAbs % len);
  const uint16_t pass = (uint16_t)(stepAbs / len);

  for (uint8_t t = 0; t < PAT_TRACKS; ++t) {
    if (!pat.trackOn[t]) continue;
    const PatStep& s = pat.step[t][idx];
    const bool lockHere = patLockN && patLockAt(t, idx);
    if (!s.on && !lockHere) continue;
    const bool fire = s.on && patCondFire(s.cond, pass);
    // A lock rides its trig, as on the machine: a trig that does not fire this
    // pass (probability, FIRST, FILL) takes its locks with it.
    if (s.on && !fire) continue;
    // Restore the last lock, apply this step's - BEFORE the note-on below.
    if (patLockN || patPendN[t]) patApplyLocks(t, idx);
    if (!fire) continue;                     // a trigless lock: parameters only

    int16_t vel = s.vel;
    if (s.accent) vel = (vel > 100) ? 127 : (int16_t)(vel + 27);
    if (s.ghost)  vel = (int16_t)((vel * 45) / 100 + 15);
    if (vel > 127) vel = 127;
    if (vel < 1)   vel = 1;

    uint32_t lenUs = ((uint32_t)s.len * beatUs) / 24u;
    PatActive& a = patActive[t];
    a.rollLeft = 0;
    if (s.roll >= 2) {
      // A rolled step: n hits evenly inside this 16th, each 60% of its slot.
      a.rollGapUs  = (beatUs / 4u) / s.roll;
      a.rollLeft   = (uint8_t)(s.roll - 1);
      a.rollNextUs = nowUs + a.rollGapUs;
      a.rollNote   = s.note;
      a.rollVel    = (uint8_t)((vel * 88) / 100);
      lenUs        = (a.rollGapUs * 6u) / 10u;
    }

    if (!patSilenceTrack(t)) continue;              // queue full: skip, never stick
    if (!patSendNoteOn(t, s.note, (uint8_t)vel)) { a.rollLeft = 0; continue; }
    a.note   = s.note;
    a.vel    = (uint8_t)vel;
    a.offUs  = nowUs + (lenUs ? lenUs : 1000u);
    a.active = true;
  }
}

// ---- ascii grid dump for the serial console -------------------------------

static void patPrintGrid() {
  Serial.printf("\n%s  root %u  scale %s  bars %u  seed %lu  engine %s\n",
                kPatGenreName[pat.genre], pat.root, kPatScaleName[pat.scale],
                pat.bars, (unsigned long)pat.seed, pat.engineOn ? "ON" : "off");
  Serial.print("prog:   ");
  static const char* const kRoman[7] = {"i", "ii", "iii", "iv", "v", "VI", "VII"};
  for (uint8_t b = 0; b < pat.bars; ++b) {
    if (b) Serial.print(" - ");
    const int8_t d = kProg[pat.genre].deg[b % 4];
    Serial.print(kRoman[((d % 7) + 7) % 7]);
  }
  Serial.println();
  const uint16_t len = (uint16_t)pat.bars * PAT_SPB;
  Serial.print("         ");
  for (uint16_t k = 0; k < len; ++k) {
    if (k % PAT_SPB == 0 && k) Serial.print("|");
    Serial.print(((k & 3) == 0) ? '.' : ' ');
  }
  Serial.println();
  for (uint8_t t = 0; t < PAT_TRACKS; ++t) {
    usbWait(96);
    Serial.print(pat.trackOn[t] ? "on  " : "off ");
    Serial.printf("T%u %-4s ", t + 1, kRole4[patRoleSet()][t]);
    for (uint16_t k = 0; k < len; ++k) {
      if (k % PAT_SPB == 0 && k) Serial.print("|");
      const PatStep& s = pat.step[t][k];
      char c = ' ';
      if (s.on) {
        if      (s.roll >= 2) c = 'r';
        else if (s.accent) c = '*';
        else if (s.slide)  c = '.';
        else if (s.ghost)  c = 'o';
        else               c = '#';
      }
      Serial.print(c);
    }
    Serial.println();
  }
  Serial.println("         # note  * accent  o ghost  . slide  r ratchet");
  // The lock lane, one line per lock: which step, which parameter, what value.
  if (patLockN) {
    Serial.printf("locks (%u):", patLockN);
    for (uint8_t i = 0; i < patLockN; ++i) {
      const PatLock& L = patLocks[i];
      if ((i % 5) == 0) { usbWait(96); Serial.print("\n  "); }
      Serial.printf("T%u@%-2u %s=%-3u  ", L.t + 1, L.k + 1, mnmParamName(L.page, L.slot), L.val);
    }
    Serial.println();
  }
  Serial.println();
}

static void patPrintNotes() {
  Serial.printf("\n%s in %s from root %u\n", kPatGenreName[pat.genre],
                kPatScaleName[pat.scale], pat.root);
  Serial.println("bar  root  3rd   5th   7th");
  for (uint8_t bar = 0; bar < pat.bars; ++bar) {
    Serial.printf(" %u   ", bar + 1);
    for (uint8_t v = 0; v < 4; ++v) {
      const uint8_t n = chordNote(bar, v, 0);
      Serial.printf("%-3s%d  ", kNoteLetter[n % 12], (n / 12) - 1);
    }
    Serial.println();
  }
  Serial.println();
}

// ---- serial console dispatch ----------------------------------------------

static bool patHandleCommand(const char* c) {
  if (c[0] == 'g' && c[1] == 'e' && c[2] == 'n' && (c[3] == ' ' || c[3] == 0)) {
    const char* a = c + 3;
    while (*a == ' ') a++;
    if (*a == 0) { patPrintGrid(); return true; }

    struct { const char* k; uint8_t g; } kMap[] = {
      {"tech", PAT_TECHNO}, {"hous", PAT_HOUSE}, {"ambi", PAT_AMBIENT},
      {"acid", PAT_ACID},   {"wave", PAT_SYNTHWAVE},
      {"frost", PAT_FROST}, {"fros", PAT_FROST}, {"fog", PAT_FOG},
      {"drill", PAT_DRILL}, {"dril", PAT_DRILL}};
    for (uint8_t i = 0; i < sizeof(kMap) / sizeof(kMap[0]); ++i) {
      const char* k = kMap[i].k; uint8_t n = 0;
      while (k[n] && a[n] == k[n]) n++;
      if (!k[n] && (a[n] == 0 || a[n] == ' ')) {
        patSelectGenre(kMap[i].g, (uint32_t)micros() ^ 0xA5A5A5A5u);
        Serial.printf("gen %s seed %lu\n", kPatGenreName[kMap[i].g],
                      (unsigned long)pat.seed);
        patPrintGrid();
        if (patIsScene(pat.genre)) scenePrintSheet();
        return true;
      }
    }
    if (a[0] == 'r' && (a[1] == 0 || a[1] == ' ')) {
      patGenerate(pat.genre, (uint32_t)micros() ^ 0x5A5A5A5Au);
      Serial.printf("regen %s seed %lu\n", kPatGenreName[pat.genre],
                    (unsigned long)pat.seed);
      patPrintGrid();
      return true;
    }
    if (a[0] == 'b' && a[1] == ' ') {
      long n = 0; const char* p = a + 2;
      while (*p >= '0' && *p <= '9') { n = n * 10 + (*p - '0'); p++; }
      if (n >= 1 && n <= 4) {
        pat.bars = (uint8_t)n;
        patGenerate(pat.genre, pat.seed);
        Serial.printf("bars %ld\n", n);
      }
      return true;
    }
    if (a[0] == 'k' && a[1] == ' ') {
      long n = 0; const char* p = a + 2;
      while (*p >= '0' && *p <= '9') { n = n * 10 + (*p - '0'); p++; }
      if (n >= 0 && n <= 127) {
        pat.root = (uint8_t)n;
        patGenerate(pat.genre, pat.seed);
        Serial.printf("root %ld\n", n);
      }
      return true;
    }
    if (a[0] == 's' && a[1] == 'c' && a[2] == ' ') {
      const char* p = a + 3;
      uint8_t sc = 0xFF;
      if (p[0] == 'h' && p[1] == 'm' && p[2] == 'i' && p[3] == 'n') sc = PSC_HMINOR;
      else for (uint8_t i = 0; i < PSC_HMINOR; ++i)
        if (p[0] == kPatScaleName[i][0] && p[1] == kPatScaleName[i][1] &&
            p[2] == kPatScaleName[i][2]) sc = i;
      if (sc != 0xFF) {
        pat.scale = sc;
        patGenerate(pat.genre, pat.seed);
        Serial.printf("scale %s\n", kPatScaleName[sc]);
      }
      return true;
    }
    Serial.println("? gen <tech|hous|ambi|acid|wave|frost|fog|drill|r|b N|k N|sc X>");
    return true;
  }

  // scene        send the current scene's sound + load its six LFOs
  // scene info   print the kit sheet only
  if (c[0] == 's' && c[1] == 'c' && c[2] == 'e' && c[3] == 'n' && c[4] == 'e' &&
      (c[5] == ' ' || c[5] == 0)) {
    const char* a = c + 5;
    while (*a == ' ') a++;
    if (!patIsScene(pat.genre)) {
      Serial.println("scene: pick one first - gen frost | gen fog | gen drill");
      return true;
    }
    if (a[0] == 'i') { scenePrintSheet(); return true; }
    sceneApply();
    Serial.printf("scene %s: sound sent, LFOs 1-6 loaded\n", kPatGenreName[pat.genre]);
    scenePrintSheet();
    return true;
  }

  if (c[0] == 'p' && c[1] == 'a' && c[2] == 't' && (c[3] == ' ' || c[3] == 0)) {
    const char* a = c + 3;
    while (*a == ' ') a++;
    if (a[0] == 'o' && a[1] == 'n' && (a[2] == 0 || a[2] == ' ')) {
      pat.engineOn = true; Serial.println("pat engine ON"); return true;
    }
    if (a[0] == 'o' && a[1] == 'f' && a[2] == 'f') {
      pat.engineOn = false; patSilenceAll();
      Serial.println("pat engine OFF (all notes off sent)"); return true;
    }
    if (a[0] == 's' && a[1] == 'h' && a[2] == 'o' && a[3] == 'w') {
      patPrintGrid(); return true;
    }
    if (a[0] == 'n' && a[1] == 'o' && a[2] == 't' && a[3] == 'e') {
      patPrintNotes(); return true;
    }
    if (a[0] == 't' && a[1] == ' ') {
      const char* p = a + 2;
      long n = 0;
      while (*p >= '0' && *p <= '9') { n = n * 10 + (*p - '0'); p++; }
      while (*p == ' ') p++;
      if (n >= 1 && n <= 6) {
        const uint8_t t = (uint8_t)(n - 1);
        if (p[0] == 'o' && p[1] == 'n') pat.trackOn[t] = true;
        else if (p[0] == 'o' && p[1] == 'f' && p[2] == 'f') {
          pat.trackOn[t] = false; patSilenceTrack(t);
        }
        Serial.printf("track %ld %s\n", n, pat.trackOn[t] ? "on" : "off");
      }
      return true;
    }
    Serial.println("? pat <on|off|show|notes|t N on|off>");
    return true;
  }
  return false;
}

static void patPrintHelp() {
  Serial.println(F(
    "\n--- melodic pattern generator --------------------------------\n"
    "  gen tech | hous | ambi | acid | wave    pick genre + regen\n"
    "  gen frost | fog | drill                  pick a SCENE (key, scale and\n"
    "                                           length come with it)\n"
    "  scene                                    send the scene's sound and load\n"
    "                                           its 6 LFOs (= E6 push on PAT)\n"
    "  scene info                               kit sheet: machine per track\n"
    "  gen r                                    regen current, new seed\n"
    "  gen b <1-4>                              bars per pattern\n"
    "  gen k <0-127>                            root MIDI note (36 = C1)\n"
    "  gen sc <min|dor|phr|pnt|hmin>            scale\n"
    "  pat on | off                             enable/disable engine\n"
    "  pat t <1-6> on | off                     per-track mute\n"
    "  pat show                                 ascii grid dump\n"
    "  pat notes                                chord notes per bar\n"
    "\n"
    "  Tracks:  T1 BASS  T2 CHORD  T3 ARP  T4 LEAD  T5 PAD  T6 ACCENT\n"
    "  (scenes assign their own roles - 'pat show' and the PAT page say which)\n"
    "  Notes go out on channel BASE+(track-1). Put the MnM in play + record\n"
    "  and every note becomes part of its own pattern; record to a DAW for\n"
    "  a .mid file.\n"
    "--------------------------------------------------------------"));
}

// ============================ display driver =================================
//
// Three things in this layer are why the panel lights up at all:
//
//   1. 0xFD 0x12 unlocks the SSD1362 command interface. Without it the panel
//      silently ignores every command that follows, including "display on".
//   2. /WR needs a real pulse. Setup, ~40 nops low, then hold.
//   3. The init parameter values (B1 B3 BC BE B6) are the ones tuned for this
//      panel, not the datasheet defaults.
//
// D/C goes LOW for a command byte AND for each of its parameters, then back
// HIGH. Only pixel data goes out with D/C high. Do not "fix" that.

// v1.10: ONE bus write - the XY6_OLED_Stress one, pin by pin, with a delay
// loop that re-reads the nop count every pass. v1.08's LUT write (all eight
// lines in one store, a tighter loop, 8 KB of lookup table) was A/B tested on
// this board in v1.09 and corrupted the panel badly, so it is gone rather than
// kept as a switch nobody should flip.
static inline void busDelay() {
    for (uint8_t i = 0; i < *(volatile uint8_t*)&WR_NOPS; i++) __asm__ volatile("nop");
}
static inline void writeBus(uint8_t v) {
    digitalWriteFast(D_BUS[0], (v >> 0) & 1); digitalWriteFast(D_BUS[1], (v >> 1) & 1);
    digitalWriteFast(D_BUS[2], (v >> 2) & 1); digitalWriteFast(D_BUS[3], (v >> 3) & 1);
    digitalWriteFast(D_BUS[4], (v >> 4) & 1); digitalWriteFast(D_BUS[5], (v >> 5) & 1);
    digitalWriteFast(D_BUS[6], (v >> 6) & 1); digitalWriteFast(D_BUS[7], (v >> 7) & 1);
    busDelay();                                                   // data setup
    digitalWriteFast(PIN_WR, LOW);
    busDelay();                                                   // /WR low
    digitalWriteFast(PIN_WR, HIGH);
    busDelay();                                                   // data hold
}

static void cmd(uint8_t v) {
    digitalWriteFast(PIN_DC, LOW);
    writeBus(v);
    digitalWriteFast(PIN_DC, HIGH);
}
static inline void dat(uint8_t v) { writeBus(v); }   // cmd() leaves D/C high

static const uint8_t INIT_SEQ[] = {
    0xFD, 0x12,             // command unlock  <-- without this, nothing happens
    0xAE,                   // display off
    0x15, 0x00, 0x7F,       // column address 0..127 (2 pixels per byte)
    0x75, 0x00, 0x3F,       // row address 0..63
    0xA0, 0x41,             // remap - 0x41, NOT 0x43: see dispRemap
    0xA1, 0x00,             // display start line
    0xA2, 0x00,             // display offset
    0xA4,                   // normal display
    0xA8, 0x3F,             // multiplex ratio 64
    0xAB, 0x01,             // internal VDD regulator
    0x81, OLED_CONTRAST,    // contrast
    0xB1, 0x51,             // phase 1/2 length
    0xB3, 0xC1,             // clock divider / oscillator
    0xB9,                   // linear greyscale table
    0xBC, 0x08,             // pre-charge voltage
    0xBE, 0x07,             // VCOMH
    0xB6, 0x0F,             // second pre-charge
    0xAF                    // display on
};

// Previous frame as the panel currently holds it. Costs 8 KB and earns it: the
// page is mostly static, so comparing against it lets us push only the rows
// that actually changed instead of blasting all 8192 bytes 30 times a second.
static uint8_t g_shadow[PANEL_W * PANEL_H / 2];
static bool    g_shadowValid = false;
static uint16_t g_lastRows = 0;      // rows sent last frame, for the status line

// -----------------------------------------------------------------------------
// WHY A ONE-BYTE GLITCH USED TO LAST FOREVER
// -----------------------------------------------------------------------------
// The shadow does not hold what the panel SHOWS. It holds what we last tried to
// send it - and this is a write-only bus, so there is no read-back and no way
// to ever learn the difference.
//
// So the row diff, which is the right optimisation, has a failure mode that is
// worse than the problem it solves. The comment above admits a byte can land
// wrong on a breadboard. When one does, g_fb still equals g_shadow for that
// row, the row is therefore judged unchanged, and it is NEVER SENT AGAIN. A
// transient single-byte error - a millisecond of noise - becomes a permanent
// artifact, and it stays until something happens to alter that row's content.
// That is the "glitches now and then and then it stays there" symptom exactly:
// the glitch is transient, the memory of it is not.
//
// The fix is to stop believing the shadow on a timer. Forgetting it once a
// second costs one full-frame repaint - 8192 bytes, about 3.5 ms of bus time,
// 0.35% duty - and turns every such artifact from permanent into something that
// clears itself within a second without anyone touching the box.
//
// Worth knowing before you read the settings page: this repaint IS the worst
// loop pass in the firmware, so LOOP US now reads ~3500 once a second by
// design. That is the cost being paid, not a regression.
static uint32_t dispHealMs   = 1000;   // 0 disables, 'heal <ms>' to change
static uint32_t g_healAt     = 0;
static uint32_t g_healCount  = 0;

// Push the frame. Only rows whose 128 bytes differ from what the panel already
// holds are transmitted, in contiguous runs, one address window per run.
//
// This is the flicker fix. Rewriting every row every frame means the panel is
// being written continuously while it scans itself out, and on a breadboard
// that is both a lot of switching noise and a lot of chances for a byte to
// land wrong. Sending a fifth of the data removes most of both.
//
// MIDI is serviced every 256 bytes rather than every 512: a frame push is
// ~1 ms of solid bus writing, and a note sitting in the queue for that long is
// audible against a hi-hat. Both directions are pumped, so incoming clock is
// never dropped either.
// v1.08: every frame goes out whole. The row diff sent a different, partial set
// of rows each frame, so a moving element could be half-new/half-old on the
// panel for a frame, and a wrong byte could survive until the heal timer. With
// the bus timing fixed and 8192 bytes costing ~2 ms, a full push at 60 Hz is
// ~12% of the CPU and gives a clean, complete image every frame - exactly what
// the stress test does. Set to 0 to go back to the row diff.
//
// v1.09: v1.08 threw the shadow away on every call, so the whole 8 KB went
// out 60 times a second whether or not a single pixel had changed. Now a frame
// identical to what the panel already holds is NOT SENT: the pages still draw
// into g_fb (RAM - the panel never sees it), but if the result matches
// g_shadow the bus stays silent. Anything that forces a repaint
// (g_shadowValid = false: page style, 'redraw', init tuning, the heal timer)
// still gets one. A frame that did change goes out exactly as in v1.08.
#define DISP_FULL_FRAME 1

// How a changed frame goes out. 'fmode' on the console switches it live.
//   1 = the XY6_OLED_Stress way (DEFAULT): /CS held low for the whole frame, address
//       window set ONCE, 8192 bytes in one stream. MIDI is still pumped every
//       256 bytes, but /CS stays asserted and no command bytes are sent - the
//       bus just sits still (D/C high, /WR high) until we come back.
//   0 = v1.08: /CS released and the window re-sent every two rows.
// The panel re-map register value, re-sent before every push. A variable so
// 'rmap <hex>' can change it live and have it STICK instead of being put back
// by the next frame.
// v1.09: 0x41, not the 0x43 every earlier version used. Bit 1 is the nibble
// (pixel-pair) order, and on this panel 0x43 put every two horizontally
// adjacent panel pixels the wrong way round. That is invisible on lines, boxes
// and anything drawn 2+ px wide - the REDOT splash, the stress test's 3x text -
// and it scrambles 1-px text: "STOP" read as "STMP", "064" as "Mby". Proven on
// the hardware by 'rmap 41' typed live.
static uint8_t dispRemap = 0x41;
static uint8_t dispMode = 1;

static void flushAll() {
    // Forget what we believe the panel holds, on a timer, so a byte that landed
    // wrong cannot outlive the second it landed in. See the note above.
    if (dispHealMs) {
        const uint32_t nowMs = millis();
        if ((uint32_t)(nowMs - g_healAt) >= dispHealMs) {
            g_healAt = nowMs;
            g_shadowValid = false;
            g_healCount++;
        }
    }

    // Nothing changed since the last push: do not touch the panel at all.
    if (g_shadowValid && memcmp(g_fb, g_shadow, sizeof g_fb) == 0) {
        g_lastRows = 0;
        return;
    }

    // v1.09: re-assert the panel geometry before EVERY push. The controller
    // registers are write-only and nothing else ever rewrites them, so any
    // byte that lands wrong as a command would otherwise stay wrong until a
    // power cycle. Eight command bytes a frame bounds that to one frame.
    digitalWriteFast(PIN_CS, LOW);
    cmd(0xA0); cmd(dispRemap);      // re-map: nibble order, col mirror, COM split
    cmd(0xA1); cmd(0x00);           // start line
    cmd(0xA2); cmd(0x00);           // offset
    cmd(0xA8); cmd(0x3F);           // multiplex 64
    digitalWriteFast(PIN_CS, HIGH);

    if (dispMode == 1) {
        digitalWriteFast(PIN_CS, LOW);
        cmd(0x15); cmd(0); cmd((uint8_t)(PANEL_W / 2 - 1));
        cmd(0x75); cmd(0); cmd((uint8_t)(PANEL_H - 1));
        for (unsigned i = 0; i < sizeof g_fb; ++i) {
            dat(uiInvert ? (uint8_t)(g_fb[i] ^ 0xFF) : g_fb[i]);
            if ((i & 0xFF) == 0xFF) { pumpMidi(); midiTxService(); }
        }
        digitalWriteFast(PIN_CS, HIGH);
        memcpy(g_shadow, g_fb, sizeof g_fb);
        g_shadowValid = true;
        g_lastRows = PANEL_H;
        return;
    }

#if DISP_FULL_FRAME
    // Changed: send the whole frame, through the chunked path below that v1.08
    // proved on this panel (window re-issued every two rows). One unbroken
    // 8 KB stream turned the screen to garbage on the real hardware in v1.09's
    // first build - one slipped byte shifts the rest of the frame.
    g_shadowValid = false;
#endif
    digitalWriteFast(PIN_CS, LOW);
    uint16_t rows = 0;
    int row = 0;
    while (row < PANEL_H) {
        if (g_shadowValid &&
            memcmp(&g_fb[row * (PANEL_W / 2)], &g_shadow[row * (PANEL_W / 2)],
                   PANEL_W / 2) == 0) { row++; continue; }
        const int start = row;
        while (row < PANEL_H &&
               (!g_shadowValid ||
                memcmp(&g_fb[row * (PANEL_W / 2)], &g_shadow[row * (PANEL_W / 2)],
                       PANEL_W / 2) != 0)) row++;
        const int end = row - 1;

        cmd(0x15); cmd(0); cmd((uint8_t)(PANEL_W / 2 - 1));
        cmd(0x75); cmd((uint8_t)start); cmd((uint8_t)end);
        for (int i = start * (PANEL_W / 2); i < (end + 1) * (PANEL_W / 2); ++i) {
            // The whole of reversed video. See uiInvert.
            dat(uiInvert ? (uint8_t)(g_fb[i] ^ 0xFF) : g_fb[i]);
            if ((i & 0xFF) == 0xFF) {
                // ---- /CS IS RELEASED WHILE WE ARE NOT DRIVING THE BUS -------
                // pumpMidi() drains the WHOLE receive buffer and parses every
                // byte of it - up to 1088 bytes during a kit dump - and it was
                // doing that with /CS still asserted. For all that time the
                // panel sat latching a bus no one was driving, with its write
                // pointer parked mid-row. Any noise that looks like a /WR edge
                // in that window writes a byte nobody asked for, and this is a
                // breadboard. Holding the chip select is not free just because
                // we are not using it.
                digitalWriteFast(PIN_CS, HIGH);
                pumpMidi();
                midiTxService();
                digitalWriteFast(PIN_CS, LOW);

                // ---- AND THE WINDOW IS RE-ISSUED ----------------------------
                // Two jobs. It makes releasing /CS correct BY CONSTRUCTION
                // rather than by assuming the controller keeps its pointer
                // across a deselect - an assumption that is almost certainly
                // true and is not worth betting a frame on. And it bounds the
                // damage from a corrupted address byte: one bad 0x75 used to
                // misplace a whole contiguous run, which on a page change is
                // most of the screen, where now it can misplace at most the
                // next 256 bytes.
                //
                // This is only safe at a ROW boundary - the controller wraps a
                // partial column window back to its own left edge rather than
                // to column 0, so resuming mid-row with a full-width window
                // would skip the start of every subsequent row. The pump fires
                // on (i & 0xFF) == 0xFF, so i + 1 is a multiple of 256 and 256
                // is exactly two rows of 128. It is always a row boundary.
                const int next = i + 1;
                if (next < (end + 1) * (PANEL_W / 2)) {
                    cmd(0x15); cmd(0); cmd((uint8_t)(PANEL_W / 2 - 1));
                    cmd(0x75); cmd((uint8_t)(next / (PANEL_W / 2)));
                    cmd((uint8_t)end);
                }
            }
        }
        memcpy(&g_shadow[start * (PANEL_W / 2)], &g_fb[start * (PANEL_W / 2)],
               (size_t)(end - start + 1) * (PANEL_W / 2));
        rows += (uint16_t)(end - start + 1);
    }
    g_shadowValid = true;
    g_lastRows = rows;
    digitalWriteFast(PIN_CS, HIGH);
}

static void setContrast(uint8_t v) {
    digitalWriteFast(PIN_CS, LOW);
    cmd(0x81); cmd(v);
    digitalWriteFast(PIN_CS, HIGH);
}

// Poke one two-byte controller command live, so the init registers that decide
// how much of the panel lights can be tuned from the serial console instead of
// by editing INIT_SEQ and reflashing.
static void dispCmd2(uint8_t c, uint8_t v) {
    digitalWriteFast(PIN_CS, LOW);
    cmd(c); cmd(v);
    digitalWriteFast(PIN_CS, HIGH);
}

static void dispBegin() {
    for (int i = 0; i < 8; ++i) pinMode(D_BUS[i], OUTPUT);
    pinMode(PIN_WR,  OUTPUT); digitalWriteFast(PIN_WR,  HIGH);
    pinMode(PIN_DC,  OUTPUT); digitalWriteFast(PIN_DC,  HIGH);
    pinMode(PIN_CS,  OUTPUT); digitalWriteFast(PIN_CS,  HIGH);
    pinMode(PIN_RES, OUTPUT);

    // v1.09: the stress test's reset timings (was 2/10/10 ms). Not the cause
    // of the unreadable text - that was the re-map value, see dispRemap - but
    // the controller must be fully out of reset before INIT_SEQ, and these are
    // the timings proven on this panel. ~290 ms, once, behind the splash.
    digitalWriteFast(PIN_RES, HIGH); delay(10);
    digitalWriteFast(PIN_RES, LOW);  delay(20);
    digitalWriteFast(PIN_RES, HIGH); delay(200);

    digitalWriteFast(PIN_CS, LOW);
    for (unsigned i = 0; i < sizeof(INIT_SEQ); ++i) cmd(INIT_SEQ[i]);
    delay(60);                      // display-on settle, as the stress test does
    // Belt and braces: re-send the geometry registers now that the controller
    // is certainly awake. They are idempotent, and they are the ones whose loss
    // is invisible until you try to read text.
    cmd(0xA0); cmd(dispRemap);      // re-map
    cmd(0xA1); cmd(0x00);           // start line
    cmd(0xA2); cmd(0x00);           // offset
    cmd(0xA8); cmd(0x3F);           // multiplex 64
    digitalWriteFast(PIN_CS, HIGH);

    memset(g_fb, 0, sizeof(g_fb));
    g_shadowValid = false;          // first frame must go out in full
    flushAll();
}

// ========================== Monomachine parameters ===========================
//
// Appendix B of the manual documents one NRPN per track carrying the parameter
// index in Data Entry MSB and the value in Data Entry LSB:
//
//   0x00..0x1F  Synth, Amp, Filter, Effect     0x38..0x3F  MIDI sequencer
//   0x20..0x37  LFO 1, LFO 2, LFO 3            0x40..0x45  Multi ENV
//   0x7F        Level
//
// That reaches all 56 track parameters, including the Monomachine's own LFO
// settings — so an XY6 LFO can modulate a Monomachine LFO.

enum MnmPage : uint8_t {
  PAGE_SYNT = 0, PAGE_AMP, PAGE_FILT, PAGE_EFFX,
  PAGE_LFO1, PAGE_LFO2, PAGE_LFO3, PAGE_MIDI, PAGE_MENV, PAGE_LEVL,
  PAGE_COUNT
};

static const char* const kSynt[8] = {"SYN1","SYN2","SYN3","SYN4",
                                     "SYN5","SYN6","SYN7","SYN8"};
static const char* const kAmpN[8] = {"ATCK","HOLD","DEC","REL",
                                     "DIST","VOL","PAN","PORT"};
static const char* const kFiltN[8]= {"BASE","WDTH","HPQ","LPQ",
                                     "ATCK","DEC","BOFS","WOFS"};
// DBAS / DWID are the delay filter's base and width. v1.22 names them as the
// machine does; they were DFBS / DFWD before the hardware survey.
static const char* const kEffxN[8]= {"EQF","EQG","SRR","DTIM",
                                     "DSND","DFB","DBAS","DWID"};
static const char* const kLfoN[8] = {"PAGE","DEST","TRIG","WAVE","MULT","SPD",
                                     "INTL","DPTH"};
static const char* const kMidiN[8]= {"MID1","MID2","MID3","MID4","MID5","MID6",
                                     "MID7","MID8"};
static const char* const kMenvN[6]= {"DEST","ATCK","DEC","LEV","DEPT","OFFS"};
static const char* const kLevlN[1]= {"LEVL"};

// `base` is the NRPN parameter index (YY). `cc` is the first MIDI CC of that
// page on the track's own channel, straight out of Appendix B:
//
//   48..55  Synthesis 1-8            72..79  Filter
//   56..63  Amp: 56 attack, 57 hold, 58 decay, 59 release,
//                 60 dist, 61 vol, 62 pan, 63 portamento
//   80..87  Effects                  88..95  LFO 1
//   104..111 LFO 2                   112..119 LFO 3
//   7 Level   10 Amp Pan
//
// Crucially the table is headed "Track 1 on MIDI base channel + 0" — each
// track listens on its OWN channel, base + track.
struct MnmPageDef { const char* name; uint8_t base, count, cc;
                    const char* const* params; };

static const MnmPageDef kMnmPages[PAGE_COUNT] = {
    {"SYNT", 0x00, 8,  48, kSynt }, {"AMP",  0x08, 8,  56, kAmpN },
    {"FILT", 0x10, 8,  72, kFiltN}, {"EFFX", 0x18, 8,  80, kEffxN},
    {"LFO1", 0x20, 8,  88, kLfoN }, {"LFO2", 0x28, 8, 104, kLfoN },
    {"LFO3", 0x30, 8, 112, kLfoN }, {"MIDI", 0x38, 8, 0xFF, kMidiN},
    {"MENV", 0x40, 6, 0xFF, kMenvN}, {"LEVL", 0x7F, 1,   7, kLevlN},
};

// -----------------------------------------------------------------------------
// THE FLAT DESTINATION LIST
// -----------------------------------------------------------------------------
// Every (page, slot) pair that has a real CC behind it, as ONE index. The
// performance page's destination chooser needs a single number it can add 1 to,
// because there one encoder owns one slot and there is no room for a second
// knob to pick the page separately.
//
// MIDI and MENV are skipped: Appendix B gives them no CC, so a destination
// there would be a knob wired to nothing.
static uint8_t destCount() {
  static uint8_t cached = 0;
  if (cached) return cached;
  uint8_t k = 0;
  for (uint8_t p = 0; p < PAGE_COUNT; ++p)
    for (uint8_t d = 0; d < kMnmPages[p].count; ++d)
      if (kMnmPages[p].cc != 0xFF) k++;
  cached = k;
  return k;
}
// Index -> (page, slot). Returns false past the end.
static bool destAt(uint16_t idx, uint8_t* page, uint8_t* slot) {
  uint16_t k = 0;
  for (uint8_t p = 0; p < PAGE_COUNT; ++p) {
    if (kMnmPages[p].cc == 0xFF) continue;
    if (idx < (uint16_t)(k + kMnmPages[p].count)) {
      *page = p; *slot = (uint8_t)(idx - k); return true;
    }
    k = (uint16_t)(k + kMnmPages[p].count);
  }
  return false;
}
// (page, slot) -> index, or 0 if the pair has no CC.
static uint16_t destIndexOf(uint8_t page, uint8_t slot) {
  uint16_t k = 0;
  for (uint8_t p = 0; p < PAGE_COUNT; ++p) {
    if (kMnmPages[p].cc == 0xFF) continue;
    if (p == page) return (uint16_t)(k + slot);
    k = (uint16_t)(k + kMnmPages[p].count);
  }
  return 0;
}

static uint8_t mnmCC(uint8_t page, uint8_t slot) {
  if (page >= PAGE_COUNT || slot >= kMnmPages[page].count) return 0xFF;
  if (kMnmPages[page].cc == 0xFF) return 0xFF;
  return (uint8_t)(kMnmPages[page].cc + slot);
}
static uint8_t mnmParamIndex(uint8_t page, uint8_t slot) {
  if (page >= PAGE_COUNT || slot >= kMnmPages[page].count) return 0xFF;
  return (uint8_t)(kMnmPages[page].base + slot);
}
static const char* mnmParamName(uint8_t page, uint8_t slot) {
  if (page >= PAGE_COUNT || slot >= kMnmPages[page].count) return "----";
  return kMnmPages[page].params[slot];
}

// =========================== MONOMACHINE VALUE MAP ===========================
// v1.22. What every parameter IS - its range, its init-kit default, its value
// names - comes from the parameter map near the top of this file, from a survey of the real
// machine. This section adds what each track's parameters currently HOLD.
//
// THE KIT MODEL. One byte per parameter per track - 7 pages x 8, the same 56
// the preset format has always reserved in TrackKitP.param - seeded with the
// init-kit defaults and overwritten by anything we learn: a knob turned on the
// machine (its CC comes back to us), or a value we send (PERF, scenes, pattern
// locks). A second bit per parameter says which of the two it is, learned or
// only the default, so the UI keeps telling the truth about what it knows -
// the rule the PERF page already lives by.
//
// Why it matters: an LFO swings around its destination's patch value. Before
// this, a destination nothing had been learned for was assumed to sit at 64:
// wrong for ATCK (0), WDTH (127), DFB (28), PHASER FB (-19), and on a list
// parameter 64 is not even a value. Now it assumes the init kit, which is what
// a freshly loaded Monomachine actually holds.
//
// Machine per track: 0 = none, else 1..22 into kMnmMachines. It decides the
// SYNT page's names, ranges and defaults. Chosen in the wizard, on the SET
// page (MACHINES) or with 'mach' on the console.
static uint8_t machineSel[6] = {0, 0, 0, 0, 0, 0};

static const uint8_t KIT_PAGES = 7;                       // PAGE_SYNT .. PAGE_LFO3
static const uint8_t KIT_SLOTS = (uint8_t)(KIT_PAGES * 8);
static_assert(KIT_SLOTS == EE_PARAM_SLOTS, "kit model and preset layout disagree");
static_assert(PAGE_SYNT == 0 && PAGE_LFO3 == KIT_PAGES - 1, "kit pages moved");
static uint8_t kitVal[6][KIT_SLOTS];
static uint8_t kitLearnedBits[6][KIT_SLOTS / 8];
static bool    kitWatch = false;          // 'kit watch': log every learned value

// The surveyed definition of a parameter, or null where the survey has none:
// MIDI, MENV, LEVL, and a SYNT page on a track with no machine chosen.
static const MnmParam* paramDef(uint8_t page, uint8_t slot, uint8_t track) {
  if (slot >= 8) return nullptr;
  switch (page) {
    case PAGE_SYNT: {
      const MnmMachine* m = mnmMachine(track < 6 ? machineSel[track] : 0);
      return m ? &m->p[slot] : nullptr;
    }
    case PAGE_AMP:  return &kMnmAmpP[slot];
    case PAGE_FILT: return &kMnmFiltP[slot];
    case PAGE_EFFX: return &kMnmEffxP[slot];
    case PAGE_LFO1: case PAGE_LFO2: case PAGE_LFO3: return &kMnmLfoP[slot];
    default:        return nullptr;
  }
}
// Init-kit value, raw. 64 where the survey has nothing - the old assumption.
static uint8_t paramDefault(uint8_t page, uint8_t slot, uint8_t track) {
  const MnmParam* d = paramDef(page, slot, track);
  return d ? mnmDefaultRaw(*d) : (uint8_t)64;
}
// Highest raw value that means something: 127, or the last entry of a list.
static uint8_t paramRawMax(uint8_t page, uint8_t slot, uint8_t track) {
  const MnmParam* d = paramDef(page, slot, track);
  return d ? mnmRawMax(*d) : (uint8_t)127;
}
// The name the machine shows: on SYNT that is the loaded machine's own name.
static const char* paramName(uint8_t page, uint8_t slot, uint8_t track) {
  if (page == PAGE_SYNT) {
    const MnmParam* d = paramDef(page, slot, track);
    if (d) return d->name;
  }
  return mnmParamName(page, slot);
}

static inline uint8_t kitIdx(uint8_t page, uint8_t slot) {
  return (page < KIT_PAGES && slot < 8) ? (uint8_t)(page * 8 + slot) : (uint8_t)0xFF;
}
// What the parameter holds, as far as we know - learned, or the init default.
static uint8_t kitGet(uint8_t track, uint8_t page, uint8_t slot) {
  const uint8_t k = kitIdx(page, slot);
  if (track >= 6 || k == 0xFF) return paramDefault(page, slot, track);
  return kitVal[track][k];
}
// True when that value came from the machine or from us, not from the survey.
static bool kitLearned(uint8_t track, uint8_t page, uint8_t slot) {
  const uint8_t k = kitIdx(page, slot);
  if (track >= 6 || k == 0xFF) return false;
  return ((kitLearnedBits[track][k >> 3] >> (k & 7)) & 1u) != 0;
}
static void kitSet(uint8_t track, uint8_t page, uint8_t slot, uint8_t val) {
  const uint8_t k = kitIdx(page, slot);
  if (track >= 6 || k == 0xFF) return;
  kitVal[track][k] = (uint8_t)(val & 0x7F);
  kitLearnedBits[track][k >> 3] |= (uint8_t)(1u << (k & 7));
}
// Back to the init kit, for pages lo..hi of one track.
static void kitResetPages(uint8_t track, uint8_t lo, uint8_t hi) {
  if (track >= 6) return;
  for (uint8_t pg = lo; pg <= hi && pg < KIT_PAGES; ++pg)
    for (uint8_t sl = 0; sl < 8; ++sl) {
      const uint8_t k = (uint8_t)(pg * 8 + sl);
      kitVal[track][k] = paramDefault(pg, sl, track);
      kitLearnedBits[track][k >> 3] &= (uint8_t)~(1u << (k & 7));
    }
}
static void kitResetAll() {
  for (uint8_t t = 0; t < 6; ++t) kitResetPages(t, 0, (uint8_t)(KIT_PAGES - 1));
}

// Preset flag bits added in v1.22. The record layout and XY6_FORMAT_VER are
// unchanged - every preset saved before still loads - so what changed MEANING
// is marked per record instead:
//   TrackKitP.flags   b1  machineId is a v1.22 id (0..22), not the old list
//                     b2  param[] holds the kit model (0xFF = not learned)
//   LfoPresetP.flags  b1  wave uses the v1.22 (hardware) order
static const uint8_t KIT_F_V122_ID   = 0x02;
static const uint8_t KIT_F_PARAMS    = 0x04;
static const uint8_t LFO_F_V122_WAVE = 0x02;

// The machine list before v1.22 was a guess of 13 names. Its ids map onto the
// surveyed machines as closely as they can; SIDLEAD and SIDBASS were never
// machines of their own and become the SID.
static uint8_t legacyMachineId(uint8_t old) {
  static const char* const kOld[13][2] = {
    {nullptr, nullptr}, {"SID", "6581"}, {"SID", "6581"}, {"SID", "6581"},
    {"FM+", "STAT"}, {"FM+", "PAR"}, {"FM+", "DYN"}, {"DPRO", "WAVE"},
    {"SWAVE", "SAW"}, {"VO-6", "VO6"}, {"GND", "SIN"}, {"GND", "NOIS"},
    {"DPRO", "BBOX"}};
  if (old >= 13 || !kOld[old][0]) return 0;
  return mnmFindMachine(kOld[old][0], kOld[old][1]);
}

// A Monomachine LFO's DEST is a list of eight whose names depend on the same
// LFO's PAGE (itself a list: PTCH SYNT AMP FILT EFFX LFO1 LFO2 LFO3 MIDI).
static const char* mnmLfoDestName(uint8_t lfoPage, uint8_t dest, uint8_t track) {
  static const uint8_t kPg[8] = {0xFF, PAGE_SYNT, PAGE_AMP, PAGE_FILT,
                                 PAGE_EFFX, PAGE_LFO1, PAGE_LFO2, PAGE_LFO3};
  if (dest > 7) dest = 7;
  if (lfoPage == 0) return kMnmPtchDest[dest];
  if (lfoPage == 8) return kMnmMidiDest[dest];
  if (lfoPage > 8)  return "--";
  return paramName(kPg[lfoPage], dest, track);
}

// A value as the machine displays it: -64..63 for bipolar parameters, the
// entry's name for a list (SAW, 1/2, OFF, S01...). out >= 8 bytes; the text is
// at most 5 characters, and at most 4 for anything but a PTCH-page LFO dest.
static void paramFormat(uint8_t page, uint8_t slot, uint8_t track, uint8_t raw,
                        char* out) {
  raw &= 0x7F;
  if (page >= PAGE_LFO1 && page <= PAGE_LFO3 && slot == 1) {
    const uint8_t lp = mnmListIndex(kitGet(track, page, 0), kMnmLists[ML_LFOPAGE].n);
    strncpy(out, mnmLfoDestName(lp, mnmListIndex(raw, 8), track), 7);
    out[7] = 0;
    return;
  }
  const MnmParam* d = paramDef(page, slot, track);
  if (d) { mnmFormat(*d, raw, out); return; }
  uint8_t k = 0;                                   // no survey entry: the number
  if (raw >= 100) out[k++] = (char)('0' + raw / 100);
  if (raw >= 10)  out[k++] = (char)('0' + (raw / 10) % 10);
  out[k++] = (char)('0' + raw % 10);
  out[k] = 0;
}

// Which (page, slot) a CC number addresses, per Appendix B.
static bool kitFromCc(uint8_t cc, uint8_t* page, uint8_t* slot) {
  for (uint8_t p = 0; p < KIT_PAGES; ++p) {
    const uint8_t c0 = kMnmPages[p].cc;
    if (c0 != 0xFF && cc >= c0 && cc < (uint8_t)(c0 + 8)) {
      *page = p; *slot = (uint8_t)(cc - c0); return true;
    }
  }
  return false;
}
// A CC from the machine: remember what that parameter now holds. `ch` is the
// 0-based channel; every track talks on base + track whatever our TX setting.
// Reached from the RX path, so the only printing is into the deferred log.
static void kitNoteCc(uint8_t ch, uint8_t cc, uint8_t val) {
  const uint8_t t = (uint8_t)((ch - (txChannel - 1)) & 0x0F);
  uint8_t pg, sl;
  if (t >= 6 || !kitFromCc(cc, &pg, &sl)) return;
  kitSet(t, pg, sl, val);
  if (kitWatch) {
    char v[8]; paramFormat(pg, sl, t, val, v);
    tmSerial.printf("kit T%u %-4s %-4s  cc %-3u raw %-3u = %s\n", t + 1,
                    kMnmPages[pg].name, paramName(pg, sl, t), cc, val, v);
  }
}

// =============================== LFO engine ==================================
//
// Timing law, from the manual: "each increase of the Multiplier halves the LFO
// cycle time" and "MULT 2x with SPD 64 gives a cycle time of sixteen 16th
// notes". Sixteen 16ths is four beats, so
//
//     cycle_beats = 512 / (SPD * MULT)
//
// SPD 16/32/64 land on exact powers of two, which is why the manual recommends
// them. SPD 127 gives 2.0157 beats rather than a clean half bar — that is real
// Monomachine behaviour and is kept, not rounded away.

static const uint8_t LFO_COUNT = ACTIVE_LFOS;
static const uint8_t LFO_STEPS = 64;            // 4 pages of 16
static const uint8_t LFO_STEPS_PER_PAGE = 16;
static const uint8_t LFO_MULT_MAX = 6;          // 1x .. 64x, as on the machine

// -----------------------------------------------------------------------------
// WAVEFORMS
// -----------------------------------------------------------------------------
// The manual: "eleven shapes... 5 basic shapes plus mirrored copies of all but
// the RND random shape."  That only balances one way:
//
//     5 basic + 5 mirrors + 1 RND = 11
//
// So there are FIVE non-random bases, and there is no sine among them (the SIN
// you see on the machine is the GND-SIN *oscillator*, not an LFO shape). The
// two decay shapes are NOT an upside-down saw: they are envelope-shaped,
// running from full deflection back to ZERO rather than to the opposite
// extreme, which is what makes them useful under ONE and HALF.
//
//   TRI   bipolar triangle          RMP   linear decay,      +1 -> 0
//   SAW   bipolar rising ramp       EXP   exponential decay, +1 -> 0
//   SQR   bipolar square
//   ...each with a vertical mirror, plus RND.
//
// SIN is kept on the end as an XY6 extra. Set MNM_WAVES_ONLY to 1 to hide it.
//
// v1.22: the ORDER is now the machine's own, from the hardware survey -
// TRI ITRI SAW ISAW SQR ISQR EXP IEXP RMP IRMP RND - so scrolling WAVE here
// walks the same list, in the same order, as scrolling it on the Monomachine.
// (EXP used to come after RMP.) Mirrors still sit on odd indices. Presets
// saved before v1.22 are renumbered on load; see Store::unpackFrom().
#define MNM_WAVES_ONLY 0

enum LfoWave : uint8_t {
  WAVE_TRI = 0, WAVE_TRI_M,   // triangle
  WAVE_SAW,     WAVE_SAW_M,   // rising ramp
  WAVE_SQR,     WAVE_SQR_M,   // square
  WAVE_EXP,     WAVE_EXP_M,   // exponential decay
  WAVE_RMP,     WAVE_RMP_M,   // linear decay  (envelope-shaped, ends at zero)
  WAVE_RND,                   // random, one value per cycle
  WAVE_MNM_COUNT,             // ---- everything above exists on the Monomachine
  WAVE_SIN = WAVE_MNM_COUNT,  // XY6 extra
  WAVE_ALL_COUNT
};
#if MNM_WAVES_ONLY
static const uint8_t WAVE_COUNT = WAVE_MNM_COUNT;
#else
static const uint8_t WAVE_COUNT = WAVE_ALL_COUNT;
#endif

// Two names per shape. The long one is what the edit page shows when there is
// room; the short one is what everything else uses.
//
// The short forms matter more than they look. The old code clipped every name
// to three characters, which turned "TRI'" into "TRI" — a mirrored waveform was
// drawn identically to its base, on the page whose whole job is telling you
// which shape is running. An I prefix reads as "inverted" at a glance and can
// never collide with the base name.
static const char* const kWaveNames[WAVE_ALL_COUNT] = {
    "TRI","ITRI","SAW","ISAW","SQR","ISQR",
    "EXP","IEXP","RMP","IRMP","RND","SIN"};
static const char* const kWaveShort[WAVE_ALL_COUNT] = {
    "TRI","ITR","SAW","ISW","SQR","ISQ",
    "EXP","IEX","RMP","IRM","RND","SIN"};
// The hardware names ARE the long names (v1.22) - checked, so the two tables
// can never drift apart.
static_assert(WAVE_MNM_COUNT == 11, "the Monomachine has eleven LFO shapes");

// Where each shape sits at phase 0.
//
// The reasoning for starting TRI and SAW at their MINIMUM: the HALF trig mode
// "runs for one half cycle and finally stops and holds the last LFO level". For
// a triangle that starts at its minimum, half a cycle ends exactly at its
// maximum — a clean one-shot ramp, which is obviously the design intent. A
// triangle starting at the centre would end half a cycle later back at the
// centre, which would make HALF useless. Same argument for SAW.
#define LFO_PHASE0_MIN 1

enum LfoTrig : uint8_t {
  TRIG_FREE = 0,  // runs continuously, never restarts or stops
  TRIG_TRIG,      // restarts on each LFO-trig, then runs continuously
  TRIG_HOLD,      // runs free, output latched on each LFO-trig
  TRIG_ONE,       // restarts, runs one cycle, stops
  TRIG_HALF,      // restarts, runs half a cycle, stops
  TRIG_COUNT
};
static const char* const kTrigNames[TRIG_COUNT] =
    {"FREE","TRIG","HOLD","ONE","HALF"};

// Three characters maximum, so a multiplier is the same width on every row of
// the overview whatever the value. v1.22: 1X..64X - the machine stops at 64X
// (hardware survey), so the XY6 does too. Older presets above 64X load as 64X.
static const char* const kMultNames[LFO_MULT_MAX + 1] = {
    "1X","2X","4X","8X","16X","32X","64X"};

enum TrigCond : uint8_t { COND_NONE = 0, COND_PCT, COND_RATIO,
                          COND_FIRST, COND_NOT_FIRST };

// `prob` is the spec's nine-position ladder and is now the authority on
// whether a step fires. `on` is kept as a fast "is this step live at all" test
// and is maintained in lockstep with prob; `cond`/`condParam` stay for the
// existing conditional-trig modes, which layer ON TOP of the probability - a
// step can be 50% AND first-pass-only.
struct LfoStep { uint8_t on : 1; uint8_t cond : 3; uint8_t pad : 4;
                 uint8_t condParam;
                 uint8_t prob; };

struct LfoParams {
  uint8_t page = PAGE_FILT, dest = 0;
  uint8_t trig = TRIG_FREE, wave = WAVE_TRI;
  uint8_t mult = 1, spd = 64, intl = 0, depth = 64;
  // Spec, LFO page: "Depth and ammount control by 3-4 Encoder". The spec does
  // not define the difference, so: DEPTH is the swing the waveform makes, and
  // AMOUNT is how much of that swing actually reaches the destination - E4
  // works as a per-LFO send level, and pulling it to 0 mutes the modulation
  // without losing the depth setting you dialled in.
  // TODO: [UX DECISION] swap the two in LfoEngine::update() if you meant
  // amount to be a bipolar/unipolar offset instead.
  uint8_t amount = 127;
  // Spec, LFO subpage: "scroll (16 steps or 8)".
  // TODO: [UX DECISION] per-LFO, held here rather than as a global setting.
  uint8_t stepCount = 16;
  uint8_t track = 0, bars = 1, baseValue = 64;
  // Hard limits on what this destination may ever be sent. The waveform is NOT
  // remapped into this window — it swings about the destination's own patch
  // value and is then clipped here. Default is the whole 0..127 range, which
  // is the Monomachine's own behaviour.
  uint8_t lo = 0, hi = 127;
  bool    enabled = false;
  LfoStep steps[LFO_STEPS];
};

struct LfoState {
  int16_t  wave = 0;      // raw shape, +/-32767, before depth
  int16_t  mod  = 0;      // this LFO's contribution in destination units
  int16_t  out  = 0;      // what actually landed, after summing and clipping
  uint8_t  value = 64;    // final 0..127 for the wire
  uint32_t phase = 0;
  bool     stopped = false;
  // Two LFOs pointed at one destination sum, so exactly one of them owns the
  // output slot and sends the summed result; the others go quiet.
  bool     owner = true;
  // The value this destination had in the patch before the LFO took it over,
  // learned by listening to the Monomachine's own CC out.
  uint8_t  capture = 64;
  bool     hasCap  = false;
};

// --- shapes ------------------------------------------------------------------
static int16_t gSine[256];

static void buildSine() {
  // Built by rotating a unit vector, so no libm call and the table is
  // identical every boot.
  double c = 0.99969881869620425, sd = 0.024541228522912288;
  double x = 1.0, y = 0.0;
  for (int n = 0; n < 256; ++n) {
    gSine[n] = (int16_t)(y * 32767.0 + (y >= 0 ? 0.5 : -0.5));
    double nx = x * c - y * sd, ny = x * sd + y * c;
    x = nx; y = ny;
  }
}

static inline int32_t clamp16(int32_t v) {
  return v > 32767 ? 32767 : (v < -32768 ? -32768 : v);
}
static inline int16_t sineAt(uint32_t p) {
  uint32_t i = p >> 24, f = (p >> 16) & 0xFF;
  int32_t a = gSine[i], b = gSine[(i + 1) & 0xFF];
  return (int16_t)(a + (((b - a) * (int32_t)f) >> 8));
}
static inline int16_t triAt(uint32_t p) {
#if LFO_PHASE0_MIN
  int32_t x = (int32_t)(p >> 16);                 // 0..65535
  int32_t v = (x < 32768) ? (-32768 + x * 2)      // rise  -1 -> +1
                          : ( 32767 - (x - 32768) * 2);   // fall +1 -> -1
  return (int16_t)clamp16(v);
#else
  int32_t x = (int32_t)(p >> 16), v;
  if (x < 16384)      v = x * 2;
  else if (x < 49152) v = 32768 - (x - 16384) * 2;
  else                v = -32768 + (x - 49152) * 2;
  return (int16_t)clamp16(v);
#endif
}
static inline int16_t sawAt(uint32_t p) {
#if LFO_PHASE0_MIN
  return (int16_t)clamp16((int32_t)(p >> 16) - 32768);
#else
  return (int16_t)(32767 - (int32_t)(p >> 16));
#endif
}
static inline int16_t sqrAt(uint32_t p) {
  return (p & 0x80000000u) ? -32768 : 32767;
}
// Linear decay. Envelope-shaped: full deflection at phase 0, falling to ZERO at
// the end of the cycle — NOT to the opposite extreme.
static inline int16_t rmpAt(uint32_t p) {
  return (int16_t)(32767 - (int32_t)(p >> 17));   // 32767 -> 0
}
// Exponential decay, same endpoints, cubed curve.
static inline int16_t expAt(uint32_t p) {
  const uint32_t r = 65535u - (p >> 16);
  // Multiplying by (r+1) rather than r is what makes the endpoints exact.
  // x*x>>16 treats 65535 as "just under 1.0", so cubing it lost two counts and
  // a trigged EXP never quite reached full deflection. x*(x+1)>>16 maps 65535
  // to exactly 65535 and 0 to exactly 0, and costs the same single multiply.
  const uint32_t r2 = (r * (r + 1u)) >> 16;
  const uint32_t r3 = (r2 * (r + 1u)) >> 16;
  return (int16_t)clamp16((int32_t)(r3 >> 1));    // 32767 -> 0, cubed
}
static inline uint32_t hash32(uint32_t x) {
  x ^= x >> 16; x *= 0x7FEB352Du;
  x ^= x >> 15; x *= 0x846CA68Bu;
  x ^= x >> 16; return x;
}

// Mirrors are a vertical flip. Negating -32768 overflows an int16, so every
// mirror goes through clamp16 rather than a bare unary minus.
static inline int16_t mirror(int16_t v) { return (int16_t)clamp16(-(int32_t)v); }

static int16_t lfoShape(uint8_t wave, uint32_t phase, uint32_t rndSeed) {
  switch (wave) {
    case WAVE_TRI:   return triAt(phase);
    case WAVE_TRI_M: return mirror(triAt(phase));
    case WAVE_SAW:   return sawAt(phase);
    case WAVE_SAW_M: return mirror(sawAt(phase));
    case WAVE_SQR:   return sqrAt(phase);
    case WAVE_SQR_M: return mirror(sqrAt(phase));
    case WAVE_RMP:   return rmpAt(phase);
    case WAVE_RMP_M: return mirror(rmpAt(phase));
    case WAVE_EXP:   return expAt(phase);
    case WAVE_EXP_M: return mirror(expAt(phase));
    case WAVE_RND:   return (int16_t)((int32_t)(hash32(rndSeed) & 0xFFFFu) - 32768);
    case WAVE_SIN:   return sineAt(phase);
    default:         return 0;
  }
}

static uint64_t lfoCycleBeatsQ32(uint8_t spd, uint8_t mult) {
  uint32_t K = (uint32_t)spd * (1u << mult);
  return K ? ((512ULL << 32) / K) : 0;
}

// Absolute phase from a Q32-beats position, split so the multiply can never
// overflow 64 bits however long the transport has been running.
//   cyclesQ32 = (pos * K) >> 9, taken mod 2^32
static inline uint32_t phaseFromPos(uint64_t pos, uint32_t K) {
  if (!K) return 0;
  uint64_t whole = pos >> 32;
  uint32_t frac  = (uint32_t)pos;
  uint32_t hi = (uint32_t)(((whole % 512u) * (K % 512u)) % 512u) << 23;
  uint32_t lo = (uint32_t)(((uint64_t)frac * K) >> 9);
  return hi + lo;
}

class LfoEngine {
 public:
  LfoParams p[LFO_COUNT];
  LfoState  s[LFO_COUNT];

  void begin(uint32_t seed) {
    buildSine();
    rng_ = seed ? seed : 0x1234567u;
    for (uint8_t i = 0; i < LFO_COUNT; ++i) {
      // All four of these used to be left uninitialized. held_ in particular
      // is read on the very first update() of a HOLD-mode LFO, before any trig
      // has happened, so a fresh board could sit there driving a filter cutoff
      // from whatever was in that stack slot at power-up.
      rndSeed_[i]     = hash32(seed + i * 0x9E3779B9u);
      held_[i]        = 0;
      trigPos_[i]     = 0;
      frozenPhase_[i] = 0;
      trigged_[i]     = false;
      for (uint8_t k = 0; k < LFO_STEPS; ++k) {
        p[i].steps[k].on = 0; p[i].steps[k].cond = COND_NONE;
        p[i].steps[k].condParam = 100;
        // PROB_100 is the resting value, not PROB_OFF: `on` already decides
        // whether a step exists, so a step you switch on should fire every pass
        // until you deliberately dial the probability down.
        p[i].steps[k].prob = PROB_100;
      }
    }
  }

  // The sequencer starting counts as an LFO-trig for every non-FREE LFO. That
  // is what makes them start on play and freeze on stop with no steps
  // programmed: their phase is measured from the transport position, which
  // only advances while the Monomachine is running.
  void onStart() {
    lastStep_ = -1;
    restoring_ = false;                 // play takes the LFOs back from restore
    for (uint8_t i = 0; i < LFO_COUNT; ++i) {
      trigPos_[i] = 0;
      trigged_[i] = true;
      s[i].stopped = false;
      if (p[i].trig == TRIG_HOLD) held_[i] = s[i].wave;
    }
  }
  // Transport stop freezes the one-shot modes where they stand, holding the
  // level they had reached rather than snapping anywhere.
  void onStop() {
    for (uint8_t i = 0; i < LFO_COUNT; ++i)
      if (p[i].trig == TRIG_ONE || p[i].trig == TRIG_HALF) {
        s[i].stopped = true; frozenPhase_[i] = s[i].phase;
      }
  }
  void trig(uint8_t i, uint64_t pos) {
    if (i >= LFO_COUNT) return;
    trigPos_[i] = pos;
    trigged_[i] = true;
    s[i].stopped = false;
    rndSeed_[i]  = rand32();
    if (p[i].trig == TRIG_HOLD) held_[i] = s[i].wave;
  }

  // Is this LFO actually producing a modulation right now? A trig-mode LFO
  // that has never been trigged is NOT — and the output stage uses this to
  // stay off the wire entirely, so a cold boot with the sequencer stopped no
  // longer overwrites six AMP parameters with the default base value before
  // anything has been captured from the machine.
  bool driving(uint8_t i) const {
    if (i >= LFO_COUNT || !p[i].enabled || restoring_) return false;
    if (p[i].trig == TRIG_FREE) return true;
    return trigged_[i];
  }

  uint8_t restValue(uint8_t i) const {
    return s[i].hasCap ? s[i].capture : p[i].baseValue;
  }
  void setCapture(uint8_t i, uint8_t v) {
    if (i < LFO_COUNT) { s[i].capture = v; s[i].hasCap = true; }
  }
  // Double-STOP: hold every destination at its captured patch value and wait.
  void restoreAll() { restoring_ = true; }
  bool restoring() const { return restoring_; }

  // ---------------------------------------------------------------------------
  // THE DEPTH MODEL
  // ---------------------------------------------------------------------------
  // The manual's worked example is the whole specification:
  //
  //   "if the target parameter is set to 64 a DPTH setting of 64 is sufficient
  //    for modulating the target value to its minimum (0) and maximum (127)"
  //
  // Base 64, depth 64, reaches 0 and 127. That is base +/- depth. So:
  //
  //     out = clamp(base + wave * depth/127, 0, 127)      wave in [-1, +1]
  //
  // The base is the destination's own PATCH value — the one captured off the
  // Monomachine's CC out — not the centre of some window. lo/hi are limits.
  //
  // The arithmetic saturates rather than wrapping, and it saturates AFTER the
  // sum — the manual's own example proves it (64 + 64 gives 127, not 0) — so a
  // flattened peak at high depth is intended behaviour, not a bug.
  //
  // Depth is unipolar 0..127. Inversion comes from choosing a mirrored
  // waveform, which is why the machine spends five of its eleven waveform
  // slots on mirrors.
  void update(uint64_t pos, uint64_t freePos, bool running) {
    stepSeq(pos, running);

    // ---- pass 1: shape -> this LFO's own contribution, in destination units --
    for (uint8_t i = 0; i < LFO_COUNT; ++i) {
      if (!driving(i)) { s[i].wave = 0; s[i].mod = 0; continue; }

      const uint32_t K = (uint32_t)p[i].spd * (1u << p[i].mult);

      uint32_t phase; bool frozen = false;
      if (p[i].trig == TRIG_FREE || p[i].trig == TRIG_HOLD) {
        // HOLD keeps its waveform running underneath: the manual is explicit
        // that only the OUTPUT is latched. Freezing the phase here is the
        // classic way to get HOLD wrong — consecutive trigs would then all
        // sample the same point instead of walking the waveform.
        phase = phaseFromPos(freePos, K);
      } else if (s[i].stopped) {
        // ONE and HALF "stop and hold the last LFO level". Hold the phase we
        // actually reached, so the destination stays where the shape left it.
        phase = frozenPhase_[i]; frozen = true;
      } else {
        const uint64_t delta = (pos >= trigPos_[i]) ? (pos - trigPos_[i]) : 0;
        const uint64_t cycle = cycleQ32(i);
        if (!cycle) phase = 0;
        else if (p[i].trig == TRIG_ONE  && delta >= cycle) {
          phase = 0xFFFFFFFFu; frozen = true;
        } else if (p[i].trig == TRIG_HALF && delta >= (cycle >> 1)) {
          phase = 0x80000000u; frozen = true;
        } else {
          phase = phaseFromPos(delta, K);
        }
        if (frozen) frozenPhase_[i] = phase;
      }

      if (phase < s[i].phase) rndSeed_[i] = rand32();   // new value each cycle
      s[i].phase = phase;

      int32_t w = lfoShape(p[i].wave, phase, rndSeed_[i]);
      if (frozen && p[i].wave == WAVE_RND) w = s[i].wave;

      if (p[i].trig == TRIG_HOLD) { s[i].wave = (int16_t)w; w = held_[i]; }
      else                         s[i].wave = (int16_t)w;

      // Interlace: "the process of alternating the LFO waveform with zero", at
      // a rate set by INTL. Zero here means no offset, so the destination drops
      // back to its patch value for the gated half — which is what gives the
      // stuttering, glittery effect the manual describes. Phase-locked to the
      // LFO's own phase, so a trig restarts the chop with the waveform.
      if (p[i].intl) {
        const uint32_t gate = phase * (uint32_t)p[i].intl;
        if (gate & 0x80000000u) w = 0;
      }

      // Shape (+/-32767) x depth (0..127) -> +/-depth, in one 32-bit multiply.
      //
      // The rounding is symmetric ON PURPOSE. A bare >>15 truncates toward
      // minus infinity, so a positive peak came out one short (depth 20 gave
      // +19 but -20) and every LFO sat a fraction below centre. You would never
      // see it on one LFO; you would absolutely hear it with two summed on a
      // filter cutoff, as a static offset that changes with depth.
      int32_t num = (int32_t)w * (int32_t)p[i].depth;
      int32_t md  = (num >= 0 ? num + 16384 : num - 16384) / 32768;
      // AMOUNT scales the finished contribution. Applied AFTER the symmetric
      // rounding above, not folded into it: rounding twice would reintroduce
      // exactly the half-count bias that rounding was added to remove.
      if (p[i].amount < 127) md = (md * (int32_t)p[i].amount) / 127;
      s[i].mod = (int16_t)md;
    }

    // ---- pass 2: sum every LFO sharing a destination, then clip ------------
    // "If you select the same destination for two different LFO tracks their
    // outputs will be added, making advanced layered LFO's possible."  Summed
    // BEFORE the clip, not clipped individually and then added — three LFOs at
    // depth 60 reaching +/-180 and clipping sounds different from three
    // separately-clipped contributions, and the machine does the former.
    for (uint8_t i = 0; i < LFO_COUNT; ++i) {
      if (!driving(i)) {
        s[i].out = 0; s[i].owner = true; s[i].value = restValue(i); continue;
      }
      int32_t acc = 0;
      bool    own = true;
      for (uint8_t j = 0; j < LFO_COUNT; ++j) {
        if (!driving(j) || !sameDest(i, j)) continue;
        if (j < i) own = false;              // lowest index owns the wire
        acc += s[j].mod;
      }
      s[i].owner = own;

      const int32_t base = (int32_t)restValue(i);
      int32_t v = base + acc;
      const int32_t wlo = p[i].lo <= p[i].hi ? p[i].lo : p[i].hi;
      const int32_t whi = p[i].lo <= p[i].hi ? p[i].hi : p[i].lo;
      if (v < wlo) v = wlo;
      if (v > whi) v = whi;
      if (v < 0)   v = 0;
      if (v > 127) v = 127;
      // v1.22: never past the parameter's own last value - a 5-entry list like
      // SID WAVE has nothing above its fifth entry to send.
      const int32_t vmax = (int32_t)paramRawMax(p[i].page, p[i].dest, p[i].track);
      if (v > vmax) v = vmax;
      s[i].value = (uint8_t)v;
      s[i].out   = (int16_t)(v - base);      // what the UI draws as deflection
    }
  }

  // Two LFOs collide when they drive the same parameter of the same track.
  bool sameDest(uint8_t a, uint8_t b) const {
    return p[a].track == p[b].track && p[a].page == p[b].page &&
           p[a].dest  == p[b].dest;
  }

 private:
  uint64_t trigPos_[LFO_COUNT];
  uint32_t frozenPhase_[LFO_COUNT];
  int16_t  held_[LFO_COUNT];
  uint32_t rndSeed_[LFO_COUNT];
  bool     trigged_[LFO_COUNT];
  int32_t  lastStep_ = -1;
  bool     restoring_ = false;
  uint32_t rng_ = 0x1234567u;

  // lfoCycleBeatsQ32() is a 64-bit division. It only changes when SPD or MULT
  // changes, but it was being recomputed for every non-free LFO on every 1 kHz
  // tick. Cached on the (spd, mult) pair.
  uint16_t cycKey_[LFO_COUNT] = {0};
  uint64_t cycVal_[LFO_COUNT] = {0};
  uint64_t cycleQ32(uint8_t i) {
    const uint16_t key = (uint16_t)((uint16_t)p[i].spd << 8 | p[i].mult) + 1u;
    if (cycKey_[i] != key) {
      cycKey_[i] = key;
      cycVal_[i] = lfoCycleBeatsQ32(p[i].spd, p[i].mult);
    }
    return cycVal_[i];
  }

  uint32_t rand32() {
    rng_ ^= rng_ << 13; rng_ ^= rng_ >> 17; rng_ ^= rng_ << 5; return rng_;
  }

  void stepSeq(uint64_t pos, bool running) {
    if (!running) { lastStep_ = -1; return; }
    // A 16th note is a quarter of a beat, so >>30 on a Q32 beat position gives
    // the absolute 16th index directly.
    uint32_t stepAbs = (uint32_t)(pos >> 30);
    if ((int32_t)stepAbs == lastStep_) return;
    lastStep_ = (int32_t)stepAbs;

    for (uint8_t i = 0; i < LFO_COUNT; ++i) {
      if (!p[i].enabled) continue;
      // Spec, LFO subpage: "scroll (16 steps or 8)". stepCount is per-LFO, so
      // one LFO can run a 3-against-4 against another without touching bars.
      const uint8_t sc = (p[i].stepCount == 8) ? 8 : 16;
      const uint16_t len = (uint16_t)p[i].bars * sc;
      if (!len) continue;                 // bars 0 would divide by zero below
      const uint16_t idx  = (uint16_t)(stepAbs % len);
      const uint16_t pass = (uint16_t)(stepAbs / len);
      const LfoStep& st = p[i].steps[idx];
      if (!st.on) continue;

      // THE PROBABILITY LADDER, applied first. A step at PROB_OFF is not a
      // step; anything below 100% rolls here, and only a surviving roll goes on
      // to the conditional-trig test below. Order matters: rolling first means
      // a 25% FIRST-pass step fires on a quarter of the first passes rather
      // than on every first pass a quarter of the time.
      const uint8_t pct = kStepProbPct[st.prob < PROB_COUNT ? st.prob : (uint8_t)PROB_100];
      if (pct == 0) continue;
      if (pct < 100 && (rand32() % 100u) >= pct) continue;

      bool fire = true;
      switch (st.cond) {
        case COND_PCT:   fire = (rand32() % 100u) < (uint32_t)st.condParam; break;
        case COND_RATIO: { uint8_t a = (st.condParam >> 4) & 0x0F;
                           uint8_t b = st.condParam & 0x0F;
                           fire = b && ((pass % b) == (a ? a - 1u : 0u)); break; }
        case COND_FIRST:     fire = (pass == 0); break;
        case COND_NOT_FIRST: fire = (pass != 0); break;
        default: break;
      }
      if (fire) trig(i, pos);
    }
  }
};

static LfoEngine lfo;

// The persistent format mirrors these dimensions as literals so its layout can
// never move because a runtime constant did. These assertions are what keep the
// two honest: change LFO_STEPS and the build stops here rather than shipping a
// format that silently truncates every step grid.
static_assert(EE_LFO_STEPS == LFO_STEPS,   "EE_LFO_STEPS must track LFO_STEPS");
static_assert(EE_LFOS      == LFO_COUNT,   "EE_LFOS must track LFO_COUNT");

// =============================== sync clock ==================================
//
// MIDI clock is 24 PPQN — one edge every 20.8 ms at 120 BPM. An LFO driven
// straight off those edges stair-steps audibly on a filter cutoff. So the
// clock is a REFERENCE, not the timebase: a DPLL locks a micros() accumulator
// to it and everything musical reads the accumulator.
//
// Position is Q32 beats in a uint64 (1 beat == 1<<32).

static const uint8_t MIDI_CLOCK = 0xF8, MIDI_START = 0xFA;
static const uint8_t MIDI_CONTINUE = 0xFB, MIDI_STOP = 0xFC, MIDI_SPP = 0xF2;

static inline uint64_t tickToQ32(uint32_t tick) {
  return ((uint64_t)tick << 32) / 24u;
}

class SyncClock {
 public:
  void begin(uint32_t nowUs, float bpm) {
    periodUs_ = (uint32_t)(60000000.0f / (bpm * 24.0f));
    rateQ40_  = rate(periodUs_);
    lastUpdateUs_ = lastTickUs_ = nowUs;
    pos_ = 0; running_ = false; locked_ = false;
  }

  void onClockTick(uint32_t nowUs) {
    if (internal_) return;                 // ignore the wire while free-running
    uint32_t dt = nowUs - lastTickUs_;
    lastTickUs_ = nowUs;
    // v1.15: a tick closer than 2 ms to the last one is not a runt, it is a
    // tick DELIVERED late - bytes that queued while the loop was busy and are
    // now parsed back to back. Dropping it (the old `return`) put the tick
    // count, and every LFO, permanently behind the machine. It still counts;
    // it just must not steer the tempo estimate.
    bool late = false;
    if (dt > 125000u) dt = periodUs_;      // gap: hold tempo, treat as restart
    else if (dt < 2000u) late = true;
    if (!late) {

    // Tempo estimate. The LFO rate rides on this, so DIN jitter here is exactly
    // the wobble you see in the BPM readout and feel in the sweep. Adaptive
    // smoothing kills it without going deaf to a real tempo change:
    //   acquiring lock -> ~8 ticks, converge fast
    //   locked, small error (jitter) -> ~64 ticks, rock-steady rate & readout
    //   locked, big error (you moved the tempo) -> ~4 ticks, snap to it
    // The phase-lock below stays at 1/32 and absorbs the difference, so a
    // steadier rate never costs alignment - the LFOs still land on the bar.
    const int32_t derr = (int32_t)dt - (int32_t)periodUs_;
    const int32_t ae   = derr < 0 ? -derr : derr;
    uint8_t shift;
    if (!locked_)                              shift = 3;   // acquire fast
    else if (ae > (int32_t)(periodUs_ >> 4))   shift = 2;   // >6%: real move
    else                                       shift = 6;   // jitter: hold steady
    periodUs_ = (uint32_t)((int32_t)periodUs_ + (derr >> shift));
    rateQ40_  = rate(periodUs_);
    }
    if (!running_) return;

    tick_++;
    // Pull 1/32 of the phase error per tick. Never move backwards — a
    // rewinding LFO is instantly audible.
    uint64_t target = tickToQ32(tick_);
    int64_t err = (int64_t)target - (int64_t)pos_;
    if (err > 0) pos_ += (uint64_t)(err >> 5);
    else if (err < 0) { uint64_t back = (uint64_t)((-err) >> 5);
                        if (pos_ > back) pos_ -= back; }
    if (lockCount_ < 24 && ++lockCount_ >= 24) locked_ = true;
  }

  void onStart(uint32_t nowUs) {
    tick_ = 0; pos_ = 0;
    running_ = true; startEdge_ = true;
    lastTickUs_ = lastUpdateUs_ = nowUs;
    lockCount_ = 0; locked_ = false;
  }
  void onContinue(uint32_t nowUs) {
    running_ = true; startEdge_ = true;
    lastTickUs_ = lastUpdateUs_ = nowUs;
  }
  void onStop(uint32_t) { running_ = false; stopEdge_ = true; }
  // Two taps of STOP on an Elektron means "back to the beginning", so the LFOs
  // should sit at phase 0 waiting.
  void rewind() { pos_ = 0; tick_ = 0; }
  void onSongPosition(uint16_t midiBeats) {     // a MIDI beat is a 16th note
    tick_ = (uint32_t)midiBeats * 6u;
    pos_  = tickToQ32(tick_);
  }

  void update(uint32_t nowUs) {
    uint32_t dt = nowUs - lastUpdateUs_;
    lastUpdateUs_ = nowUs;
    if (!running_ || !dt) return;
    if (dt > 100000u) dt = 100000u;
    pos_ += (rateQ40_ * (uint64_t)dt) >> 8;
  }

  // Internal clock, so the LFO can be exercised with no Monomachine attached.
  void setInternal(bool on, uint32_t nowUs) {
    internal_ = on;
    running_  = on;
    lastUpdateUs_ = nowUs;
    if (on) { pos_ = 0; tick_ = 0; locked_ = true; }
    else    { locked_ = false; lockCount_ = 0; }
  }
  void setBpm(float b) {
    if (b < 20.0f) b = 20.0f;
    if (b > 300.0f) b = 300.0f;
    periodUs_ = (uint32_t)(60000000.0f / (b * 24.0f));
    rateQ40_  = rate(periodUs_);
  }
  bool internal() const { return internal_; }
  uint32_t ticks() const { return tick_; }

  uint64_t position() const { return pos_; }
  bool     running()  const { return running_; }
  bool     locked()   const { return locked_; }
  uint32_t period()   const { return periodUs_; }
  // Beats-per-microsecond in Q40. The free-running position in loop() used to
  // recompute this as a 64-bit division on every single pass of the main loop —
  // roughly a hundred thousand of them a second for a number that changes only
  // when the tempo does. Exposed here so that becomes one multiply and a shift.
  uint64_t rateQ40()  const { return rateQ40_; }
  float    bpm()      const { return periodUs_ ? 60000000.0f /
                                     ((float)periodUs_ * 24.0f) : 0.0f; }
  bool takeStart() { bool v = startEdge_; startEdge_ = false; return v; }
  bool takeStop()  { bool v = stopEdge_;  stopEdge_  = false; return v; }

 private:
  static uint64_t rate(uint32_t periodUs) {      // beats per microsecond, Q40
    return periodUs ? (((1ULL << 40) / 24) / periodUs) : 0;
  }
  uint64_t pos_ = 0, rateQ40_ = 0;
  uint32_t lastUpdateUs_ = 0, lastTickUs_ = 0, periodUs_ = 20833;
  uint32_t tick_ = 0;
  bool running_ = false, locked_ = false, startEdge_ = false, stopEdge_ = false;
  bool internal_ = false;
  uint8_t lockCount_ = 0;
};

static SyncClock clk;

// ======================== incoming SysEx state machine =======================
//
// A byte-at-a-time parser. It is handed one byte per call and returns
// immediately; there is no buffering of a whole message before parsing, no
// waiting for 0xF7, and no loop of any kind.
//
// IT IS NEVER CALLED WITH A SYSTEM REAL-TIME BYTE. handleMidiByte() tests for
// 0xF8..0xFF first and returns, so clock, start, stop and active sense are
// dealt with and gone before this parser is reached. That is the whole fix for
// the frozen-LFO problem: because a Real-Time byte never enters the state
// machine, it cannot advance a state, land in a data buffer, or be mistaken
// for the 0xF7 that ends a message. The parser resumes on the next data byte
// with its state exactly as it was, and never knows the clock tick happened.
//
// Any OTHER status byte does abort the message in progress, which is what the
// MIDI spec requires: an unterminated SysEx is abandoned, not merged with
// whatever follows it.
//
// Turbo messages are recognised by their header and handed to the negotiator.
// Everything else is counted and discarded so its data bytes can never be read
// as channel messages.

static const uint8_t kTurboHdr[5] = {0x00, 0x20, 0x3C, 0x00, 0x00};

class SysexRx {
 public:
  // ABORTED means: a status byte ended an unterminated message, and that byte
  // has NOT been consumed - the caller still has to handle it as itself.
  enum Result : uint8_t { NONE = 0, TURBO_MSG = 1, FOREIGN_MSG = 2, ABORTED = 3 };

  void reset() { st_ = IDLE; hdr_ = 0; len_ = 0; over_ = false; }

  uint8_t feed(uint8_t b, uint32_t nowMs) {
    lastMs_ = nowMs;
    switch (st_) {
      case IDLE:
        if (b == 0xF0) { st_ = HDR; hdr_ = 0; }
        return NONE;

      case HDR:
        if (b == 0xF7) { st_ = IDLE; return FOREIGN_MSG; }   // empty sysex
        if (b & 0x80)  { st_ = IDLE; return ABORTED; }
        if (hdr_ < 3) {                    // 00 20 3C - the Elektron mfr ID
          if (b != kTurboHdr[hdr_]) { st_ = FOREIGN; return NONE; }
          ++hdr_;
          return NONE;
        }
        // Bytes 4 and 5 are a product / device id field, and this is the single
        // most likely reason a handshake goes nowhere. Insisting on 00 00 sends
        // every turbo message a machine stamps with its OWN id - 03 00 on a
        // Monomachine, 02 00 on a Machinedrum - straight into the FOREIGN
        // bucket, after which the negotiator sits waiting for a reply it has
        // already thrown away and then blindly changes speed on its own.
        //
        // Learn the field instead and echo it back, which is what a device-id
        // field is for. The command whitelist in CMD below is what stops a kit
        // dump from walking into the turbo parser now that this is wildcarded.
        peer_[hdr_ - 3] = b;
        if (++hdr_ >= 5) st_ = CMD;
        return NONE;

      case CMD:
        if (b == 0xF7) { st_ = IDLE; return FOREIGN_MSG; }
        if (b & 0x80)  { st_ = IDLE; return ABORTED; }
        // Only these are turbo. Everything else on the Elektron manufacturer ID
        // is a dump, a kit, a global - not ours, and not to be parsed as ours.
        // 0x14 / 0x15 are the speed test. They were missing, so every one the
        // machine sent went in the FOREIGN bucket and the handshake stalled
        // with no evidence anywhere that it had even been attempted.
        // v1.19: 0x10..0x17 is the whole handshake (0x16 / 0x17 are the second
        // test); the legacy TurboLight 0x20 / 0x21 are no longer handled.
        if (b < 0x10 || b > 0x17) { st_ = FOREIGN; return NONE; }
        peerKnown_ = true;                 // the id bytes we just learned are real
        cmd_ = b; len_ = 0; over_ = false; st_ = DATA;
        return NONE;

      case DATA:
        if (b == 0xF7) {
          st_ = IDLE;
          // A payload longer than any turbo message is not a turbo message,
          // whatever its header said. Report it as foreign rather than acting
          // on a command byte followed by a truncated argument list.
          return over_ ? FOREIGN_MSG : TURBO_MSG;
        }
        if (b & 0x80)  { st_ = IDLE; return ABORTED; }
        if (len_ < sizeof(data_)) data_[len_++] = b; else over_ = true;
        return NONE;

      default:                              // FOREIGN
        if (b == 0xF7) { st_ = IDLE; return FOREIGN_MSG; }
        if (b & 0x80)  { st_ = IDLE; return ABORTED; }
        return NONE;
    }
  }

  // A SysEx that simply stops - cable pulled, sender reset, machine powered
  // off mid-dump - leaves no terminator and no status byte behind it, so
  // nothing in feed() can ever fire. Without this the parser sits in DATA
  // forever and every subsequent data byte is swallowed. Called from loop().
  bool tick(uint32_t nowMs, uint32_t stallMs) {
    if (st_ == IDLE) return false;
    if ((uint32_t)(nowMs - lastMs_) < stallMs) return false;
    reset();
    return true;
  }

  bool active() const { return st_ != IDLE; }
  uint8_t cmd() const { return cmd_; }
  uint8_t len() const { return len_; }
  uint8_t at(uint8_t i) const { return i < len_ ? data_[i] : 0; }
  const uint8_t* data() const { return data_; }     // len() bytes of payload
  // The product / device id the peer stamps into bytes 4 and 5 of its turbo
  // messages. 00 00 until we have actually seen one, which is the right thing
  // to send first because that is what the TM-1 and MegaCommand use.
  uint8_t peerId(uint8_t i) const { return peer_[i & 1]; }
  bool    peerKnown() const { return peerKnown_; }

 private:
  enum St : uint8_t { IDLE, HDR, CMD, DATA, FOREIGN };
  St       st_  = IDLE;
  uint8_t  hdr_ = 0, cmd_ = 0, len_ = 0;
  bool     over_ = false;
  uint32_t lastMs_ = 0;
  uint8_t  data_[32];
  uint8_t  peer_[2] = {0x00, 0x00};
  bool     peerKnown_ = false;
};

static SysexRx sysex1;

// ============================= NRPN output path ==============================
//
// Budgeted, not throttled by a fixed rate: a token bucket sized from the live
// baud, urgency-ordered across LFOs, plus change detection. Nothing here
// touches the port directly — messages go into the CC queue and drain behind
// any pending notes.

// The manual's NRPN table is terse enough that more than one reading of it is
// defensible, and a wrong reading looks exactly like a dead cable. So all four
// live here and you switch with "mode" over serial until the Monomachine
// answers.
//
//   0  NRPN  99=0      98=track  6=yy     38=value
//   1  NRPN  99=track  98=yy     6=value             (textbook NRPN)
//   2  NRPN  99=0      98=yy     6=value             (channel picks the track)
//   3  CC    <ccNum>=value                           (plain CC)
// Mode 3 is the default, because Appendix B documents it exactly: CC 58 is
// Track 1 Amp Decay, on the track's own channel.
// Byte-level turbo logging. On by default while this is being commissioned -
// it only prints during a handshake, which is a handful of lines per attempt.
static bool turboVerbose = true;
static uint8_t txMode    = 3;
// (txChannel is defined near the top of the file - the pattern generator needs it.)
static uint8_t txCC      = 58;
// Track N listens on base + N-1.
static bool    txPerTrack = true;
static const char* const kTxModeName[4] = {
    "NRPN 99=0 98=trk 6=yy 38=val",
    "NRPN 99=trk 98=yy 6=val",
    "NRPN 99=0 98=yy 6=val",
    "CC <n>=val"};

class MnmOutput {
 public:
  void begin() {
    addrKey_ = 0xFFFFFFFFu; budget_ = 0; lastLen_ = 0; lastUs_ = 0;
    for (uint8_t i = 0; i < LFO_COUNT; ++i) slot_[i] = Slot();
  }
  void setBaud(uint32_t baud) {
    baud_        = baud;
    bytesPerSec_ = baud / 10u;               // MIDI is 8N1: ten bits per byte
    addrKey_ = 0xFFFFFFFFu;                  // re-address after a speed change
  }
  uint32_t wireBaud() const { return baud_; }

  void setParam(uint8_t i, uint8_t track, uint8_t yy, uint8_t value) {
    if (i >= LFO_COUNT) return;
    // An unmapped destination (the MIDI and MENV pages have no CC) used to
    // return here with the slot untouched, so the previous address stayed live
    // and we carried on writing to a parameter the user had already navigated
    // away from. Drop the slot instead.
    if (yy == 0xFF) { slot_[i] = Slot(); return; }
    Slot& s = slot_[i];
    if (!s.used || s.track != track || s.yy != yy) {
      // Re-addressed: force a send even if the value happens to match, so the
      // new destination hears from us immediately. Turning the DEST encoder
      // used to leave the new parameter untouched until the waveform happened
      // to move the value.
      s.used = true; s.track = track; s.yy = yy;
      s.value = value; s.dirty = true; s.sent = 0xFF;
      return;
    }
    if (s.value != value || s.sent == 0xFF) {
      s.value = value; s.dirty = (s.sent != value);
    }
  }
  void clearSlot(uint8_t i) { if (i < LFO_COUNT) slot_[i] = Slot(); }
  void resend() { for (uint8_t i = 0; i < LFO_COUNT; ++i)
                    if (slot_[i].used) { slot_[i].dirty = true;
                                         slot_[i].sent = 0xFF; }
                  addrKey_ = 0xFFFFFFFFu; }

  // The Monomachine turns sluggish if we keep its MIDI input pegged - it has to
  // parse everything we send while also running its own UI. So we take only 40%
  // of the port and cap EACH LFO to a fixed rate, slowing that cap right down
  // for a moment whenever the user is touching the Monomachine.
  void noteUserActivity(uint32_t nowUs) { backoffUntil_ = nowUs + 350000u; }

  void service(uint32_t nowUs) {
    uint32_t dt = nowUs - lastUs_;
    lastUs_ = nowUs;
    if (!dt) return;
    if (dt > 100000u) dt = 100000u;
    // Held in thousandths of a byte so slow refill keeps its precision. The
    // old form divided bytesPerSec_ by 1000 FIRST, which at 31250 baud threw
    // away 4% of the budget to integer truncation before it did anything else.
    budget_ += (uint32_t)(((uint64_t)bytesPerSec_ * dt * 40u) / 100000u);
    const uint32_t cap = LFO_COUNT * 12u * 1000u;
    if (budget_ > cap) budget_ = cap;

    // Per-LFO rate cap. The old value was a flat 50 Hz whatever was running,
    // which wasted most of the wire when one LFO was up: a full 0->127 sweep
    // over one bar at 120 BPM asks for 63 updates a second, so a lone LFO was
    // being deliberately stair-stepped for no reason. Share the available
    // update rate out among however many LFOs are actually driving, clamped to
    // something the far end can digest.
    uint8_t active = 0;
    for (uint8_t i = 0; i < LFO_COUNT; ++i) if (slot_[i].used) active++;
    if (!active) active = 1;
    const bool busy = (int32_t)(nowUs - backoffUntil_) < 0;
    uint32_t interval = busy ? 66000u : (uint32_t)(2500u * active);
    // Floor: 12 ms (80 Hz) on a standard link, 4 ms (250 Hz) once Turbo MIDI
    // has widened the wire. At 31250 baud 80 Hz per LFO is already most of the
    // port; at 8x the port stops being the constraint and the only reason to
    // hold back is how fast the Monomachine can absorb parameter changes.
    const uint32_t floorUs = (bytesPerSec_ > 6000u) ? 4000u : 12000u;
    if (interval < floorUs) interval = floorUs;
    if (interval > 66000u) interval = 66000u;

    // Pick by URGENCY, not by turn.
    //
    // Straight round-robin gives a stationary LFO exactly as many slots as one
    // sweeping the full range, so under pressure the fast one stair-steps while
    // the slow one spends its budget resending a value nobody can see change.
    // Score each slot by how far the Monomachine's idea of the parameter has
    // drifted from ours, breaking ties by how long it has waited, and send the
    // worst offender first.
    for (uint8_t guard = 0; guard < LFO_COUNT; ++guard) {
      if (budget_ < 3000u) break;
      uint8_t  best = 0xFF;
      uint32_t bestScore = 0;
      for (uint8_t i = 0; i < LFO_COUNT; ++i) {
        const Slot& s = slot_[i];
        if (!s.used || !s.dirty) continue;
        const uint32_t waited = nowUs - s.lastUs;
        if (waited < interval) continue;                       // rate cap
        const int32_t d = (int32_t)s.value -
                          (int32_t)(s.sent == 0xFF ? s.value : s.sent);
        const uint32_t err = (uint32_t)(d < 0 ? -d : d);
        const uint32_t score =
            err * 256u + (waited > 65535u ? 65535u : waited) / 4u + 1u;
        if (score > bestScore) { bestScore = score; best = i; }
      }
      if (best == 0xFF) break;
      if (!emit(best)) break;
      slot_[best].lastUs = nowUs;
    }
  }

  uint8_t  lastLen()   const { return lastLen_; }
  // The value last actually queued for a slot, or 0xFF if none. Used to tell
  // the Monomachine's own CC out apart from our echo when capturing.
  uint8_t  sentValue(uint8_t i) const { return i < LFO_COUNT ? slot_[i].sent : 0xFF; }
  // The one before it. An echo coming back through a THRU arrives a few
  // milliseconds after we queued the message, by which time we may already
  // have queued the next value — so matching against only the newest one
  // occasionally mistook our own output for a knob move.
  uint8_t  prevValue(uint8_t i) const { return i < LFO_COUNT ? slot_[i].prev : 0xFF; }
  uint8_t  lastByte(uint8_t i) const { return lastMsg_[i]; }

 private:
  struct Slot { bool used = false; uint8_t track = 0, yy = 0, value = 64,
                sent = 0xFF, prev = 0xFF; bool dirty = false;
                uint32_t lastUs = 0; };

  bool emit(uint8_t i) {
    Slot& s = slot_[i];
    if (!s.used || !s.dirty) return false;

    const uint32_t key = ((uint32_t)txMode << 24) |
                         ((uint32_t)(txChannel + (txPerTrack ? s.track : 0)) << 16) |
                         ((uint32_t)s.track << 8) | s.yy;
    const bool needAddr = (key != addrKey_);
    const uint8_t chan = (uint8_t)((txChannel - 1 + (txPerTrack ? s.track : 0)) & 0x0F);
    const uint8_t ch = (uint8_t)(0xB0 | (chan & 0x0F));
    uint8_t msg[12]; uint8_t k = 0;

    switch (txMode) {
      case 1:
        if (needAddr) { msg[k++]=ch; msg[k++]=99; msg[k++]=s.track;
                        msg[k++]=ch; msg[k++]=98; msg[k++]=s.yy; }
        msg[k++]=ch; msg[k++]=6; msg[k++]=s.value;
        break;
      case 2:
        if (needAddr) { msg[k++]=ch; msg[k++]=99; msg[k++]=0;
                        msg[k++]=ch; msg[k++]=98; msg[k++]=s.yy; }
        msg[k++]=ch; msg[k++]=6; msg[k++]=s.value;
        break;
      case 3:
        // In CC mode the slot's address field holds the CC number itself.
        msg[k++]=ch; msg[k++]=(uint8_t)(s.yy & 0x7F); msg[k++]=s.value;
        break;
      default:
        if (needAddr) { msg[k++]=ch; msg[k++]=99; msg[k++]=0;
                        msg[k++]=ch; msg[k++]=98; msg[k++]=s.track; }
        msg[k++]=ch; msg[k++]=6;  msg[k++]=s.yy;
        msg[k++]=ch; msg[k++]=38; msg[k++]=s.value;
        break;
      }

    // Queued, not written. A full queue means the wire is genuinely saturated,
    // in which case dropping one frame of a ramp is the right answer — we will
    // be back with a fresher value in a few milliseconds anyway.
    if (!qCtrl.push(msg, k)) return false;
    addrKey_ = key;
    lastLen_ = k;
    for (uint8_t n = 0; n < k; ++n) lastMsg_[n] = msg[n];
    s.prev = s.sent; s.sent = s.value; s.dirty = false;
    budget_ = (budget_ > (uint32_t)k * 1000u) ? budget_ - k * 1000u : 0;
    return true;
  }

  Slot     slot_[LFO_COUNT];
  uint8_t  lastLen_ = 0, lastMsg_[12] = {0};
  uint32_t addrKey_ = 0xFFFFFFFFu;
  uint32_t baud_ = 31250, bytesPerSec_ = 3125, budget_ = 0, lastUs_ = 0;
  uint32_t backoffUntil_ = 0;
};

static MnmOutput mnmOut;

// v1.18: THE ONE WAY TO MOVE THE BASE CHANNEL. The settings page and the 'ch'
// command both assigned txChannel directly - and every note-off and p-lock
// restore is addressed from txChannel at the moment it is SENT. A note playing
// when the channel moved got its note-off on the new channel, so it never
// stopped; a locked parameter was restored on the new channel and stayed
// locked on the old. Release everything on the old channel first. The stick
// forgets what it sent too: those values were for the old channels.
static void setBaseChannel(uint8_t ch) {
  if (ch < 1 || ch > 16 || ch == txChannel) return;
  patSilenceAll();              // note-offs and lock restores, old channel
  txChannel = ch;
  joyForget(true, true);
  mnmOut.resend();
}

// =============================================================================
// SCENES (v1.11) - FROST, FOG, DRILL
// =============================================================================
// Three complete starting points, each one written from a pattern blueprint
// for the Monomachine's own engines. A scene is four things:
//
//   NOTES   written by the pattern generator like any genre, as SCALE DEGREES,
//           so the key and scale controls still transpose it. The seed varies
//           the details - a bell a step early, an alternate kick pattern, a
//           ghost note that only sometimes plays - never the character.
//   LOCKS   per-step parameter locks over MIDI (see patApplyLocks): delay
//           throws, bit-crushed hits, portamento slides, filter jumps.
//   ROLLS   ratchets, played by the XY6 itself: 32nds, 16th triplets, 64ths.
//   SETUP   the shared-page sound (filter / amp / delay / crush) for every
//           track, and a configuration for all six XY6 LFOs. Sent ONLY when you
//           ask - E6 on the PAT page or 'scene' - because it replaces the LFO
//           setup you have and moves parameters on the machine.
//
// What the XY6 cannot do, and so does not pretend to: load machines (that is a
// kit SysEx the firmware does not write) or set the tempo (the Monomachine is
// the clock master). Each scene therefore carries a KIT SHEET - which machine
// goes on which track - and a tempo, and prints both ('scene info').
//
// Only the SHARED pages (AMP, FILTER, EFFECTS) are ever touched. Their CC
// numbers are the same for every machine; the SYNTH page's meaning changes
// with the machine, so a scene writing SYN1..SYN8 blind would be writing
// nonsense into half the kits it met.
//
// No scene LFO targets a parameter the same scene locks. They would fight: the
// LFO rewrites its destination continuously and would erase the lock.

struct SceneParam { uint8_t t, page, slot, val; };
// every: 0 = the LFO's trig steps follow its track's notes, N = a trig every N
// 16ths. Ignored for FREE, which never needs a trig.
struct SceneLfo   { uint8_t t, page, slot, wave, trig, spd, mult, depth, intl, every; };
struct SceneDef {
  const char* name;
  uint8_t root, scale, bars, bpm;
  const char* kit[PAT_TRACKS];
  const SceneParam* snd; uint8_t nSnd;
  SceneLfo lfo[6];
};

// Slot numbers, for reading the tables below:
//   AMP   0 ATCK 1 HOLD 2 DEC 3 REL 4 DIST 5 VOL 6 PAN 7 PORT
//   FILT  0 BASE 1 WDTH 2 HPQ 3 LPQ 4 ATCK 5 DEC 6 BOFS 7 WOFS
//   EFFX  0 EQF  1 EQG  2 SRR 3 DTIM 4 DSND 5 DFB 6 DFBS 7 DFWD

// ---- FROST: ambient, SAW II / Skee Mask's ambient side. 76 BPM, D minor. ----
static const SceneParam kSndFrost[] = {
  {0, PAGE_FILT, 0,  30}, {0, PAGE_FILT, 1,  90}, {0, PAGE_AMP, 1, 110},
  {0, PAGE_AMP,  3, 100}, {0, PAGE_AMP,  6,  64}, {0, PAGE_EFFX, 4, 50},
  {1, PAGE_EFFX, 4,  70}, {1, PAGE_EFFX, 5,  75}, {1, PAGE_EFFX, 3, 48},
  {1, PAGE_EFFX, 2,   0}, {1, PAGE_AMP,  6,  64},
  {2, PAGE_AMP,  7,  45}, {2, PAGE_AMP,  4,   0},
  {3, PAGE_FILT, 0,  50}, {3, PAGE_FILT, 1,  20}, {3, PAGE_FILT, 2, 90},
  {3, PAGE_FILT, 3,  90}, {3, PAGE_EFFX, 2,  20},
  {4, PAGE_FILT, 0,  60}, {4, PAGE_FILT, 1,  25}, {4, PAGE_AMP, 5,  70},
  {4, PAGE_AMP,  7,   0}, {4, PAGE_EFFX, 4,  90}, {4, PAGE_EFFX, 5, 80},
};
// ---- FOG: Skee Mask broken techno. 136 BPM, F minor, two-bar loop. ---------
static const SceneParam kSndFog[] = {
  {0, PAGE_AMP,  2,  40}, {0, PAGE_FILT, 0,  20},
  {1, PAGE_FILT, 0,  90}, {1, PAGE_FILT, 2,  30}, {1, PAGE_AMP, 2,  20},
  {1, PAGE_AMP,  6,  64}, {1, PAGE_EFFX, 2,  20},
  {2, PAGE_AMP,  7,  10}, {2, PAGE_FILT, 0,  20},
  {3, PAGE_EFFX, 4, 110}, {3, PAGE_EFFX, 3,  48}, {3, PAGE_EFFX, 5, 92},
  {3, PAGE_EFFX, 6,  40}, {3, PAGE_EFFX, 7,  50}, {3, PAGE_FILT, 0, 60},
  {4, PAGE_AMP,  5,  55}, {4, PAGE_AMP,  4,  10}, {4, PAGE_FILT, 1, 60},
  {5, PAGE_EFFX, 2,  50}, {5, PAGE_EFFX, 4,  30}, {5, PAGE_AMP, 2,  30},
};
// ---- DRILL: Aphex braindance, Drukqs melancholy. 164 BPM, A minor. ---------
static const SceneParam kSndDrill[] = {
  {0, PAGE_EFFX, 2,   0}, {0, PAGE_EFFX, 3,  40}, {0, PAGE_EFFX, 5, 30},
  {0, PAGE_EFFX, 4,   0},
  {1, PAGE_FILT, 0,  90}, {1, PAGE_AMP,  6,  64},
  {2, PAGE_EFFX, 4,  60}, {2, PAGE_EFFX, 5,  50}, {2, PAGE_AMP, 7,   0},
  {2, PAGE_FILT, 0,  70}, {2, PAGE_AMP,  6,  64},
  {3, PAGE_AMP,  7,  35}, {3, PAGE_FILT, 0,  40},
  {4, PAGE_AMP,  7,  30}, {4, PAGE_AMP,  4,  15}, {4, PAGE_AMP, 5, 110},
  {5, PAGE_EFFX, 4,  90}, {5, PAGE_EFFX, 3,  90}, {5, PAGE_FILT, 1, 60},
};

// LFO cycle length is 512 / (SPD x MULT) beats, MULT as the multiplier (the
// table holds its index: 0 = 1X, 1 = 2X, 2 = 4X, 3 = 8X, 4 = 16X, 5 = 32X).
// The odd lengths - 7/16 (4X, 73), 5/16 (4X, 102), 3/16 (8X, 85), ~6 bars
// (1X, 21), ~5 bars (1X, 26) - are the polyrhythms: none of them lines up with
// the bar, so the modulation never repeats the same way twice.
static const SceneDef kScene[3] = {
  { "FROST", 38 /*D2*/, PSC_MINOR, 4, 76,
    { "SWAVE-ENS   chord pad", "FM+STAT     bell melody", "SID-6581    sub drone",
      "GND-NOIS    wind", "VO-6        ghost voice",
      "FX-REVERB   route T1 T2 T5 into it (or GND-SIN)" },
    kSndFrost, (uint8_t)(sizeof kSndFrost / sizeof kSndFrost[0]),
    { {0, PAGE_FILT, 0, WAVE_TRI,   TRIG_FREE,  21, 0, 18,  0, 0},   // pad cutoff ~6 bars
      {0, PAGE_AMP,  6, WAVE_TRI_M, TRIG_FREE,  26, 0, 40,  0, 0},   // pad pan ~5 bars
      {1, PAGE_AMP,  6, WAVE_RND,   TRIG_HOLD,  64, 4, 50,  0, 0},   // bell: new pan per note
      {3, PAGE_FILT, 0, WAVE_SAW_M, TRIG_FREE,  32, 0, 30,  0, 0},   // wind gusts, 4 bars
      {3, PAGE_FILT, 1, WAVE_TRI,   TRIG_FREE,  64, 1, 12, 40, 0},   // stuttering breath
      {1, PAGE_EFFX, 3, WAVE_TRI,   TRIG_FREE,  16, 0,  3,  0, 0} } },// tape wobble, 8 bars
  { "FOG", 41 /*F2*/, PSC_MINOR, 2, 136,
    { "DPRO-BBOX   kick/snare (pitch + locks)", "GND-NOIS    hats", "SID-6581    bass",
      "FM+DYN      dub stab", "SWAVE-PULS  pad bed", "VO-6        vocal chops" },
    kSndFog, (uint8_t)(sizeof kSndFog / sizeof kSndFog[0]),
    { {1, PAGE_AMP,  2, WAVE_RND,   TRIG_HOLD,  64, 4, 15,  0, 0},   // hats: decay per hit
      {1, PAGE_EFFX, 2, WAVE_TRI,   TRIG_FREE,  73, 2, 20,  0, 0},   // hat crush, 7/16
      {1, PAGE_AMP,  6, WAVE_TRI_M, TRIG_FREE, 102, 2, 40,  0, 0},   // hat pan, 5/16
      {2, PAGE_FILT, 0, WAVE_EXP,   TRIG_ONE,   64, 4, 50,  0, 0},   // bass pluck, 1/8
      {3, PAGE_FILT, 0, WAVE_RND,   TRIG_HOLD,  64, 4, 25,  0, 0},   // stab bite per hit
      {4, PAGE_FILT, 1, WAVE_TRI,   TRIG_FREE,  16, 0, 20,  0, 0} } },// pad width, 8 bars
  { "DRILL", 33 /*A1*/, PSC_MINOR, 4, 164,
    { "DPRO-BBOX   drill kit", "GND-NOIS    micro hats", "FM+PAR      e-piano melody",
      "SID-6581    countermelody", "GND-SIN     sub", "DPRO-WAVE   pad" },
    kSndDrill, (uint8_t)(sizeof kSndDrill / sizeof kSndDrill[0]),
    { {1, PAGE_FILT, 0, WAVE_RND,   TRIG_HOLD,  64, 5, 30,  0, 0},   // hats: tone per hit
      {1, PAGE_AMP,  6, WAVE_TRI,   TRIG_FREE,  85, 3, 40,  0, 0},   // hat pan, 3/16
      {2, PAGE_FILT, 0, WAVE_EXP,   TRIG_ONE,   64, 3, 40,  0, 0},   // e-piano strike, 1 beat
      {2, PAGE_AMP,  6, WAVE_TRI_M, TRIG_FREE,  26, 0, 30,  0, 0},   // e-piano drift ~5 bars
      {3, PAGE_FILT, 0, WAVE_TRI,   TRIG_HALF,  64, 2, 40,  0, 0},   // counter: opens, stays
      {4, PAGE_AMP,  5, WAVE_RMP_M, TRIG_TRIG,  64, 3, 40,  0, 4} } } // sub pump on the beat
};
#define SCENE_OF(g) (kScene[(g) - PAT_FROST])

static bool sceneDefaults(uint8_t genre, uint8_t* root, uint8_t* scale, uint8_t* bars) {
  if (!patIsScene(genre)) return false;
  *root = SCENE_OF(genre).root; *scale = SCENE_OF(genre).scale; *bars = SCENE_OF(genre).bars;
  return true;
}
static bool patSceneBase(uint8_t t, uint8_t page, uint8_t slot, uint8_t* out) {
  if (!patIsScene(pat.genre)) return false;
  const SceneDef& sc = SCENE_OF(pat.genre);
  for (uint8_t i = 0; i < sc.nSnd; ++i)
    if (sc.snd[i].t == t && sc.snd[i].page == page && sc.snd[i].slot == slot) {
      *out = sc.snd[i].val; return true;
    }
  return false;
}

// ---- FROST generator --------------------------------------------------------
// A pad changing chord once, a seven-note bell line that lands differently
// every seed, a sub drone that glides, wind, and a voice that barely speaks.
static void patGenFrost() {
  // T1 PAD - two long chords: i, then III. The change blooms: the filter
  // opens and more of it goes to the delay.
  patSet(0, 0,  patScaleNote(0, 2), 80, 192);
  patSet(0, 32, patScaleNote(2, 2), 80, 192);
  patLock(0, 32, PAGE_FILT, 1, 110);
  patLock(0, 32, PAGE_EFFX, 4, 90);

  // T2 BELL - A F C D A G F over four bars (degree 7 is the octave). Only the
  // pentatonic degrees of the minor scale, so any swap stays consonant.
  static const uint8_t kPos[7] = {2, 10, 18, 29, 37, 48, 57};
  static const int8_t  kDeg[7] = {4, 2, 6, 7, 4, 3, 2};
  static const int8_t  kPenta[5] = {0, 2, 3, 4, 6};
  for (uint8_t i = 0; i < 7; ++i) {
    int16_t pos = kPos[i];
    if (i) { const uint32_t r = patRandN(10);          // drift a step, sometimes
             if (r < 2) pos--; else if (r >= 8) pos++; }
    int8_t deg = kDeg[i];
    if (deg < 7 && patRandPct(20)) {                   // a neighbouring tone
      uint8_t j = 0; while (j < 5 && kPenta[j] != deg) j++;
      if (j < 5) deg = kPenta[(j + (patRandPct(50) ? 1 : 4)) % 5];
    }
    patSet(1, (uint8_t)pos, patScaleNote(deg, 2), (uint8_t)((i & 1) ? 84 : 94), 18);
    // The fifth bell swells into feedback and dies before the sixth; the last
    // one is crushed, so the cycle ends broken.
    if (i == 4) { patLock(1, (uint8_t)pos, PAGE_EFFX, 5, 115);
                  patLock(1, (uint8_t)pos, PAGE_EFFX, 4, 110); }
    if (i == 6)   patLock(1, (uint8_t)pos, PAGE_EFFX, 2, 75);
  }
  if (patRandPct(60)) patSet(1, 44, patScaleNote(6, 2), 60, 12, 4, PC_PCT_50);

  // T3 SUB - D, gliding down to C for the second half (PORT is in the sound).
  patSet(2, 0,  patScaleNote(0, 0),  100, 192);
  patSet(2, 32, patScaleNote(-1, 0), 100, 192);
  patLock(2, 32, PAGE_AMP, 4, 20);                     // a quiet swell of DIST

  // T4 WIND - a held noise per half, the filter LFOs do the work.
  patSet(3, 0,  patScaleNote(0, 2), 70, 192);
  patSet(3, 32, patScaleNote(0, 2), 70, 192);

  // T5 VOICE - twice in four bars; the second slides into its vowel.
  patSet(4, 24, patScaleNote(4, 2), 70, 36);
  patSet(4, 56, patScaleNote(patRandPct(50) ? 2 : 3, 2), 70, 36);
  patLock(4, 56, PAGE_AMP, 7, 60);
  // T6 is the FX track: no notes.
}

// ---- FOG generator ----------------------------------------------------------
// Kicks that refuse the grid, hats in threes against the fours, a rubber bass
// with one slide, and a stab thrown into a delay that swallows half a bar.
static void patGenFog() {
  static const uint8_t kKick[3][6] = {{0, 7, 10, 16, 26, 0xFF},
                                      {0, 6, 10, 17, 26, 29},
                                      {0, 7, 11, 16, 24, 27}};
  static const uint8_t kSag[3] = {26, 26, 27};         // the kick that sags
  const uint8_t kick = patScaleNote(0, 1), snare = patScaleNote(4, 2);
  for (uint8_t blk = 0; blk * 32 < pat.bars * PAT_SPB; ++blk) {
    const uint8_t o = (uint8_t)(blk * 32);
    const uint8_t v = (uint8_t)patRandN(3);
    // T1 DRUMS - one BBOX track: kicks low, snares by pitch and a brighter
    // filter lock, and one kick dropped a minor sixth with a long decay.
    for (uint8_t i = 0; i < 6 && kKick[v][i] != 0xFF; ++i) {
      const uint8_t k = kKick[v][i];
      if (k == kSag[v]) { patSet(0, o + k, (uint8_t)(kick - 8), 110, 6);
                          patLock(0, o + k, PAGE_AMP, 2, 90); }
      else patSet(0, o + k, kick, (uint8_t)(k == 0 ? 118 : 110), 3, k == 0 ? 1 : 0);
    }
    for (uint8_t k = 4; k < 32; k += 8) { patSet(0, o + k, snare, 100, 4);
                                          patLock(0, o + k, PAGE_FILT, 0, 70); }
    // T2 HATS - every third 16th: eleven hits cycling against a 16-step bar.
    const uint8_t drop = patRandPct(50) ? (uint8_t)(3 * (1 + patRandN(4))) : 0xFF;
    for (uint8_t k = 0; k < 32; k += 3) {
      if (k == drop) continue;
      const uint8_t n = (uint8_t)(k / 3);
      patSet(1, o + k, 84, (uint8_t)((n & 1) ? 62 : 82), 2, (n % 3 == 2) ? 4 : 0);
    }
    for (uint8_t k = 15; k < 32; k += 15) {            // two hats that sing
      patLock(1, o + k, PAGE_FILT, 2, 110);
      patLock(1, o + k, PAGE_FILT, 0, 110);
    }
    // T3 BASS - syncopated roots of each bar's chord; the octave jump slides.
    static const uint8_t kB[8] = {2, 5, 9, 13, 14, 18, 21, 25};
    for (uint8_t i = 0; i < 8; ++i) {
      const uint8_t k = kB[i], bar = (uint8_t)((o + k) / PAT_SPB);
      uint8_t note = chordNote(bar, 0, 0), fl = 0;
      if (k == 14) { note = chordNote(bar, 0, 1); fl = 2; }
      if (k == 21 && patRandPct(30)) note = chordNote(bar, 2, 0);
      if (k == 2 || k == 18) fl |= 1;
      patSet(2, o + k, note, (uint8_t)((fl & 1) ? 112 : 98), (uint8_t)(k == 13 ? 3 : 4), fl);
    }
    patLock(2, o + 14, PAGE_AMP, 7, 70);
    if (patRandPct(50))
      patSet(2, o + 29, chordNote((uint8_t)((o + 29) / PAT_SPB), 3, 0), 70, 3, 4, PC_PCT_50);
    // T4 STAB - one stab and a ghost that becomes a dub throw.
    patSet(3, o + 6,  chordNote((uint8_t)((o + 6) / PAT_SPB), 0, 2), 105, 3);
    patSet(3, o + 22, chordNote((uint8_t)((o + 22) / PAT_SPB), 0, 2), 60, 3, 4);
    patLock(3, o + 22, PAGE_EFFX, 3, 24);
    patLock(3, o + 22, PAGE_EFFX, 5, 120);
    // T5 PAD - the third of each chord, a bar each.
    patSet(4, o + 0,  chordNote((uint8_t)(o / PAT_SPB), 1, 2), 60, 96);
    patSet(4, o + 16, chordNote((uint8_t)((o + 16) / PAT_SPB), 1, 2), 60, 96);
    // T6 VOX - three chops; the last dissolves into bit noise.
    patSet(5, o + 11, chordNote((uint8_t)((o + 11) / PAT_SPB), 2, 3), 85, 3);
    const uint8_t v2 = patRandPct(40) ? 20 : 19;
    patSet(5, o + v2, chordNote((uint8_t)((o + v2) / PAT_SPB), 0, 3), 85, 3);
    patSet(5, o + 29, chordNote((uint8_t)((o + 29) / PAT_SPB), 1, 3), 85, 3);
    patLock(5, o + 29, PAGE_EFFX, 2, 110);
    patLock(5, o + 29, PAGE_EFFX, 4, 100);
  }
}

// ---- DRILL generator --------------------------------------------------------
// A sad, plain e-piano melody over drums that cannot sit still: ratcheted
// snares falling in pitch, hat buzzes, and a comb-filter zap on one snare.
static void patGenDrill() {
  const uint8_t kick = patScaleNote(0, 1), snare = patScaleNote(4, 3);
  // T1 DRUMS
  static const uint8_t kK[6] = {0, 6, 10, 32, 38, 43}, kS[4] = {4, 12, 36, 44};
  for (uint8_t i = 0; i < 6; ++i) {
    const bool down = (kK[i] == 0 || kK[i] == 32);
    patSet(0, kK[i], kick, (uint8_t)(down ? 120 : 110), 3, down ? 1 : 0);
  }
  if (patRandPct(50)) patSet(0, patRandPct(50) ? 22 : 54, kick, 80, 3, 4, PC_PCT_50);
  for (uint8_t i = 0; i < 4; ++i) patSet(0, kS[i], snare, 105, 4);
  patLock(0, 12, PAGE_EFFX, 2, 90);                    // the zap: crushed,
  patLock(0, 12, PAGE_EFFX, 3, 1);                     //   shortest delay,
  patLock(0, 12, PAGE_EFFX, 5, 115);                   //   feedback near
  patLock(0, 12, PAGE_EFFX, 4, 100);                   //   runaway
  patSet(0, 14, snare, 80, 2); patRoll(0, 14, patRandPct(50) ? 3 : 2);
  patSet(0, 15, snare, 92, 2); patRoll(0, 15, 4);
  for (uint8_t i = 1; i <= 3; ++i) {                   // the falling roll
    patSet(0, (uint8_t)(44 + i), (uint8_t)(snare - 2 * i), 90, 2);
    patRoll(0, (uint8_t)(44 + i), (uint8_t)(1 + i));
  }
  // T2 HATS - 16ths with stumbles, accents on the beat, buzzes at the ends.
  uint8_t gap[6] = {3, 8, 21, 39, 0xFF, 0xFF};
  for (uint8_t g = 4; g < 6; ++g) {                    // two more, off the beat
    const uint8_t k = (uint8_t)(1 + patRandN(63));
    if ((k & 3) && k != 14 && k != 30 && k != 40 && k != 46 && k != 62) gap[g] = k;
  }
  for (uint8_t k = 0; k < PAT_STEPS; ++k) {
    bool skip = false;
    for (uint8_t g = 0; g < 6; ++g) if (gap[g] == k) skip = true;
    if (skip) continue;
    const uint8_t vel = (k & 3) == 0 ? 90 : ((k & 1) == 0 ? 60 : 45);
    patSet(1, k, 84, vel, 2, vel == 45 ? 4 : 0);
  }
  patRoll(1, 14, 4); patRoll(1, 30, 2); patRoll(1, 46, 4); patRoll(1, 62, 4);
  patRoll(1, 40, 3);
  // T3 E-PIANO - A C E D B G E . A C F E D . C. Plain on purpose.
  static const uint8_t kP[13] = {0, 3, 6, 10, 16, 19, 26, 32, 35, 38, 42, 48, 58};
  static const int8_t  kD[13] = {0, 2, 4, 3, 1, 6, 4, 0, 2, 5, 4, 3, 2};
  static const int8_t  kO[13] = {3, 3, 3, 3, 3, 2, 2, 3, 3, 3, 3, 3, 3};
  for (uint8_t i = 0; i < 13; ++i) {
    int8_t deg = kD[i];
    if (kP[i] == 16 && patRandPct(30)) deg = 2;        // B becomes C, sometimes
    const bool held = (kP[i] == 26 || kP[i] == 58);
    patSet(2, kP[i], patScaleNote(deg, kO[i]), 95, (uint8_t)(held ? 48 : 10),
           (kP[i] == 0 || kP[i] == 32) ? 1 : 0);
  }
  if (patRandPct(50)) patSet(2, 37, patScaleNote(4, 3), 55, 4, 4, PC_PCT_50);
  patLock(2, 26, PAGE_EFFX, 4, 120);                   // the held note echoes
  patLock(2, 26, PAGE_EFFX, 5, 100);                   //   into the break
  patLock(2, 58, PAGE_EFFX, 4, 100);                   // the last one slides in
  patLock(2, 58, PAGE_AMP,  7, 40);
  // T4 COUNTERMELODY - answers in the gaps, stepping down.
  static const uint8_t kC[4] = {12, 28, 44, 60};
  for (uint8_t i = 0; i < 4; ++i)
    patSet(3, kC[i], patScaleNote((int8_t)(6 - i), 3), 88, 12);
  // T5 SUB - A, F, D with a dive into the last.
  patSet(4, 0,  patScaleNote(0, 0),  110, 192);
  patSet(4, 32, patScaleNote(-2, 0), 110, 96);
  patSet(4, 48, patScaleNote(-4, 0), 110, 96);
  patLock(4, 48, PAGE_AMP, 7, 80);
  // T6 PAD - the chord root, two long notes.
  patSet(5, 0,  chordNote(0, 0, 2), 60, 192);
  patSet(5, 32, chordNote(2, 0, 2), 60, 192);
}

static void patGenScene(uint8_t genre) {
  switch (genre) {
    case PAT_FROST: patGenFrost(); break;
    case PAT_FOG:   patGenFog();   break;
    case PAT_DRILL: patGenDrill(); break;
    default: break;
  }
}

// ---- apply: the sound and the six LFOs ---------------------------------------
static uint32_t g_sceneSentMs = 0;      // for the PAT page's "SENT" confirmation

static bool sceneApply() {
  if (!patIsScene(pat.genre)) return false;
  const SceneDef& sc = SCENE_OF(pat.genre);
  // The sound. CC queue: these are settings, not timing - they can wait for
  // the notes, and ~20 of them are 60 bytes, well inside the queue.
  for (uint8_t i = 0; i < sc.nSnd; ++i) {
    const SceneParam& q = sc.snd[i];
    const uint8_t cc = mnmCC(q.page, q.slot);
    if (cc == 0xFF) continue;
    const uint8_t m[3] = {(uint8_t)(0xB0 | patChan(q.t)), cc, (uint8_t)(q.val & 0x7F)};
    if (qCtrl.push(m, 3)) kitSet(q.t, q.page, q.slot, q.val);   // v1.22
  }
  // The LFOs. Each one's base is the value the sound table just sent, so it
  // swings around the sound the scene designed rather than a stale capture.
  const uint8_t bars = (pat.bars >= 1 && pat.bars <= 4) ? pat.bars : 4;
  for (uint8_t i = 0; i < LFO_COUNT && i < 6; ++i) {
    const SceneLfo& L = sc.lfo[i];
    LfoParams& p = lfo.p[i];
    p.enabled = true;
    p.track = L.t; p.page = L.page; p.dest = L.slot;
    p.wave = L.wave; p.trig = L.trig; p.spd = L.spd; p.mult = L.mult;
    p.depth = L.depth; p.intl = L.intl; p.amount = 127;
    p.lo = 0; p.hi = 127; p.bars = bars; p.stepCount = 16;
    for (uint8_t k = 0; k < LFO_STEPS; ++k) {
      LfoStep& st = p.steps[k];
      st.cond = COND_NONE; st.condParam = 100; st.prob = PROB_100;
      st.on = 0;
      if (L.trig != TRIG_FREE && k < bars * 16)
        st.on = L.every ? ((k % L.every) == 0) : pat.step[L.t][k].on;
    }
    // v1.22: where the scene's sound table says nothing, the kit model's
    // value (learned, or the init default) - not a blanket 64.
    uint8_t base = kitGet(L.t, L.page, L.slot);
    patSceneBase(L.t, L.page, L.slot, &base);
    p.baseValue = base;
    lfo.setCapture(i, base);
  }
  mnmOut.resend();
  g_sceneSentMs = millis();
  uiTouch();
  return true;
}

static void scenePrintSheet() {
  if (!patIsScene(pat.genre)) return;
  const SceneDef& sc = SCENE_OF(pat.genre);
  usbWait(128);
  Serial.printf("\nSCENE %s   set the Monomachine to %u BPM\n", sc.name, sc.bpm);
  Serial.println("  kit - load these machines (the XY6 cannot):");
  for (uint8_t t = 0; t < PAT_TRACKS; ++t) {
    usbWait(96);
    Serial.printf("    T%u  %s\n", t + 1, sc.kit[t]);
  }
  usbWait(128);
  Serial.printf("  %u locks, %u sound settings, 6 LFOs.  E6 on PAT (or 'scene')\n"
                "  sends the sound and loads the LFOs - it REPLACES LFOs 1-6.\n",
                patLockN, sc.nSnd);
}

// =============================== Turbo MIDI ==================================
//
// v1.19: THE INITIATOR, BUILT TO THE PROTOCOL AS CAPTURED
//
// Elektron's TurboMIDI runs the same 8-N-1 DIN link at a multiple of 31250
// baud. Every control message is
//
//     F0  00 20 3C 00 00  <cmd>  <data...>  F7
//
// and the XY6 now drives the handshake itself, one state per step:
//
//  step  state        what happens                                     UART
//   1    OFF          ordinary MIDI                                    31250
//   2    REQUEST      -> 10                                            31250
//   3    WAIT_CAPS    <- 11 7F 01 0F 00       speeds on offer          31250
//   4    NEGOTIATE    -> 12 08 07             SPEED1 10x, SPEED2 8x    31250
//   5    WAIT_ACK     <- 13                                            31250
//   6    SWITCH1      UART to SPEED1 on the ACK's F7, then 16 raw 00   312500
//   7    FIRST_TEST   -> 14 55 55 55 55 00 00 00 00                    312500
//   8    WAIT_ECHO    <- 15 55 55 55 55 00 00 00 00                    312500
//   9    SECOND_TEST  -> 16                                            312500
//  10    WAIT_RESULT  <- 17                                            312500
//  11    FINALIZE     UART to SPEED2 on the result's F7                250000
//        SETTLE       10 ms, nothing transmitted                       250000
//        LOCKED       ordinary MIDI, plus FE every 150 ms              250000
//        REVERT       UART back to 31250, keepalive stopped            31250
//
// The link is proven at 10x and run at 8x, so it has margin the moment it
// goes live. SPEED1 / SPEED2 are TURBO_SPEED1_CODE / TURBO_SPEED2_CODE in the
// config block; the codes are indices into kTmSpeeds[].
//
// HOW IT SITS IN THE LOOP. Nothing here waits. Every state is a test against
// a deadline, a received message or the UART's own idle flag, ticked from
// loop() by service() and, for the replies, from handleMidiByte() the moment
// the message's F7 lands - so the baud switch in steps 6 and 11 happens on
// the pass that parses the F7, not a frame later.
//
// WHAT THE REST OF THE BOX SEES:
//   * From step 4 until LOCKED - and all through REVERT - the negotiator HOLDS
//     THE WIRE: midiTxService() sends only its messages (qTurbo); notes and
//     CCs wait in their queues, patTick skips steps, the stick stands still.
//     A byte sent at the wrong baud is garbage to the machine, and a note-on
//     among the garbage is a stuck note. Before step 4 nothing is held: the
//     request and the capability answer are ordinary 1x SysEx.
//   * Nothing ever switches the UART with a byte still in it: every switch
//     waits for qTurbo, the software ring AND the LPUART's transmit-complete
//     flag (tmTxIdle) - bounded by the step timeout.
//   * The keepalive (FE) is sent by midiTxService(), not from here - see
//     g_tmKeepalive by the TX queues.
//
// WHEN IT GOES WRONG. Any step that times out, or a reply that is wrong, ends
// the attempt. Before the first switch (steps 2-5) nothing has changed, so the
// wire is simply released at 1x. From step 6 on, REVERT: wait for the UART to
// drain, back to 31250, keepalive off, then hold the wire TURBO_REVERT_HOLD_MS
// - long enough for the machine's own active-sensing timeout to put it back
// at 1x too - and clear every channel with All Notes Off. Once LOCKED, the
// same REVERT runs if received traffic stops parsing (the machine is at a
// different speed) or if a machine that WAS sending its own keepalive goes
// silent for TURBO_PEER_SILENT_MS.
//
// v1.20: AND THE RESPONDER. A Monomachine with TURBO enabled sends 10 on its
// own, every few seconds, looking for a TM-1 - so the XY6 answers too, the
// same exchange with the roles swapped:
//
//        R_WAIT_SET   <- 10        -> 11 7F 01 0F 00                    31250
//        R_ACK        <- 12 s1 s2  -> 13, UART to s1 once it has left     31250
//        R_WAIT_TEST  <- 00 x16, 14 pattern   -> 15 the same pattern    s1
//        R_WAIT_TEST2 <- 16        -> 17, then FINALIZE / SETTLE / LOCKED
//                                     at s2, as above                   s2
//
// The machine picks the speeds. Answering is on from boot; 'turbo off' stops
// it until 'turbo'. If both ends start at once, the machine leads. A step at
// the new speed that gets no reply is reported with what DID arrive since the
// switch (see reportLine) - which tells a silent machine, a speed mismatch or
// a too-slow MIDI IN optocoupler apart.

static const uint32_t kTmSpeeds[12] = {31250,  31250,  62500,  104062, 125000,
                                       156250, 208125, 250000, 312500, 415625,
                                       500000, 625000};
static const char* const kTmNames[12] = {"1X","1X","2X","3.3X","4X","5X",
                                         "6.7X","8X","10X","13X","16X","20X"};
// Three characters at most, because the badge shares the header line with a
// page title and one other field. "3.3X" would not fit; "3X" reads fine there
// and the exact figure is a `turbo ?` away.
static const char* const kTmBadge[12] = {"1X","1X","2X","3X","4X","5X",
                                         "6X","8X","10X","13X","16X","20X"};

static const uint8_t TM_CMD_SPEED_REQ    = 0x10;   // -> what speeds?
static const uint8_t TM_CMD_SPEED_ANS    = 0x11;   // <- these
static const uint8_t TM_CMD_SPEED_SET    = 0x12;   // -> SPEED1, SPEED2
static const uint8_t TM_CMD_SPEED_ACK    = 0x13;   // <- agreed; both go to SPEED1
static const uint8_t TM_CMD_SPEED_TEST   = 0x14;   // -> test pattern at SPEED1
static const uint8_t TM_CMD_SPEED_RES    = 0x15;   // <- the pattern, echoed
static const uint8_t TM_CMD_SPEED_TEST2  = 0x16;   // -> second test
static const uint8_t TM_CMD_SPEED_RES2   = 0x17;   // <- result; both go to SPEED2

// 0x55 is alternating bits, the pattern a UART misframes first at a wrong baud.
static const uint8_t kTmTestPattern[8] = {0x55, 0x55, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00};
// The capability answer, as a Monomachine gives it: 1x..10x on offer. The
// initiator compares the machine's answer with it (for the log only - the
// check is that SPEED1 and SPEED2 are offered); the responder answers with it.
static const uint8_t kTmCaps[4] = {0x7F, 0x01, 0x0F, 0x00};

// v1.21: 'turbo loop' - the speeds it walks (1x..10x), and the bytes it sends
// at each: the real speed-test SysEx, then sixteen more with every kind of edge
// pattern in them. Anything the MIDI IN smears shows.
static const uint8_t kLoopCodes[8] = {1, 2, 3, 4, 5, 6, 7, 8};
static const uint8_t kLoopPattern[32] = {
    0xF0, 0x00, 0x20, 0x3C, 0x00, 0x00, 0x14, 0x55, 0x55, 0x55, 0x55,
    0x00, 0x00, 0x00, 0x00, 0xF7,
    0x2A, 0x33, 0x66, 0x4C, 0x19, 0x0F, 0x70, 0x01,
    0x7E, 0x5A, 0x69, 0x12, 0x24, 0x48, 0xB0, 0x7F};

class TurboMidi {
 public:
  enum St : uint8_t { OFF, REQUEST, WAIT_CAPS, NEGOTIATE, WAIT_ACK, SWITCH1,
                      FIRST_TEST, WAIT_ECHO, SECOND_TEST, WAIT_RESULT,
                      FINALIZE, SETTLE, LOCKED, REVERT,
                      // v1.20: the responder - the machine leads, we answer
                      R_WAIT_SET, R_ACK, R_WAIT_TEST, R_WAIT_TEST2,
                      // v1.21: 'turbo loop', the MIDI IN self-test
                      LOOP };

  // txMax is Serial1.availableForWrite() on an idle port - how the negotiator
  // recognises that the software ring has gone empty.
  void begin(int txMax) { txMax_ = txMax; st_ = OFF; cur_ = 1; }

  uint8_t  speedIdx()  const { return cur_; }
  uint32_t baud()      const { return kTmSpeeds[cur_ < 12 ? cur_ : 0]; }
  const char* speedName() const { return kTmNames[cur_ < 12 ? cur_ : 0]; }
  bool     locked()    const { return st_ == LOCKED; }
  bool     negotiating() const { return st_ != OFF && st_ != LOCKED; }
  // Steps 4..11, REVERT, and the responder from its ACK on: only the
  // negotiator's own bytes may move.
  bool     holdsWire() const {
    return (st_ >= NEGOTIATE && st_ <= SETTLE) || st_ == REVERT ||
           st_ == R_ACK || st_ == R_WAIT_TEST || st_ == R_WAIT_TEST2 || st_ == LOOP;
  }
  // v1.21: THE SPEED CEILING ('turbo max'). The fastest speed code this
  // XY6 will use - asked for when it leads, offered when the machine does.
  // SPEED1 (the test) is the ceiling, SPEED2 (the run) one step below it:
  // at the default that is exactly 10x / 8x. Lower it when the MIDI IN
  // cannot receive the top speeds cleanly - 'turbo loop' measures that.
  uint8_t  maxCode()    const { return maxCode_; }
  uint8_t  speed1Code() const { return maxCode_; }
  uint8_t  speed2Code() const { return maxCode_ > 2 ? (uint8_t)(maxCode_ - 1) : maxCode_; }
  void     setMax(uint8_t code) {
    if (code >= 2 && code <= TURBO_SPEED1_CODE) maxCode_ = code;
  }
  bool     looping() const { return st_ == LOOP; }
  // Whether a handshake the machine starts is answered. On from boot, so a
  // machine with TURBO enabled brings the link up by itself, as with a TM-1;
  // 'turbo off' clears it, 'turbo' sets it again.
  bool     answersMachine() const { return allow_; }
  const char* stateName() const {
    switch (st_) {
      case REQUEST:     return "REQUEST";
      case WAIT_CAPS:   return "WAIT CAPS";
      case NEGOTIATE:   return "NEGOTIATE";
      case WAIT_ACK:    return "WAIT ACK";
      case SWITCH1:     return "SWITCH1";
      case FIRST_TEST:  return "TEST 1";
      case WAIT_ECHO:   return "WAIT ECHO";
      case SECOND_TEST: return "TEST 2";
      case WAIT_RESULT: return "WAIT RES";
      case FINALIZE:    return "FINALIZE";
      case SETTLE:      return "SETTLE";
      case LOCKED:      return "LOCKED";
      case REVERT:      return "REVERT";
      case R_WAIT_SET:  return "R WAIT SET";
      case R_ACK:       return "R ACK";
      case R_WAIT_TEST: return "R WAIT TEST";
      case R_WAIT_TEST2:return "R WAIT TEST2";
      case LOOP:        return "LOOP TEST";
      default:          return "OFF";
    }
  }
  // What the status badge shows, and how loudly.
  //   0 = base speed, plain text     1 = negotiating, outlined
  //   2 = locked above 1x, inverted chip
  uint8_t badgeStyle() const {
    if (st_ == LOCKED && cur_ > 1) return 2;
    if (negotiating()) return 1;
    return 0;
  }
  const char* badgeText() const {
    if (negotiating()) return "NEG";
    return kTmBadge[cur_ < 12 ? cur_ : 0];
  }

  // ---- what the receive path tells us ---------------------------------------
  // Every byte that arrives, valid or not. The LOCKED watchdog needs to tell
  // "the line is idle" apart from "the line is carrying nonsense".
  // v1.20: the first bytes after each switch are kept, raw, for the report
  // when a step at the new speed times out.
  void noteAnyByte(uint8_t b) {
    if (anyRx_ < 0xFFFF) anyRx_++;
    if (b == 0xF0 && f0N_ < 0xFFFF) f0N_++;          // v1.21: SysEx that STARTED
    if (rawN_ < sizeof raw_) raw_[rawN_++] = b;
  }
  // Bytes whose exact value we can name. At the wrong baud a stream of
  // misframed noise produces status-looking bytes all day, so only these count.
  // FE from the machine also tells us it keeps the link alive itself - after
  // which its silence means the link has gone.
  void noteLiveByte(uint8_t b, uint32_t nowMs) {
    if (b == 0xF8 || b == 0xFA || b == 0xFB || b == 0xFC || b == 0xFE) {
      lastRxMs_ = nowMs;
      if (rxSeen_ < 0xFFFF) rxSeen_++;
      if (b == 0xFE && st_ == LOCKED) peerKeepalive_ = true;
    }
  }
  // A complete, correctly-headed turbo message is the strongest liveness
  // evidence there is - noise does not counterfeit six header bytes.
  void noteTurboSeen(uint32_t nowMs) { lastRxMs_ = nowMs;
                                       if (rxSeen_ < 0xFFFF) rxSeen_++;
                                       if (msgN_ < 0xFFFF) msgN_++; }   // ...and ARRIVED

  // v1.21: 'turbo loop' - every byte received while the self-test runs comes
  // here instead of the MIDI parsers (see handleMidiByte).
  void loopByte(uint8_t b) { if (loopRxN_ < sizeof loopRx_) loopRx_[loopRxN_++] = b; }

  // ---- what the UI and the console ask for ----------------------------------
  // Start the handshake: 'turbo', or SET > TURBO > ENGAGE.
  void start(uint32_t nowMs) {
    allow_ = true; refusedLogged_ = false;
    if (st_ == LOCKED) { tmSerial.printf("turbo: already at %s\n", speedName()); return; }
    if (negotiating()) { tmSerial.println(F("turbo: busy, try again in a moment")); return; }
    if (sweeping_)     { tmSerial.println(F("turbo: sweep running - try again after it")); return; }
    tmSerial.printf("turbo: requesting SPEED1 %s (test) / SPEED2 %s (run)\n",
                    kTmNames[speed1Code()], kTmNames[speed2Code()]);
    enter(REQUEST, nowMs);
    advance(nowMs);
  }
  // v1.21: 'turbo loop'. Patch the XY6's MIDI OUT 1 to its own MIDI IN 1
  // (machine unplugged): every speed from 1x to 10x in turn, the same bytes
  // out and back, and what came back intact. It measures the one thing a
  // handshake cannot tell apart from a machine problem - whether this MIDI IN
  // can receive the speed at all.
  void startLoop(uint32_t nowMs) {
    if (st_ != OFF) { tmSerial.println(F("turbo loop: only from 1X, idle - 'turbo off' first")); return; }
    tmSerial.println(F("turbo loop: OUT 1 must be patched to IN 1 (machine unplugged)."));
    loopIdx_ = 0; loopPhase_ = 0; loopBest_ = 0; loopBroken_ = false;
    enter(LOOP, nowMs);
    advance(nowMs);
  }
  // Back to 1x: 'turbo off', or ENGAGE with SPEED set to 1X.
  void stop(uint32_t nowMs) {
    allow_ = false; refusedLogged_ = false;
    if (st_ == OFF) { tmSerial.println(F("turbo: at 1X, and the machine's requests will"
                                         " not be answered until 'turbo'")); return; }
    if (st_ == REVERT) return;
    fail("turbo off requested", nowMs);
  }
  // Switch with no handshake at all - a diagnostic, for a machine already
  // sitting at that speed. Keepalive and watchdog run as for a real lock.
  void forceSpeed(uint8_t code, uint32_t nowMs) {
    if (code < 1 || code > 11 || negotiating()) return;
    if (code == 1) { stop(nowMs); return; }
    tmSerial.printf("turbo: FORCING %s with no handshake\n", kTmNames[code]);
    g_tmKeepalive = false;       // quiet through the switch and the settle,
    target_ = code;              //   even when forced from a live link
    enter(FINALIZE, nowMs);
    advance(nowMs);
  }

  // ---- the device-id sweep ('turbo sweep') ----------------------------------
  // For a machine that never answers step 2: walk byte 4 of the header through
  // all 128 values, one 0x10 every 60 ms, and watch for ANY 0x11. The receive
  // side learns the id an answer arrives with (SysexRx::peerId), and every
  // message after that is stamped with it.
  void startSweep(uint32_t nowMs) {
    if (st_ != OFF) { tmSerial.println(F("turbo: sweep only from 1X, idle")); return; }
    sweepId_ = 0; sweeping_ = true; deadline_ = nowMs;
    tmSerial.println(F("turbo: sweeping device-id byte 4 = 00..7F, 60 ms apart,\n"
                       "       watching for any 0x11 answer."));
  }
  bool sweeping() const { return sweeping_; }

  // ---- a complete turbo message from SysexRx --------------------------------
  // Called from handleMidiByte() on the message's F7 - which may be inside the
  // display flush, so this only logs to the deferred ring and never blocks.
  void onMessage(uint8_t cmd, uint8_t len, const uint8_t* d, uint32_t nowMs) {
    logRx(cmd, len, d);
    switch (cmd) {
      case TM_CMD_SPEED_ANS:                                        // step 3
        if (sweeping_) { sweeping_ = false;
                         tmSerial.printf(">> TURBO SWEEP: answered. Peer id %02X %02X is"
                                         " learned and used from now on.\n",
                                         sysex1.peerId(0), sysex1.peerId(1)); }
        if (st_ != WAIT_CAPS) return;
        if (!capsOffer(len, d)) {
          tmSerial.printf("turbo: the machine does not offer %s and %s - staying at 1X.\n"
                          "       Check its GLOBAL > TURBO setting, or lower 'turbo max'.\n",
                          kTmNames[speed1Code()], kTmNames[speed2Code()]);
          fail(nullptr, nowMs);
          return;
        }
        enter(NEGOTIATE, nowMs);
        break;
      case TM_CMD_SPEED_ACK:                                        // step 5
        if (st_ != WAIT_ACK) return;
        enter(SWITCH1, nowMs);
        break;
      case TM_CMD_SPEED_RES:                                        // step 8
        if (st_ != WAIT_ECHO) return;
        if (len != sizeof kTmTestPattern ||
            memcmp(d, kTmTestPattern, sizeof kTmTestPattern) != 0) {
          fail("speed test came back wrong - the two ends are not framing"
               " the same at SPEED1", nowMs);
          return;
        }
        enter(SECOND_TEST, nowMs);
        break;
      case TM_CMD_SPEED_RES2:                                       // step 10
        if (st_ != WAIT_RESULT) return;
        target_ = speed2Code();
        enter(FINALIZE, nowMs);
        break;
      // ---- the responder (v1.20): the machine leads, we answer -------------
      // The same exchange as above with the roles swapped - what a TM-1 does.
      case TM_CMD_SPEED_REQ:                                        // <- 10
        if (!machineMayLead()) return;
        {
          // What we offer: codes 1..maxCode_, bit (code - 1), 7 bits a byte -
          // at the default ceiling exactly the 7F 01 0F 00 a machine gives.
          const uint16_t mask = (uint16_t)((1u << maxCode_) - 1u);
          const uint8_t caps[4] = {(uint8_t)(mask & 0x7F), (uint8_t)((mask >> 7) & 0x7F),
                                   (uint8_t)(kTmCaps[2] & mask), (uint8_t)(kTmCaps[3] & (mask >> 7))};
          if (!sendCmd(TM_CMD_SPEED_ANS, caps, sizeof caps)) return;
        }
        tmSerial.printf(">> TURBO: the machine asks what we support - answered up to %s\n",
                        kTmNames[maxCode_]);
        enter(R_WAIT_SET, nowMs);
        break;
      case TM_CMD_SPEED_SET:                                        // <- 12 s1 s2
        if (st_ != R_WAIT_SET && !machineMayLead()) return;
        if (len < 2 || d[0] < 1 || d[0] > 11 || d[1] < 1 || d[1] > 11) {
          tmSerial.println(F("turbo: the machine's 12 names a speed we do not know - ignored"));
          if (st_ == R_WAIT_SET) fail(nullptr, nowMs);
          return;
        }
        if (d[0] > maxCode_ || d[1] > maxCode_) {
          tmSerial.printf("turbo: the machine asks for %s / %s, above 'turbo max' %s -"
                          " not acknowledged\n", kTmNames[d[0]], kTmNames[d[1]], kTmNames[maxCode_]);
          if (st_ == R_WAIT_SET) fail(nullptr, nowMs);
          return;
        }
        speed1_ = d[0]; target_ = d[1];
        if (!sendCmd(TM_CMD_SPEED_ACK, nullptr, 0)) return;
        tmSerial.printf(">> TURBO: the machine asks for %s (test) / %s (run) - acknowledged\n",
                        kTmNames[speed1_], kTmNames[target_]);
        enter(R_ACK, nowMs);          // switches once our 13 has left the UART
        break;
      case TM_CMD_SPEED_TEST:                                       // <- 14 pattern
        if (st_ != R_WAIT_TEST) return;
        // Echo exactly what arrived: the machine is the judge of whether it
        // survived the new speed.
        if (!sendCmd(TM_CMD_SPEED_RES, d, len > 16 ? 16 : len)) return;
        enter(R_WAIT_TEST2, nowMs);
        break;
      case TM_CMD_SPEED_TEST2:                                      // <- 16
        if (st_ != R_WAIT_TEST2) return;
        if (!sendCmd(TM_CMD_SPEED_RES2, nullptr, 0)) return;
        enter(FINALIZE, nowMs);       // to SPEED2 once our 17 has left the UART
        break;
      default:
        return;
    }
    advance(nowMs);     // the switch and the next send happen NOW, on this F7
  }

  // ---- ticked from loop() ----------------------------------------------------
  void service(uint32_t nowMs, uint32_t /*nowUs*/) {
    serviceSweep(nowMs);
    advance(nowMs);
    switch (st_) {
      case WAIT_ECHO:
        // v1.20: a machine slower to switch than the 16-byte pad allows
        // misses the test. Send it again - pad and all - while there is time.
        if (!timedOut(nowMs) && tries_ < TURBO_TEST_TRIES &&
            (uint32_t)(nowMs - testMs_) >= (uint32_t)TURBO_TEST_RETRY_MS) {
          tmSerial.printf("turbo: no echo yet - test again (%u of %u)\n",
                          (unsigned)(tries_ + 1), (unsigned)TURBO_TEST_TRIES);
          sendTest(nowMs);
          break;
        }
        // fall through
      case WAIT_CAPS: case WAIT_ACK: case WAIT_RESULT:
      case R_WAIT_SET: case R_WAIT_TEST: case R_WAIT_TEST2:
        if (timedOut(nowMs)) {
          tmSerial.printf("turbo: no reply in %u ms at %s (%s).\n",
                          (unsigned)TURBO_STEP_TIMEOUT_MS, stateName(), speedName());
          if (st_ == WAIT_CAPS)
            tmSerial.println(F("       Is the machine's MIDI OUT wired to pin 0, set to\n"
                               "       OUT (not THRU), with TURBO enabled? 'turbo sweep'\n"
                               "       tries other device ids."));
          if (cur_ != 1) reportLine();
          fail(nullptr, nowMs);
        }
        break;
      case LOCKED: watchdog(nowMs); break;
      default: break;
    }
  }

  // v1.20: called from pumpMidi() - every loop pass, every 256 bytes of a
  // display push, the EEPROM driver, usbWait - so a switch that is waiting on
  // the UART to drain happens within ~0.2 ms of it draining even mid-frame.
  // It has to: as responder, the machine starts talking at SPEED1 the moment
  // our 13 has arrived, behind only its 16-byte (0.5 ms) pad.
  void poll(uint32_t nowMs) { if (st_ != OFF && st_ != LOCKED) advance(nowMs); }

  // Counters for 'turbo ?' and 's'.
  uint32_t locks()    const { return locks_; }
  uint32_t reverts()  const { return reverts_; }
  bool     peerKeepalive() const { return peerKeepalive_; }

 private:
  // Run every state that can complete on this pass, one after another, so a
  // send, a switch and the next send never wait for another loop pass. Stops
  // at the first state that has to wait for something. Bounded: the longest
  // chain is SWITCH1 -> FIRST_TEST -> WAIT_ECHO.
  void advance(uint32_t nowMs) {
    for (uint8_t guard = 0; guard < 4 && step(nowMs); ++guard) {}
  }
  // One state's work. True if the state moved on.
  bool step(uint32_t nowMs) {
    switch (st_) {
      case REQUEST:                                                 // step 2
        if (!sendCmd(TM_CMD_SPEED_REQ, nullptr, 0)) { fail("TX queue full", nowMs); return true; }
        enter(WAIT_CAPS, nowMs);
        return true;
      case NEGOTIATE: {                                             // step 4
        const uint8_t d[2] = {speed1Code(), speed2Code()};
        if (!sendCmd(TM_CMD_SPEED_SET, d, 2)) { fail("TX queue full", nowMs); return true; }
        enter(WAIT_ACK, nowMs);
        return true;
      }
      case SWITCH1: {                                               // step 6
        if (!txIdle()) {
          if (timedOut(nowMs)) { fail("TX never went idle for SWITCH1", nowMs); return true; }
          return false;
        }
        switchTo(speed1Code(), nowMs);          // what our 12 asked for
        // The 16-byte pad goes out with the test, in step 7 - see sendTest().
        enter(FIRST_TEST, nowMs);
        return true;
      }
      case FIRST_TEST:                                              // step 7
        tries_ = 0;
        if (!sendTest(nowMs)) { fail("TX queue full", nowMs); return true; }
        enter(WAIT_ECHO, nowMs);
        return true;
      case LOOP:                                    // 'turbo loop'
        return loopStep(nowMs);
      case R_ACK:                                   // responder: our 13 is out
        if (!txIdle()) {
          if (timedOut(nowMs)) { fail("TX never went idle after our ACK", nowMs); return true; }
          return false;
        }
        switchTo(speed1_, nowMs);
        enter(R_WAIT_TEST, nowMs);
        return true;
      case SECOND_TEST:                                             // step 9
        if (!sendCmd(TM_CMD_SPEED_TEST2, nullptr, 0)) { fail("TX queue full", nowMs); return true; }
        enter(WAIT_RESULT, nowMs);
        return true;
      case FINALIZE:                                                // step 11
        if (!txIdle()) {
          if (timedOut(nowMs)) { fail("TX never went idle for FINALIZE", nowMs); return true; }
          return false;
        }
        switchTo(target_, nowMs);
        // micros(), read here - not a timestamp from the top of loop(), which
        // can be a console command's worth of milliseconds old.
        settleUs_ = micros() + TURBO_SETTLE_US;
        enter(SETTLE, nowMs);
        return true;
      case SETTLE:
        if ((int32_t)(micros() - settleUs_) < 0) return false;
        lock(nowMs);
        return true;
      case REVERT:
        if (!revertSwitched_) {
          // Do not cut a byte off mid-frame - but a UART that never drains
          // must not keep the box in REVERT either.
          if (!txIdle() && !timedOut(nowMs)) return false;
          switchTo(1, nowMs);
          revertSwitched_ = true;
          holdUntilMs_ = nowMs + TURBO_REVERT_HOLD_MS;
          return true;
        }
        if ((int32_t)(nowMs - holdUntilMs_) < 0) return false;
        st_ = OFF;
        allNotesOff();            // anything misframed may have read as a note-on
        tmSerial.println(F(">> TURBO OFF - back to 31250 baud"));
        uiTouch();
        return true;
      default:
        return false;
    }
  }

  void enter(St s, uint32_t nowMs) {
    st_ = s;
    deadline_ = nowMs + TURBO_STEP_TIMEOUT_MS;
    uiTouch();
  }
  bool timedOut(uint32_t nowMs) const { return (int32_t)(nowMs - deadline_) >= 0; }

  // Nothing left to transmit anywhere: our queue, the software ring, and the
  // LPUART's FIFO and shift register (tmTxIdle, the transmit-complete flag).
  // The other queues are frozen while we hold the wire, so this is only ever
  // waiting on our own bytes.
  bool txIdle() const {
    return qTurbo.pending() == 0 && Serial1.availableForWrite() >= txMax_ && tmTxIdle();
  }

  // The attempt is over. While the UART is still at 31250 there is nothing to
  // undo; once it has moved, REVERT. `why` may be null when the caller has
  // already said why.
  void fail(const char* why, uint32_t nowMs) {
    if (why) tmSerial.printf("turbo: %s.\n", why);
    g_tmKeepalive = false;
    sweeping_ = false;
    if (cur_ == 1) {
      if (st_ != OFF) tmSerial.println(F("turbo: staying at 31250 baud"));
      st_ = OFF;
      uiTouch();
      return;
    }
    reverts_++;
    revertSwitched_ = false;
    tmSerial.println(F("turbo: reverting to 31250 baud"));
    enter(REVERT, nowMs);
  }

  void lock(uint32_t nowMs) {
    st_ = LOCKED;
    locks_++;
    lastRxMs_ = wdMs_ = nowMs;
    anyRx_ = 0; rxSeen_ = 0;
    peerKeepalive_ = false;
    // Keepalive on, first FE straight away, then every TURBO_KEEPALIVE_MS.
    g_tmKeepaliveDl.arm(nowMs, 0);
    g_tmKeepalive = true;
    uiTouch();
    tmSerial.printf(">> TURBO LOCKED at %s (%lu baud), keepalive FE every %u ms\n",
                    speedName(), (unsigned long)baud(), (unsigned)TURBO_KEEPALIVE_MS);
  }

  // Two independent ways a live link is seen to have died.
  void watchdog(uint32_t nowMs) {
    // 1. Traffic that does not parse: the machine is at another speed (it
    //    timed out, was power-cycled, or dropped to 1x from its own menu), so
    //    its bytes arrive misframed. A few stray bytes are not evidence; a
    //    window with plenty of traffic and none of it recognisable is.
    if ((uint32_t)(nowMs - wdMs_) >= (uint32_t)TURBO_WATCHDOG_MS) {
      const uint16_t got = anyRx_, good = rxSeen_;
      anyRx_ = 0; rxSeen_ = 0; wdMs_ = nowMs;
      if (got > 8 && good == 0) {
        char why[80];
        snprintf(why, sizeof why, "%u bytes at %s and none parsed - the link is"
                 " out of sync", (unsigned)got, speedName());
        fail(why, nowMs);
        return;
      }
    }
    // 2. The machine was keeping the link alive itself and has stopped. An
    //    idle MIDI line is idle at every baud, so silence only counts once it
    //    has shown us it sends FE.
    if (peerKeepalive_ && (uint32_t)(nowMs - lastRxMs_) >= (uint32_t)TURBO_PEER_SILENT_MS)
      fail("the machine's keepalive stopped - the link has gone", nowMs);
  }

  // Everything a baud change touches, in one place.
  void switchTo(uint8_t code, uint32_t nowMs) {
    cur_ = code;
    // tmPortRestart, not a bare begin(): it also flushes the hardware RX FIFO
    // and the sticky error flags, which begin() leaves holding bytes from the
    // old speed. The extra RX / TX memory from setup() survives it.
    tmPortRestart(kTmSpeeds[code]);
    mnmOut.setBaud(kTmSpeeds[code]);    // the CC budget follows the wire
    midiTxSetBaud(kTmSpeeds[code]);     // and so does the UART's lead
    sysex1.reset();
    midiRxReset();                      // running status / SPP from the old speed
    g_txInSysex = false;                // the ring was just emptied
    anyRx_ = 0; rxSeen_ = 0; rawN_ = 0; f0N_ = 0; msgN_ = 0;
    lastRxMs_ = wdMs_ = nowMs;
    tmSerial.printf(">> TURBO: UART now %lu baud (%s)\n",
                    (unsigned long)kTmSpeeds[code], kTmNames[code]);
    uiTouch();
  }

  // Step 6's pad and step 7's test, in that order, into qTurbo. Sixteen raw
  // 00s, not SysEx: a data byte with no running status is discarded by the
  // receiver, so they are pure time for its UART to settle at the new divisor
  // before the pattern arrives. Also the retry, pad and all.
  bool sendTest(uint32_t nowMs) {
    static const uint8_t kPad[16] = {0};
    if (!qTurbo.push(kPad, sizeof kPad)) return false;
    logHex("TURBO TX (pad):", kPad, sizeof kPad);
    if (!sendCmd(TM_CMD_SPEED_TEST, kTmTestPattern, sizeof kTmTestPattern)) return false;
    tries_++; testMs_ = nowMs;
    return true;
  }

  // Can a handshake the machine starts be taken up now? Only at 1x with
  // nothing under way - or while our own request is still waiting for its
  // answer, in which case both ends started at once and the machine leads.
  bool machineMayLead() {
    if (!(st_ == OFF || st_ == WAIT_CAPS || st_ == R_WAIT_SET) || cur_ != 1 || sweeping_)
      return false;
    if (!allow_) {
      if (!refusedLogged_)
        tmSerial.println(F("turbo: the machine asks for TurboMIDI - not answering after\n"
                           "       'turbo off'. Type 'turbo' to allow it again."));
      refusedLogged_ = true;
      return false;
    }
    return true;
  }

  // Why a step at the new speed got no reply, from what reached us since the
  // switch. v1.20 judged by "anything parsed", and one keepalive FE - a byte
  // that survives almost any line - made it blame the machine's timing when
  // every message was arriving mangled. Now: did SysEx START, and did any
  // ARRIVE WHOLE?
  void reportLine() const {
    char b[3 * sizeof raw_ + 1]; int k = 0;
    for (uint8_t i = 0; i < rawN_; ++i) k += snprintf(b + k, sizeof b - (size_t)k, " %02X", raw_[i]);
    b[k] = 0;
    tmSerial.printf("       at %s since the switch: %u bytes in, %u SysEx started, %u arrived"
                    " whole.%s%s\n", speedName(), (unsigned)anyRx_, (unsigned)f0N_,
                    (unsigned)msgN_, rawN_ ? " First bytes:" : "", b);
    if (!anyRx_)
      tmSerial.println(F("       NOTHING arrived: the machine sent nothing at this speed, or\n"
                         "       the XY6's MIDI IN does not pass it at all. 'turbo loop' tells."));
    else if (msgN_)
      tmSerial.println(F("       Messages arrive whole at this speed, so the wire works: the\n"
                         "       machine sent something else, or not in time."));
    else if (f0N_)
      tmSerial.println(F("       The machine IS talking at this speed, but not one message arrived\n"
                         "       whole. Bytes with one long low stretch (F0 FE 00) get through\n"
                         "       and the rest are mangled: the XY6's MIDI IN is smearing the\n"
                         "       edges - its optocoupler (or pull-up) is too slow for this speed.\n"
                         "       'turbo loop' measures the fastest speed it passes; 'turbo max'\n"
                         "       keeps TurboMIDI at or below it."));
    else
      tmSerial.println(F("       Bytes arrive but no SysEx starts: the two ends are at different\n"
                         "       speeds, or the MIDI IN mangles everything here. 'turbo loop' tells."));
  }

  // ---- 'turbo loop' ---------------------------------------------------------
  // One speed per pass through here: switch (once the UART is idle), send the
  // pattern, collect for 40 ms, judge. Then back to 1x and a verdict.
  bool loopStep(uint32_t nowMs) {
    const uint8_t n = (uint8_t)sizeof kLoopCodes;
    if (loopPhase_ == 0) {
      if (!txIdle()) {
        if (!timedOut(nowMs)) return false;
        loopBroken_ = true;                       // give up, go home
        loopIdx_ = n;
      }
      if (loopIdx_ >= n) { loopDone(nowMs); return true; }
      switchTo(kLoopCodes[loopIdx_], nowMs);
      loopRxN_ = 0;
      // Two halves: a queue message is at most 31 bytes. Back to back in
      // qTurbo, so on the wire they are one unbroken 32-byte run.
      if (!qTurbo.push(kLoopPattern, 16) || !qTurbo.push(kLoopPattern + 16, 16)) {
        loopBroken_ = true;
        loopDone(nowMs);
        return true;
      }
      loopPhase_ = 1;
      loopUntilMs_ = nowMs + 40;
      return true;
    }
    if (loopRxN_ < sizeof kLoopPattern && (int32_t)(nowMs - loopUntilMs_) < 0) return false;
    uint8_t same = 0;
    for (uint8_t i = 0; i < loopRxN_ && i < sizeof kLoopPattern; ++i) same += (loopRx_[i] == kLoopPattern[i]);
    const bool ok = (loopRxN_ == sizeof kLoopPattern && same == sizeof kLoopPattern);
    char got[3 * 12 + 1]; int k = 0;
    for (uint8_t i = 0; i < loopRxN_ && i < 12; ++i) k += snprintf(got + k, sizeof got - (size_t)k, " %02X", loopRx_[i]);
    got[k] = 0;
    const uint8_t code = kLoopCodes[loopIdx_];
    tmSerial.printf("  %-4s %6lu baud: %2u of %u back intact%s%s\n", kTmNames[code],
                    (unsigned long)kTmSpeeds[code], (unsigned)same, (unsigned)sizeof kLoopPattern,
                    ok ? "" : "   got:", ok ? "" : (loopRxN_ ? got : " nothing"));
    if (ok && loopBest_ == loopIdx_) loopBest_ = (uint8_t)(loopIdx_ + 1);   // unbroken run from 1x
    loopIdx_++;
    loopPhase_ = 0;
    deadline_ = nowMs + TURBO_STEP_TIMEOUT_MS;
    return true;
  }
  void loopDone(uint32_t nowMs) {
    if (cur_ != 1) switchTo(1, nowMs);
    st_ = OFF;
    uiTouch();
    if (loopBroken_) { tmSerial.println(F("turbo loop: stopped - could not transmit")); return; }
    if (loopBest_ == 0) {
      tmSerial.println(F("turbo loop: nothing came back intact even at 1X - is OUT 1 patched\n"
                         "            to IN 1, with the machine unplugged?"));
      return;
    }
    const uint8_t best = kLoopCodes[loopBest_ - 1];
    tmSerial.printf("turbo loop: this MIDI IN passes up to %s cleanly.\n", kTmNames[best]);
    if (best < 2)
      tmSerial.println(F("            TurboMIDI needs at least 2X - the input stage has to change."));
    else if (best < TURBO_SPEED1_CODE)
      tmSerial.printf("            'turbo max %lu' keeps TurboMIDI inside that (tests at %s,\n"
                      "            runs one step lower); 'save' keeps the setting.\n",
                      (unsigned long)(kTmSpeeds[best] / 31250u), kTmNames[best]);
    else
      tmSerial.println(F("            Full speed is fine: the MIDI IN is not the problem."));
  }

  // Bit (code - 1) of the 14-bit mask in the first two data bytes is speed
  // code `code` on offer: the captured 7F 01 is codes 1..8, i.e. 1x..10x.
  bool capsOffer(uint8_t len, const uint8_t* d) const {
    if (len < 2) return false;
    const uint16_t mask = (uint16_t)((d[0] & 0x7F) | ((uint16_t)(d[1] & 0x7F) << 7));
    const bool same = (len == sizeof kTmCaps && memcmp(d, kTmCaps, len) == 0);
    tmSerial.printf("turbo: machine offers mask %04X%s\n", (unsigned)mask,
                    same ? " (as captured)" : " - differs from the captured 7F 01 0F 00");
    return ((mask >> (speed1Code() - 1)) & 1u) && ((mask >> (speed2Code() - 1)) & 1u);
  }

  // One turbo message into qTurbo, whole. Bytes 4 and 5 are 00 00 until the
  // machine has sent us a turbo message, and whatever it stamped there after.
  bool sendCmd(uint8_t cmd, const uint8_t* d, uint8_t n) {
    uint8_t m[24]; uint8_t k = 0;
    if (n > sizeof(m) - 8) return false;
    m[k++] = 0xF0;
    m[k++] = 0x00; m[k++] = 0x20; m[k++] = 0x3C;    // Elektron manufacturer ID
    m[k++] = sysex1.peerId(0); m[k++] = sysex1.peerId(1);
    m[k++] = cmd;
    for (uint8_t i = 0; i < n; ++i) m[k++] = d[i];
    m[k++] = 0xF7;
    logHex("TURBO TX:", m, k);
    return qTurbo.push(m, k);
  }

  void serviceSweep(uint32_t nowMs) {
    if (!sweeping_ || (int32_t)(nowMs - deadline_) < 0) return;
    if (sweepId_ > 0x7F) {
      sweeping_ = false;
      tmSerial.println(F(">> TURBO SWEEP: no 0x11 at any of the 128 ids. The device id\n"
                         "   is not the problem - check the cable, the machine's MIDI\n"
                         "   OUT setting and its TURBO setting."));
      return;
    }
    // Built here rather than through sendCmd(), which stamps the LEARNED id -
    // the whole point is to try one we have not learned.
    const uint8_t m[8] = {0xF0, 0x00, 0x20, 0x3C, sweepId_, 0x00, TM_CMD_SPEED_REQ, 0xF7};
    qTurbo.push(m, sizeof m);
    if ((sweepId_ & 0x0F) == 0) tmSerial.printf("   ... probing product id %02X\n", sweepId_);
    sweepId_++;
    deadline_ = nowMs + 60;
  }

  // After a revert the machine may have received misframed bytes, and a
  // misframed byte can look like a note-on. Clear every channel we use - on
  // the LIVE base channel.
  void allNotesOff() {
    patSilenceAll();
    for (uint8_t t = 0; t < 6; ++t) {
      const uint8_t m[3] = {(uint8_t)(0xB0 | patChan(t)), 123, 0};   // All Notes Off
      qNote.push(m, 3);
    }
  }

  // Both directions logged as raw hex, through the deferred ring - never a
  // blocking USB write, because onMessage() can run inside the display flush.
  static void logHex(const char* dir, const uint8_t* m, uint8_t n) {
    if (!turboVerbose) return;
    char line[24 + 32 * 3];
    int k = snprintf(line, sizeof line, "%s", dir);
    for (uint8_t i = 0; i < n && k < (int)sizeof line - 4; ++i)
      k += snprintf(line + k, sizeof line - (size_t)k, " %02X", m[i]);
    snprintf(line + k, sizeof line - (size_t)k, "\n");
    tmLogPut(line);
  }
  static void logRx(uint8_t cmd, uint8_t len, const uint8_t* d) {
    if (!turboVerbose) return;
    uint8_t m[24]; uint8_t k = 0;
    m[k++] = 0xF0; m[k++] = 0x00; m[k++] = 0x20; m[k++] = 0x3C;
    m[k++] = sysex1.peerId(0); m[k++] = sysex1.peerId(1); m[k++] = cmd;
    for (uint8_t i = 0; i < len && k < sizeof(m) - 1; ++i) m[k++] = d[i];
    m[k++] = 0xF7;
    logHex("TURBO RX:", m, k);
  }

  St       st_ = OFF;
  uint8_t  cur_ = 1;                 // speed code the UART is at now
  uint8_t  target_ = TURBO_SPEED2_CODE;
  int      txMax_ = 63;
  uint32_t deadline_ = 0, settleUs_ = 0, holdUntilMs_ = 0;
  bool     revertSwitched_ = false;
  // rxSeen_ is 16 bits on purpose: a uint8_t wraps at 256, which is exactly
  // the clock bytes 160 BPM puts in a 4 s watchdog window (v6, defect 4).
  uint16_t rxSeen_ = 0, anyRx_ = 0;
  uint32_t lastRxMs_ = 0, wdMs_ = 0;
  bool     peerKeepalive_ = false;
  uint8_t  sweepId_ = 0;
  bool     sweeping_ = false;
  uint32_t locks_ = 0, reverts_ = 0;
  uint8_t  speed1_ = TURBO_SPEED1_CODE;   // responder: what the machine asked for
  uint8_t  tries_ = 0;                    // initiator: tests sent this attempt
  uint32_t testMs_ = 0;
  bool     allow_ = true, refusedLogged_ = false;
  uint8_t  raw_[12];                      // first bytes after a switch
  uint8_t  rawN_ = 0;
  uint16_t f0N_ = 0, msgN_ = 0;           // v1.21: SysEx started / arrived whole
  uint8_t  maxCode_ = TURBO_SPEED1_CODE;  // v1.21: 'turbo max'
  uint8_t  loopIdx_ = 0, loopPhase_ = 0, loopBest_ = 0, loopRxN_ = 0;
  bool     loopBroken_ = false;
  uint32_t loopUntilMs_ = 0;
  uint8_t  loopRx_[40];
};

static TurboMidi turbo;

static bool turboHoldsWire() { return turbo.holdsWire(); }
static void turboNoteLiveByte(uint8_t b) { turbo.noteLiveByte(b, millis()); }

// LFO 1 overrides, driven from the serial console.
static int16_t  manualVal  = -1;   // >=0 sends this fixed value instead of the LFO
static int16_t  yyOverride = -1;   // >=0 uses this raw YY instead of page/slot
static uint8_t  huntMode   = 0;    // 0 off, 1 NRPN yy sweep, 2 CC sweep
static uint16_t huntPos    = 0;
static uint32_t huntNextMs = 0;

// Non-blocking 'ramp'. The old one sat in a loop calling delay(25) two hundred
// and fifty-four times — six and a third seconds during which MIDI was not
// pumped, the display did not redraw and incoming clock piled up in the UART
// until it overran. Same sweep, run from the main loop.
static uint8_t  rampMode = 0;      // 0 off, 1 rising, 2 falling
static uint32_t rampNextMs = 0;

static uint8_t activeYY();

// ===================== receive counters and link telemetry ==================
// Declared here, above the UI, because the DIAG page renders them. They used
// to sit down with loop() and were only reachable by the console.
// Receive counters. These are the first thing to look at when nothing moves:
// if rxBytes is 0 the Monomachine's OUT is not reaching pin 0, and no amount
// of fiddling with the parameter address will help.
static uint32_t rxBytes = 0, rxClock = 0, rxStart = 0, rxStop = 0;
static uint32_t rxSpp = 0, rxSysex = 0, rx2Bytes = 0;
static uint32_t rxSysexAbort = 0, rxDumpLost = 0;
// First 8 bytes of the most recent incoming SysEx, shown on the DIAG page.
// If the Monomachine ever opens a turbo negotiation, its header lands here and
// you can read it off the panel instead of needing a serial console.
static uint8_t  sxLastHdr[8] = {0};
static uint8_t  sxLastLen = 0, sxCapturing = 0;
// A "last CC seen" block used to live here to feed a DIAG row that never got
// wired up when DIAG folded into SETTINGS. Removed rather than left as four
// dead global writes on the hot RX path: captureCc() is reached from
// handleMidiByte(), which runs on every single received byte.
static bool     rxDump = false, rx2Dump = false;
// Highest fill ever observed in the Serial1 receive buffer. The one number
// that answers "am I close to overflowing?" - see the TURBO/RX status line.
static uint16_t rxPeak = 0, rxCapacity = 0;
// MCP23017 transactions that failed. Declared up here with the other counters
// for the same reason they are: the settings page renders it, and the UI layer
// is compiled well before the I2C helpers further down.
static uint32_t mcpErrors = 0;

// Transport events are recorded here by the byte handler and announced from
// loop(). See the note by usbReady().
static const uint8_t EVT_START = 1, EVT_CONT = 2, EVT_STOP = 4, EVT_STOP2 = 8;
static uint8_t pendingEvt = 0;

// ================================== UI =======================================
//
// Three pages. The on-board tactile button (MCP23017 GPA6) walks them in order
// — LFO overview -> LFO edit -> PATTERN -> LFO overview — so a user with no
// serial console can still reach every page. Encoder 1 push also toggles LFO
// overview <-> edit so the LFO flow keeps its short-cut.
//
// -----------------------------------------------------------------------------
// THE ONE RULE THIS LAYER NOW FOLLOWS
// -----------------------------------------------------------------------------
// Never set small text inside a lit block.
//
// The previous pages were built almost entirely out of inverted chips: a solid
// filled rectangle with the label knocked out of it in black. That is a
// perfectly good look on a backlit LCD and close to the worst possible one on a
// passive-matrix OLED, because the lit pixels bloom sideways into the unlit
// ones. At a 5-pixel glyph width the bloom is comparable to the stroke width,
// so the counters inside A, B, D, O and R fill in and every word turns into a
// bright smear. It is exactly the "text is mostly unreadable" symptom.
//
// So: bright strokes on black, everywhere. Selection is shown with a cursor
// mark or a brightness step, never by inverting. Headings and live values use
// the bold renderer (2px strokes) because those are the things you read from
// across the room. The dim shade was raised from 5/15 to 9/15 — one third of
// full brightness is not a legible grey on this panel, it is a rumour. And
// every layout got more air: 11px row pitch on the edit page instead of 9,
// 9-10px on the lists, values pushed hard right against a knocked-out margin so
// a moving trace can never run through a digit.

// -----------------------------------------------------------------------------
// FOUR PAGES, TWO OF THEM WITH A SUBPAGE
// -----------------------------------------------------------------------------
// The board button (MCP23017 GPA6) is the whole navigation model:
//
//   SHORT PRESS   next top-level page, wrapping    PERF -> LFO -> PAT -> SET
//   HOLD 0.5 s    enter or leave the current page's subpage
//
// Short press has to fire on RELEASE, not on the press edge, because that is
// the only way to tell it apart from the start of a hold. So the page does not
// change until you let go - which is correct, and is also why the hold can
// cancel it.
//
// Pages 3 and 4 have no subpage and a hold there does nothing at all. That is
// deliberate: a gesture that sometimes does nothing is better than one that
// does something different on every page.
enum UiPage : uint8_t {
  UPAGE_PERF = 0,          // six encoders, live performance. Sub: destinations.
  UPAGE_LFO,               // six-LFO overview.               Sub: per-LFO edit.
  UPAGE_PATTERN,           // melodic pattern generator.      No subpage.
  UPAGE_SETTINGS,          // presets, MIDI, turbo, display, link. No subpage.
  UPAGE_COUNT
};

// Which page a boot lands on. v1.16: PERF, the performance page - after the
// logo the box is immediately playable, six faders under six knobs. (The
// first-run wizard already handed over to PERF; now every boot does.)
#define UI_BOOT_PAGE   UPAGE_PERF

// How long the board button has to be held to open a subpage: BTN_HOLD_MS, and
// only BTN_HOLD_MS. There used to be a UI_HOLD_MS here holding its own copy of
// the number, never read by anything - so the two would have disagreed the
// moment either moved, and the one that looked authoritative was the dead one.

// How often the settings page is marked stale so its live LINK counters move.
// It is the only page whose content changes with no input at all, and at the
// 30 Hz frame rate that meant it could never idle. Four times a second is
// faster than anyone reads a counter and leaves 26 frames in 30 to MIDI.
#define UI_SET_TICK_MS  250u

// Set by anything that changes what is on screen. A page that is not animated
// is not redrawn at all until this is set, which is the difference between
// "the flush skips unchanged rows" (already true) and "we did not spend the
// CPU drawing them in the first place".
static bool uiDirty = true;
static void uiTouch() { uiDirty = true; }

struct LfoUi { uint8_t sel = 0, paramRow = 0, stepPage = 0, stepCur = 0;
               uint8_t page = UI_BOOT_PAGE;
               // True while the current page's subpage is showing. At most one
               // subpage per page, so a bool is the whole of it.
               bool    inSub = false; };
static LfoUi ui;

// Pattern-page cursor state. Persists across page switches so muting a track
// then leaving and coming back keeps the same track under the cursor.
static uint8_t patUiTrack = 0;
static uint8_t patUiBar   = 0;

static void u8s3(uint8_t v, char* o) {
  o[0] = (char)('0' + (v / 100) % 10);
  o[1] = (char)('0' + (v / 10) % 10);
  o[2] = (char)('0' + v % 10); o[3] = 0;
}
static void u8s(uint8_t v, char* o) {
  if (v >= 100) { u8s3(v, o); return; }
  if (v >= 10)  { o[0] = (char)('0' + v / 10); o[1] = (char)('0' + v % 10);
                  o[2] = 0; return; }
  o[0] = (char)('0' + v); o[1] = 0;
}
// "120.0" into a buffer of at least 8 bytes.
static void bpmStr(uint32_t bpmX100, char* o) {
  uint32_t whole = bpmX100 / 100;
  if (whole > 999) whole = 999;
  const uint32_t dec = (bpmX100 % 100) / 10;
  uint8_t k = 0;
  if (whole >= 100) o[k++] = (char)('0' + (whole / 100) % 10);
  if (whole >= 10)  o[k++] = (char)('0' + (whole / 10) % 10);
  o[k++] = (char)('0' + whole % 10);
  o[k++] = '.';
  o[k++] = (char)('0' + dec);
  o[k] = 0;
}
// Add a signed delta to a value and clamp. Defined here rather than beside
// setup() because every page handler needs it.
static uint8_t addClamp(uint8_t v, int8_t d, uint8_t lo, uint8_t hi) {
  int32_t n = (int32_t)v + d;
  if (n < lo) n = lo;
  if (n > hi) n = hi;
  return (uint8_t)n;
}

// Copy at most n characters. Used to keep a label inside its column instead of
// letting it run off the lit area.
static void clipN(const char* s, char* o, uint8_t n) {
  uint8_t i = 0;
  while (s[i] && i < n) { o[i] = s[i]; i++; }
  o[i] = 0;
}

// NOTE ON SIGNATURES IN THIS FILE
// The Arduino IDE hoists a prototype for every free function to the top of the
// sketch, above the struct definitions. So no free function here may name a
// sketch-defined type in its signature — these take an LFO index and look the
// structs up from the globals instead.
static const char* destName(uint8_t i) {
  // paramName() bounds the track itself, and on the SYNT page it names the
  // parameter after the machine chosen for that track (v1.22).
  return paramName(lfo.p[i].page, lfo.p[i].dest, lfo.p[i].track);
}

// An 8x5 thumbnail of the shape, drawn as a connected trace rather than eight
// loose dots — at this size an unjoined square or saw is unreadable.
static void waveGlyph(int16_t x, int16_t y, uint8_t wave, uint8_t shade) {
  int16_t prev = 0;
  for (int16_t i = 0; i < 8; ++i) {
    const uint32_t ph = (uint32_t)(((uint64_t)i << 32) / 8);
    const int32_t  v  = lfoShape(wave, ph, 0x1234u + (uint32_t)i);
    const int16_t  yy = (int16_t)(y + 2 - ((v * 2) / 32768));
    if (i) gfx.vSpan((int16_t)(x + i), prev, yy, shade);
    else   gfx.px(x, yy, shade);
    prev = yy;
  }
}

// =============================================================================
// SECTION: THE STYLE LAYER                          new in v7.1, rebuilt v1.24
// =============================================================================
//
// v1.24: THE MONOMACHINE GRID. Every page is built from the same few pieces,
// and they now follow the machine this box sits next to:
//
//   GRID     regions split by 1px G_DARK rules, the full width or height -
//            header from body, cell from cell, bank from bank.
//   TYPE     values in the 5x7 LCD face (the 3x5 only where 5x7 will not fit
//            the cell - stText decides), labels in the 3x5, 4 letters max.
//   FOCUS    exactly one solid G_WHITE block per page with its text knocked
//            out in black: the selected parameter, row or track. Nothing else
//            is filled, outlined or boxed, so the block is where the eye lands.
//   BAR      G_DARK empty space, G_WHITE value, no outline. Bipolar parameters
//            fill from the centre, lists show a block at their entry.
//
// The old vocabulary - every label a filled chip, an outline for "not
// selected" - put six to eight white blocks on every page, and the selection
// was one block among them.
//
// 'style 0' is kept for a panel whose bloom eats knocked-out text: the focus
// block becomes a G_WHITE outline with WHITE text in it. Same geometry.
static bool uiChipStyle = true;

static const int16_t ST_CHIP_H = 7;    // a 3x5 label line: 5px glyph + 1px each side
static const int16_t ST_ROW    = 9;    // a list row: a 5x7 line + 1px each side
static const int16_t ST_HDR_Y  = 0;    // header text row 1..7, rule at y 9
static const int16_t ST_BODY_Y = 11;   // first body pixel below the header rule
static const int16_t ST_FOOT_Y = 249;  // footer hints, under the rule at y 246

// The focus block. Returns the shade to draw the text inside it in.
static uint8_t stFocus(int16_t x, int16_t y, int16_t w, int16_t h) {
  if (uiChipStyle) { gfx.fillRect(x, y, w, h, G_WHITE); return G_BLACK; }
  gfx.rect(x, y, w, h, G_WHITE);
  return G_WHITE;
}

// Width a 3x5 label needs inside a focus block (2px of air each side).
static int16_t stChipW(const char* t) { return (int16_t)(gfx.width35(t) + 4); }

// A 3x5 label. Focused: the block with the text knocked out. Not focused:
// plain G_MID text in exactly the same place, so moving the focus never
// shifts a pixel of the layout.
static void stChip(int16_t x, int16_t y, int16_t w, const char* t, bool on) {
  const uint8_t sh = on ? stFocus(x, y, w, ST_CHIP_H) : G_MID;
  gfx.text35((int16_t)(x + 2), (int16_t)(y + 1), t, sh);
}
static int16_t stChipAuto(int16_t x, int16_t y, const char* t, bool on = true) {
  const int16_t w = stChipW(t);
  stChip(x, y, w, t, on);
  return w;
}

// A value in the 5x7 LCD face when it fits in maxW, else in the 3x5 - so a
// long list entry ("160CT", "F000203C") shrinks instead of running into its
// neighbour. y is the top of a 7px line; the 3x5 sits one pixel down in it,
// on the same centre. align 0: x is the left edge; 1: x is the centre;
// 2: x is one past the right edge (the text35R convention). Returns the width.
static int16_t stText(int16_t x, int16_t y, int16_t maxW, const char* s, uint8_t sh,
                      uint8_t align) {
  const bool big = gfx.textWidth(s) <= maxW;
  const int16_t w = big ? gfx.textWidth(s) : gfx.width35(s);
  const int16_t x0 = (align == 0) ? x : (align == 1) ? (int16_t)(x - w / 2) : (int16_t)(x - w);
  if (big) gfx.text(x0, y, s, sh);
  else     gfx.text35(x0, (int16_t)(y + 1), s, sh);
  return w;
}

// Header: the page's name in bold 5x7 WHITE on the left, its state in 5x7
// LIGHT on the right, one G_DARK rule the full width under both at y 9. The
// header is identity, not focus, so it is never a block.
static void stHeader(const char* left, const char* right) {
  int16_t lw;
  if (gfx.textBoldWidth(left) <= UI_W - 20) {
    gfx.textBold(0, (int16_t)(ST_HDR_Y + 1), left, G_WHITE);
    lw = gfx.textBoldWidth(left);
  } else {
    gfx.text(0, (int16_t)(ST_HDR_Y + 1), left, G_WHITE);
    lw = gfx.textWidth(left);
  }
  if (right) stText(UI_W, (int16_t)(ST_HDR_Y + 1), (int16_t)(UI_W - lw - 3), right, G_LIGHT, 2);
  gfx.hLine(0, (int16_t)(ST_HDR_Y + 9), UI_W, G_DARK);
}

// Footer: encoder hints in 3x5 G_MID under a G_DARK rule at y 246. They are
// help, not data, so they are the quietest text on the page.
static void stFooter(const char* left, const char* right) {
  gfx.hLine(0, (int16_t)(ST_FOOT_Y - 3), UI_W, G_DARK);
  if (left)  gfx.text35(0, ST_FOOT_Y, left, G_MID);
  if (right) gfx.text35R(UI_W, ST_FOOT_Y, right, G_MID);
}

// A list row, ST_ROW tall: label in 3x5 G_MID at the left, value in WHITE at
// the right (5x7 when it fits). The focused row is ONE block the full width
// with label and value knocked out of it - an Elektron menu line.
static void stRow(int16_t y, const char* label, const char* value, bool sel) {
  uint8_t lsh = G_MID, vsh = G_WHITE;
  if (sel) lsh = vsh = stFocus(0, y, UI_W, ST_ROW);
  gfx.text35(2, (int16_t)(y + 2), label, lsh);
  if (value && *value)
    stText((int16_t)(UI_W - 1), (int16_t)(y + 1),
           (int16_t)(UI_W - gfx.width35(label) - 7), value, vsh, 2);
}

// Bordered group: a 1px G_MID frame, its title in 3x5 G_LIGHT set into a gap
// cut in the top edge.
static void stBox(int16_t x, int16_t y, int16_t w, int16_t h, const char* title) {
  gfx.rect(x, y, w, h, G_MID);
  if (!title) return;
  const int16_t tw = gfx.width35(title);
  gfx.hLine((int16_t)(x + 2), y, (int16_t)(tw + 4), G_BLACK);
  gfx.text35((int16_t)(x + 4), (int16_t)(y - 2), title, G_LIGHT);
}

// ---- bars -------------------------------------------------------------------
// One bar for every quantity on every page: G_DARK is the empty space, the
// value is solid, and there is no outline - so a bar always reads the same.
//   BAR_FILL     from the left (or the bottom)
//   BAR_CENTRE   bipolar: from the centre either way; at centre, a 1px line
//   BAR_THUMB    a list: a block at the entry's place, as wide as one entry
enum : uint8_t { BAR_FILL = 0, BAR_CENTRE, BAR_THUMB };

static void stBarH(int16_t x, int16_t y, int16_t w, int16_t h, int32_t v, int32_t vmax,
                   uint8_t mode, uint8_t sh) {
  if (w < 2 || h < 1) return;
  gfx.fillRect(x, y, w, h, G_DARK);
  if (vmax <= 0) return;
  if (v < 0) v = 0;
  if (v > vmax) v = vmax;
  if (mode == BAR_THUMB) {
    int16_t tw = (int16_t)(w / (vmax + 1));
    if (tw < 2) tw = 2;
    gfx.fillRect((int16_t)(x + (int32_t)(w - tw) * v / vmax), y, tw, h, sh);
  } else if (mode == BAR_CENTRE) {
    const int16_t c = (int16_t)(x + (int32_t)(w - 1) * ((vmax + 1) / 2) / vmax);
    const int16_t q = (int16_t)(x + (int32_t)(w - 1) * v / vmax);
    gfx.fillRect(q < c ? q : c, y, (int16_t)((q < c ? c - q : q - c) + 1), h, sh);
  } else {
    const int16_t f = (int16_t)(((int32_t)w * v + vmax / 2) / vmax);
    if (f > 0) gfx.fillRect(x, y, f, h, sh);
  }
}
// The same, standing up: value from the bottom (or the middle).
static void stBarV(int16_t x, int16_t y, int16_t w, int16_t h, int32_t v, int32_t vmax,
                   uint8_t mode, uint8_t sh) {
  if (h < 2 || w < 1) return;
  gfx.fillRect(x, y, w, h, G_DARK);
  if (vmax <= 0) return;
  if (v < 0) v = 0;
  if (v > vmax) v = vmax;
  const int16_t b = (int16_t)(y + h - 1);                     // bottom row
  if (mode == BAR_THUMB) {
    int16_t th = (int16_t)(h / (vmax + 1));
    if (th < 2) th = 2;
    gfx.fillRect(x, (int16_t)(b - th + 1 - (int32_t)(h - th) * v / vmax), w, th, sh);
  } else if (mode == BAR_CENTRE) {
    const int16_t c = (int16_t)(b - (int32_t)(h - 1) * ((vmax + 1) / 2) / vmax);
    const int16_t q = (int16_t)(b - (int32_t)(h - 1) * v / vmax);
    gfx.fillRect(x, q < c ? q : c, w, (int16_t)((q < c ? c - q : q - c) + 1), sh);
  } else {
    const int16_t f = (int16_t)(((int32_t)h * v + vmax / 2) / vmax);
    if (f > 0) gfx.fillRect(x, (int16_t)(b - f + 1), w, f, sh);
  }
}
// A Monomachine parameter's bar: its own range and its own kind - SID WAVE
// is five entries, PAN is bipolar, VOL runs 0..127. raw is the wire value.
static void stParamBar(int16_t x, int16_t y, int16_t w, int16_t h, bool vertical,
                       uint8_t page, uint8_t slot, uint8_t track, uint8_t raw, uint8_t sh) {
  const MnmParam* d = paramDef(page, slot, track);
  const uint8_t n = d ? mnmParamCount(*d) : 0;
  int32_t v = raw & 0x7F, vmax = 127;
  uint8_t mode = (d && d->kind == MNK_S7) ? BAR_CENTRE : BAR_FILL;
  if (n > 1) { v = mnmListIndex(raw, n); vmax = n - 1; mode = BAR_THUMB; }
  if (vertical) stBarV(x, y, w, h, v, vmax, mode, sh);
  else          stBarH(x, y, w, h, v, vmax, mode, sh);
}
// Where a raw value lands along a parameter's bar, 0..len-1 from the start
// (left, or bottom) - for marking a centre across a bar.
static int16_t stParamPos(int16_t len, uint8_t page, uint8_t slot, uint8_t track, uint8_t raw) {
  const MnmParam* d = paramDef(page, slot, track);
  const uint8_t n = d ? mnmParamCount(*d) : 0;
  if (n > 1) return (int16_t)((int32_t)(len - 1) * mnmListIndex(raw, n) / (n - 1));
  return (int16_t)((int32_t)(len - 1) * (raw & 0x7F) / 127);
}

// Every horizontal bar the pages had before v1.24 comes through here.
static void stBar(int16_t x, int16_t y, int16_t w, int16_t h,
                  int32_t v, int32_t vmax) {
  stBarH(x, y, w, h, v, vmax, BAR_FILL, G_WHITE);
}

// The trace is drawn by calling the same lfoShape() the engine uses to produce
// the outgoing value, so the picture cannot disagree with what you hear.
//
// v1.24 - SHARP. One pixel of trace per column, joined to the last by a
// vertical span: every pixel is drawn at full shade or not drawn. (v1.10 split
// each column's intensity across two pixels - anti-aliasing, which is exactly
// the softness the Monomachine's screen does not have.) Inside a 1px frame -
// G_MID for the window in focus, G_DARK otherwise - a G_DARK dotted zero line,
// and while the LFO drives, the part of the cycle already played in WHITE and
// the rest in G_MID, split by a 1px G_LIGHT playhead.
static void drawWaveBox(int16_t x, int16_t y, int16_t w, int16_t h,
                        uint8_t idx, bool playhead, uint8_t border) {
  const LfoParams& p = lfo.p[idx];
  const LfoState&  s = lfo.s[idx];
  const bool hot = (border == G_WHITE);          // selected, enabled window
  gfx.rect(x, y, w, h, hot ? G_MID : G_DARK);
  const int16_t ix = (int16_t)(x + 1), iy = (int16_t)(y + 1);
  const int16_t iw = (int16_t)(w - 2), ih = (int16_t)(h - 2);
  if (iw < 4 || ih < 4) return;
  const int16_t mid = (int16_t)(iy + ih / 2), half = (int16_t)((ih - 1) / 2);
  gfx.dotHLine(ix, mid, iw, G_DARK, 2);

  const int16_t hx = playhead
      ? (int16_t)(ix + (int16_t)(((uint64_t)s.phase * iw) >> 32)) : (int16_t)-1;
  if (playhead) gfx.vLine(hx, iy, ih, G_LIGHT);
  const uint8_t shPast = hot ? G_WHITE : G_LIGHT;
  const uint8_t shNext = G_MID;
  const uint8_t shIdle = hot ? G_WHITE : G_LIGHT;  // not driving: one shade

  // ONE divide per box: the phase advances by a constant across the width.
  const uint32_t phStep   = (uint32_t)(0x100000000ULL / (uint32_t)iw);
  const uint32_t seedStep = (16u * 65536u) / (uint32_t)iw;
  const bool     isRnd    = (p.wave == WAVE_RND);
  // Depth AND amount, so the trace is the truth: pulling E4 down flattens the
  // picture exactly as much as it flattens what goes on the wire.
  const float gainF = (float)p.depth * (float)(p.amount < 127 ? p.amount : 127)
                      / (127.0f * 128.0f * 32768.0f) * (float)half;
  uint32_t ph = 0, seedA = 0;
  int16_t prev = mid;
  for (int16_t i = 0; i < iw; ++i) {
    // RND holds one value per cycle; the preview shows sixteen cycles of it.
    const uint32_t seed = isRnd ? (0x5EEDu + (seedA >> 16)) : 0x5EEDu;
    int32_t v = lfoShape(p.wave, ph, seed);
    if (p.intl) { const uint32_t g = ph * (uint32_t)p.intl;
                  if (g & 0x80000000u) v = 0; }
    int16_t yy = (int16_t)lroundf((float)mid - (float)v * gainF);
    if (yy < iy) yy = iy;
    if (yy > iy + ih - 1) yy = (int16_t)(iy + ih - 1);
    if (i == 0) prev = yy;
    const int16_t cx = (int16_t)(ix + i);
    const uint8_t sh = !playhead ? shIdle : (cx <= hx ? shPast : shNext);
    gfx.vSpan(cx, prev, yy, sh);
    prev = yy;
    ph    += phStep;
    seedA += seedStep;
  }
}

// Panel ruler. Everything here is thin strokes, so it draws almost no current.
// If this shows all the way to "56" the panel is fine and the earlier
// truncation was the UI drawing too much white; if it stops at the same place
// the page did, it is the panel or its wiring.
static void drawRuler() {
  gfx.clear();
  gfx.rect(0, 0, 64, 256, SH_ON);          // border of the FULL 64-wide area
  gfx.textBold(0, 4,  "WIDTH", SH_ON);
  gfx.textBold(0, 13, "TEST",  SH_ON);
  for (uint8_t k = 0; k < 8; ++k) {
    const int16_t y = (int16_t)(28 + k * 26);
    const int16_t w = (int16_t)(8 * (k + 1));
    gfx.hLine(0, y, w, SH_ON);
    gfx.hLine(0, (int16_t)(y + 1), w, SH_ON);
    gfx.vLine((int16_t)(w - 1), (int16_t)(y - 3), 8, SH_ON);
    char b[4]; u8s((uint8_t)w, b);
    int16_t tx = (int16_t)(w - gfx.textWidth(b));
    if (tx < 0) tx = 0;
    gfx.text(tx, (int16_t)(y + 4), b, SH_ON);
  }
  gfx.text(0, 236, "SET", SH_ON);
  gfx.text(0, 245, "UI_W", SH_ON);
  flushAll();
}

// Row test. Eight bands, each spanning the FULL 64 logical columns — which is
// the panel's 64 COM lines, because the display is mounted rotated and logical
// x maps to a physical row. This tells you WHICH rows are dark, which is what
// the mux / stl / offs registers actually move.
static void drawRowTest() {
  gfx.clear();
  for (uint8_t b = 0; b < 8; ++b) {
    const int16_t x0 = (int16_t)(b * 8);
    if (b & 1) gfx.fillRect(x0, 30, 7, 100, SH_MID);
    else       gfx.rect(x0, 30, 7, 100, SH_ON);
    char n[4]; u8s((uint8_t)x0, n);
    gfx.text(x0, 140, n, SH_ON);
    gfx.text(x0, 12, n, SH_ON);
  }
  gfx.textBold(0, 170, "ROWS", SH_ON);
  gfx.text(0, 180, "0-63", SH_ON);
  gfx.text(0, 196, "COUNT", SH_ON);
  gfx.text(0, 206, "BANDS", SH_ON);
  gfx.text(0, 222, "TRY", SH_ON);
  gfx.text(0, 232, "MUX63", SH_ON);
  flushAll();
}


// ---------------------------------------------------------------------------
// TURBO LINK BADGE - the one status element that appears on every page
// ---------------------------------------------------------------------------
// Fixed slot: the top-right of the header line, right edge at UI_W-1, rows 0..7.
// Every page's title bar was re-laid out to keep that corner clear, so the
// badge sits in the same place whichever page you are on.
//
// Three states, deliberately different SHAPES rather than three shades, because
// a shade difference on this panel is the first thing to vanish at arm's length:
//     locked above 1x   filled chip, text knocked out   - unmissable
//     negotiating       outlined box, "NEG"
//     base 31250        plain dim text, "1X"            - quiet, it is normal
//
// Returns the x it started at, so the caller can lay out to its left.
static int16_t drawTurboBadge(int16_t rx, int16_t y) {
  const char* t = turbo.badgeText();
  const uint8_t style = turbo.badgeStyle();
  const int16_t tw = gfx.width35(t);
  if (style == 0) {
    const int16_t x = (int16_t)(rx - tw);
    gfx.text35(x, (int16_t)(y + 1), t, SH_DIM);
    return x;
  }
  const int16_t w = (int16_t)(tw + 5);
  const int16_t x = (int16_t)(rx - w + 1);
  if (style == 2) {
    gfx.fillRect(x, y, w, 7, SH_ON);
    gfx.text35((int16_t)(x + 3), (int16_t)(y + 1), t, SH_OFF);
  } else {
    gfx.rect(x, y, w, 7, SH_MID);
    gfx.text35((int16_t)(x + 3), (int16_t)(y + 1), t, SH_MID);
  }
  return x;
}

// The performance page has no free corner - its top row is three number chips
// at fixed pixels - so there the badge goes in the middle band at twice the
// size. That band exists because the mockup's joystick field is disabled, and a
// large centred "8X" is more prominent than any corner badge could be.
#if PERF_SHOW_JOYFIELD || !PERF_FULL_FADERS
static void drawTurboBadgeBig(int16_t cy) {
  const char* t = turbo.badgeText();
  const uint8_t style = turbo.badgeStyle();
  const int16_t tw = (int16_t)(gfx.textWidth(t) * 2);
  const int16_t bw = (int16_t)(tw + 12), bh = 28;
  const int16_t x = (int16_t)((UI_W - bw) / 2), y = (int16_t)(cy - bh / 2);
  const int16_t lx = (int16_t)(x + (bw - gfx.width35("TURBO")) / 2);
  if (style == 2) {
    gfx.fillRect(x, y, bw, bh, SH_ON);
    gfx.text35(lx, (int16_t)(y + 3), "TURBO", SH_OFF);
    gfx.textScaled((int16_t)(x + (bw - tw) / 2), (int16_t)(y + 11), t, SH_OFF, 2);
  } else {
    gfx.rect(x, y, bw, bh, style ? SH_MID : SH_FAINT);
    gfx.text35(lx, (int16_t)(y + 3), "TURBO", style ? SH_MID : SH_DIM);
    gfx.textScaled((int16_t)(x + (bw - tw) / 2), (int16_t)(y + 11), t,
                   style ? SH_ON : SH_DIM, 2);
  }
}
#endif

// =============================================================================
// MOTION LAYER (v1.08)
// =============================================================================
// v1.24: what is left of it is the page transitions. v1.08 also glided every
// fader, cursor and meter toward its target (a time-constant follower) and
// drew each moving edge with a partial grey pixel; both went with the
// Monomachine grid - the machine's own screen snaps, and so does this one.
//
//   transitions  page change = the pages slide sideways, subpage = crossfade.
//                Composited in panel space: one logical column is one 128-byte
//                panel row, so a slide is 64 memcpy's per frame. The default
//                for both is CUT (SET > TRANSITION).
static float g_dt             = 1.0f / 60.0f;  // seconds, set per rendered frame
static bool  g_motionBusy     = false;         // something was moving last frame
static bool  g_motionBusyNext = false;

static inline float mEaseOutCubic(float t) { const float u = 1.0f - t; return 1.0f - u * u * u; }

// Transition lengths.
static const uint32_t MT_SLIDE_MS = 160;
static const uint32_t MT_FADE_MS  = 113;

// ---- page transitions ------------------------------------------------------
static uint8_t  g_prevFb[PANEL_W * PANEL_H / 2];   // the page we are leaving
static uint8_t  g_tmpFb [PANEL_W * PANEL_H / 2];   // composite scratch
static int8_t   g_transKind  = 0;   // 0 none, +1/-1 slide, 2 fade, 3 wipe,
                                    // 4 dissolve, 5 flash
// v1.16: the page-transition style. v1.17: two of them, SET > TRANSITION -
// PAGES between the four main pages, SUB for anything touching a sub-page.
enum TransStyle : uint8_t { TS_SLIDE = 0, TS_WIPE, TS_DISSOLVE, TS_FLASH, TS_CUT, TS_COUNT };
static const char* const kTransName[TS_COUNT] = {"SLIDE", "WIPE", "DISSOLVE", "FLASH", "CUT"};
static uint8_t uiTransStyle = TS_CUT;        // main page <-> main page: instant
static uint8_t uiTransSub   = TS_CUT;        // v1.24: a cut, as on the machine -
                                             //   DISSOLVE is still in SET > TRANSITION
static uint32_t g_transStart = 0, g_transDur = 1;

static inline int transRowOf(int x) {              // logical column -> panel row
#if ROTATE_FLIP
  return x;
#else
  return PANEL_H - 1 - x;
#endif
}
static uint16_t g_transFramesCur = 0, g_transFramesLast = 0;   // 'trans' reports it
static void transBegin(int8_t kind, uint32_t durMs) {
  memcpy(g_prevFb, g_fb, sizeof g_fb);             // g_fb still holds the old page
  g_transKind  = kind;
  g_transFramesCur = 0;                            // count this transition only
  g_transStart = micros();
  g_transDur   = durMs * 1000u;
}
// Called after the NEW page has been drawn into g_fb, before flushAll().
static void transCompose(uint32_t nowUs) {
  if (!g_transKind) return;
  // v1.16 FIX - the reason no transition was ever seen (since v1.08).
  // transBegin() stamps g_transStart with a fresh micros(), but the frame it is
  // composed on uses nowUs, sampled at the TOP of loop() - earlier. So the very
  // first elapsed time was negative; as uint32 that is ~4.29e9 us, past every
  // duration, and the transition ended before drawing a single frame. Signed,
  // and clamped to 0: the first frame shows the old page, then it moves.
  int32_t sel = (int32_t)(nowUs - g_transStart);
  if (sel < 0) sel = 0;
  const uint32_t el = (uint32_t)sel;
  if (el >= g_transDur) { g_transKind = 0; g_transFramesLast = g_transFramesCur;
                          g_transFramesCur = 0; return; }
  g_transFramesCur++;
  const float e = mEaseOutCubic((float)el / (float)g_transDur);
  const int RB = PANEL_W / 2;
  if (g_transKind >= 3) {
    // v1.16 styles. All composited per NIBBLE straight in panel space - no
    // per-pixel function calls - so each costs well under a millisecond of a
    // 33 ms frame, and none of them waits for anything: time drives the frame,
    // the frame never drives time. Panel column pxx is logical row y (the
    // panel is mounted rotated); panel row is logical column x.
    // DISSOLVE (v1.16, smoothed): an 8x8 ordered-dither order - 64 steps, not
    // 16 - and every pixel CROSSFADES through the grey levels inside its own
    // window instead of snapping, so the page resolves softly. Paced by a
    // smoothstep rather than the ease-out the other styles use, so the change
    // is spread evenly across the frames instead of front-loaded.
    static const uint8_t kBayer8[8][8] = {
      { 0, 32,  8, 40,  2, 34, 10, 42}, {48, 16, 56, 24, 50, 18, 58, 26},
      {12, 44,  4, 36, 14, 46,  6, 38}, {60, 28, 52, 20, 62, 30, 54, 22},
      { 3, 35, 11, 43,  1, 33,  9, 41}, {51, 19, 59, 27, 49, 17, 57, 25},
      {15, 47,  7, 39, 13, 45,  5, 37}, {63, 31, 55, 23, 61, 29, 53, 21}};
    static const float DSOFT = 0.35f;              // each pixel's fade window
    const float tl = (float)el / (float)g_transDur;
    const float ts = tl * tl * (3.0f - 2.0f * tl) * (1.0f + DSOFT);
    const int wipeY = (int)(e * 257.0f) - 1;       // -1 .. 256
    for (int r = 0; r < PANEL_H; ++r) {
      uint8_t* nb = &g_fb[r * RB];
      const uint8_t* ob = &g_prevFb[r * RB];
      for (int c = 0; c < RB; ++c) {
        uint8_t hi = (uint8_t)(nb[c] >> 4), lo = (uint8_t)(nb[c] & 15);
        for (int k = 0; k < 2; ++k) {
          const int pxx = c * 2 + k;
#if ROTATE_FLIP
          const int y = PANEL_W - 1 - pxx, x = r;
#else
          const int y = pxx, x = PANEL_H - 1 - r;
#endif
          const uint8_t o = k ? (uint8_t)(ob[c] & 15) : (uint8_t)(ob[c] >> 4);
          uint8_t v = k ? lo : hi;
          if (g_transKind == 3) {                  // WIPE: new page drawn in
            if (y > wipeY) v = o;                  //   from the top, hard edge
            else if (y == wipeY) v = SH_ON;        //   led by a bright scan line
          } else if (g_transKind == 4) {           // DISSOLVE: soft ordered
            const float a = (ts - (float)kBayer8[y & 7][x & 7] * (1.0f / 64.0f)) / DSOFT;
            if (a <= 0.0f)     v = o;
            else if (a < 1.0f) v = (uint8_t)((float)o + ((float)v - (float)o) * a + 0.5f);
          } else {                                 // FLASH: new page, inverted,
            v = (uint8_t)(15 - v);                 //   for the first frames only
          }
          if (k) lo = v; else hi = v;
        }
        nb[c] = (uint8_t)((hi << 4) | lo);
      }
    }
  } else if (g_transKind == 2) {                   // crossfade, per nibble
    const uint16_t a = (uint16_t)(e * 16.0f + 0.5f), ia = (uint16_t)(16 - a);
    for (unsigned i = 0; i < sizeof g_fb; ++i) {
      const uint8_t o = g_prevFb[i], n = g_fb[i];
      const uint8_t hi = (uint8_t)((((o >> 4) * ia) + ((n >> 4) * a) + 8) >> 4);
      const uint8_t lo = (uint8_t)((((o & 15) * ia) + ((n & 15) * a) + 8) >> 4);
      g_fb[i] = (uint8_t)((hi << 4) | lo);
    }
  } else {                                         // slide sideways
    const int off = (int)(e * (float)SCR_W + 0.5f);
    for (int x = 0; x < SCR_W; ++x) {
      const uint8_t* src;
      if (g_transKind > 0) {                       // next page enters from the right
        const int s2 = x + off;
        src = (s2 < SCR_W) ? &g_prevFb[transRowOf(s2) * RB] : &g_fb[transRowOf(s2 - SCR_W) * RB];
      } else {                                     // previous page enters from the left
        const int s2 = x - off;
        src = (s2 >= 0) ? &g_prevFb[transRowOf(s2) * RB] : &g_fb[transRowOf(s2 + SCR_W) * RB];
      }
      memcpy(&g_tmpFb[transRowOf(x) * RB], src, RB);
    }
    memcpy(g_fb, g_tmpFb, sizeof g_fb);
  }
  g_motionBusyNext = true;
}

// -----------------------------------------------------------------------------
// PAGE 1 — LFO OVERVIEW
// -----------------------------------------------------------------------------
// v1.24 - SIX STRIPS ON THE GRID.
//   y  0..9    header: LFO and the tempo, rule at 9
//   y 11..18   RUN / STOP (3x5) and the turbo badge, rule at 19
//   y 20..241  six strips of 37px, each ending in a G_DARK rule:
//     +0..8    the title line - number (5x7 MID), destination (5x7 LIGHT),
//              value (5x7 WHITE, right). The selected LFO is the focus
//              block, the full width. A bypassed LFO is all MID, "OFF".
//     +10..30  its scope: 1px frame (G_MID in focus, G_DARK otherwise)
//     +32..33  depth (x 0..29) and amount (x 34..63), 2px bars
// The v1.10 page marked the selection with a gliding 2px bar, a chip for the
// selected name and a dimmer text for the rest; one block says it alone.
static void drawOverview(bool running, const char* statLbl, uint32_t bpmX100) {
  gfx.clear();
  const int16_t W = UI_W;
  char b[12], v[8];

  { bpmStr(bpmX100, b);
    uint8_t k = 0; while (b[k] && b[k] != '.') k++;
    b[k] = 0;                                   // integer BPM; the decimal is noise
    stHeader("LFO", b); }
  gfx.text35(0, 12, running ? "RUN" : "STOP", running ? G_WHITE : G_MID);
  if (statLbl && *statLbl) gfx.text35(22, 12, statLbl, G_LIGHT);
  drawTurboBadge((int16_t)(W - 1), 11);
  gfx.hLine(0, 19, W, G_DARK);

  const int16_t top = 20, rowH = 37;
  for (uint8_t i = 0; i < LFO_COUNT; ++i) {
    const int16_t y = (int16_t)(top + i * rowH);
    const LfoParams& p = lfo.p[i];
    uint8_t nsh = G_MID, dsh = p.enabled ? G_LIGHT : G_MID, vsh = p.enabled ? G_WHITE : G_MID;
    if (ui.sel == i) nsh = dsh = vsh = stFocus(0, y, W, ST_ROW);
    const char nb[2] = {(char)('1' + i), 0};
    gfx.text(1, (int16_t)(y + 1), nb, nsh);
    const char* nm = destName(i);
    gfx.text(9, (int16_t)(y + 1), nm, dsh);
    if (!p.enabled) strcpy(v, "OFF");
    else paramFormat(p.page, p.dest, p.track, lfo.s[i].value, v);
    stText((int16_t)(W - 1), (int16_t)(y + 1),
           (int16_t)(W - 13 - gfx.textWidth(nm)), v, vsh, 2);

    drawWaveBox(0, (int16_t)(y + 10), W, 21, i, lfo.driving(i),
                (ui.sel == i && p.enabled) ? G_WHITE : G_MID);
    const uint8_t msh = p.enabled ? G_WHITE : G_MID;
    stBarH(0,  (int16_t)(y + 32), 30, 2, p.depth,  127, BAR_FILL, msh);
    stBarH(34, (int16_t)(y + 32), 30, 2, p.amount, 127, BAR_FILL, msh);
    if (i + 1 < LFO_COUNT) gfx.hLine(0, (int16_t)(y + rowH - 1), W, G_DARK);
  }
  stFooter("E3 DPTH", "E4 AMT");
}

// -----------------------------------------------------------------------------
// PAGE 2 — LFO EDIT
// -----------------------------------------------------------------------------
// v1.24 - THE MONOMACHINE'S LFO PAGE. The eight LFO parameters in the order
// the machine lists them, as a 2 x 4 grid of cells. E1 walks the cells in
// reading order; E2 changes the one in focus.
//
//   y  0..9     header: EDIT and the LFO, rule at 9
//   y 11..19    track (3x5 MID) and its machine (5x7 LIGHT), rule at 20
//   y 21..104   the grid: cells x 0..30 and 32..63 either side of a rule at
//               x 31, 21 tall with a rule under each pair. In a cell:
//                 +1..7   the name, 3x5 - the focus block, the cell's full
//                         width, on the selected one
//                 +10..16 the value, 5x7 WHITE (WAVE adds its shape, LIGHT)
//                 +18..19 a 2px bar: a block at the entry for PAGE, DEST,
//                         TRIG, WAVE and MULT; a fill for SPD, INTL, DPTH
//   y 106..153  the steps: bars/page and the cursor step's probability, then
//               4 x 4 cells of 14 x 8 on a 16 x 10 pitch. A cell's fill
//               HEIGHT is its probability; the frame is WHITE on the cursor,
//               LIGHT on the playing step, MID on an on step, DARK when off.
//               Steps past an 8-step sequence are a dotted DARK dash.
//   y 158..215  the scope, 1px frame, the output value knocked out top right
//   y 219..223  the reachable window: LO and HI either side of a bar - the
//               window MID on DARK, the output a 1px WHITE line
//   y 227..235  AMT: label, bar, value
static void drawEdit(uint32_t absStep16, bool running) {
  gfx.clear();
  const LfoParams& p = lfo.p[ui.sel];
  const LfoState&  s = lfo.s[ui.sel];
  const int16_t W = UI_W;
  char b[16], v[8];

  { b[0] = 'L'; b[1] = (char)('1' + ui.sel); b[2] = 0;
    stHeader("EDIT", b); }

  // Track and machine - an unknown machine says so rather than lying about a
  // profile we do not have.
  { const uint8_t t = (p.track < 6) ? p.track : 0;
    b[0] = 'T'; b[1] = (char)('1' + t); b[2] = 0;
    gfx.text35(0, 13, b, G_MID);
    stText(10, 12, (int16_t)(W - 10), mnmMachineLabel(machineSel[t]), G_LIGHT, 0); }
  gfx.hLine(0, 20, W, G_DARK);

  // ---- the eight parameters -----------------------------------------------
  static const char* const kLbl[8] = {
      "PAGE", "DEST", "TRIG", "WAVE", "MULT", "SPD", "INTL", "DPTH"};
  char nb[3][4];
  u8s(p.spd, nb[0]); u8s(p.intl, nb[1]); u8s(p.depth, nb[2]);
  const char* vals[8] = {kMnmPages[p.page].name, destName(ui.sel),
                         kTrigNames[p.trig], kWaveShort[p.wave],
                         kMultNames[p.mult], nb[0], nb[1], nb[2]};
  const int16_t bv[8] = {p.page, p.dest, p.trig, p.wave, p.mult, p.spd, p.intl, p.depth};
  const int16_t bm[8] = {PAGE_COUNT - 1, (int16_t)(kMnmPages[p.page].count - 1),
                         TRIG_COUNT - 1, WAVE_COUNT - 1, LFO_MULT_MAX, 127, 127, 127};
  const int16_t GY = 21, CH = 21;
  gfx.vLine(31, GY, (int16_t)(4 * CH - 1), G_DARK);
  for (uint8_t r = 0; r < 8; ++r) {
    const int16_t x0 = (r & 1) ? 32 : 0, cw = (r & 1) ? 32 : 31;
    const int16_t y  = (int16_t)(GY + (r >> 1) * CH);
    const uint8_t lsh = (ui.paramRow == r) ? stFocus(x0, (int16_t)(y + 1), cw, ST_CHIP_H)
                                           : G_MID;
    gfx.text35((int16_t)(x0 + 2), (int16_t)(y + 2), kLbl[r], lsh);
    stText((int16_t)(x0 + 2), (int16_t)(y + 10), (int16_t)(cw - 4 - (r == 3 ? 10 : 0)),
           vals[r], G_WHITE, 0);
    // The WAVE cell carries the shape beside its name: "SAW" and "ISW" are
    // three letters apart, the picture is instant.
    if (r == 3) waveGlyph((int16_t)(x0 + cw - 10), (int16_t)(y + 11), p.wave, G_LIGHT);
    stBarH((int16_t)(x0 + 2), (int16_t)(y + 18), (int16_t)(cw - 4), 2, bv[r], bm[r],
           r < 5 ? BAR_THUMB : BAR_FILL, G_WHITE);
    if (r & 1) gfx.hLine(0, (int16_t)(y + CH - 1), W, G_DARK);
  }

  // ---- the probability grid ------------------------------------------------
  // Spec: each step is one of nine activation levels, so a cell is a fill
  // HEIGHT, not on/off - a pattern thinning toward the end of a bar is a shape.
  const uint8_t sc = (p.stepCount == 8) ? 8 : 16;
  { b[0] = 'B'; b[1] = (char)('0' + p.bars); b[2] = '/';
    b[3] = (char)('1' + ui.stepPage); b[4] = 0;
    gfx.text35(0, 108, "STEP", G_MID);
    gfx.text35(19, 108, b, G_LIGHT);
    const LfoStep& cs = p.steps[(ui.stepPage * sc + (ui.stepCur % sc)) % LFO_STEPS];
    if (!cs.on) strcpy(v, "OFF");
    else { u8s(kStepProbPct[cs.prob < PROB_COUNT ? cs.prob : (uint8_t)PROB_100], v);
           strcat(v, "%"); }
    stText(W, 107, 30, v, cs.on ? G_WHITE : G_MID, 2); }

  const uint16_t seqLen = (uint16_t)p.bars * sc;
  const uint16_t live = seqLen ? (uint16_t)(absStep16 % seqLen) : 0;
  for (uint8_t k = 0; k < 16; ++k) {
    const int16_t sx = (int16_t)(1 + (k % 4) * 16), sy = (int16_t)(116 + (k / 4) * 10);
    // At stepCount 8 the back half is not part of the sequence: a dotted dash
    // where the cell would be, so the grid keeps its shape.
    if (k >= sc) { gfx.dotHLine(sx, (int16_t)(sy + 4), 14, G_DARK, 2); continue; }
    const uint8_t idx = (uint8_t)((ui.stepPage * sc + k) % LFO_STEPS);
    const LfoStep& st = p.steps[idx];
    uint8_t fr = st.on ? G_MID : G_DARK;
    if (running && (uint16_t)(ui.stepPage * sc + k) == live) fr = G_LIGHT;
    if ((ui.stepCur % sc) == k) fr = G_WHITE;
    gfx.rect(sx, sy, 14, 8, fr);
    if (st.on) {
      const uint8_t pct = kStepProbPct[st.prob < PROB_COUNT ? st.prob : (uint8_t)PROB_100];
      int16_t fh = (int16_t)((6 * pct + 50) / 100);
      if (fh < 1) fh = 1;
      gfx.fillRect((int16_t)(sx + 1), (int16_t)(sy + 7 - fh), 12, fh, G_WHITE);
    }
  }
  gfx.hLine(0, 156, W, G_DARK);

  // ---- scope ---------------------------------------------------------------
  const int16_t boxY = 158, boxH = 58;
  drawWaveBox(0, boxY, W, boxH, ui.sel, lfo.driving(ui.sel),
              p.enabled ? G_WHITE : G_MID);
  // The value as the machine would show it (v1.22), knocked out of the trace
  // so a moving line can never run through a digit.
  paramFormat(p.page, p.dest, p.track, p.enabled ? s.value : lfo.restValue(ui.sel), v);
  { const int16_t vw = gfx.textWidth(v) <= 40 ? gfx.textWidth(v) : gfx.width35(v);
    gfx.fillRect((int16_t)(W - 3 - vw), (int16_t)(boxY + 1), (int16_t)(vw + 2), 9, G_BLACK);
    stText((int16_t)(W - 2), (int16_t)(boxY + 2), 40, v, G_WHITE, 2); }

  // ---- the reachable window --------------------------------------------------
  { const int16_t ry = 219;
    u8s3(p.lo, b); gfx.text35(0, ry, b, G_MID);
    u8s3(p.hi, v); gfx.text35R(W, ry, v, G_MID);
    const int16_t bx = 14, bw = 36;
    const int16_t lo = p.lo <= p.hi ? p.lo : p.hi, hi = p.lo <= p.hi ? p.hi : p.lo;
    const int16_t xa = (int16_t)(bx + (bw - 1) * lo / 127), xb = (int16_t)(bx + (bw - 1) * hi / 127);
    gfx.fillRect(bx, ry, bw, 5, G_DARK);
    gfx.fillRect(xa, (int16_t)(ry + 1), (int16_t)(xb - xa + 1), 3, G_MID);
    gfx.vLine((int16_t)(bx + (bw - 1) * s.value / 127), ry, 5, G_WHITE); }

  // ---- amount --------------------------------------------------------------
  { u8s(p.amount, v);
    stRow(227, "AMT", v, false);
    stBarH(18, 230, 26, 3, p.amount, 127, BAR_FILL, G_WHITE); }

  stFooter("E2 VAL", "E4 PROB");
}

// -----------------------------------------------------------------------------
// PAGE 3 — PATTERN GENERATOR
// -----------------------------------------------------------------------------
// Header, transport, key, the four-bar progression, the viewed bar's chord, a
// 6-track step grid with a playhead, the whole pattern in miniature, and a live
// read-out of what each track is sounding right now with its velocity.
//
// That last block replaced the encoder legend. A legend is a thing you read
// once; what note the bass is actually playing is a thing you read constantly,
// and it is the only way to tell from the box whether `gen k 36` put the line
// where you wanted it. The encoder map lives in the file header and in `?`.
// v1.24 - on the grid:
//   y  0..9    header: PAT and the genre
//   y 11..17   PLAY / STOP (5x7) and the tempo, rule at 19
//   y 20..28   KEY row
//   y 30..38   the progression: four 15px cells on 1px rules, the bar being
//              viewed is the focus block
//   y 41..47   that bar's chord tones, 5x7 LIGHT; rule at 49
//   y 51..103  six tracks x 16 steps: the role label (focus block on the
//              cursor track, struck through when muted), a 2x5 WHITE bar per
//              note (2x2 for a ghost, broken for a ratchet), a WHITE dot under
//              an accent, a LIGHT one under a lock, a DARK dot for a rest, and
//              a 1px LIGHT playhead; rule at 105
//   y 106..126 the whole pattern in miniature; rule at 129
//   y 130..138 bar / seed row; rule at 140
//   y 143..    PLAYING: what each track sounds right now - the role WHITE
//              when it plays, its note in 5x7 and its velocity as a bar
static void drawPattern(uint32_t absStep16, bool running, uint32_t bpmX100) {
  gfx.clear();
  const int16_t W = UI_W;
  char b[20], v[8];

  stHeader("PAT", kPatGenreShort[pat.genre]);

  // Engine state and tempo. A state, not a focus: PLAY is WHITE, STOP MID.
  gfx.text(0, 11, pat.engineOn ? "PLAY" : "STOP", pat.engineOn ? G_WHITE : G_MID);
  bpmStr(bpmX100, b);
  gfx.textRight(W, 11, b, running ? G_WHITE : G_MID);
  gfx.hLine(0, 19, W, G_DARK);

  // Key and scale - the line you check before you play a note.
  { char nn[6]; patNoteName(pat.root, nn);
    uint8_t k2 = 0;
    for (uint8_t i2 = 0; nn[i2] && k2 < 6; ++i2) b[k2++] = nn[i2];
    b[k2++] = ' ';
    const char* sc2 = kPatScaleName[pat.scale];
    while (*sc2 && k2 < 14) b[k2++] = *sc2++;
    b[k2] = 0;
    stRow(20, "KEY", b, false); }

  // The four-bar progression: one cell per bar on 1px rules, the bar the grid
  // shows is the focus block. "VII" and "iii" drop to 3x5 to fit their cell.
  { static const char* const kRoman[7] = {"i","ii","iii","iv","v","VI","VII"};
    const ChordProg& cp = kProg[pat.genre];
    for (uint8_t bar = 0; bar < 4; ++bar) {
      const int16_t x0 = (int16_t)(bar * 16);
      const bool live = (bar < pat.bars);
      const int8_t dg = cp.deg[bar % 4];
      uint8_t sh = live ? G_LIGHT : G_MID;
      if (live && bar == patUiBar) sh = stFocus(x0, 30, 15, ST_ROW);
      stText((int16_t)(x0 + 8), 31, 13, live ? kRoman[((dg % 7) + 7) % 7] : "-", sh, 1);
      if (bar < 3) gfx.vLine((int16_t)(x0 + 15), 30, ST_ROW, G_DARK);
    } }

  // Chord tones of the viewed bar - the answer to "did gen k 36 put the line
  // where I wanted it", which no amount of grid drawing gives you.
  { uint8_t k2 = 0;
    for (uint8_t vo = 0; vo < 4; ++vo) {
      const uint8_t n = chordNote(patUiBar, vo, 0);
      const char* nn = kNoteLetter[n % 12];
      while (*nn && k2 < 14) b[k2++] = *nn++;
      if (vo < 3 && k2 < 14) b[k2++] = ' ';
    }
    b[k2] = 0;
    stText(0, 41, W, b, G_LIGHT, 0); }
  gfx.hLine(0, 49, W, G_DARK);

  // ---- 6-track grid for the viewed bar -------------------------------------
  const char* const* kTrLbl = kRole2[patRoleSet()];   // roles follow the genre
  const int16_t cellW = 3, rowH = 9, gridY = 51;
  const int16_t gridX = (int16_t)(W - 16 * cellW);
  const uint16_t seqLen = (uint16_t)pat.bars * PAT_SPB;
  const uint16_t live   = seqLen ? (uint16_t)(absStep16 % seqLen) : 0;
  const uint16_t liveBar = (uint16_t)(live / PAT_SPB);
  const uint16_t liveStep = (uint16_t)(live % PAT_SPB);

  for (uint8_t t = 0; t < PAT_TRACKS; ++t) {
    const int16_t y = (int16_t)(gridY + t * rowH);
    const bool on = pat.trackOn[t];
    uint8_t lsh = on ? G_LIGHT : G_MID;
    if (patUiTrack == t) lsh = stFocus(0, y, 13, ST_CHIP_H);
    gfx.text35(2, (int16_t)(y + 1), kTrLbl[t], lsh);
    if (!on) gfx.hLine(1, (int16_t)(y + 3), 11, lsh);          // muted: struck through
    const uint8_t nsh = on ? G_WHITE : G_MID;
    for (uint8_t k2 = 0; k2 < 16; ++k2) {
      const int16_t cx = (int16_t)(gridX + k2 * cellW);
      const uint16_t idx = (uint16_t)(patUiBar * PAT_SPB + k2);
      if (idx >= seqLen) continue;
      const PatStep& st = pat.step[t][idx];
      if (st.on && st.roll >= 2) {
        // A ratchet reads as a broken column - several hits in one cell.
        for (int16_t yy = 1; yy <= 5; yy += 2) gfx.fillRect(cx, (int16_t)(y + yy), 2, 1, nsh);
      } else if (st.on) {
        gfx.fillRect(cx, (int16_t)(y + (st.ghost ? 4 : 1)), 2, st.ghost ? 2 : 5, nsh);
        if (st.accent) gfx.px((int16_t)(cx + 1), (int16_t)(y + 7), G_WHITE);
      } else {
        gfx.px(cx, (int16_t)(y + 3), G_DARK);
      }
      if (patLockN && patLockAt(t, idx)) gfx.px(cx, (int16_t)(y + 7), G_LIGHT);
      if (running && liveBar == patUiBar && liveStep == k2)
        gfx.vLine((int16_t)(cx + 2), y, (int16_t)(rowH - 1), G_LIGHT);
    }
  }
  gfx.hLine(0, 105, W, G_DARK);

  // ---- whole pattern in miniature, one pixel column per step ---------------
  { const int16_t my = 108;
    const uint16_t full = (uint16_t)pat.bars * PAT_SPB;
    for (uint8_t t = 0; t < PAT_TRACKS; ++t)
      for (uint16_t k2 = 0; k2 < full && k2 < (uint16_t)W; ++k2)
        if (pat.step[t][k2].on)
          gfx.vLine((int16_t)k2, (int16_t)(my + t * 3), 2, pat.trackOn[t] ? G_WHITE : G_MID);
    if (running && full)
      gfx.vLine((int16_t)(absStep16 % full), (int16_t)(my - 1),
                (int16_t)(PAT_TRACKS * 3 + 1), G_LIGHT);
    for (uint8_t bar = 1; bar < pat.bars; ++bar)
      gfx.px((int16_t)(bar * PAT_SPB), (int16_t)(my + PAT_TRACKS * 3 + 1), G_MID); }
  gfx.hLine(0, 129, W, G_DARK);

  // ---- bar / seed ----------------------------------------------------------
  { b[0] = 'B'; b[1] = (char)('1' + patUiBar); b[2] = '/';
    b[3] = (char)('0' + pat.bars); b[4] = 0;
    uint32_t seed = pat.seed; char t2[12]; uint8_t n = 0;
    while (seed && n < 10) { t2[n++] = (char)('0' + seed % 10); seed /= 10; }
    if (!n) t2[n++] = '0';
    uint8_t k2 = 0;
    while (n && k2 < 5) v[k2++] = t2[--n];     // low digits are the ones that vary
    v[k2] = 0;
    stRow(130, b, v, false); }
  gfx.hLine(0, 140, W, G_DARK);

  // ---- live read-out: what each track is actually sounding -----------------
  gfx.text35(0, 143, "PLAYING", G_MID);
  for (uint8_t t = 0; t < PAT_TRACKS; ++t) {
    const int16_t y = (int16_t)(151 + t * 11);
    const bool act = patActive[t].active && pat.trackOn[t];
    gfx.text35(2, (int16_t)(y + 1), kTrLbl[t], act ? G_WHITE : G_MID);
    if (act) {
      char nn[6]; patNoteName(patActive[t].note, nn);
      gfx.text(16, y, nn, G_WHITE);
      // Velocity as a bar: six bar lengths side by side are the mix at a glance.
      stBarH(38, (int16_t)(y + 2), (int16_t)(W - 38), 3, patActive[t].vel, 127, BAR_FILL, G_WHITE);
    } else {
      gfx.text35(16, (int16_t)(y + 1), "--", G_MID);
    }
  }
  // E6 turns the cursor as always; on a scene its PUSH sends the scene, and the
  // footer says so - then confirms for a moment after it has gone out.
  if (patIsScene(pat.genre))
    stFooter("E1 GEN", (millis() - g_sceneSentMs < 1500u && g_sceneSentMs) ? "SENT" : "E6 SCN");
  else
    stFooter("E1 GEN", "E6 TRK");
}


// -----------------------------------------------------------------------------
// PAGE 4 - SIX-ENCODER PERFORMANCE
// -----------------------------------------------------------------------------
// Geometry ported verbatim from pagePerform() in XY6_DEMO.ino, the sketch the
// mockups were rendered from: three columns at x = 7, 26, 45, faders 13 wide.
// The bank extents are now the reference render's own - 24..127 and 129..231,
// decoded from the 7x PNG rather than eyeballed. Because the numbers come
// from the original rather than from measuring a screenshot, this is 1:1 by
// construction in the 3x5 font.
//
// The disc version of the design puts a joystick field between the two banks.
// Your newer mockup does not, so it is off by default.
#define PERF_SHOW_JOYFIELD 0

// v1.24 - THE PERF GRID. The encoders' own layout, 3 x 2, as the cells of one
// rigid grid (the v1.14 capsules, number chips and triangle cursor are gone):
//
//   x  0        joystick X meter             x 63       joystick Y meter
//   x  2..20    column 1    rule x 21        x 22..41   column 2
//   x 43..61    column 3    rule x 42
//   y  0..126   bank 1 (E1-E3)   rule y 127   y 128..255 bank 2 (E4-E6)
//
// Each cell, from its top:
//   +0..6     the name, 4 letters of 3x5 - on the slot the last turn touched,
//             the focus block, the cell's full width
//   +9..15    the value, 5x7 WHITE (3x5 when it will not fit); "--" in MID
//             for a slot nothing is known about
//   +18..123  a 6 x 106 bar standing up: G_DARK track, WHITE value. Bipolar
//             parameters fill from the middle, lists show a block at their
//             entry. Under a running LFO the bar is the live output and a
//             1px G_LIGHT notch across it marks the centre the knob sets.
static const int16_t kPerfCellX[3] = {2, 22, 43};
static const int16_t kPerfCellW[3] = {19, 20, 19};
static const int16_t PERF_BANK_Y[2] = {0, 128};
static const int16_t PERF_BAR_Y = 18, PERF_BAR_H = 106, PERF_BAR_W = 6;

// Six live parameters, one per encoder. Labels and defaults match the mockup.
// The fixed label is gone. Once a destination is editable the label has to come
// FROM the destination, or the moment you change one the panel is lying about
// what the knob does. mnmParamName() is the source of truth, clipped to the
// three characters the 13-pixel column can hold.
//
// track is now per slot rather than one global. Six encoders addressing six
// different Monomachine tracks is the whole point of a performance page, and
// the single perfTrack it replaced made that impossible.
// `known` is the fix for a class of bug this file has hit twice now: cached
// state that outlives the address it belonged to. Re-point a slot and its old
// VALUE is meaningless - the machine has never been told that number for this
// parameter, and the parameter almost certainly holds something else. Drawing
// it anyway makes the panel confidently wrong, which is exactly what the fixed
// `label` field used to do.
//
// So a slot starts unknown - at boot these six numbers are arbitrary defaults,
// not anything read from the machine - and becomes known when you move it or
// when the Monomachine tells us what the parameter actually holds.
//
// v1.22: `value` of an unknown slot is no longer arbitrary. It is the kit
// model's belief - the init-kit default until something is learned - so the
// first turn of a knob starts from what a freshly loaded kit really holds, and
// a slot pointed at a parameter the kit model HAS learned arrives known.
// perfSeed() below does that; setup() seeds all six.
struct PerfSlot { uint8_t page, dest, value, track; bool known; };
static PerfSlot perfSlot[6] = {
    {PAGE_AMP,  2, 64, 0, false}, {PAGE_FILT, 3,   0, 0, false},
    {PAGE_AMP,  6, 64, 0, false}, {PAGE_AMP,  3,  64, 0, false},
    {PAGE_AMP,  0,  0, 0, false}, {PAGE_FILT, 1, 127, 0, false},
};
// "--" until we have a value we can stand behind. out needs 8 bytes.
static void perfValStr(uint8_t i, char* out) {
  if (perfSlot[i].known) {
    paramFormat(perfSlot[i].page, perfSlot[i].dest, perfSlot[i].track,
                perfSlot[i].value, out);
    out[4] = 0;          // four characters is all a 19-pixel column holds
  }
  else { out[0] = '-'; out[1] = '-'; out[2] = 0; }
}
static void perfLabel(uint8_t i, char* out4) {   // out4 needs 5 bytes
  clipN(paramName(perfSlot[i].page, perfSlot[i].dest, perfSlot[i].track), out4, 4);
}
// Point a slot's value at what the kit model believes its parameter holds.
// Called whenever a slot is re-addressed (page, parameter or track).
static void perfSeed(uint8_t i) {
  if (i >= 6) return;
  const PerfSlot& p = perfSlot[i];
  perfSlot[i].value = kitGet(p.track, p.page, p.dest);
  perfSlot[i].known = kitLearned(p.track, p.page, p.dest);
}
static uint8_t perfCursor = 0;     // which slot the last turn touched
// The value last transmitted for each slot. An incoming CC carrying exactly
// this is our own output coming back through a THRU or a merge, not a knob
// being turned on the machine.
static uint8_t perfSent[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Queued, never written straight to the port - same discipline as everything
// else that transmits, so a performance sweep cannot stall the MIDI loop.
// v1.12 - PERF and the LFOs.
// The LFO (lowest index) currently DRIVING slot c's exact destination, or -1.
// Same track, same page, same parameter - that is the only case where the two
// are fighting over one knob on the machine.
static int8_t perfLfoFor(uint8_t c) {
  if (c >= 6) return -1;
  for (uint8_t i = 0; i < LFO_COUNT; ++i) {
    const LfoParams& p = lfo.p[i];
    if (lfo.driving(i) && p.track == perfSlot[c].track &&
        p.page == perfSlot[c].page && p.dest == perfSlot[c].dest) return (int8_t)i;
  }
  return -1;
}
static bool perfAnyLfoLinked() {
  for (uint8_t c = 0; c < 6; ++c) if (perfLfoFor(c) >= 0) return true;
  return false;
}
// The knob sets the centre of EVERY LFO on that destination - running or not -
// so one that starts later (TRIG, ONE, HALF) swings around the fader, not
// around whatever it last captured.
static void perfSetLfoBase(uint8_t c, uint8_t v) {
  for (uint8_t i = 0; i < LFO_COUNT; ++i) {
    LfoParams& p = lfo.p[i];
    if (p.track == perfSlot[c].track && p.page == perfSlot[c].page &&
        p.dest == perfSlot[c].dest) { p.baseValue = v; lfo.setCapture(i, v); }
  }
}

static void perfSend(uint8_t i) {
  if (i >= 6) return;
  const uint8_t cc = mnmCC(perfSlot[i].page, perfSlot[i].dest);
  if (cc == 0xFF) return;
  const uint8_t ch =
      (uint8_t)(0xB0 | ((txChannel - 1 + perfSlot[i].track) & 0x0F));
  const uint8_t m[3] = {ch, cc, (uint8_t)(perfSlot[i].value & 0x7F)};
  if (qCtrl.push(m, 3)) {
    perfSent[i] = perfSlot[i].value;
    // The machine now holds it - the kit model says so too (v1.22).
    kitSet(perfSlot[i].track, perfSlot[i].page, perfSlot[i].dest, perfSlot[i].value);
  }
}

// v1.22. An LFO pointed somewhere new swings around what THAT parameter
// holds. The capture it had belonged to the old destination - keeping it is
// how an LFO moved from VOL (64) to ATCK (0) used to open every attack
// halfway. The kit model knows the new parameter's value, or its default.
static void lfoRetarget(uint8_t i) {
  if (i >= LFO_COUNT) return;
  LfoParams& p = lfo.p[i];
  const uint8_t v = kitGet(p.track, p.page, p.dest);
  p.baseValue = v;
  if (kitLearned(p.track, p.page, p.dest)) lfo.setCapture(i, v);
  else lfo.s[i].hasCap = false;
}

// v1.22. Choose the machine on track t (0 = none). Its SYNT page becomes that
// machine's init values, and every LFO and PERF slot aimed at that SYNT page
// re-reads them - the old machine's numbers mean nothing on the new one.
static void applyMachine(uint8_t t, uint8_t id) {
  if (t >= 6) return;
  if (id >= MACHINE_COUNT) id = 0;
  if (machineSel[t] == id) return;
  machineSel[t] = id;
  kitResetPages(t, PAGE_SYNT, PAGE_SYNT);
  for (uint8_t i = 0; i < LFO_COUNT; ++i)
    if (lfo.p[i].track == t && lfo.p[i].page == PAGE_SYNT) lfoRetarget(i);
  for (uint8_t c = 0; c < 6; ++c)
    if (perfSlot[c].track == t && perfSlot[c].page == PAGE_SYNT) {
      perfSeed(c); perfSent[c] = 0xFF;
    }
  uiTouch();
}

// The joystick, one pixel wide at each edge: X at x 0, Y at x 63, centred on
// the bank rule (y 127) - the stick's rest. A G_DARK track and the deflection
// in WHITE from the centre, up for + and down for -. (v1.13's dashed tracks,
// letters and anti-aliased tip gave way to the grid; the meters still appear
// only while the stick is in use, and now appear and go at once.)
static void perfJoyMeter(int16_t x, uint8_t v) {
  stBarV(x, 0, 1, 256, v, 127, BAR_CENTRE, G_WHITE);
}

// One cell - see the grid above.
static void perfDrawCell(uint8_t sl, uint8_t col, int16_t y0) {
  const PerfSlot& ps = perfSlot[sl];
  const int16_t x0 = kPerfCellX[col], w = kPerfCellW[col];
  const int16_t cx = (int16_t)(x0 + w / 2);
  char lb[6], vb[8];
  perfLabel(sl, lb);
  const uint8_t lsh = (perfCursor == sl) ? stFocus(x0, y0, w, ST_CHIP_H) : G_MID;
  gfx.text35Centre(cx, (int16_t)(y0 + 1), lb, lsh);

  const int16_t bx = (int16_t)(x0 + (w - PERF_BAR_W) / 2), by = (int16_t)(y0 + PERF_BAR_Y);
  const int8_t li = perfLfoFor(sl);
  if (li >= 0) {
    // Under a running LFO: what the machine is actually being sent, with the
    // centre the knob sets as a notch across the bar.
    const uint8_t live = lfo.s[li].value;
    paramFormat(ps.page, ps.dest, ps.track, live, vb);
    vb[4] = 0;
    stText(cx, (int16_t)(y0 + 9), (int16_t)(w - 1), vb, G_WHITE, 1);
    stParamBar(bx, by, PERF_BAR_W, PERF_BAR_H, true, ps.page, ps.dest, ps.track, live, G_WHITE);
    const int16_t ty = (int16_t)(by + PERF_BAR_H - 1 -
        stParamPos(PERF_BAR_H, ps.page, ps.dest, ps.track, lfo.restValue((uint8_t)li)));
    gfx.hLine((int16_t)(bx - 2), ty, (int16_t)(PERF_BAR_W + 4), G_LIGHT);
    return;
  }
  // An empty track is the honest picture of a slot we have no value for.
  perfValStr(sl, vb);
  stText(cx, (int16_t)(y0 + 9), (int16_t)(w - 1), vb, ps.known ? G_WHITE : G_MID, 1);
  if (ps.known)
    stParamBar(bx, by, PERF_BAR_W, PERF_BAR_H, true, ps.page, ps.dest, ps.track, ps.value, G_WHITE);
  else
    gfx.fillRect(bx, by, PERF_BAR_W, PERF_BAR_H, G_DARK);
}

static void drawPerform(uint32_t step16, bool running, uint32_t bpmX100) {
  (void)step16; (void)running; (void)bpmX100;
  gfx.clear();
  gfx.vLine(21, 0, 256, G_DARK);
  gfx.vLine(42, 0, 256, G_DARK);
  gfx.hLine(1, 127, 62, G_DARK);
  for (uint8_t c = 0; c < 3; ++c) {
    perfDrawCell(c, c, PERF_BANK_Y[0]);
    perfDrawCell((uint8_t)(c + 3), c, PERF_BANK_Y[1]);
  }
  // Only when a joystick is compiled in, and only while it is in use.
  if (g_joyUiX != 0xFF && g_joyShow) {
    perfJoyMeter(0,  g_joyUiX);
    perfJoyMeter(63, g_joyUiY);
  }
}

// -----------------------------------------------------------------------------
// BOOT SEQUENCE
// -----------------------------------------------------------------------------
// Inverted Elektron aesthetic: black geometry on a white field, which is the
// reverse of everything else this OS draws. It reads as a deliberate mode
// rather than a page that has not loaded yet, and the final phase dissolves the
// white back to black so the UI arrives rather than snapping in.
//
// Nothing moves linearly and nothing blinks. Every element is positioned and
// shaded by an easing function of elapsed time, so a frame that arrives late
// lands where it should have rather than sliding the whole sequence later. That
// also means the animation is a PURE FUNCTION of one number - drawBoot(ms) with
// no state and no history - which is what lets it be interrupted, restarted or
// skipped at any instant without leaving anything half-finished.
//
// ---- the easing functions ---------------------------------------------------
// All take and return 0..1. Written as multiplies rather than powf() because
// they run 60 times a second and powf on a general float is far more expensive
// than three multiplications.
//
//   easeOutCubic     1-(1-t)^3      decelerates into its target. The workhorse
//                                   for anything arriving on screen: fast at
//                                   first, settling gently, which reads as
//                                   weight rather than teleportation.
//   easeInOutCubic   4t^3 / ...     accelerates then decelerates, symmetric.
//                                   Used for the field fades, where a hard
//                                   start or stop would look like a switch.
//   easeOutQuint     1-(1-t)^5      a more emphatic deceleration for the rule,
//                                   which travels furthest and would look slow
//                                   under cubic.
//   easeInOutSine    (1-cos(pi t))/2  gentlest of the three, no discontinuity
//                                   in any derivative. The progress sweep uses
//                                   it so nothing about it draws attention.
static inline float easeOutCubic(float t) {
  const float u = 1.0f - t;
  return 1.0f - u * u * u;
}
static inline float easeOutQuint(float t) {
  const float u = 1.0f - t;
  return 1.0f - u * u * u * u * u;
}
static inline float easeInOutCubic(float t) {
  if (t < 0.5f) return 4.0f * t * t * t;
  const float u = -2.0f * t + 2.0f;
  return 1.0f - (u * u * u) * 0.5f;
}
static inline float easeInOutSine(float t) {
  return -(cosf(3.14159265f * t) - 1.0f) * 0.5f;
}

// Normalised, clamped progress of one phase. Returns 0 before it starts and 1
// after it ends, so overlapping phases compose without any state.
static inline float bootPhase(uint32_t ms, uint32_t start, uint32_t dur) {
  if (!dur || ms <= start) return 0.0f;
  const uint32_t d = ms - start;
  if (d >= dur) return 1.0f;
  return (float)d / (float)dur;
}

// Blend a foreground shade over the field. a=0 is invisible (element equals the
// background), a=1 is fully drawn. On a 4bpp panel this is a real greyscale
// fade, not a dither, so it stays smooth at any size.
static inline uint8_t bootMix(uint8_t bg, uint8_t fg, float a) {
  if (a <= 0.0f) return bg;
  if (a >= 1.0f) return fg;
  const float v = (float)bg + ((float)fg - (float)bg) * a;
  return (uint8_t)(v + 0.5f);
}

// ---- timeline, 3600 ms ------------------------------------------------------
// Phases overlap on purpose. Nothing in a polished sequence waits for the thing
// before it to finish; each element starts while the previous is still settling,
// which is what makes it read as one movement instead of a list of steps.
// BOOT_FIELD_IN is gone with the field. Its 620 ms ease WAS the opening
// animation when the background was the lit thing; now that the marks are lit,
// each one already carries its own arrival curve, and multiplying the two
// together double-faded the opening - measured, the rule rendered at shade 0
// until ~200 ms and shade 2 at 240 ms, so the first fifth of a second of the
// sequence was literally black on top of the 625 ms of black that setup() spends
// before it. Only the closing dissolve survives as an envelope.
// The rule starts at 80 ms rather than 420. On the old white-field splash the
// FIELD arriving was the first event and covered the opening half-second; with
// the field gone that half-second is just black, and a box that shows nothing
// for 400 ms after power-on reads as a box that has not booted. It now runs
// 80..1100 ms, still overlapping the wordmark by 200 ms the way it used to.
static const uint32_t BOOT_RULE       =   80, BOOT_RULE_D       = 1020;
static const uint32_t BOOT_LOGO       =  900, BOOT_LOGO_D       = 820;
static const uint32_t BOOT_VER        = 1620, BOOT_VER_D        = 560;
static const uint32_t BOOT_SWEEP      = 1950, BOOT_SWEEP_D      = 950;
static const uint32_t BOOT_FIELD_OUT  = 3180, BOOT_FIELD_OUT_D  = 420;
static const uint32_t BOOT_TOTAL_MS   = 3600;

// Vertical anchors. The panel is 256 tall, so the block is centred on 128 with
// the wordmark sitting a little above centre - optical centre, not arithmetic.
// 0 = plain, 1 = bold, 2 = heavy. Each step adds a device pixel to every
// stroke. 2 is very solid at this size and starts to close the counters in R
// and D, so 1 is the default.
#define BOOT_LOGO_WEIGHT 1

// Three elements now the model name is gone. Respaced rather than left with a
// hole where it used to be: the rule, wordmark and version sit closer together
// and the block still balances on the panel's optical centre.
static const int16_t BOOT_RULE_Y  = 100;
static const int16_t BOOT_LOGO_Y  = 114;
static const int16_t BOOT_VER_Y   = 138;
static const int16_t BOOT_SWEEP_Y = 158;

static void drawBoot(uint32_t ms) {
  const int16_t W = UI_W;

  // ---- the envelope -------------------------------------------------------
  // THIS SCREEN IS THE ONE THAT IS REVERSED, AND IT COULD NOT BE DONE WITH THE
  // uiInvert XOR. Run the old white-field splash through that and the fades go
  // backwards: a field that faded IN to white becomes a screen that starts
  // fully lit and fades OUT to black, and the closing fade - which used to
  // collapse the field to black to hand over - becomes a fade UP to full white
  // immediately before the first page. The box would have blinked fully lit at
  // power-on and again on the way in, which is precisely the behaviour this
  // screen was being blamed for. A polarity is not a post-process; the
  // animation has to be authored for the one it is in.
  //
  // So the field is gone. The two curves that used to bring it in and take it
  // away now drive the brightness of the MARKS, which are the lit thing here,
  // and the closing dissolve still works because it is fading the ink out
  // rather than the ground in. Panel load goes from 96-100% to about 4%.
  const float out = easeInOutCubic(bootPhase(ms, BOOT_FIELD_OUT, BOOT_FIELD_OUT_D));
  const float env = 1.0f - out;
  gfx.clear();

  // Black ground, lit ink - the same way round as every other page in the OS,
  // so the handover at the end of the sequence is a dissolve into the first
  // page rather than a cut from white to black.
  const uint8_t bg = SH_OFF, ink = SH_ON;

  // ---- rule: expands from the centre, decelerating hard -------------------
  {
    const float p = easeOutQuint(bootPhase(ms, BOOT_RULE, BOOT_RULE_D));
    const int16_t half = (int16_t)((float)(W - 16) * 0.5f * p + 0.5f);
    if (half > 0) {
      const uint8_t sh = bootMix(bg, ink, p * env);
      gfx.hLine((int16_t)(W / 2 - half), BOOT_RULE_Y, (int16_t)(half * 2), sh);
    }
  }

  // ---- wordmark: rises into place while fading up -------------------------
  // Two eased properties at once - position and opacity - is what separates a
  // considered animation from a fade. The slide is only six pixels; large
  // travel looks cheap at this size.
  {
    const float t = bootPhase(ms, BOOT_LOGO, BOOT_LOGO_D);
    if (t > 0.0f) {
      const float e = easeOutCubic(t);
      const int16_t dy = (int16_t)((1.0f - e) * 6.0f + 0.5f);
      const uint8_t sh = bootMix(bg, ink, e * env);
      // Scale 2 at weight 1: strokes go from 2px to 3px. Five characters step
      // 12 apart and the last glyph is 11 wide, so the wordmark is 59 across
      // and centres at x=2 with a pixel to spare.
      gfx.textScaledWeight(2, (int16_t)(BOOT_LOGO_Y + dy), "REDOT", sh, 2,
                           BOOT_LOGO_WEIGHT);
    }
  }

  // ---- firmware version ---------------------------------------------------
  {
    const float e = easeOutCubic(bootPhase(ms, BOOT_VER, BOOT_VER_D));
    if (e > 0.0f) {
      const uint8_t sh = bootMix(bg, ink, e * env);
      const char* t = "OS " XY6_VERSION;
      gfx.text35((int16_t)((W - gfx.width35(t)) / 2), BOOT_VER_Y, t, sh);
    }
  }

  // ---- progress sweep -----------------------------------------------------
  // A hairline that grows from the centre outward on the gentlest curve in the
  // set, so it registers as "something is happening" without competing with the
  // wordmark for attention. It is honest about being decorative: nothing here
  // waits on it, and the sequence can be skipped at any point.
  {
    const float p = easeInOutSine(bootPhase(ms, BOOT_SWEEP, BOOT_SWEEP_D));
    if (p > 0.0f) {
      const int16_t span = (int16_t)(W - 24);
      const int16_t half = (int16_t)((float)span * 0.5f * p + 0.5f);
      const uint8_t sh = bootMix(bg, ink, 0.55f * env);  // deliberately fainter
      if (half > 0)
        gfx.hLine((int16_t)(W / 2 - half), BOOT_SWEEP_Y, (int16_t)(half * 2), sh);
    }
  }
}

// The DIAG page is gone as a top-level page. Everything it showed - the RX and
// TX counters, the buffer high-water mark, the turbo state and baud, and the
// header bytes of the last SysEx in - is now the LINK block at the bottom of
// the settings page, which is reachable in one button press instead of four.

// ============================ page input handlers ============================
// One pair per page. Pulling these out of loop() is what makes the page table
// below possible: adding a page is now a row in an array plus three functions,
// with no edits to the main loop at all.


static void encPerform(const int8_t* d) {
  for (uint8_t i = 0; i < 6; ++i) {
    if (!d[i]) continue;
    perfCursor = i;
    // v1.22: each parameter's own range - a list stops at its last entry.
    const uint8_t vmax = paramRawMax(perfSlot[i].page, perfSlot[i].dest, perfSlot[i].track);
    const int8_t li = perfLfoFor(i);
    if (li >= 0) {
      // Under a running LFO the knob moves the CENTRE, starting from where the
      // LFO really is centred. No CC here: the LFO's own output carries the
      // new centre to the machine on its next update, with the swing on top.
      const uint8_t base = addClamp(lfo.restValue((uint8_t)li), d[i], 0, vmax);
      perfSlot[i].value = base;
      perfSlot[i].known = true;
      perfSetLfoBase(i, base);
      continue;
    }
    perfSlot[i].value = addClamp(perfSlot[i].value, d[i], 0, vmax);
    perfSlot[i].known = true;        // we are about to tell the machine
    perfSend(i);
    perfSetLfoBase(i, perfSlot[i].value);   // an idle LFO starts from here
  }
}
static void btnPerform(uint8_t pressed) {
  // v1.22: a push returns its slot to the parameter's INIT-KIT value (the
  // centre, under an LFO) - VOL to 64, PAN to centre, WDTH to 127 - rather
  // than to raw 0, which on a bipolar parameter was hard left / full negative.
  for (uint8_t i = 0; i < 6; ++i) {
    if (!(pressed & (1u << i))) continue;
    const uint8_t v = paramDefault(perfSlot[i].page, perfSlot[i].dest, perfSlot[i].track);
    perfSlot[i].value = v; perfSlot[i].known = true; perfCursor = i;
    if (perfLfoFor(i) < 0) perfSend(i);
    perfSetLfoBase(i, v);
  }
}

// Spec, LFO page: "6x LFO / Depth and ammount control by 3-4 Encoder / First
// Encoder scrolls". E6 keeps its old depth binding as well, because that is
// what your fingers already know and the two cannot disagree.
static void encOverview(const int8_t* d) {
  if (d[0]) ui.sel = addClamp(ui.sel, d[0], 0, LFO_COUNT - 1);
  LfoParams& p = lfo.p[ui.sel];
  if (d[2]) p.depth  = addClamp(p.depth,  d[2], 0, 127);
  if (d[3]) p.amount = addClamp(p.amount, d[3], 0, 127);
  if (d[5]) p.depth  = addClamp(p.depth,  d[5], 0, 127);
}
static void btnOverview(uint8_t pressed) {
  LfoParams& p = lfo.p[ui.sel];
  if (pressed & (1 << 0)) ui.inSub = true;    // same as a 0.5 s button hold
  if (pressed & (1 << 1)) p.enabled = !p.enabled;
  if (pressed & (1 << 2)) { p.trig = (uint8_t)((p.trig + 1) % TRIG_COUNT);
                            ui.paramRow = 2; }
  if (pressed & (1 << 3)) {
    static const uint8_t kIntl[5] = {0, 4, 8, 16, 32};
    uint8_t n = 0;
    while (n < 5 && kIntl[n] != p.intl) n++;
    p.intl = kIntl[(n + 1) % 5];
    ui.paramRow = 6;
  }
  if (pressed & (1 << 4)) { p.bars = (uint8_t)(p.bars % 4 + 1);
                            if (ui.stepPage >= p.bars) ui.stepPage = 0; }
  if (pressed & (1 << 5)) lfo.trig(ui.sel, clk.position());
}

// Spec, LFO subpage, verbatim:
//   "First Encoder scroll / Second Encoder changes value /
//    Third Encoder step sequencer scroll (16 steps or 8) /
//    Fourth Encoder rotation changes step into Active 0, 10%, 20%, 33%, 50%,
//    66%, 75, 90, 100% activation"
//
// This is a real change from the old one-parameter-per-knob layout. It is what
// the spec sketch asks for, and it is what makes room for the step grid to have
// two knobs of its own on the same page.
static void encEdit(const int8_t* d) {
  LfoParams& p = lfo.p[ui.sel];

  // E1 - scroll the eight parameter rows
  if (d[0]) ui.paramRow = addClamp(ui.paramRow, d[0], 0, 7);

  // E2 - change the value of whichever row the cursor is on
  if (d[1]) {
    switch (ui.paramRow) {
      case 0: p.page = addClamp(p.page, d[1], 0, PAGE_COUNT - 1);
              if (p.dest >= kMnmPages[p.page].count) p.dest = 0;
              lfoRetarget(ui.sel); mnmOut.resend(); break;
      case 1: p.dest = addClamp(p.dest, d[1], 0,
                                (uint8_t)(kMnmPages[p.page].count - 1));
              lfoRetarget(ui.sel); mnmOut.resend(); break;
      case 2: p.trig = addClamp(p.trig, d[1], 0, (uint8_t)(TRIG_COUNT - 1)); break;
      case 3: p.wave = addClamp(p.wave, d[1], 0, (uint8_t)(WAVE_COUNT - 1)); break;
      case 4: p.mult = addClamp(p.mult, d[1], 0, LFO_MULT_MAX); break;
      case 5: p.spd  = addClamp(p.spd,  d[1], 1, 127); break;
      case 6: p.intl = addClamp(p.intl, d[1], 0, 127); break;
      default:p.depth= addClamp(p.depth,d[1], 0, 127); break;
    }
  }

  // E3 - walk the step grid. Wrapping across the 16-cell page boundary moves
  // the page too, so a full sweep of E3 covers every step of every bar rather
  // than stopping at the edge of whatever page happens to be showing.
  if (d[2]) {
    const uint8_t sc   = (p.stepCount == 8) ? 8 : 16;
    const int16_t span = (int16_t)(p.bars * sc);
    int16_t abs = (int16_t)(ui.stepPage * sc + (ui.stepCur % sc)) + d[2];
    while (abs < 0)     abs = (int16_t)(abs + span);
    while (abs >= span) abs = (int16_t)(abs - span);
    ui.stepPage = (uint8_t)(abs / sc);
    ui.stepCur  = (uint8_t)(abs % sc);
  }

  // E4 - THE PROBABILITY LADDER. Nine detents; PROB_OFF is the step being off,
  // so `on` is kept in lockstep rather than being a second thing to set.
  if (d[3]) {
    const uint8_t sc  = (p.stepCount == 8) ? 8 : 16;
    const uint8_t idx = (uint8_t)((ui.stepPage * sc + (ui.stepCur % sc)) % LFO_STEPS);
    LfoStep& st = p.steps[idx];
    uint8_t pr = st.on ? st.prob : (uint8_t)PROB_OFF;
    pr = addClamp(pr, d[3], 0, (uint8_t)(PROB_COUNT - 1));
    st.prob = pr ? pr : (uint8_t)PROB_100;   // keep a sane value behind an off step
    st.on   = pr ? 1 : 0;
  }

  // E5 / E6 stay useful rather than idle: bar length and this LFO's send level.
  if (d[4]) {
    p.bars = addClamp(p.bars, d[4], 1, 4);
    // Shrinking the bar count can strand the step cursor on a page that is no
    // longer part of the sequence - editing steps that will never fire.
    if (ui.stepPage >= p.bars) { ui.stepPage = (uint8_t)(p.bars - 1);
                                 ui.stepCur = 0; }
  }
  if (d[5]) p.amount = addClamp(p.amount, d[5], 0, 127);
}
static void btnEdit(uint8_t pressed) {
  if (pressed & (1 << 0)) { ui.inSub = false; return; }
  btnOverview(pressed & (uint8_t)~1u);      // the other five behave identically
}

static void encPatternPage(const int8_t* d) {
  if (d[0]) { patSelectGenre(addClamp(pat.genre, d[0], 0, (uint8_t)(PAT_GENRE_COUNT - 1)),
                             pat.seed);
              if (patUiBar >= pat.bars) patUiBar = 0; }
  if (d[1]) { pat.seed = (uint32_t)((int32_t)pat.seed + d[1]);
              if (pat.seed == 0) pat.seed = 1;
              patGenerate(pat.genre, pat.seed); }
  if (d[2]) { pat.bars = addClamp(pat.bars, d[2], 1, 4);
              patGenerate(pat.genre, pat.seed);
              if (patUiBar >= pat.bars) patUiBar = 0; }
  if (d[3]) { pat.root = addClamp(pat.root, d[3], 12, 96);
              patGenerate(pat.genre, pat.seed); }
  if (d[4]) { pat.scale = addClamp(pat.scale, d[4], 0,
                                   (uint8_t)(PSC_SCALE_COUNT - 1));
              patGenerate(pat.genre, pat.seed); }
  if (d[5]) patUiTrack = addClamp(patUiTrack, d[5], 0, PAT_TRACKS - 1);
}
static void btnPatternPage(uint8_t pressed) {
  if (pressed & (1 << 0)) patGenerate(pat.genre, (uint32_t)micros() ^ 0xA5A5A5A5u);
  if (pressed & (1 << 1)) { pat.engineOn = !pat.engineOn;
                            if (!pat.engineOn) patSilenceAll(); }
  if (pressed & (1 << 2)) patUiBar = (uint8_t)((patUiBar + 1) % (pat.bars ? pat.bars : 1));
  if (pressed & (1 << 3)) { pat.trackOn[patUiTrack] = !pat.trackOn[patUiTrack];
                            if (!pat.trackOn[patUiTrack]) patSilenceTrack(patUiTrack); }
  if (pressed & (1 << 4)) { pat.scale = (uint8_t)((pat.scale + 1) % PSC_SCALE_COUNT);
                            patGenerate(pat.genre, pat.seed); }
  if (pressed & (1 << 5)) sceneApply();                 // scenes only; else a no-op
}


// -----------------------------------------------------------------------------
// PAGE 1 SUBPAGE - WHERE EACH ENCODER POINTS
// -----------------------------------------------------------------------------
// One encoder owns one slot, here as on the page itself: turning encoder N
// walks slot N through every (page, slot) pair on the Monomachine that has a
// real CC behind it - 57 of them - and pushing encoder N steps that slot's
// track. There is no cursor to move first and no second knob to pick the page,
// because the whole appeal of six encoders is that the knob you reach for is
// the one that does the thing.
//
// The meter under each row is not decoration: with 57 destinations on one
// detented knob you need to see roughly where in the list you are.
// v1.24: six rows of 39px on G_DARK rules, one per slot. The slot E1 has
// selected is the focus: its first line is the block, the full width.
//   +0..8    slot number (3x5 MID) and the parameter (5x7 WHITE); "?" at the
//            right while its value is not known
//   +11..15  page and track (3x5 LIGHT), the CC it lands on (3x5 MID, right)
//   +18..24  the value (5x7 WHITE) and its bar from x 26 - or "--" and the
//            init-kit value, MID: where the first turn will start from
//   +29..31  where the parameter sits among the 57: a 3px LIGHT block on a
//            1px G_DARK line
static void drawPerfDest(uint32_t step16, bool running, uint32_t bpmX100) {
  (void)step16; (void)running; (void)bpmX100;
  gfx.clear();
  const int16_t W = UI_W;
  char b[16];

  { b[0] = 'T'; b[1] = (char)('1' + perfSlot[perfCursor].track); b[2] = 0;
    stHeader("DEST", b); }

  const uint16_t total = destCount();
  for (uint8_t i = 0; i < 6; ++i) {
    const int16_t y = (int16_t)(ST_BODY_Y + i * 39);
    const PerfSlot& p = perfSlot[i];
    uint8_t nsh = G_MID, psh = G_WHITE;
    if (perfCursor == i) nsh = psh = stFocus(0, y, W, ST_ROW);
    { const char nb[2] = {(char)('1' + i), 0};
      gfx.text35(2, (int16_t)(y + 2), nb, nsh); }
    gfx.text(8, (int16_t)(y + 1), paramName(p.page, p.dest, p.track), psh);
    if (!p.known) gfx.text35R((int16_t)(W - 1), (int16_t)(y + 2), "?", nsh);

    { uint8_t k = 0;
      const char* pg = kMnmPages[p.page].name;
      while (*pg && k < 4) b[k++] = *pg++;
      b[k++] = ' '; b[k++] = 'T'; b[k++] = (char)('1' + p.track); b[k] = 0;
      gfx.text35(0, (int16_t)(y + 11), b, G_LIGHT); }
    { const uint8_t cc = mnmCC(p.page, p.dest);
      if (cc == 0xFF) { b[0] = '-'; b[1] = '-'; b[2] = 0; }
      else { b[0] = 'C'; b[1] = 'C'; u8s(cc, b + 2); }
      gfx.text35R(W, (int16_t)(y + 11), b, G_MID); }

    if (p.known) {
      paramFormat(p.page, p.dest, p.track, p.value, b);
      stText(0, (int16_t)(y + 18), 24, b, G_WHITE, 0);
      stParamBar(26, (int16_t)(y + 19), (int16_t)(W - 26), 5, false,
                 p.page, p.dest, p.track, p.value, G_WHITE);
    } else {
      char dv[8];
      paramFormat(p.page, p.dest, p.track, paramDefault(p.page, p.dest, p.track), dv);
      gfx.text(0, (int16_t)(y + 18), "--", G_MID);
      snprintf(b, sizeof b, "INIT %s", dv);
      gfx.text35(16, (int16_t)(y + 19), b, G_MID);
    }

    gfx.hLine(0, (int16_t)(y + 30), W, G_DARK);
    { const int32_t den = total > 1 ? total - 1 : 1;
      gfx.fillRect((int16_t)((W - 3) * (int32_t)destIndexOf(p.page, p.dest) / den),
                   (int16_t)(y + 29), 3, 3, G_LIGHT); }
    if (i < 5) gfx.hLine(0, (int16_t)(y + 38), W, G_DARK);
  }
  stFooter("E2 DEST", "E3 TRK");
}

// Spec, encoder subpage: "6 Destination / First encoder scroll / second changes
// destination". A scroll list, not one knob per slot - so E3 is free to carry
// the track, which previously needed a push.
static void encPerfDest(const int8_t* d) {
  const uint16_t total = destCount();
  if (!total) return;
  if (d[0]) perfCursor = addClamp(perfCursor, d[0], 0, 5);
  const uint8_t i = perfCursor;
  if (d[1]) {
    int32_t idx = (int32_t)destIndexOf(perfSlot[i].page, perfSlot[i].dest) + d[1];
    if (idx < 0) idx = 0;
    if (idx > (int32_t)total - 1) idx = (int32_t)total - 1;
    uint8_t pg, sl;
    if (destAt((uint16_t)idx, &pg, &sl)) {
      perfSlot[i].page = pg; perfSlot[i].dest = sl;
      // The stored value belonged to the OLD parameter. The new one holds
      // whatever the kit model knows - learned, or only the init default, in
      // which case the slot is honestly unknown (v1.22).
      perfSeed(i);
    }
    // A re-addressed slot has told the machine nothing yet, so the echo filter
    // must not think the next CC we send is our own coming back.
    perfSent[i] = 0xFF;
  }
  if (d[2]) { perfSlot[i].track = addClamp(perfSlot[i].track, d[2], 0, 5);
              perfSeed(i); perfSent[i] = 0xFF; }
}
static void btnPerfDest(uint8_t pressed) {
  for (uint8_t i = 0; i < 6; ++i)
    if (pressed & (1u << i)) {
      perfSlot[i].track = (uint8_t)((perfSlot[i].track + 1) % 6);
      perfSeed(i);           // v1.22: another track, another value
      perfCursor = i;
      perfSent[i] = 0xFF;
    }
}

// -----------------------------------------------------------------------------
// PAGE 1 SUBPAGE 2 - WHERE THE JOYSTICK GOES                            v1.17
// -----------------------------------------------------------------------------
// The six encoders are the six tracks: push N and track N is under the stick,
// or no longer is. One track, all six, or any mix - each gets its CCs on its
// own channel. E1 / E2 pick what X / Y move from the same 57 destinations as
// DEST, and E3 is the quick pick: T1 .. T6 on their own, then ALL.
static uint8_t joyDestCC(uint8_t idx) {
  uint8_t pg, sl;
  return destAt(idx, &pg, &sl) ? mnmCC(pg, sl) : 0xFF;
}
// "OFF", "T3", "4TRK", "ALL". out needs 5 bytes.
static void joyMaskLabel(uint8_t m, char* out) {
  uint8_t n = 0, last = 0;
  for (uint8_t t = 0; t < 6; ++t) if (m & (1u << t)) { n++; last = t; }
  if (n == 0)      strcpy(out, "OFF");
  else if (n == 6) strcpy(out, "ALL");
  else if (n == 1) { out[0] = 'T'; out[1] = (char)('1' + last); out[2] = 0; }
  else             { out[0] = (char)('0' + n); strcpy(out + 1, "TRK"); }
}

// One axis, 26px: the axis letter (5x7 MID), the parameter (5x7 WHITE), the
// CC (3x5 MID, right); the page (3x5 LIGHT); the stick's value (5x7 WHITE)
// and its bar from the centre - the stick rests at 64, so its bar is bipolar
// whatever it drives.
static void drawJoyAxis(int16_t y, const char* axis, uint8_t destIdx, uint8_t v) {
  const int16_t W = UI_W;
  char b[16];
  uint8_t pg = 0, sl = 0;
  const bool ok = destAt(destIdx, &pg, &sl);
  gfx.text(0, y, axis, G_MID);
  gfx.text(8, y, ok ? mnmParamName(pg, sl) : "----", G_WHITE);
  const uint8_t cc = ok ? mnmCC(pg, sl) : 0xFF;
  if (cc == 0xFF) { b[0] = '-'; b[1] = '-'; b[2] = 0; }
  else { b[0] = 'C'; b[1] = 'C'; u8s(cc, b + 2); }
  gfx.text35R(W, (int16_t)(y + 1), b, G_MID);
  gfx.text35(8, (int16_t)(y + 10), ok ? kMnmPages[pg].name : "", G_LIGHT);
  if (v == 0xFF) { gfx.text35(0, (int16_t)(y + 19), "NO STICK", G_MID); return; }
  u8s3(v, b);
  gfx.text(0, (int16_t)(y + 18), b, G_WHITE);
  stBarH(20, (int16_t)(y + 19), (int16_t)(W - 20), 5, v, 127, BAR_CENTRE, G_WHITE);
}

// The stick as a position: a 1px MID frame, a DARK dotted centre cross, DARK
// crosshairs through the stick, and a 3x3 WHITE mark where they meet. Up is up.
static void drawJoyPad(int16_t x0, int16_t y0, int16_t sz, uint8_t vx, uint8_t vy) {
  gfx.rect(x0, y0, sz, sz, G_MID);
  const int16_t cx = (int16_t)(x0 + sz / 2), cy = (int16_t)(y0 + sz / 2);
  gfx.dotHLine((int16_t)(x0 + 2), cy, (int16_t)(sz - 4), G_DARK, 2);
  for (int16_t yy = (int16_t)(y0 + 2); yy < y0 + sz - 2; yy += 2) gfx.px(cx, yy, G_DARK);
  if (vx == 0xFF || vy == 0xFF) return;
  const int16_t in = (int16_t)(sz - 6);                  // mark stays inside
  const int16_t px = (int16_t)(x0 + 3 + (in * vx) / 127);
  const int16_t py = (int16_t)(y0 + 3 + (in * (127 - vy)) / 127);
  gfx.hLine((int16_t)(x0 + 1), py, (int16_t)(sz - 2), G_DARK);
  gfx.vLine(px, (int16_t)(y0 + 1), (int16_t)(sz - 2), G_DARK);
  gfx.fillRect((int16_t)(px - 1), (int16_t)(py - 1), 3, 3, G_WHITE);
}

// v1.24: header; X and Y (26px each) on rules; the pad (60 x 60); then the six
// tracks as a 3 x 2 grid of cells on 1px rules. The tracks under the stick
// are the blocks - this is a multi-select, so there can be several - and a
// 2x2 BLACK mark in a block's corner lights for JOY_ACT_MS after that track
// was actually sent something, so ALL can be seen reaching all six.
static void drawPerfJoy(uint32_t step16, bool running, uint32_t bpmX100) {
  (void)step16; (void)running; (void)bpmX100;
  gfx.clear();
  const int16_t W = UI_W;
  const uint32_t nowMs = millis();
  char b[16];

  joyMaskLabel(g_joyMask, b);
  stHeader("JOY", b);

  drawJoyAxis(12, "X", g_joyDestX, g_joyUiX);
  gfx.hLine(0, 38, W, G_DARK);
  drawJoyAxis(40, "Y", g_joyDestY, g_joyUiY);
  gfx.hLine(0, 66, W, G_DARK);
  drawJoyPad(2, 70, (int16_t)(W - 4), g_joyUiX, g_joyUiY);
  gfx.hLine(0, 133, W, G_DARK);

  gfx.text35(0, 136, "TRACKS", G_MID);
  { char cb[6] = {'C', 'H', 0, 0, 0, 0};
    u8s(txChannel, cb + 2);
    gfx.text35R(W, 136, cb, G_MID); }
  static const int16_t kCx[3] = {0, 22, 43}, kCw[3] = {21, 20, 21};
  gfx.hLine(0, 143, W, G_DARK);
  gfx.vLine(21, 144, 35, G_DARK);
  gfx.vLine(42, 144, 35, G_DARK);
  gfx.hLine(0, 161, W, G_DARK);
  gfx.hLine(0, 179, W, G_DARK);
  for (uint8_t t = 0; t < 6; ++t) {
    const int16_t x = kCx[t % 3], w = kCw[t % 3];
    const int16_t y = (int16_t)(144 + (t / 3) * 18);
    const bool on = (g_joyMask & (1u << t)) != 0;
    const uint8_t sh = on ? stFocus(x, y, w, 17) : G_MID;
    const char tb[3] = {'T', (char)('1' + t), 0};
    char chb[6] = {'C', 'H', 0, 0, 0, 0};
    u8s((uint8_t)(((txChannel - 1 + t) & 0x0F) + 1), chb + 2);
    gfx.textCentre((int16_t)(x + w / 2), (int16_t)(y + 1), tb, sh);
    gfx.text35Centre((int16_t)(x + w / 2), (int16_t)(y + 10), chb, sh);
    if (on && g_joyActMs[t] && (uint32_t)(nowMs - g_joyActMs[t]) < JOY_ACT_MS)
      gfx.fillRect((int16_t)(x + w - 3), (int16_t)(y + 1), 2, 2, G_BLACK);
  }

  gfx.text35(0, 186, "PUSH E1-E6", G_MID);
  gfx.text35(0, 194, "TRACK ON / OFF", G_MID);
  gfx.text35(0, 206, "E3 SOLO .. ALL", G_MID);
  stFooter("E1 X E2 Y", "E3 TRK");
}

static const uint8_t kJoyQuick[7] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x3F};
static void encPerfJoy(const int8_t* d) {
  const uint8_t total = destCount();
  if (d[0] && total) { const uint8_t v = addClamp(g_joyDestX, d[0], 0, (uint8_t)(total - 1));
                       if (v != g_joyDestX) { g_joyDestX = v; joyForget(true, false); } }
  if (d[1] && total) { const uint8_t v = addClamp(g_joyDestY, d[1], 0, (uint8_t)(total - 1));
                       if (v != g_joyDestY) { g_joyDestY = v; joyForget(false, true); } }
  if (d[2]) {
    // From a hand-made mix the first detent lands on an end of the list.
    int8_t i = (d[2] > 0) ? -1 : 7;
    for (uint8_t k = 0; k < 7; ++k) if (kJoyQuick[k] == g_joyMask) i = (int8_t)k;
    int16_t v = (int16_t)(i + d[2]);
    if (v < 0) v = 0;
    if (v > 6) v = 6;
    g_joyMask = kJoyQuick[v];
  }
}
static void btnPerfJoy(uint8_t pressed) {
  g_joyMask = (uint8_t)((g_joyMask ^ pressed) & 0x3F);
}

// PERF has two sub-pages on the ring now; these hand over to the one showing.
static void drawPerfSub(uint32_t step16, bool running, uint32_t bpmX100) {
  if (g_perfSub == 1) drawPerfJoy(step16, running, bpmX100);
  else                drawPerfDest(step16, running, bpmX100);
}
static void encPerfSub(const int8_t* d) {
  if (g_perfSub == 1) encPerfJoy(d); else encPerfDest(d);
}
static void btnPerfSub(uint8_t pressed) {
  if (g_perfSub == 1) btnPerfJoy(pressed); else btnPerfDest(pressed);
}

// =============================================================================
// SECTION: PRESET STORE                                            new in v7
// =============================================================================
// The bridge between working state and the packed wire format. Everything the
// settings page's SAVE / LOAD / DELETE rows used to apologise for lives here.

static PresetP   gRec;                 // one record buffer, packed/unpacked here
static GlobalCfgP gGlobal;
// machineSel[] lives with the kit model (MONOMACHINE VALUE MAP) since v1.22.
static uint32_t  sxMsgBytes = 0;
static bool      sxGotDump  = false;

enum StoreOp : uint8_t {
  ST_IDLE = 0, ST_BOOT_GLOBALS, ST_LOAD_REC, ST_SAVE_REC, ST_SAVE_GLOBALS, ST_ERR
};
// Boot animation clock. elapsedMillis counts up on its own; nothing polls or
// blocks on it. Declared here rather than down with setup() because the boot
// router below is what decides where the splash hands over.
static elapsedMillis bootMs;
static bool bootActive = true;
static ButtonGesture gBtn;

// One authority on what the panel's contrast register should hold right now.
// Two callers used to set it independently, and they disagree during the
// splash: applyGlobals() lands from the EEPROM read WHILE the animation is
// running and would slam the register back to uiContrast mid-fade, which shows
// up as a brightness step partway through the boot.
static void uiApplyContrast() {
  // The splash is drawn the same way round as every other page now, so there is
  // no longer a screen that needs its own contrast: the only thing that decides
  // whether the background is the lit thing is reversed video.
  setContrast(uiInvert ? (uint8_t)OLED_CONTRAST_FIELD : uiContrast);
}

// One entry point, so the shadow invalidation can never be forgotten: the
// shadow holds what the PANEL currently shows, and reversing the video changes
// every byte of that without touching a single byte of the framebuffer.
static void uiSetInvert(bool on) {
  if (uiInvert == on) return;
  uiInvert = on;
  g_shadowValid = false;
  uiApplyContrast();
  uiTouch();
}

static uint8_t presetSlot = 1;         // 1..EE_PRESET_SLOTS, shown 1-based
static uint8_t storeOp   = ST_IDLE;
static uint8_t storeSlot = 0;
static bool    storeErasing = false;   // v1.15: a DELETE must not become "last preset"
static char    storeMsg[24] = "";

// System state. The splash always runs; where it hands over is decided once,
// when the globals read lands.
static uint8_t sysState = SYS_SPLASH;
static uint8_t wizState = WIZ_SPLASH;
static uint8_t wizCursor = 0;
static uint8_t uiHoldRing = 0;         // 0..255 hold-gesture feedback

struct Store {
  static bool busy() { return storeOp != ST_IDLE && storeOp != ST_ERR; }

  static void packInto(uint8_t slot) {
    memset(&gRec, 0, sizeof(gRec));
    gRec.hdr.magic     = XY6_MAGIC_PRESET;
    gRec.hdr.version   = XY6_FORMAT_VER;
    gRec.hdr.size      = (uint16_t)(sizeof(PresetP) - 12);   // after the crc field
    gRec.hdr.saveCount = ++gGlobal.saveCount;
    const char* nm = "XY6 PRESET  ";
    for (uint8_t i = 0; i < 12; ++i) gRec.hdr.name[i] = nm[i];
    gRec.hdr.name[10] = (char)('0' + ((slot + 1) / 10));
    gRec.hdr.name[11] = (char)('0' + ((slot + 1) % 10));

    for (uint8_t t = 0; t < EE_TRACKS; ++t) {
      gRec.kit[t].machineId = machineSel[t];
      gRec.kit[t].level     = 100;
      // b0 a dump arrived, b1 machineId is a v1.22 id, b2 param[] holds the
      // kit model: a learned value as itself, a default as 0xFF (a 7-bit
      // value can never be 0xFF, so the byte says which it was).
      gRec.kit[t].flags     = (uint8_t)((sxGotDump ? 1 : 0) | KIT_F_V122_ID | KIT_F_PARAMS);
      for (uint8_t k = 0; k < EE_PARAM_SLOTS; ++k)
        gRec.kit[t].param[k] = kitLearned(t, (uint8_t)(k / 8), (uint8_t)(k % 8))
                                 ? kitVal[t][k] : (uint8_t)0xFF;
    }
    for (uint8_t i = 0; i < 6; ++i) {
      gRec.enc[i].page  = perfSlot[i].page;
      gRec.enc[i].slot  = perfSlot[i].dest;
      gRec.enc[i].track = perfSlot[i].track;
      gRec.enc[i].value = perfSlot[i].value;
      gRec.enc[i].flags = perfSlot[i].known ? 1 : 0;
    }
    for (uint8_t i = 0; i < LFO_COUNT; ++i) {
      LfoPresetP& d = gRec.lfo[i]; const LfoParams& L = lfo.p[i];
      d.destTrack = L.track; d.destPage = L.page; d.destSlot = L.dest;
      d.wave = L.wave; d.trig = L.trig; d.mult = L.mult; d.spd = L.spd;
      d.intl = L.intl; d.depth = L.depth; d.amount = L.amount;
      d.lo = L.lo; d.hi = L.hi; d.bars = L.bars; d.baseValue = L.baseValue;
      d.stepCount = L.stepCount;
      d.flags = (uint8_t)((L.enabled ? 1 : 0) | LFO_F_V122_WAVE);
      for (uint8_t k = 0; k < EE_LFO_STEPS; ++k)
        d.steps[k].prob = L.steps[k].on ? L.steps[k].prob : (uint8_t)PROB_OFF;
    }
    gRec.pat.seed  = pat.seed;
    gRec.pat.genre = pat.genre; gRec.pat.bars = pat.bars;
    gRec.pat.root  = pat.root;  gRec.pat.scale = pat.scale;
    gRec.pat.flags = pat.engineOn ? 1 : 0;
    uint8_t on = 0;
    for (uint8_t t = 0; t < PAT_TRACKS; ++t) if (pat.trackOn[t]) on |= (uint8_t)(1u << t);
    gRec.pat.trackOn = on;
    for (uint8_t t = 0; t < EE_PAT_TRACKS; ++t)
      for (uint8_t k = 0; k < EE_PAT_STEPS; ++k) {
        const PatStep& sp = pat.step[t][k];
        PatStepP& dp = gRec.pat.step[t][k];
        dp.note = sp.note; dp.vel = sp.vel; dp.len = sp.len;
        dp.flags = (uint8_t)((sp.on ? 1 : 0) | (sp.accent ? 2 : 0) |
                             (sp.slide ? 4 : 0) | (sp.ghost ? 8 : 0) |
                             ((sp.cond & 0x0F) << 4));
      }
    // CRC last - so a torn write fails verification rather than loading as a
    // plausible preset. NOTE offset 12 is the crc field itself (magic 4,
    // version 2, size 2, saveCount 4), so the range covers the field, which
    // is still 0 here. unpackFrom() zeroes it again to check.
    gRec.hdr.crc = xy6Crc16((const uint8_t*)&gRec + 12, gRec.hdr.size);
  }

  static bool unpackFrom() {
    if (gRec.hdr.magic   != XY6_MAGIC_PRESET)  { strcpy(storeMsg, "EMPTY SLOT"); return false; }
    if (gRec.hdr.version != XY6_FORMAT_VER)    { strcpy(storeMsg, "BAD VERSION");return false; }
    // v1.15: EXACT, not "at most". A smaller size meant a smaller CRC and the
    // rest of the record loaded without ever being verified.
    if (gRec.hdr.size != sizeof(PresetP) - 12) { strcpy(storeMsg, "BAD SIZE");   return false; }
    // v1.23 FIX: every LOAD said CRC FAIL. The range starts at offset 12,
    // which is the crc field itself (the "after the crc field" it was meant
    // to be forgot saveCount), and packInto() computes it while that field is
    // still 0. Checked with the stored CRC in place it could never match.
    // Checked the way it was computed, every preset ever saved loads - no
    // format change, nothing to re-save.
    { const uint16_t want = gRec.hdr.crc;
      gRec.hdr.crc = 0;
      const uint16_t got = xy6Crc16((const uint8_t*)&gRec + 12, gRec.hdr.size);
      gRec.hdr.crc = want;
      if (got != want) { strcpy(storeMsg, "CRC FAIL"); return false; } }

    // Machines, then the kit model on top of them - the SYNT defaults depend on
    // which machine it is. Presets from before v1.22 carry an id into the old
    // guessed list and no kit values; both are translated, not refused.
    for (uint8_t t = 0; t < EE_TRACKS; ++t) {
      const TrackKitP& K = gRec.kit[t];
      const uint8_t id = (K.flags & KIT_F_V122_ID) ? K.machineId
                                                   : legacyMachineId(K.machineId);
      machineSel[t] = (id < MACHINE_COUNT) ? id : (uint8_t)0;
      kitResetPages(t, 0, (uint8_t)(KIT_PAGES - 1));
      if (K.flags & KIT_F_PARAMS)
        for (uint8_t k = 0; k < EE_PARAM_SLOTS; ++k)
          if (K.param[k] != 0xFF) kitSet(t, (uint8_t)(k / 8), (uint8_t)(k % 8), K.param[k]);
    }
    for (uint8_t i = 0; i < 6; ++i) {
      // Every field is range-checked on the way IN. A stale or corrupted byte
      // that survives the CRC must not become an out-of-bounds index into
      // kMnmPages a frame later.
      uint8_t pg = (uint8_t)(gRec.enc[i].page % PAGE_COUNT);
      uint8_t sl = gRec.enc[i].slot;
      if (sl >= kMnmPages[pg].count) sl = 0;
      perfSlot[i].page  = pg;
      perfSlot[i].dest  = sl;
      perfSlot[i].track = (uint8_t)(gRec.enc[i].track % 6);
      perfSlot[i].value = (uint8_t)(gRec.enc[i].value & 0x7F);
      perfSlot[i].known = (gRec.enc[i].flags & 1) != 0;
      perfSent[i] = 0xFF;
    }
    for (uint8_t i = 0; i < LFO_COUNT; ++i) {
      const LfoPresetP& d = gRec.lfo[i]; LfoParams& L = lfo.p[i];
      L.page  = (uint8_t)(d.destPage % PAGE_COUNT);
      L.dest  = (d.destSlot < kMnmPages[L.page].count) ? d.destSlot : 0;
      L.track = (uint8_t)(d.destTrack % 6);
      // v1.22 put EXP before RMP, as on the machine; older presets saved the
      // old numbering (6/7 RMP, 8/9 EXP) and are swapped back into place.
      uint8_t wv = (uint8_t)(d.wave % WAVE_COUNT);
      if (!(d.flags & LFO_F_V122_WAVE) && wv >= 6 && wv <= 9)
        wv = (uint8_t)(wv < 8 ? wv + 2 : wv - 2);
      L.wave  = wv;
      L.trig  = (uint8_t)(d.trig % TRIG_COUNT);
      // Past 64X (the XY6 used to go to 2048X): the machine's fastest.
      L.mult  = (d.mult <= LFO_MULT_MAX) ? d.mult : LFO_MULT_MAX;
      L.spd   = (d.spd >= 1 && d.spd <= 127) ? d.spd : 64;   // v1.15: same range
      L.intl  = d.intl & 0x7F;                               //   as the encoders
      L.depth = d.depth & 0x7F; L.amount = d.amount & 0x7F;
      L.lo = d.lo & 0x7F; L.hi = d.hi & 0x7F;
      L.bars = (d.bars >= 1 && d.bars <= 4) ? d.bars : 1;
      L.baseValue = d.baseValue & 0x7F;
      L.stepCount = (d.stepCount == 8) ? 8 : 16;
      L.enabled = (d.flags & 1) != 0;
      for (uint8_t k = 0; k < EE_LFO_STEPS; ++k) {
        const uint8_t pr = (uint8_t)(d.steps[k].prob % PROB_COUNT);
        L.steps[k].prob = pr ? pr : (uint8_t)PROB_100;
        L.steps[k].on   = pr ? 1 : 0;
      }
    }
    pat.seed  = gRec.pat.seed ? gRec.pat.seed : 1;
    pat.genre = (uint8_t)(gRec.pat.genre % PAT_GENRE_COUNT);
    pat.bars  = (gRec.pat.bars >= 1 && gRec.pat.bars <= 4) ? gRec.pat.bars : 4;
    pat.root  = gRec.pat.root & 0x7F;
    pat.scale = (uint8_t)(gRec.pat.scale % PSC_SCALE_COUNT);
    pat.engineOn = (gRec.pat.flags & 1) != 0;
    for (uint8_t t = 0; t < PAT_TRACKS; ++t)
      pat.trackOn[t] = (gRec.pat.trackOn >> t) & 1u;
    for (uint8_t t = 0; t < EE_PAT_TRACKS; ++t)
      for (uint8_t k = 0; k < EE_PAT_STEPS; ++k) {
        const PatStepP& dp = gRec.pat.step[t][k];
        PatStep& sp = pat.step[t][k];
        sp.note = dp.note & 0x7F; sp.vel = dp.vel & 0x7F; sp.len = dp.len;
        sp.on = (dp.flags & 1) ? 1 : 0;   sp.accent = (dp.flags & 2) ? 1 : 0;
        sp.slide = (dp.flags & 4) ? 1 : 0; sp.ghost = (dp.flags & 8) ? 1 : 0;
        sp.cond = (uint8_t)((dp.flags >> 4) & 0x0F);
        sp.roll = 0;
      }
    // v1.11: a scene's locks and ratchets are not in the preset record - they
    // are a pure function of genre + seed + key + length, all of which are, so
    // regenerate. The steps come out identical to the ones just unpacked.
    if (patIsScene(pat.genre)) patGenerate(pat.genre, pat.seed);
    // Everything downstream of the swap has to be told. The pattern grid, the
    // LFO destinations and the bar lengths all just changed underneath a
    // sequencer that may be running: the step-edge trackers still hold indices
    // from the pattern that is no longer loaded, so the first step after a load
    // would be skipped or double-fired, and any note currently sounding belongs
    // to a pattern that no longer exists.
    patSilenceAll();
    patLastStep = -1;
    if (ui.stepPage >= lfo.p[ui.sel].bars) ui.stepPage = 0;
    mnmOut.resend();          // every destination may have moved
    strcpy(storeMsg, "LOADED");
    return true;
  }

  static void begin() {
    memset(&gGlobal, 0, sizeof(gGlobal));
    storeOp = ST_BOOT_GLOBALS;
    // Queued, not waited on: the splash animation covers the ~30 ms read, which
    // is the difference between an animation doing useful work and a stall with
    // a picture over it.
    HalEeprom::beginRead(EE_ADDR_GLOBAL, (uint8_t*)&gGlobal, sizeof(gGlobal));
  }
  // v1.18: may an operation start? A busy store used to answer "NO EEPROM" -
  // so a second press of SAVE during a save put NO EEPROM on the screen, over
  // the progress bar of a save that was working fine. Busy now leaves the
  // running operation's own message alone, and the caller hears "no".
  static bool canStart() {
    if (busy()) return false;
    if (!HalEeprom::present()) { strcpy(storeMsg, "NO EEPROM"); return false; }
    return true;
  }
  static bool saveTo(uint8_t slot) {
    if (!canStart()) return false;
    storeSlot = (uint8_t)(slot % EE_PRESET_SLOTS);
    packInto(storeSlot);
    storeErasing = false;
    storeOp = ST_SAVE_REC;
    strcpy(storeMsg, "SAVING");
    HalEeprom::beginWrite((uint16_t)(EE_PRESET_BASE + EE_PRESET_STRIDE * storeSlot),
                          (const uint8_t*)&gRec, sizeof(PresetP));
    return true;
  }
  static bool loadFrom(uint8_t slot) {
    if (!canStart()) return false;
    storeSlot = (uint8_t)(slot % EE_PRESET_SLOTS);
    storeOp = ST_LOAD_REC;
    strcpy(storeMsg, "LOADING");
    HalEeprom::beginRead((uint16_t)(EE_PRESET_BASE + EE_PRESET_STRIDE * storeSlot),
                         (uint8_t*)&gRec, sizeof(PresetP));
    return true;
  }
  // "Delete" clears the record's magic so the slot reads as empty. Erasing 4 KB
  // to 0xFF would cost 128 write cycles for no gain: the magic IS the tenancy.
  static bool eraseSlot(uint8_t slot) {
    if (!canStart()) return false;
    storeSlot = (uint8_t)(slot % EE_PRESET_SLOTS);
    memset(&gRec.hdr, 0, sizeof(gRec.hdr));
    storeErasing = true;
    storeOp = ST_SAVE_REC;
    strcpy(storeMsg, "DELETING");
    HalEeprom::beginWrite((uint16_t)(EE_PRESET_BASE + EE_PRESET_STRIDE * storeSlot),
                          (const uint8_t*)&gRec, sizeof(PresetHdrP));
    return true;
  }
  static void writeGlobals() {
    gGlobal.magic   = XY6_MAGIC_GLOBAL;
    gGlobal.version = XY6_FORMAT_VER;
    gGlobal.size    = (uint16_t)(sizeof(GlobalCfgP) - 14);
    gGlobal.baseChannel = txChannel;
    gGlobal.txMode  = txMode;
    gGlobal.perTrack = txPerTrack ? 1 : 0;
    gGlobal.contrast = uiContrast;
    gGlobal.uiWidth  = (uint8_t)UI_W;
    gGlobal.font     = uiFont;
    // Spare bits of an existing byte rather than eating into rsv[], so the
    // 32-byte size assert on GlobalCfgP still holds and old globals still load:
    // a record written before these bits existed has them clear, which reads as
    // "not inverted, line style" - the pre-existing behaviour, not a surprise.
    // Bit 3 says "bits 1 and 2 mean something". Without it a globals record
    // written by an earlier build - where those bits were reserved and therefore
    // zero - reads back as "not inverted, line style" and silently overrides the
    // compiled-in defaults on the first warm boot after an update. You would
    // flash a firmware whose default is reversed video, see it once during the
    // wizard, save, and then never see it again, with nothing on screen to
    // explain why. Presence has to be recorded separately from value.
    gGlobal.flags = (uint8_t)((gGlobal.flags & ~0x0Eu) | 0x08u |
                              (uiInvert    ? 0x02u : 0u) |
                              (uiChipStyle ? 0x04u : 0u));
    gGlobal.turboBias = 0;              // v1.19: unused, kept for the layout
    gGlobal.turboSpeedIdx = turbo.maxCode();   // v1.21: 'turbo max' 
    // v1.17: rsv[0] held v1.16's single style - the dissolve that was picked
    // for everything - so it now means SUB; PAGES is new, in rsv[4].
    gGlobal.rsv[0] = (uint8_t)(uiTransSub + 1);     // 0 = older firmware
    gGlobal.rsv[4] = (uint8_t)(uiTransStyle + 1);
    gGlobal.rsv[1] = (uint8_t)(0x80u | (g_joyMask & 0x3Fu));   // v1.17: b7 = set
    gGlobal.rsv[2] = (uint8_t)(g_joyDestX + 1);                //   0 = older
    gGlobal.rsv[3] = (uint8_t)(g_joyDestY + 1);
    gGlobal.crc = xy6Crc16((const uint8_t*)&gGlobal + 14, gGlobal.size);
    storeOp = ST_SAVE_GLOBALS;
    HalEeprom::beginWrite(EE_ADDR_GLOBAL, (const uint8_t*)&gGlobal, sizeof(gGlobal));
  }
  // Applied only on a warm boot, after the globals have passed their CRC.
  static void applyGlobals() {
    if (gGlobal.baseChannel >= 1 && gGlobal.baseChannel <= 16) txChannel = gGlobal.baseChannel;
    if (gGlobal.txMode <= 3) txMode = gGlobal.txMode;
    txPerTrack = (gGlobal.perTrack != 0);
    if (gGlobal.contrast >= 16) { uiContrast = gGlobal.contrast; uiApplyContrast(); }
    if (gGlobal.uiWidth >= 8 && gGlobal.uiWidth <= 64) UI_W = gGlobal.uiWidth;
    uiFont = gGlobal.font ? UIFONT_5X7 : UIFONT_3X5;
    if (gGlobal.flags & 0x08) {          // only if this firmware wrote them
      uiChipStyle = (gGlobal.flags & 0x04) != 0;
      // Straight to the variable, not through uiSetInvert: this runs from the
      // EEPROM read during the splash, and the shadow is invalidated below.
      uiInvert = (gGlobal.flags & 0x02) != 0;
    }
    uiApplyContrast();
    if (gGlobal.rsv[0] >= 1 && gGlobal.rsv[0] <= TS_COUNT) uiTransSub   = (uint8_t)(gGlobal.rsv[0] - 1);
    if (gGlobal.rsv[4] >= 1 && gGlobal.rsv[4] <= TS_COUNT) uiTransStyle = (uint8_t)(gGlobal.rsv[4] - 1);
    turbo.setMax(gGlobal.turboSpeedIdx);     // v1.21: ignores 0 / out of range
    if (gGlobal.rsv[1] & 0x80u) g_joyMask = (uint8_t)(gGlobal.rsv[1] & 0x3Fu);
    if (gGlobal.rsv[2] >= 1 && gGlobal.rsv[2] <= destCount()) g_joyDestX = (uint8_t)(gGlobal.rsv[2] - 1);
    if (gGlobal.rsv[3] >= 1 && gGlobal.rsv[3] <= destCount()) g_joyDestY = (uint8_t)(gGlobal.rsv[3] - 1);
    g_shadowValid = false;
  }

  // Ticked from loop(). Every branch is a state test, never a wait.
  static void service() {
    const uint8_t es = HalEeprom::state();
    switch (storeOp) {
      case ST_BOOT_GLOBALS:
        if (es == EE_DONE || es >= EE_ERR_NAK) {
          HalEeprom::clear();
          storeOp = ST_IDLE;
          // THE BOOT ROUTE. Spec: "All these preset should show while booting
          // if on device is no presets", and otherwise "Loading last Preset".
          // v1.15: size and version are checked BEFORE the CRC reads `size`
          // bytes - it used to trust a length straight out of EEPROM and could
          // read up to 64 KB past this 32-byte struct.
          const bool wizardNeeded =
              (es != EE_DONE) || (gGlobal.magic != XY6_MAGIC_GLOBAL) ||
              (gGlobal.version != XY6_FORMAT_VER) ||
              (gGlobal.size != (uint16_t)(sizeof(GlobalCfgP) - 14)) ||
              !(gGlobal.flags & 0x01) || (gGlobal.lastPreset >= EE_PRESET_SLOTS) ||
              (xy6Crc16((const uint8_t*)&gGlobal + 14, gGlobal.size) != gGlobal.crc);
          if (wizardNeeded) memset(&gGlobal, 0, sizeof(gGlobal));
          else { presetSlot = (uint8_t)(gGlobal.lastPreset + 1); applyGlobals(); }
        }
        break;
      case ST_LOAD_REC:
        if (es == EE_DONE) { HalEeprom::clear(); storeOp = ST_IDLE;
                             unpackFrom(); uiTouch(); }
        else if (es >= EE_ERR_NAK) { HalEeprom::clear(); storeOp = ST_ERR;
                                     strcpy(storeMsg, "READ FAIL"); uiTouch(); }
        break;
      case ST_SAVE_REC:
        if (es == EE_DONE) {
          HalEeprom::clear();
          if (storeErasing) {             // v1.15: a delete is not a save
            storeErasing = false; storeOp = ST_IDLE;
            strcpy(storeMsg, "DELETED"); uiTouch();
            break;
          }
          gGlobal.lastPreset = storeSlot;
          gGlobal.flags |= 0x01;          // the first-run wizard is complete
          writeGlobals();                 // chains into ST_SAVE_GLOBALS
        } else if (es >= EE_ERR_NAK) {
          HalEeprom::clear(); storeOp = ST_ERR;
          strcpy(storeMsg, "WRITE FAIL"); uiTouch();
        }
        break;
      case ST_SAVE_GLOBALS:
        if (es == EE_DONE) { HalEeprom::clear(); storeOp = ST_IDLE;
                             strcpy(storeMsg, "SAVED"); uiTouch(); }
        else if (es >= EE_ERR_NAK) { HalEeprom::clear(); storeOp = ST_ERR;
                                     strcpy(storeMsg, "WRITE FAIL"); uiTouch(); }
        break;
      default: break;
    }
  }
};

// =============================================================================
// SECTION: FIRST-RUN WIZARD                                        new in v7
// =============================================================================
// Spec: "1 REDOT (dot getting bigger)" -> "2 machine select" -> "3 KIT sysex
// recieve, Animated Page" -> "Preset SAVE", shown only when the device has no
// presets on it.

static void drawWizard(uint32_t nowMs) {
  gfx.clear();
  const int16_t W = UI_W;
  char v[12];

  switch (wizState) {
    case WIZ_SPLASH:
      stHeader("SETUP", "1/3");
      gfx.text35(0, 22, "NO PRESETS ON", SH_ON);
      gfx.text35(0, 30, "THIS DEVICE.", SH_ON);
      stBox(0, 46, W, 34, "FIRST RUN");
      gfx.text35(3, 58, "3 STEPS, THEN", SH_MID);
      gfx.text35(3, 66, "IT SAVES.", SH_MID);
      stChipAuto(0, 92, "CLICK TO START", true);
      break;

    case WIZ_MACHINE_SELECT:
      // v1.24: a menu - T1..T6 in 3x5, the machine in 5x7, the track under
      // E1 the focus block the full width.
      stHeader("MACHINE", "2/3");
      gfx.text35(0, 12, "E1 TRK E2 MACH", G_MID);
      gfx.hLine(0, 19, W, G_DARK);
      for (uint8_t t = 0; t < 6; ++t) {
        const int16_t y = (int16_t)(21 + t * 11);
        uint8_t tsh = G_MID, msh = machineSel[t] ? G_WHITE : G_MID;
        if (wizCursor == t) tsh = msh = stFocus(0, y, W, ST_ROW);
        const char lb[3] = {'T', (char)('1' + t), 0};
        gfx.text35(2, (int16_t)(y + 2), lb, tsh);
        stText(12, (int16_t)(y + 1), (int16_t)(W - 14), mnmMachineLabel(machineSel[t]), msh, 0);
      }
      stFooter("CLICK", "NEXT");
      break;

    case WIZ_SYSEX_WAIT: {
      stHeader("KIT", "3/3");
      gfx.text35(0, 22, "SEND A KIT FROM", SH_DIM);
      gfx.text35(0, 30, "THE MONOMACHINE", SH_DIM);
      gfx.text35(0, 38, "SYSEX SEND MENU", SH_DIM);
      // The animation is the honest part of this page: on a screen that cannot
      // show progress, it is what says the box is listening.
      stBox(0, 52, W, 20, nullptr);
      const int16_t inner = (int16_t)(W - 4);
      const int16_t ph = (int16_t)((nowMs / 40) % (uint32_t)(inner * 2));
      const int16_t x  = (ph < inner) ? ph : (int16_t)(inner * 2 - ph);
      gfx.fillRect((int16_t)(2 + x - 4 < 2 ? 2 : 2 + x - 4), 58, 9, 8, SH_ON);
      u8s((uint8_t)(rxBytes > 250 ? 250 : rxBytes), v);
      stRow(80, "RX", v, false);
      stFooter("CLICK", "SKIP");
    } break;

    case WIZ_SYSEX_OK:
      stHeader("KIT", "3/3");
      stChipAuto(0, 22, sxGotDump ? "DUMP OK" : "SKIPPED", true);
      u8s((uint8_t)(sxMsgBytes > 250 ? 250 : sxMsgBytes), v);
      stRow(34, "BYTES", v, false);
      // Honest about what is and is not implemented, rather than implying the
      // engine placements were decoded when they were not.
      gfx.text35(0, 50, "PARSE: TODO", G_MID);
      stFooter("CLICK", "SAVE");
      break;

    default:
      stHeader("SAVING", nullptr);
      stChipAuto(0, 24, storeMsg, true);
      stBar(0, 36, W, 9, HalEeprom::progress(), (int32_t)sizeof(PresetP));
      break;
  }
  // The hold bar is drawn on every wizard screen, so the hold gesture is
  // discoverable from the very first page the user ever sees.
  if (uiHoldRing) {
    const int16_t w = (int16_t)(((int32_t)uiHoldRing * (W - 2)) / 255);
    gfx.fillRect(1, 244, w, 2, SH_ON);
  }
}

static void wizEncoders(const int8_t* d) {
  if (wizState != WIZ_MACHINE_SELECT) return;
  if (d[0]) wizCursor = addClamp(wizCursor, d[0], 0, 5);
  if (d[1]) applyMachine(wizCursor,
              addClamp(machineSel[wizCursor], d[1], 0, (uint8_t)(MACHINE_COUNT - 1)));
}

static void wizAdvance() {
  switch (wizState) {
    case WIZ_SPLASH:         wizState = WIZ_MACHINE_SELECT; break;
    case WIZ_MACHINE_SELECT: wizState = WIZ_SYSEX_WAIT;     // a fresh wait: an old
                             sxGotDump = false; sxMsgBytes = 0; break;  // dump is not this one
    case WIZ_SYSEX_WAIT:     wizState = WIZ_SYSEX_OK;       break;   // skip allowed
    case WIZ_SYSEX_OK:       wizState = WIZ_SAVE;
                             Store::saveTo(0);              break;
    default: break;                                          // WIZ_SAVE waits
  }
  uiTouch();
}

// -----------------------------------------------------------------------------
// PAGE 4 - SETTINGS, PRESETS, AND THE LINK BLOCK
// -----------------------------------------------------------------------------
// A scrolling list rather than a fixed layout, because this is the page that
// grows. Encoder 1 moves the cursor, encoder 6 changes the selected value, and
// pushing encoder 1 fires an action row. Encoders 2-5 are deliberately idle
// here: a settings page where four knobs silently change something you cannot
// see is how patches get wrecked.
//
// The LINK block at the bottom is the old DIAG page, folded in. It has to stay
// reachable without a console - the turbo bring-up needs LAST SYSEX IN, and
// reading the device-id bytes off the panel is the single fastest way to find
// out why a handshake is going nowhere.
//
// PRESETS: SAVE / LOAD / DELETE act on the SLOT row through Store (above), a
// state machine on the 1 kHz EEPROM tick, so nothing here waits on the part.
enum SetKind : uint8_t { SK_HEAD = 0, SK_VAL, SK_ACT, SK_INFO };
enum : uint8_t {
  SI_SLOT = 1, SI_SAVE, SI_LOAD,
  SI_CH, SI_PERTRK, SI_MODE,
  SI_TSPEED, SI_TGO,
  SI_BRIGHT, SI_WIDTH, SI_FONT,
  SI_RX, SI_CLK, SI_SYX, SI_TX, SI_PEAK, SI_STATE, SI_BAUD, SI_I2C,
  SI_HDRA, SI_HDRB,
  SI_DELETE, SI_EE, SI_LOOP, SI_SKIP, SI_STYLE, SI_INVERT, SI_TRANS, SI_TRANSSUB,
  SI_MACH1, SI_MACH2, SI_MACH3, SI_MACH4, SI_MACH5, SI_MACH6       // v1.22
};
struct SetRow { uint8_t kind, id; const char* label; };
static const SetRow kSetRows[] = {
  {SK_HEAD, 0,         "PRESETS"},
  {SK_VAL,  SI_SLOT,   "SLOT"},
  {SK_ACT,  SI_SAVE,   "SAVE"},
  {SK_ACT,  SI_LOAD,   "LOAD"},
  {SK_ACT,  SI_DELETE, "DELETE"},
  {SK_HEAD, 0,         "MIDI"},
  {SK_VAL,  SI_CH,     "BASE CH"},
  {SK_VAL,  SI_PERTRK, "PER TRK"},
  {SK_VAL,  SI_MODE,   "TX MODE"},
  // v1.22: which machine each track has loaded. It names the SYNT page and
  // gives it its real ranges and init values everywhere on the XY6.
  {SK_HEAD, 0,         "MACHINES"},
  {SK_VAL,  SI_MACH1,  "T1"},
  {SK_VAL,  SI_MACH2,  "T2"},
  {SK_VAL,  SI_MACH3,  "T3"},
  {SK_VAL,  SI_MACH4,  "T4"},
  {SK_VAL,  SI_MACH5,  "T5"},
  {SK_VAL,  SI_MACH6,  "T6"},
  {SK_HEAD, 0,         "TURBO"},
  {SK_VAL,  SI_TSPEED, "SPEED"},
  {SK_ACT,  SI_TGO,    "ENGAGE"},
  {SK_HEAD, 0,         "DISPLAY"},
  {SK_VAL,  SI_BRIGHT, "BRIGHT"},
  {SK_VAL,  SI_WIDTH,  "WIDTH"},
  {SK_VAL,  SI_STYLE,  "STYLE"},
  {SK_VAL,  SI_INVERT, "INVERT"},
  {SK_HEAD, 0,         "TRANSITION"},
  {SK_VAL,  SI_TRANS,  "PAGES"},
  {SK_VAL,  SI_TRANSSUB, "SUB"},
  {SK_HEAD, 0,         "LINK"},
  {SK_INFO, SI_RX,     "RX"},
  {SK_INFO, SI_CLK,    "CLOCK"},
  {SK_INFO, SI_SYX,    "SYSEX"},
  {SK_INFO, SI_TX,     "TX"},
  {SK_INFO, SI_PEAK,   "RX PEAK"},
  {SK_INFO, SI_STATE,  "STATE"},
  {SK_INFO, SI_BAUD,   "BAUD"},
  // Non-zero here means the MCP23017 is dropping transactions, which is now a
  // visible fault rather than a phantom button press. See mcpReadOk().
  {SK_INFO, SI_I2C,    "I2C ERR"},
  {SK_INFO, SI_EE,     "EEPROM"},
  {SK_INFO, SI_LOOP,   "LOOP US"},
  {SK_INFO, SI_SKIP,   "FRM SKIP"},
  {SK_INFO, SI_HDRA,   "SYSEX IN"},
  {SK_INFO, SI_HDRB,   ""},
};
static const uint8_t SET_ROWS = (uint8_t)(sizeof(kSetRows) / sizeof(kSetRows[0]));

static uint8_t setCur = 1;          // starts on the first selectable row
static uint8_t setTop = 0;          // first row drawn
// v1.19: the two states the initiator knows - 1X, or the TurboMIDI link
// (tested at SPEED1, run at SPEED2). ENGAGE goes to whichever is shown.
static const uint8_t kSetTurboIdx[2] = {1, TURBO_SPEED2_CODE};
static uint8_t setTurboSel = 1;     // the turbo speed

// The cursor may rest on an INFO row even though there is nothing to change
// there. That is not cosmetic: the window only scrolls to follow the cursor, so
// excluding INFO rows meant the LINK block below the fold - including the
// second half of the last SysEx header, which is where the command byte lives -
// could never be brought on screen at all.
static bool setSelectable(uint8_t r) {
  return r < SET_ROWS && kSetRows[r].kind != SK_HEAD;
}

static void setValue(uint8_t id, char* out, uint8_t cap) {
  out[0] = 0;
  switch (id) {
    case SI_SLOT:   u8s(presetSlot, out); break;
    case SI_SAVE:
    case SI_LOAD:
    case SI_DELETE: snprintf(out, cap, Store::busy() ? ".." : "GO"); break;
    case SI_EE:     snprintf(out, cap, HalEeprom::present()
                                       ? (storeOp == ST_ERR ? "ERR" : "OK") : "ABSENT");
                    break;
    case SI_LOOP:   snprintf(out, cap, "%lu", (unsigned long)gStats.passUsMax); break;
    case SI_SKIP:   snprintf(out, cap, "%lu",
                             (unsigned long)gStats.frameSkipsBacklog); break;
    case SI_CH:     u8s(txChannel, out); break;
    case SI_PERTRK: snprintf(out, cap, txPerTrack ? "ON" : "OFF"); break;
    // Two calls, not one runtime-selected format string. The old form handed
    // an argument to a format that may not consume it - legal varargs, but the
    // compiler cannot check a format it does not know at compile time, so it is
    // a trap waiting for whoever edits the strings next.
    case SI_MODE:   if (txMode == 3) snprintf(out, cap, "CC");
                    else             snprintf(out, cap, "NRPN%u", txMode);
                    break;
    case SI_TSPEED: snprintf(out, cap, "%s", setTurboSel ? kTmNames[turbo.speed2Code()] : "1X"); break;
    case SI_MACH1: case SI_MACH2: case SI_MACH3:
    case SI_MACH4: case SI_MACH5: case SI_MACH6:
      snprintf(out, cap, "%s", mnmMachineLabel(machineSel[id - SI_MACH1])); break;
    case SI_TGO:    snprintf(out, cap, "GO"); break;
    case SI_BRIGHT: snprintf(out, cap, "%02X", (unsigned)uiContrast); break;
    case SI_WIDTH:  u8s((uint8_t)UI_W, out); break;
    case SI_STYLE:  snprintf(out, cap, uiChipStyle ? "CHIP" : "LINE"); break;
    case SI_INVERT: snprintf(out, cap, uiInvert ? "ON" : "OFF"); break;
    case SI_TRANS:  snprintf(out, cap, "%s", kTransName[uiTransStyle < TS_COUNT ? uiTransStyle : 0]); break;
    case SI_TRANSSUB: snprintf(out, cap, "%s", kTransName[uiTransSub < TS_COUNT ? uiTransSub : 0]); break;
    case SI_RX:     snprintf(out, cap, "%lu", (unsigned long)(rxBytes % 1000000)); break;
    case SI_CLK:    snprintf(out, cap, "%lu", (unsigned long)(rxClock % 1000000)); break;
    case SI_SYX:    snprintf(out, cap, "%lu", (unsigned long)(rxSysex % 10000)); break;
    case SI_TX:     snprintf(out, cap, "%lu", (unsigned long)(txBytesOut % 1000000)); break;
    case SI_PEAK:   snprintf(out, cap, "%u/%u", (unsigned)rxPeak, (unsigned)rxCapacity); break;
    case SI_STATE:  snprintf(out, cap, "%s", turbo.stateName()); break;
    case SI_BAUD:   snprintf(out, cap, "%lu", (unsigned long)turbo.baud()); break;
    case SI_I2C:    snprintf(out, cap, "%lu", (unsigned long)mcpErrors); break;
    case SI_HDRA:
    case SI_HDRB: {
      static const char* H = "0123456789ABCDEF";
      const uint8_t base = (uint8_t)((id == SI_HDRA) ? 0 : 4);
      uint8_t k = 0;
      // Honour cap. This was the one branch in setValue that ignored the bound
      // it was handed - safe at 8 bytes into 20, and a silent overrun the first
      // time either number changes.
      for (uint8_t i = 0; i < 4 && (uint8_t)(base + i) < sxLastLen &&
                          (uint8_t)(k + 3) <= cap; ++i) {
        out[k++] = H[(sxLastHdr[base + i] >> 4) & 0xF];
        out[k++] = H[sxLastHdr[base + i] & 0xF];
      }
      out[k] = 0;
      if (!k) snprintf(out, cap, (id == SI_HDRA) ? "NONE" : "");
      break;
    }
    default: break;
  }
}

static void setAdjust(uint8_t id, int8_t d) {
  switch (id) {
    case SI_SLOT:   presetSlot = addClamp(presetSlot, d, 1, EE_PRESET_SLOTS); break;
    case SI_CH:     setBaseChannel(addClamp(txChannel, d, 1, 16)); break;
    case SI_PERTRK: txPerTrack = !txPerTrack; mnmOut.resend(); break;
    case SI_MODE:   txMode = addClamp(txMode, d, 0, 3); mnmOut.resend(); break;
    case SI_TSPEED: setTurboSel = addClamp(setTurboSel, d, 0, 1); break;
    case SI_MACH1: case SI_MACH2: case SI_MACH3:
    case SI_MACH4: case SI_MACH5: case SI_MACH6: {
      const uint8_t t = (uint8_t)(id - SI_MACH1);
      applyMachine(t, addClamp(machineSel[t], d, 0, (uint8_t)(MACHINE_COUNT - 1)));
      break;
    }
    case SI_BRIGHT: {
      int32_t v = (int32_t)uiContrast + (int32_t)d * 16;
      if (v < 16)  v = 16;
      if (v > 255) v = 255;
      uiContrast = (uint8_t)v;
      setContrast(uiContrast);
      break;
    }
    case SI_WIDTH:  UI_W = addClamp((uint8_t)UI_W, d, 8, 64);
                    g_shadowValid = false; break;
    case SI_STYLE:  uiChipStyle = !uiChipStyle; g_shadowValid = false; break;
    case SI_INVERT: uiSetInvert(!uiInvert); break;
    case SI_TRANS:  uiTransStyle = addClamp(uiTransStyle, d, 0, (uint8_t)(TS_COUNT - 1)); break;
    case SI_TRANSSUB: uiTransSub = addClamp(uiTransSub, d, 0, (uint8_t)(TS_COUNT - 1)); break;
    default: break;
  }
}

static void setActivate(uint8_t id) {
  switch (id) {
    // These are live now. The write is a state machine on the 1 kHz EEPROM
    // tick, so the LFOs keep running and the clock keeps locking throughout -
    // the progress bar under the row is the only sign anything is happening.
    //
    // tmSerial, not Serial: this runs from the button handler in loop(), and a
    // bare Serial.printf into a host that has stopped draining spins for up to
    // 120 ms - about 3750 received bytes at 250000 baud.
    case SI_SAVE:
      if (Store::saveTo((uint8_t)(presetSlot - 1)))
        tmSerial.printf("preset: saving slot %u (%u bytes)\n",
                        presetSlot, (unsigned)sizeof(PresetP));
      break;
    case SI_LOAD:
      if (Store::loadFrom((uint8_t)(presetSlot - 1)))
        tmSerial.printf("preset: loading slot %u\n", presetSlot);
      break;
    case SI_DELETE:
      if (Store::eraseSlot((uint8_t)(presetSlot - 1)))
        tmSerial.printf("preset: clearing slot %u\n", presetSlot);
      break;
    case SI_TGO:
      if (kSetTurboIdx[setTurboSel] == 1) turbo.stop(millis());
      else                                turbo.start(millis());
      break;
    default: break;
  }
}

// v1.24: an Elektron menu. Section heads are 3x5 LIGHT with a G_DARK rule
// running on to the right edge; rows are the 3x5 label (MID) and the value
// (WHITE, read-outs LIGHT) at the right, 9px apart; the cursor row is the
// focus block, the full width but the scroll bar - a 1px G_DARK track at
// x 63 with a WHITE thumb.
static void drawSettings(uint32_t step16, bool running, uint32_t bpmX100) {
  (void)step16; (void)running; (void)bpmX100;
  gfx.clear();
  const int16_t W = UI_W;
  const int16_t RW = (int16_t)(W - 2);          // rows stop short of the scroll bar

  stHeader("SET", "CFG");

  const int16_t top = ST_BODY_Y, pitch = ST_ROW, bot = 218;
  const uint8_t vis = (uint8_t)((bot - top) / pitch);

  // Keep the cursor on screen without ever scrolling past the end.
  if (setCur < setTop) setTop = setCur;
  if (setCur >= (uint8_t)(setTop + vis)) setTop = (uint8_t)(setCur - vis + 1);
  if (setTop + vis > SET_ROWS)
    setTop = (SET_ROWS > vis) ? (uint8_t)(SET_ROWS - vis) : 0;

  for (uint8_t k = 0; k < vis; ++k) {
    const uint8_t r = (uint8_t)(setTop + k);
    if (r >= SET_ROWS) break;
    const SetRow& row = kSetRows[r];
    const int16_t y = (int16_t)(top + k * pitch);

    if (row.kind == SK_HEAD) {
      gfx.text35(0, (int16_t)(y + 2), row.label, G_LIGHT);
      const int16_t tx = (int16_t)(gfx.width35(row.label) + 3);
      gfx.hLine(tx, (int16_t)(y + 4), (int16_t)(RW - tx), G_DARK);
      continue;
    }
    char v[20]; setValue(row.id, v, sizeof v);
    uint8_t lsh = G_MID, vsh = (row.kind == SK_INFO) ? G_LIGHT : G_WHITE;
    if (setCur == r) lsh = vsh = stFocus(0, y, RW, ST_ROW);
    gfx.text35(2, (int16_t)(y + 2), row.label, lsh);
    stText((int16_t)(RW - 1), (int16_t)(y + 1),
           (int16_t)(RW - gfx.width35(row.label) - 7), v, vsh, 2);
  }

  // Scroll position - the only cue that there is more page below the fold.
  if (SET_ROWS > vis) {
    const int16_t h = (int16_t)((int32_t)(bot - top) * vis / SET_ROWS);
    const int16_t yy = (int16_t)(top + (int32_t)(bot - top) * setTop / SET_ROWS);
    gfx.vLine((int16_t)(W - 1), top, (int16_t)(bot - top), G_DARK);
    gfx.vLine((int16_t)(W - 1), yy, h > 3 ? h : 3, G_WHITE);
  }

  // A save is the one action here that takes long enough to need saying so.
  gfx.hLine(0, 221, W, G_DARK);
  if (Store::busy()) {
    stChipAuto(0, 224, storeMsg, true);
    stBar(0, 234, W, 3, HalEeprom::progress(), (int32_t)sizeof(PresetP));
  } else if (storeMsg[0]) {
    stChipAuto(0, 224, storeMsg, storeOp == ST_ERR);
    gfx.text35(0, 235, "E1 PUSH = DO", G_MID);
  } else {
    gfx.text35(0, 235, "E1 PUSH = DO", G_MID);
  }
  stFooter("E1 SEL", "E6 VAL");
}

static void encSettings(const int8_t* d) {
  if (d[0]) {
    int16_t r = (int16_t)setCur;
    int8_t step = d[0] > 0 ? 1 : -1;
    for (int8_t k = 0; k < (d[0] > 0 ? d[0] : -d[0]); ++k) {
      int16_t nr = r;
      do { nr = (int16_t)(nr + step); }
      while (nr >= 0 && nr < SET_ROWS && !setSelectable((uint8_t)nr));
      if (nr < 0 || nr >= SET_ROWS) break;      // stop at the ends, do not wrap
      r = nr;
    }
    setCur = (uint8_t)r;
  }
  if (d[5] && setSelectable(setCur)) setAdjust(kSetRows[setCur].id, d[5]);
}
static void btnSettings(uint8_t pressed) {
  if ((pressed & (1 << 0)) && setSelectable(setCur)) {
    if (kSetRows[setCur].kind == SK_ACT) setActivate(kSetRows[setCur].id);
    else                                 setAdjust(kSetRows[setCur].id, 1);
  }
}

// ---- draw adapters, so every page has one signature ------------------------
static void drawOverviewA(uint32_t step16, bool running, uint32_t bpmX100) {
  (void)step16;
  // The link speed used to live in this slot; it is the badge's job now, so
  // this line carries only the hunt indicator and is otherwise blank rather
  // than printing the same multiplier twice on one page.
  drawOverview(running, huntMode ? "HUNT" : "", bpmX100);
}
static void drawEditA(uint32_t step16, bool running, uint32_t bpmX100) {
  (void)bpmX100; drawEdit(step16, running);
}
static void drawPatternA(uint32_t step16, bool running, uint32_t bpmX100) {
  drawPattern(step16, running, bpmX100);
}

// ============================== the page table ===============================
// `animated` says the page has live content of its own - a scope trace, a
// playhead - and must redraw every frame. A page without it is drawn only when
// uiTouch() has been called, so parking on the performance page costs nothing
// between knob turns and leaves the whole loop to MIDI.
struct UiPageDef {
  const char* name;
  void (*draw)(uint32_t, bool, uint32_t);
  void (*enc)(const int8_t*);
  void (*btn)(uint8_t);
  bool animated;
  // The subpage a 0.5 s hold of the board button opens. A null draw means this
  // page has none and the hold does nothing.
  const char* subName;
  void (*subDraw)(uint32_t, bool, uint32_t);
  void (*subEnc)(const int8_t*);
  void (*subBtn)(uint8_t);
  bool subAnimated;
};
static const UiPageDef kUiPages[UPAGE_COUNT] = {
  // PERF stays un-animated: nothing on it moves by itself. The badge changing
  // is a state change, not an animation, and the turbo state machine calls
  // uiTouch() when it happens - which is exactly the redraw trigger asked for.
  {"PERF", drawPerform,   encPerform,     btnPerform,     false,
   "DEST", drawPerfSub,   encPerfSub,     btnPerfSub,     false},
  {"LFO",  drawOverviewA, encOverview,    btnOverview,    true,
   "EDIT", drawEditA,     encEdit,        btnEdit,        true },
  {"PAT",  drawPatternA,  encPatternPage, btnPatternPage, true,
   nullptr, nullptr,      nullptr,        nullptr,        false},
  // Animated because the LINK block's counters move on their own.
  // NOT animated. The LINK counters do move on their own, but nobody needs
  // them at 30 Hz, and this was the only page in the OS that could never idle -
  // 25 snprintf per frame, forever, for digits you read once. loop() ticks it
  // dirty four times a second instead; see UI_SET_TICK_MS.
  {"SET",  drawSettings,  encSettings,    btnSettings,    false,
   nullptr, nullptr,      nullptr,        nullptr,        false},
};

// Whether the current page even has a subpage to hold for.
static bool uiHasSub() { return kUiPages[ui.page].subDraw != nullptr; }
static void uiToggleSub() {
  if (!uiHasSub()) return;
  ui.inSub = !ui.inSub;
  if (ui.inSub && ui.page == UPAGE_PERF) g_perfSub = 0;   // v1.17: in at DEST
  uiTouch();
}
// v1.17: the name of the sub-page on screen - PERF has two now.
static const char* uiSubName() {
  if (ui.page == UPAGE_PERF && g_perfSub == 1) return "JOY";
  return kUiPages[ui.page].subName;
}

// v1.16 - THE SUB-PAGE RING.
// Every sub-page instance of every page, in page order. A click in SUB mode
// walks this ring and stays in SUB mode; only a hold leaves it. The LFO page's
// EDIT sub-page is six instances - one per LFO - so a click walks EDIT L1 to
// EDIT L6, which is the Elektron "page button cycles LFO1/2/3" habit.
// To add a sub-page later: give its page a subDraw and an entry here.
struct SubRingEntry { uint8_t page, count; };
static const SubRingEntry kSubRing[] = {
  {UPAGE_PERF, 2},             // DEST, JOY      (the instance is g_perfSub)
  {UPAGE_LFO,  LFO_COUNT},     // EDIT L1 .. L6  (the instance IS ui.sel)
};
static const uint8_t SUB_RING_N = sizeof(kSubRing) / sizeof(kSubRing[0]);

static void uiNextSub();
// v1.18: declared, not left to the Arduino IDE's generated prototypes - the
// only function in the file that relied on them, so any other C++ toolchain
// (the host test in test/host, PlatformIO with a .cpp) failed right here.
static void uiNextPage();
// v1.16: the board button's two actions, as functions, so the console can
// drive exactly the same state machine the button does ('click', 'hold').
static void uiBoardHold() {
  if (sysState != SYS_RUN) return;
  if (ui.inSub) { ui.inSub = false; uiTouch(); }   // SUB  -> MAIN, same page
  else          uiToggleSub();                    // MAIN -> SUB (if it has one)
}
static void bootFinish();
static void wizAdvance();
static void uiBoardClick() {
  if      (bootActive)             bootFinish();
  else if (sysState == SYS_WIZARD) wizAdvance();
  else if (sysState == SYS_FAULT)  { sysState = SYS_RUN; uiTouch(); }
  else if (ui.inSub)               uiNextSub();   // SUB:  next sub-page
  else                             uiNextPage();  // MAIN: next page
}

static void uiNextSub() {
  uint8_t r = 0;
  while (r < SUB_RING_N && kSubRing[r].page != ui.page) r++;
  if (r >= SUB_RING_N) { ui.inSub = false; uiTouch(); return; }   // defensive
  uint8_t inst = (uint8_t)((ui.page == UPAGE_LFO  ? ui.sel :
                             ui.page == UPAGE_PERF ? g_perfSub : 0) + 1);
  if (inst >= kSubRing[r].count) { r = (uint8_t)((r + 1) % SUB_RING_N); inst = 0; }
  ui.page = kSubRing[r].page;
  if (ui.page == UPAGE_PERF) g_perfSub = inst;
  if (ui.page == UPAGE_LFO) {
    ui.sel = (uint8_t)(inst < LFO_COUNT ? inst : 0);
    if (ui.stepPage >= lfo.p[ui.sel].bars) ui.stepPage = 0;
  }
  ui.inSub = true;
  uiTouch();
}
static void uiNextPage() {
  ui.page  = (uint8_t)((ui.page + 1) % UPAGE_COUNT);
  ui.inSub = false;            // a page change always lands on the top level
  uiTouch();
}

// ============================ boot routing ===================================
// Spec, normal boot: "1 Redot Logo Animation -> Loading last Preset" -> "2
// Encoder Page". Spec, cold boot: "All these preset should show while booting
// if on device is no presets".
//
// The splash is not a stall with a picture over it: the globals read is in
// flight behind it, which is why the decision can be made the instant it ends.
static void bootFinish() {
  if (Store::busy()) return;              // globals still in flight; try again
  bootActive = false;
  if (!HalEeprom::present()) { sysState = SYS_FAULT; uiTouch(); return; }
  const bool wizardNeeded = (gGlobal.magic != XY6_MAGIC_GLOBAL) ||
                            !(gGlobal.flags & 0x01);
  if (wizardNeeded) { wizState = WIZ_SPLASH; sysState = SYS_WIZARD; }
  else {
    Store::loadFrom(gGlobal.lastPreset);
    sysState = SYS_LOADING;
  }
  uiTouch();
}

// A box that cannot reach its storage must SAY so, rather than booting into a
// UI whose SAVE key silently does nothing. Everything else still works.
static void drawFault() {
  gfx.clear();
  const int16_t W = UI_W;
  stHeader("FAULT", "EE");
  stChipAuto(0, 22, "EEPROM 0X50", true);
  gfx.text35(0, 34, "NOT RESPONDING", SH_ON);
  stBox(0, 48, W, 30, "CHECK");
  gfx.text35(3, 60, "SDA 18 SCL 19", SH_MID);
  gfx.text35(3, 68, "AND ITS POWER", SH_MID);
  gfx.text35(0, 86, "PRESETS ARE", SH_DIM);
  gfx.text35(0, 94, "DISABLED. THE", SH_DIM);
  gfx.text35(0, 102, "REST STILL RUNS", SH_DIM);
  stFooter("CLICK", "GO ON");
}

// Spec: "i could send midi on monomachine i will recieve via [di]rect play".
// The hook is on the schedule at full loop rate so the Step 3 implementation
// has somewhere to live that is already correctly placed in the priority order.
static void MIDI_HandleDirectPlayCapture(uint32_t /*nowUs*/) {
  // TODO: [UX DECISION] echo note-on/off received from the Monomachine straight
  // back out so it sounds while you play, AND write it into pat.step[] at the
  // current 16th. The receive side needs handleMidiByte to stop discarding
  // note messages, which is a deliberate change to make with the pattern
  // engine in front of you rather than as a rider on this merge.
}

// ============================ encoders + MCP23017 ============================

static volatile uint8_t encPrev[6];
static volatile int8_t  encAcc[6];
static const int8_t kQuad[16] = {0,-1,1,0, 1,0,0,-1, -1,0,0,1, 0,1,-1,0};

// ---------------------------------------------------------------------------
// INTERRUPT-DRIVEN QUADRATURE
// ---------------------------------------------------------------------------
// The previous version polled all six encoders at 5 kHz from the main loop,
// which is genuinely fine for a hand-turned PEC16 - fast hands manage perhaps
// 400 transitions a second, so 5 kHz is twelve times oversampled. It has one
// real hole though: the poll stops while the loop is busy. A full-frame display
// push is about a millisecond of solid bus writing, and during that window the
// encoders are not being read at all. A fast flick across that gap can present
// two transitions where the poll sees one, and a two-bit state machine reading
// 00 -> 11 cannot tell which way it went, so the step is dropped.
//
// Edge interrupts close that hole: the decoder runs when the encoder moves, not
// when the loop gets around to it, so a redraw, a SysEx burst and a turbo speed
// change can no longer cost you a detent. Every pin on a Teensy 4 supports
// interrupts, so all twelve are attached.
// v1.10: DETENT-ALIGNED stepping. The old decoder counted quarter-steps and
// called every 4 of them a detent, carrying the remainder forward. One missed
// or doubled transition (a bounce the table could not cancel, a flick faster
// than the pins settle) left that remainder off by one FOREVER, so from then on
// the step fired halfway between detents and a click sometimes did nothing.
// Now a step is committed only when the encoder arrives back at its REST state
// (the state it sat in at boot, i.e. a detent), with at least half a cycle of
// net travel in one direction - and the accumulator is cleared there. Every
// detent re-aligns the count, so an error can cost at most the one click it
// happened in.
static volatile int8_t encSteps[6];    // whole detents, consumed by encodersRead
static uint8_t         encRest[6];     // pin state at a detent (read at boot)
static void encEdge(uint8_t i) {
  const uint8_t cur = (uint8_t)((digitalReadFast(PIN_ENC[i].a) << 1) |
                                 digitalReadFast(PIN_ENC[i].b));
  encAcc[i]  = (int8_t)(encAcc[i] + kQuad[((encPrev[i] << 2) | cur) & 0x0F]);
  encPrev[i] = cur;
  if (cur == encRest[i]) {
    if      (encAcc[i] >=  2 && encSteps[i] <  120) encSteps[i]++;
    else if (encAcc[i] <= -2 && encSteps[i] > -120) encSteps[i]--;
    encAcc[i] = 0;
  }
}
// attachInterrupt takes a plain function pointer, so one trampoline each.
static void encISR0() { encEdge(0); }
static void encISR1() { encEdge(1); }
static void encISR2() { encEdge(2); }
static void encISR3() { encEdge(3); }
static void encISR4() { encEdge(4); }
static void encISR5() { encEdge(5); }

static void encodersBegin() {
  for (uint8_t i = 0; i < 6; ++i) {
    pinMode(PIN_ENC[i].a, INPUT_PULLUP);
    pinMode(PIN_ENC[i].b, INPUT_PULLUP);
    encPrev[i] = (uint8_t)((digitalReadFast(PIN_ENC[i].a) << 1) |
                            digitalReadFast(PIN_ENC[i].b));
    encRest[i] = encPrev[i];            // a knob at rest sits on a detent
    encAcc[i] = 0;
    encSteps[i] = 0;
  }
  void (*const isr[6])() = {encISR0, encISR1, encISR2, encISR3, encISR4, encISR5};
  for (uint8_t i = 0; i < 6; ++i) {
    attachInterrupt(digitalPinToInterrupt(PIN_ENC[i].a), isr[i], CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_ENC[i].b), isr[i], CHANGE);
  }
}

// Drain whole detents. The accumulator is touched by an ISR, so the read and
// the subtract have to be one indivisible operation - otherwise an edge landing
// between them is silently discarded, which is the very thing interrupts were
// added to prevent.
// Switches are active low.
//
// This used to return a rising-edge mask and throw everything else away, which
// is fine for "did it click" and useless for "is it still down". A 0.5 s hold
// needs the LEVEL, and telling a short press apart from the first two seconds
// of a hold needs the RELEASE edge - so all three come out.
//
// Declared up here, above encodersRead, because the coarse interceptor below
// reads btnLevel to decide whether a detent is worth 1 or 7.
static uint8_t gpaPrev = 0xFF;
static uint8_t gpaPend = 0xFF;
static uint8_t btnDown = 0;    // newly pressed this scan
static uint8_t btnUp   = 0;    // newly released this scan
static uint8_t btnLevel = 0;   // 1 = held right now
// False when the last I2C read failed or was still being debounced. The hold
// detector needs this: a dropped transaction that reads as 0xFF is otherwise
// indistinguishable from "nothing pressed", which on a release-triggered
// gesture silently walks the page.
static bool    gpaOk = false;

//
// THE GLOBAL COARSE INTERCEPTOR       spec: "pressing encoder and rotating
//                                            should be 7 values +-"
//
// Applied once, here, between the decoder and the UI - so no page can forget it
// and no page can implement it differently.
//
// encTurnedHeld is the other half, and the half that is easy to miss: push-and-
// turn and push-to-click are the same physical gesture up to the instant you
// turn, so a coarse sweep must not ALSO fire that encoder's push action when
// you let go. Without the suppression every coarse edit ends in an accidental
// button press - on the LFO page that is a bypass toggle you did not ask for.
static uint8_t encTurnedHeld = 0;   // rotated since being pressed
static uint8_t encClick      = 0;   // clean click, consumed by the UI dispatch

static void encodersRead(int8_t* d) {
  const uint8_t held = (uint8_t)(btnLevel & 0x3F);
  for (uint8_t i = 0; i < 6; ++i) {
    noInterrupts();
    const int8_t steps = encSteps[i];      // whole detents since last harvest
    encSteps[i] = 0;
    interrupts();
    const uint8_t bit = (uint8_t)(1u << i);
    if (steps && (held & bit)) {
      // Saturate rather than wrap: a fast flick can present several detents in
      // one 1 kHz harvest, and 3 detents x 7 is already past int8_t.
      int32_t big = (int32_t)steps * ENC_STEP_COARSE;
      if (big >  120) big =  120;
      if (big < -120) big = -120;
      d[i] = (int8_t)big;
      encTurnedHeld |= bit;                // arm the click suppression
    } else {
      d[i] = (int8_t)(steps * ENC_STEP_FINE);
    }
  }
}

// LED bits on port B, from the schematic:
//   GPB0 -> R5 -> D2      GPB1 -> R6 -> D3     GPB2 -> R7 -> D4
//   GPB3 -> R8 -> D5      GPB4 -> R1 -> D1
// All cathodes go to GND, so a 1 lights the LED. D2..D5 are the page LEDs;
// D1 sits on its own resistor and is the tempo LED.
static const uint8_t LED_PAGE1 = 0x01, LED_PAGE2 = 0x02;
static const uint8_t LED_PAGE3 = 0x04, LED_PAGE4 = 0x08;
static const uint8_t LED_TEMPO = 0x10;

static bool mcpWrite(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(I2C_ADDR_MCP);
  Wire.write(reg); Wire.write(val);
  if (Wire.endTransmission() != 0) { mcpErrors++; return false; }
  return true;
}

// v1.15 - I2C FAULT BACK-OFF.
// The Teensy 4 Wire driver gives up on a stuck bus only after 16-50 ms, and the
// switches are scanned 200 times a second: a loose MCP23017 wire turned every
// scan into a stall of the whole loop, and with it MIDI clock handling. After 3
// failures in a row the expander is left alone for 20 ms, doubling to 640 ms;
// the first good read ends the back-off and RE-CONFIGURES the chip, because an
// expander that browned out comes back with its pull-ups off and its LEDs as
// inputs - and floating inputs read as buttons being pressed.
static uint8_t  i2cFailStreak = 0;
static uint32_t i2cHoldUntilMs = 0;
static bool     i2cHeld(uint32_t nowMs) {
  return i2cFailStreak >= 3 && (int32_t)(nowMs - i2cHoldUntilMs) < 0;
}
static void mcpConfigure();
static void i2cNoteResult(bool ok, uint32_t nowMs) {
  if (ok) {
    const bool recovered = (i2cFailStreak >= 3);
    i2cFailStreak = 0;
    if (recovered) mcpConfigure();
    return;
  }
  if (i2cFailStreak < 250) i2cFailStreak++;
  if (i2cFailStreak >= 3) {
    const uint8_t k = (uint8_t)((i2cFailStreak - 3) > 5 ? 5 : (i2cFailStreak - 3));
    i2cHoldUntilMs = nowMs + (20u << k);
  }
}
// Returns false on a bus error instead of inventing a value.
//
// mcpRead()'s 0xFF-on-failure was indistinguishable from "no buttons pressed".
// That was harmless while only PRESS edges mattered, but the board button now
// acts on RELEASE - so one dropped transaction mid-press read as a release and
// silently advanced the page, or killed a hold that was halfway through.
static bool mcpReadOk(uint8_t reg, uint8_t* out) {
  Wire.beginTransmission(I2C_ADDR_MCP);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0)              { mcpErrors++; return false; }
  if (Wire.requestFrom((int)I2C_ADDR_MCP, 1) != 1)   { mcpErrors++; return false; }
  if (!Wire.available())                             { mcpErrors++; return false; }
  *out = (uint8_t)Wire.read();
  return true;
}
static uint8_t ledShadow = 0xFF;
static void mcpConfigure() {
  mcpWrite(0x00, 0xFF);   // IODIRA: port A all inputs (switches + button)
  mcpWrite(0x0C, 0xFF);   // GPPUA:  pull-ups on
  mcpWrite(0x01, 0x00);   // IODIRB: port B all outputs (LEDs)
  mcpWrite(0x15, 0x00);   // OLATB:  LEDs off
  ledShadow = 0xFF;       // whatever we think the LEDs show, rewrite them
}
static void mcpBegin() {
  Wire.begin();
  Wire.setClock(400000);
  mcpConfigure();
}

// v1.10: per-switch debounce. Each bit must read the same for BTN_STABLE_SCANS
// consecutive scans before its level is believed. The v1.09 rule was "two
// agreeing reads of the whole port", i.e. one 5 ms interval - shorter than the
// 10-20 ms a PEC16 push switch can bounce, so a bounce could land as a second
// click. Per-bit also means one bouncing switch no longer holds back an edge on
// a different switch, which a whole-port comparison did.
#define BTN_STABLE_SCANS 3              // 3 scans at 200 Hz = 10..15 ms
static uint8_t gpaCnt[8];               // consecutive agreeing scans, per bit
static void buttonsScan() {
  uint8_t cur;
  gpaOk = false;
  btnDown = btnUp = 0;
  const uint32_t nowMs = millis();
  if (i2cHeld(nowMs)) return;               // v1.15: backing off a bad bus
  const bool ok = mcpReadOk(0x12, &cur);
  i2cNoteResult(ok, nowMs);
  if (!ok) {
    // Bus error: report NO edges and leave the level as it was. Synthesising a
    // release here is exactly what walks the page on you.
    return;
  }
  gpaOk = true;
  uint8_t stable = gpaPrev;
  for (uint8_t b = 0; b < 8; ++b) {
    const uint8_t m = (uint8_t)(1u << b);
    if ((cur ^ gpaPend) & m) { gpaCnt[b] = 0; continue; }     // still moving
    if (gpaCnt[b] < BTN_STABLE_SCANS) ++gpaCnt[b];
    if (gpaCnt[b] >= BTN_STABLE_SCANS - 1)                    // held long enough
      stable = (uint8_t)((stable & ~m) | (cur & m));
  }
  gpaPend  = cur;
  btnDown  = (uint8_t)(~stable & gpaPrev);
  btnUp    = (uint8_t)(stable & (uint8_t)~gpaPrev);
  btnLevel = (uint8_t)~stable;
  gpaPrev  = stable;
}

// Only write the LED register when the bits actually change. It used to be
// rewritten on every beat edge whether or not anything differed, and each write
// is a blocking 400 kHz I2C transaction in the middle of the main loop.
static void ledSet(uint8_t bits) {
  if (bits == ledShadow) return;
  const uint32_t nowMs = millis();
  if (i2cHeld(nowMs)) return;
  // v1.15: the shadow only moves when the write lands, so a failed write is
  // retried on the next call instead of the LEDs staying wrong until the
  // pattern happens to change.
  const bool ok = mcpWrite(0x15, bits);
  i2cNoteResult(ok, nowMs);
  if (ok) ledShadow = bits;
}

// ================================ main =======================================

// Extra UART memory, handed to Serial1 before begin(). See setup().
static uint8_t serial1RxBuf[1024];
static uint8_t serial1TxBuf[512];

// Boot animation clock. elapsedMillis is a Teensy core type that counts up on
// its own; nothing here polls or blocks on it.
static uint64_t freePos = 0;      // advances whether or not the MnM is running,
static uint32_t freeLastUs = 0;   // so FREE-mode LFOs never freeze between takes
static uint8_t  sppState = 0, sppLo = 0;
static uint32_t ledOnUntil = 0;   // tempo LED one-shot

// (receive counters moved above the UI layer - the DIAG page reads them)


// 'dumpraw' capture. A kit dump is a couple of KB, far bigger than the little
// handshake buffer, so this has its own.
static bool     dumpRawOn = false, dumpIn = false;
static bool     dumpReady = false;    // printed from loop(), never from the ISR
static uint16_t dumpLen = 0;          //   path or from inside a display flush
static uint8_t  dumpBuf[4096];
static uint32_t dumpLastByteMs = 0;

static void printDump() {
  Serial.printf("\n===== SysEx captured: %u bytes =====\n", (unsigned)dumpLen);
  if (dumpLen >= 6)
    Serial.printf("header: %02X %02X %02X %02X %02X %02X ...  (F0 + mfr/model/cmd)\n",
        dumpBuf[0], dumpBuf[1], dumpBuf[2], dumpBuf[3], dumpBuf[4], dumpBuf[5]);
  for (uint16_t i = 0; i < dumpLen; i += 16) {
    wdtFeed();
    usbWait(96);                       // room for this line, MIDI kept flowing
    Serial.printf("%04X: ", (unsigned)i);
    for (uint16_t j = i; j < i + 16 && j < dumpLen; ++j)
      Serial.printf("%02X ", dumpBuf[j]);
    Serial.println();
  }
  if (dumpLen >= sizeof(dumpBuf))
    Serial.println("** buffer was full - message may be truncated **");
  Serial.println("===== end - copy everything above and send it =====\n");
}

static bool     rulerMode = false;   // 'disp' shows the panel ruler instead
static bool     rowTestMode = false; // 'rows' shows the COM-line band test
static uint32_t txRate = 0;          // bytes/s actually sent, last full second

// Channel-voice parser state (running status) and a counter of captures made.
static uint8_t  cvStatus = 0, cvData1 = 0;
static bool     cvHave1 = false;
static uint32_t rxCapture = 0;

// Half-assembled channel-voice and Song Position state from BEFORE a baud
// change is poison afterwards: the first data byte at the new speed would be
// folded into a message begun at the old one. sysex1.reset() already covered
// the SysEx parser; this covers the other two.
static void midiRxReset() {
  cvStatus = 0; cvHave1 = false; sppState = 0;
  // An in-progress header capture must be abandoned too, or bytes grabbed
  // BEFORE the speed change get concatenated with bytes from after it into one
  // bogus "LAST SYSEX IN" - on the very panel row you use to diagnose turbo.
  // A capture that already completed is left alone: that one is still true.
  if (sxCapturing) { sxCapturing = 0; sxLastLen = 0; }
}

// An incoming CC from the Monomachine. If it addresses a destination one of our
// LFOs drives, remember its value as that destination's patch value - unless it
// is exactly what we last sent there, which would just be our own output echoed
// back on a THRU/merge.
//
// Returns true if this looked like a genuine move of a knob on the machine.
// That return is the fix for a nasty one: the backoff that drops our CC rate to
// 15 Hz "because the user is busy" used to fire on EVERY incoming CC, our own
// echo included. With a THRU or a merge in the loop, the six LFOs echoed back
// at us continuously and the backoff never expired, so the output ran
// permanently at a third of its rate and every sweep stair-stepped.
static bool captureCc(uint8_t ch, uint8_t cc, uint8_t val) {
  // v1.22: in NRPN modes nothing we send comes back as a CC, so every CC is
  // the machine telling us a value - the kit model takes it unconditionally.
  if (txMode != 3) { kitNoteCc(ch, cc, val); return true; }
  bool ours = false, matched = false;

  // ---- performance page ---------------------------------------------------
  // Turning ATTACK on the machine now moves the ATK fader here. The page used
  // to be write-only: it transmitted on every encoder detent and never listened,
  // so the moment you touched the hardware the two disagreed and stayed that
  // way. Same echo test as the LFO destinations below - a CC carrying exactly
  // what we last sent is our own output returning, not a person.
  for (uint8_t i = 0; i < 6; ++i) {
    const uint8_t pcc = mnmCC(perfSlot[i].page, perfSlot[i].dest);
    if (pcc == 0xFF || pcc != cc) continue;
    if (ch != (uint8_t)((txChannel - 1 + perfSlot[i].track) & 0x0F)) continue;
    matched = true;
    if (val == perfSent[i]) { ours = true; continue; }
    if (perfSlot[i].value != val || !perfSlot[i].known) {
      perfSlot[i].value = val;
      perfSlot[i].known = true; // the machine just told us what it holds
      perfSent[i] = val;        // do not echo it straight back at the machine
      perfCursor = i;           // and show which fader just moved
      uiTouch();                // PERF is a static page; it needs telling
    }
  }

  for (uint8_t i = 0; i < LFO_COUNT; ++i) {
    const LfoParams& p = lfo.p[i];
    const uint8_t ecc = mnmCC(p.page, p.dest);
    if (ecc == 0xFF) continue;
    const uint8_t ech = (uint8_t)((txChannel - 1 + (txPerTrack ? p.track : 0)) & 0x0F);
    if (ch != ech || cc != ecc) continue;
    matched = true;
    if (val == mnmOut.sentValue(i) || val == mnmOut.prevValue(i)) {
      ours = true; continue;
    }
    lfo.setCapture(i, val);
    rxCapture++;
  }

  // ---- joystick (v1.18) ----------------------------------------------------
  // The stick's own CCs coming back through a THRU or a merge. The two loops
  // above know nothing about them, so every echoed stick move counted as a
  // person at the machine and held the LFOs at the 15 Hz back-off for as long
  // as the stick moved - bug 5 of the previous revision, again, by a new
  // route. Matched on track and CC within 100 ms of the stick sending there,
  // not on value: the stick moves on before its echo gets back.
  const uint8_t jx = joyDestCC(g_joyDestX), jy = joyDestCC(g_joyDestY);
  if (cc == jx || cc == jy) {
    const uint32_t nowMs = millis();
    for (uint8_t t = 0; t < 6; ++t) {
      if (!(g_joyMask & (1u << t)) || !g_joyActMs[t]) continue;
      if (ch != (uint8_t)((txChannel - 1 + t) & 0x0F)) continue;
      if ((uint32_t)(nowMs - g_joyActMs[t]) < 100u) { matched = true; ours = true; }
    }
  }
  // A CC that matched one of our destinations and carried exactly the value we
  // last put there is our own echo. Anything else is a person - and whatever a
  // person set is what the kit model now believes (v1.22). An echo of an LFO's
  // swing must not land there: that is modulation, not the patch.
  if (!(matched && ours)) kitNoteCc(ch, cc, val);
  return !(matched && ours);
}

static void handleMidiByte(uint8_t b) {
  // micros() is read where it is needed, not at the top. This function runs on
  // every received byte - ~31000 a second at 10x turbo - and only the transport
  // branch and the CC backoff ever want a timestamp.
  rxBytes++;
  // usbReady(): the Teensy core spins in usb_serial_write for up to
  // TX_TIMEOUT_MSEC (120) when the host has stopped draining the port. This
  // function runs from inside the display flush, so a print here can stall the
  // whole loop for 120 ms - about 3750 bytes at 10x turbo, four times the
  // receive buffer. Nothing on this path writes unless there is room.
  if (rxDump) {
    if (usbReady(4)) { Serial.print(b < 16 ? " 0" : " "); Serial.print(b, HEX); }
    else rxDumpLost++;
  }
  // Feeds the turbo link watchdog. See TurboMidi::noteLiveByte for why this is
  // not simply "any status byte".
  // v1.21: during 'turbo loop' our own bytes are coming back; they are the
  // measurement, and none of them is MIDI meant for us.
  if (turbo.looping()) { turbo.loopByte(b); return; }
  turbo.noteAnyByte(b);
  if (b & 0x80) turboNoteLiveByte(b);
  // Realtime bytes may appear anywhere, including inside a SysEx, so they are
  // dispatched before the SysEx collector ever sees them.
  if (b >= 0xF8) {
    const uint32_t now = micros();
    if      (b == MIDI_CLOCK)  { clk.onClockTick(now); rxClock++; }
    // Transport events are ANNOUNCED from loop(), not printed here. Printing
    // from a byte handler that runs inside the display flush is exactly the
    // 120 ms stall described above, and it used to happen on the one byte you
    // least want to be late for.
    else if (b == MIDI_START)  { clk.onStart(now); lfo.onStart(); rxStart++;
                                 pendingEvt |= EVT_START; }
    else if (b == MIDI_CONTINUE) { clk.onContinue(now); lfo.onStart(); rxStart++;
                                 pendingEvt |= EVT_CONT; }
    else if (b == MIDI_STOP) {
      // Two STOPs inside 600 ms is the Monomachine's "double-tap stop" —
      // rewind to the start and hold there until play.
      static uint32_t lastStopMs = 0;
      const uint32_t ms = millis();
      clk.onStop(now); lfo.onStop(); patSilenceAll(); rxStop++;
      if (lastStopMs && (ms - lastStopMs) < 600) {
        clk.rewind();
        lfo.restoreAll();        // hand every destination back its patch value
        lfo.update(0, 0, false); // and show it immediately
        pendingEvt |= EVT_STOP2;
        lastStopMs = 0;
      } else {
        pendingEvt |= EVT_STOP;
        lastStopMs = ms;
      }
    }
    return;
  }
  if (sppState) {
    if (b & 0x80) {
      sppState = 0;                    // v1.15: a status byte ENDS a truncated
    } else {                           //   SPP and is then handled as itself
      if (sppState == 1) { sppLo = b; sppState = 2; }
      else { clk.onSongPosition((uint16_t)(sppLo | ((uint16_t)b << 7))); sppState = 0; }
      return;
    }
  }
  if (b == MIDI_SPP) { sppState = 1; rxSpp++; return; }

  // 'dumpraw' mode: capture one whole SysEx verbatim. The PRINT happens from
  // loop(): this function is called from inside the display flush, and dumping
  // two kilobytes of hex to USB serial from there stalled the panel write with
  // /CS still asserted.
  if (dumpRawOn) {
    if (b == 0xF0) { dumpIn = true; dumpLen = 0; dumpReady = false; }
    if (dumpIn) {
      if (dumpLen < sizeof(dumpBuf)) dumpBuf[dumpLen++] = b;
      dumpLastByteMs = millis();
      if (b == 0xF7 || dumpLen >= sizeof(dumpBuf)) { dumpIn = false; dumpReady = true; }
      // Deliberately NOT returning: the capture is a tap, not a diversion, so
      // a turbo handshake still negotiates while dumpraw is armed.
    }
  }

  // SysEx. Turbo messages go to the negotiator, everything else is counted and
  // discarded so its data bytes are never read as channel messages. Real-Time
  // was dealt with above and never reaches this parser.
  // v1.15: the MESSAGE is tracked separately from the 8-byte HEADER capture.
  // Both used to live under sxCapturing, which stops after 8 bytes - so a kit
  // dump's length froze at 9 and its closing F7 was never seen.
  static bool sxInMsg = false;
  if (b == 0xF0) { sxCapturing = 1; sxLastLen = 0; sxMsgBytes = 0; sxInMsg = true; }
  else if ((b & 0x80) && b != 0xF7) sxInMsg = false;      // aborted by a status byte
  if (sxInMsg) {
    sxMsgBytes++;
    // The first-run wizard advances when this flips (see loop()).
    if (b == 0xF7) { if (sxMsgBytes > 8) sxGotDump = true; sxInMsg = false; }
  }
  if (sxCapturing) {
    if (sxLastLen < sizeof(sxLastHdr)) sxLastHdr[sxLastLen++] = b;
    else sxCapturing = 0;
    if (b == 0xF7) sxCapturing = 0;
  }
  if (b == 0xF0 || sysex1.active()) {
    const uint8_t r = sysex1.feed(b, millis());
    if (r == SysexRx::TURBO_MSG) {
      rxSysex++;
      // Liveness first: onMessage() may switch the baud, which restarts the
      // count of what arrived at the new speed - this message came at the old.
      turbo.noteTurboSeen(millis());
      turbo.onMessage(sysex1.cmd(), sysex1.len(), sysex1.data(), millis());
      return;
    }
    if (r == SysexRx::FOREIGN_MSG) {
      rxSysex++;
      return;
    }
    if (r != SysexRx::ABORTED) return;
    // ABORTED: an unterminated SysEx was ended by a status byte. Terminating
    // the message does not consume that byte - it is still a status byte in
    // its own right and the MIDI spec says to act on it. The old code returned
    // here, so the first channel message after any truncated SysEx (a cable
    // knocked out mid-dump, a machine powered off mid-transfer) was silently
    // eaten, and a 0xF0 arriving inside another SysEx lost the new message.
    rxSysexAbort++;
    if (b == 0xF0) { sysex1.feed(b, millis()); return; }   // starts the next one
    // anything else: fall through and be parsed as the status byte it is
  }

  // Channel-voice parser, running status aware. We care about Control Change
  // so the XY6 can LEARN each destination's patch value from the Monomachine's
  // own CC out - the value it had "before" an LFO applies.
  if (b & 0x80) {                              // a status byte
    cvStatus = ((b & 0xF0) == 0xB0) ? b : 0;   // track CC, ignore notes etc.
    cvHave1  = false;
    return;
  }
  if (!cvStatus) return;                        // data with no CC status: skip
  if (!cvHave1) { cvData1 = b; cvHave1 = true; return; }
  const uint8_t ch = (uint8_t)(cvStatus & 0x0F);   // 0-based channel
  const uint8_t cc = cvData1, val = b;
  cvHave1 = false;                              // running status: reuse cvStatus
  if (captureCc(ch, cc, val)) mnmOut.noteUserActivity(micros());
}

// Drain both MIDI inputs. Called from the main loop and from inside flushAll(),
// so a frame push can never let the RX buffer overrun.
static void pumpMidi() {
  turbo.poll(millis());       // v1.20: a switch waiting on the UART, taken now
  // Sampled before draining, so it is the true high-water mark of how far
  // behind the loop ever fell. At 10x this is the headroom number that matters.
  const int a = Serial1.available();
  if (a > (int)rxPeak) rxPeak = (uint16_t)a;
  while (Serial1.available()) handleMidiByte((uint8_t)Serial1.read());
  while (Serial2.available()) {
    uint8_t b = (uint8_t)Serial2.read();
    rx2Bytes++;
    if (rx2Dump) {
      if (usbReady(4)) { Serial.print(b < 16 ? " 0" : " "); Serial.print(b, HEX); }
      else rxDumpLost++;
    }
  }
}

// ============================ serial console =================================
//
// When nothing moves on the Monomachine, you need to find WHICH link is broken,
// not guess again. Read the status block top to bottom and the broken link
// names itself.
//
//   rx bytes 0            the Monomachine's OUT is not reaching pin 0
//   rx bytes but no clock the cable is fine, the MnM is not sending clock
//   RUN never appears     clock arrives but no START — check the MnM's sync out
//   tx bytes climbing     we are transmitting; the address must be wrong -> hunt
//
// Type ? for the command list. Everything is live; nothing needs a reboot.

static char    cmdBuf[48];
static uint8_t cmdLen = 0;
static bool    monitorOn = false;

static bool isCmd(const char* line, const char* name) {
  uint8_t i = 0;
  while (name[i]) { if (line[i] != name[i]) return false; i++; }
  return line[i] == 0 || line[i] == ' ';
}

// Number after the first space. Returns -1 if there isn't one.
static long parseArg(const char* s, bool hex) {
  while (*s && *s != ' ') s++;
  while (*s == ' ') s++;
  if (!*s) return -1;
  long v = 0; bool any = false;
  while (*s) {
    char c = *s++; int d;
    if (c >= '0' && c <= '9') d = c - '0';
    else if (hex && c >= 'a' && c <= 'f') d = c - 'a' + 10;
    else if (hex && c >= 'A' && c <= 'F') d = c - 'A' + 10;
    else break;
    v = v * (hex ? 16 : 10) + d; any = true;
  }
  return any ? v : -1;
}

static uint8_t activeYY() {
  if (yyOverride >= 0) return (uint8_t)yyOverride;
  return (txMode == 3) ? mnmCC(lfo.p[0].page, lfo.p[0].dest)
                       : mnmParamIndex(lfo.p[0].page, lfo.p[0].dest);
}

static void printHelp() {
  Serial.println(F(
    "\n--- commands -------------------------------------------------\n"
    "  s              status snapshot\n"
    "  m              live monitor on/off\n"
    "  v <0-127>      send ONE fixed value now (LFO paused) - the key test\n"
    "  v off          hand control back to the LFO\n"
    "  ramp           slow 0->127->0 sweep once (non-blocking)\n"
    "  hn             HUNT: walk NRPN yy 0x00..0x1F, 2 s each\n"
    "  hc             HUNT: walk CC 0..127, 1.2 s each (forces mode 3)\n"
    "  hx             stop hunting\n"
    "  mode <0-3>     0 NRPN(trk) 1 NRPN(std) 2 NRPN(ch) 3 plain CC\n"
    "  ch <1-16>      MIDI channel\n"
    "  tr <1-6>       Monomachine track\n"
    "  yy <hex>       raw parameter index, e.g. yy 0A\n"
    "  cc <0-127>     force a raw CC number (clears with 'pg'/'ds')\n"
    "  pt 0 | 1       send on base channel, or base+track (default 1)\n"
    "  pg <0-9>       page   ds <0-7>  dest slot\n"
    "  w <0-11>  sp <0-127>  mu <0-6>  dp <0-127>  bs <0-127>  tg <0-4>\n"
    "                 w: 0 TRI 1 ITRI 2 SAW 3 ISAW 4 SQR 5 ISQR 6 EXP 7 IEXP\n"
    "                    8 RMP 9 IRMP 10 RND 11 SIN (XY6 extra)\n"
    "                 mu: 0 1X .. 6 64X   (the Monomachine's own order)\n"
    "                 tg: 0 FREE 1 TRIG 2 HOLD 3 ONE 4 HALF\n"
    "  lo <0-127>     MIN reachable value for this destination\n"
    "  hi <0-127>     MAX reachable value\n"
    "  cap            show captured patch values;  cap clear  to forget them\n"
    "  mach           machine per track + the list;  mach <t> <id|name>\n"
    "                 e.g. mach 1 dyn  /  mach 2 sid-6581  /  mach 3 0 (none)\n"
    "  kit [t]        what the XY6 believes track t holds, every page, as the\n"
    "                 machine shows it (* = learned, else init-kit default)\n"
    "  kit watch      log each value learned from the machine's CC out -\n"
    "                 turn SID WAVE TRI->NOISE: raw 0..4 = list mode is right\n"
    "  kit init [t]   forget what was learned: back to the init kit\n"
    "  e              LFO 1 on/off\n"
    "  turbo          TurboMIDI: handshake tested at 10x, run at 8x, FE\n"
    "                 keepalive every 150 ms while it is up\n"
    "  turbo off      back to 31250 baud, and stop answering the machine\n"
    "  turbo ?        link state\n"
    "  turbo loop     MIDI IN self-test, 1x..10x: patch OUT 1 to IN 1 first\n"
    "  turbo max <n>  fastest speed to use: 2 3 4 5 6 8 10 (default 10)\n"
    "  turbo v        toggle raw hex logging (ON at boot)\n"
    "  turbo sweep    walk the device-id byte 00..7F looking for ANY answer\n"
    "  turbo f <n>    FORCE 1, 8 or 10x with no handshake (diagnostic)\n"
    "                 NOTE: the machine's MIDI OUT must be wired back into\n"
    "                 XY6 pin 0, set to OUT not THRU, with TURBO enabled.\n"
    "  save [1-15]    write the current state to a preset slot (non-blocking)\n"
    "  load [1-15]    read a preset slot back\n"
    "  wipe           clear globals so the next boot runs the first-run wizard\n"
    "  wiz            open the first-run wizard now\n"
    "  page <1-4>     jump to a page (PERF / LFO / PAT / SET)\n"
    "  sub            toggle the current page's subpage (same as a 0.5 s hold)\n"
    "  joy            joystick routing. joy all | off | 1-6 (solo) | tog <n>\n"
    "                 joy x <i> | y <i> (destination 0-56) | sweep <s> (dry run)\n"
    "  invert 0 | 1   reverse black and white across the whole OS\n"
    "  style 0 | 1    chip style: 1 inverted blocks, 0 strokes on black\n"
    "  boot           replay the startup logo animation\n"
    "  clk i | clk e  internal clock (no MnM needed) or external\n"
    "  bpm <n>        internal tempo\n"
    "  disp           panel ruler - find how much of the screen actually lights\n"
    "  rows           COM-line band test - find WHICH rows are dark\n"
    "  uiw <8-64>     usable panel width; pages relay out live\n"
    "  mux <15-63>    multiplex ratio (63 = all 64 rows)   <- try this first\n"
    "  stl <0-63>     display start line     offs <0-63>  display offset\n"
    "  rmap <hex>     re-map byte (default 41; try 01 if half the panel is dark)\n"
    "  bright <0-255> contrast\n"
    "  wr <8-200>     /WR pulse width in nops - raise it if the screen speckles\n"
    "  fmode 0 | 1    frame send: 1 = stress-test stream, 0 = v1.08 chunks\n"
    "  redraw         force one full-frame refresh\n"
    "  heal <ms>      how often the panel is repainted in full whether or not\n"
    "                 anything changed, so a byte that landed wrong cannot stay\n"
    "                 on screen. 0 disables. Default 1000.\n"
    "  led <0-4>|all|off  drive one LED directly (4 = tempo)\n"
    "  dumpraw        capture one Monomachine SysEx dump as clean hex\n"
    "  dumpshow       reprint the last captured dump (to copy it again)\n"
    "  rx             raw hex dump of MIDI IN 1\n"
    "  p2             raw hex dump of MIDI IN 2\n"
    "  loop           send a test pattern on OUT 1 (patch OUT1 -> IN2)\n"
    "--------------------------------------------------------------"));
}

static void printStatus() {
  const LfoParams& p = lfo.p[0];
  const uint32_t bpmX100 = (uint32_t)(clk.bpm() * 100.0f);
  const uint64_t pos = clk.position();
  const uint32_t beats = (uint32_t)(pos >> 32);
  const uint32_t frac  = (uint32_t)(((pos & 0xFFFFFFFFull) * 1000ull) >> 32);

  usbWait(128);
  Serial.println(F("\n=== XY6 STATUS ==============================================="));
  Serial.printf("CLOCK   src %s  %s  bpm %lu.%02lu  lock %s  ticks %lu  pos %lu.%03lu\n",
      clk.internal() ? "INT" : "EXT", clk.running() ? "RUN " : "STOP",
      (unsigned long)(bpmX100 / 100), (unsigned long)(bpmX100 % 100),
      clk.locked() ? "yes" : "no ", (unsigned long)clk.ticks(),
      (unsigned long)beats, (unsigned long)frac);
  Serial.printf("RX  1   bytes %lu  clock %lu  start %lu  stop %lu  spp %lu  sysex %lu\n",
      (unsigned long)rxBytes, (unsigned long)rxClock, (unsigned long)rxStart,
      (unsigned long)rxStop, (unsigned long)rxSpp, (unsigned long)rxSysex);
  Serial.printf("RX  2   bytes %lu\n", (unsigned long)rx2Bytes);
  Serial.printf("TURBO   %s at %s  (%lu baud)  keepalive %s  %lu FE sent\n",
      turbo.stateName(), turbo.speedName(), (unsigned long)turbo.baud(),
      g_tmKeepalive ? "ON" : "off", (unsigned long)g_tmFeSent);
  // Peak RX fill against buffer size is the headroom figure. If peak ever
  // approaches capacity the loop is falling behind and bytes are at risk;
  // in normal running at 10x it should stay in single or low double digits.
  {
    const uint32_t bps = turbo.baud() / 10u;
    Serial.printf("RXBUF   peak %u of %u bytes (%u%%)  = %lu ms of slack at %s"
                  "   sysex aborts %lu  log drops %lu\n",
        (unsigned)rxPeak, (unsigned)rxCapacity,
        rxCapacity ? (unsigned)((uint32_t)rxPeak * 100u / rxCapacity) : 0u,
        bps ? (unsigned long)((uint32_t)rxCapacity * 1000u / bps) : 0ul,
        turbo.speedName(), (unsigned long)rxSysexAbort,
        (unsigned long)rxDumpLost);
  }
  usbWait(128);
  if (g_joyUiX != 0xFF)
    Serial.printf("JOY     X %3u  Y %3u   centre %u / %u   travel X %d..%d  Y %d..%d"
                  "   sent %lu CC (%u/%u) to mask %02X\n",
        g_joyUiX, g_joyUiY, g_joyCx, g_joyCy, g_joyRng[0], g_joyRng[1],
        g_joyRng[2], g_joyRng[3], (unsigned long)g_joySent,
        (unsigned)joyDestCC(g_joyDestX), (unsigned)joyDestCC(g_joyDestY),
        (unsigned)g_joyMask);
  Serial.printf("PANEL   UI_W %u  /WR %u nops  last frame %u of 64 rows\n",
      (unsigned)UI_W, (unsigned)WR_NOPS, (unsigned)g_lastRows);
  Serial.printf("        self-heal %lu ms, %lu full repaints so far%s\n",
      (unsigned long)dispHealMs, (unsigned long)g_healCount,
      dispHealMs ? "" : "   [DISABLED - glitches will persist]");
  Serial.printf("TX      ch %u (base %u%s)  mode %u %s\n",
      (unsigned)(((txChannel - 1 + (txPerTrack ? p.track : 0)) & 0x0F) + 1), txChannel,
      txPerTrack ? " + track" : "", txMode, kTxModeName[txMode & 3]);
  Serial.printf("        %lu B/s on the wire  |  queues: note %u pend %lu drop,"
                "  ctrl %u pend %lu drop\n",
      (unsigned long)txRate, (unsigned)qNote.pending(),
      (unsigned long)qNote.drops(), (unsigned)qCtrl.pending(),
      (unsigned long)qCtrl.drops());
  Serial.printf("        baud %lu (%lu B/s)  total sent %lu  notes %lu\n",
      (unsigned long)turbo.baud(), (unsigned long)(turbo.baud() / 10u),
      (unsigned long)txBytesOut, (unsigned long)patNotesSent);
  usbWait(128);
  Serial.printf("TARGET  track %u  page %s  slot %u %s  %s %u%s\n",
      p.track + 1, kMnmPages[p.page].name, p.dest,
      mnmParamName(p.page, p.dest), txMode == 3 ? "CC" : "yy", activeYY(),
      yyOverride >= 0 ? "  (raw override)" : "");
  Serial.printf("LFO 1   %s  %s  wave %s  trig %s  spd %u  mult %s  intl %u  dpth %u\n",
      p.enabled ? "ON " : "OFF", lfo.driving(0) ? "driving" : "idle",
      kWaveNames[p.wave], kTrigNames[p.trig],
      p.spd, kMultNames[p.mult], p.intl, p.depth);
  Serial.printf("RANGE   MIN %u  MAX %u  base %u  captured %s%u  %s\n",
      p.lo, p.hi, p.baseValue, lfo.s[0].hasCap ? "" : "(none) ",
      lfo.s[0].capture, lfo.restoring() ? "[RESTORING patch, waiting for play]" : "");
  Serial.printf("        phase %u%%  value %u%s\n",
      (unsigned)((uint64_t)lfo.s[0].phase * 100ull >> 32), lfo.s[0].value,
      manualVal >= 0 ? "   [MANUAL OVERRIDE ACTIVE - type 'v off']" : "");
  Serial.print(F("LAST TX "));
  if (!mnmOut.lastLen()) Serial.print(F("(nothing sent yet)"));
  for (uint8_t i = 0; i < mnmOut.lastLen(); ++i) {
    uint8_t b = mnmOut.lastByte(i);
    Serial.print(b < 16 ? " 0" : " "); Serial.print(b, HEX);
  }
  Serial.println();
  if (rxBytes == 0)
    Serial.println(F("HINT    rx is 0 - MIDI IN 1 is not receiving. Check the MnM's\n"
                     "        MIDI OUT into pin 0, and that it is sending clock."));
  else if (rxClock && !clk.running())
    Serial.println(F("HINT    clock is arriving but no START. Press play on the MnM,\n"
                     "        or use 'clk i' to run the LFO on the internal clock."));
  else if (txBytesOut > 200)
    Serial.println(F("HINT    we are transmitting but nothing moves? The address is\n"
                     "        wrong. Try 'hn' to hunt the NRPN index, or 'mode 1'\n"
                     "        / 'mode 2', or 'hc' to hunt plain CCs."));
  Serial.printf("LOOP    worst pass %lu us  avg %lu us  frames yielded to MIDI %lu\n",
      (unsigned long)gStats.passUsMax, (unsigned long)gStats.passUsAvg,
      (unsigned long)gStats.frameSkipsBacklog);
  Serial.printf("STORE   eeprom %s  state %u  slot %u/%u  %s\n",
      HalEeprom::present() ? "present" : "ABSENT", HalEeprom::state(),
      presetSlot, EE_PRESET_SLOTS, storeMsg[0] ? storeMsg : "(idle)");
  Serial.printf("        preset record %u bytes of %u per slot  sys state %u\n",
      (unsigned)sizeof(PresetP), (unsigned)EE_PRESET_STRIDE, sysState);
  if (!HalEeprom::present())
    Serial.println(F("HINT    no EEPROM at 0x50 - presets are disabled. Check SDA 18\n"
                     "        and SCL 19, and that the 24LC512 has power."));
  if (qNote.drops())
    Serial.println(F("NOTE    the note queue has dropped messages - the wire is\n"
                     "        oversubscribed. Mute a pattern track, or lower the\n"
                     "        LFO count, or accept coarser CC."));
  Serial.println(F("=============================================================="));
}

// ---- v1.22: machines and the kit model on the console ----------------------
// Machine by number (0..22) or by text: the label ("FM+DYN", "sid-6581"), the
// short name ("dyn", "6581") - case does not matter. 0xFF = no match.
static uint8_t machineByText(const char* s) {
  if (!*s) return 0xFF;
  if (*s >= '0' && *s <= '9') {
    const long n = atol(s);
    return (n >= 0 && n < MACHINE_COUNT) ? (uint8_t)n : (uint8_t)0xFF;
  }
  char u[12]; uint8_t k = 0;
  while (s[k] && s[k] != ' ' && k < sizeof(u) - 1) {
    u[k] = (char)((s[k] >= 'a' && s[k] <= 'z') ? s[k] - 32 : s[k]); k++;
  }
  u[k] = 0;
  if (!strcmp(u, "NONE") || !strcmp(u, "----")) return 0;
  for (uint8_t id = 1; id < MACHINE_COUNT; ++id) {
    const MnmMachine* m = mnmMachine(id);
    if (!strcmp(u, m->label) || !strcmp(u, m->name)) return id;
  }
  return 0xFF;
}
// The n-th space-separated word of a command line (0 = the command itself).
static const char* cmdWord(const char* c, uint8_t n) {
  while (n--) {
    while (*c && *c != ' ') c++;
    while (*c == ' ') c++;
  }
  return c;
}
static void printMachines() {
  usbWait(128);
  Serial.print(F("machines:"));
  for (uint8_t t = 0; t < 6; ++t)
    Serial.printf("  T%u %s", t + 1, mnmMachineLabel(machineSel[t]));
  Serial.println();
  usbWait(256);
  Serial.print(F("ids: 0 none"));
  for (uint8_t id = 1; id < MACHINE_COUNT; ++id) {
    if (id % 8 == 0) { Serial.println(); usbWait(128); Serial.print(F("    ")); }
    Serial.printf("  %u %s", id, mnmMachineLabel(id));
  }
  Serial.println();
}
static void printKit(uint8_t t) {
  if (t >= 6) return;
  usbWait(128);
  Serial.printf("T%u  %s   (* = learned from the machine or sent by us,"
                " else the init-kit default)\n", t + 1, mnmMachineLabel(machineSel[t]));
  for (uint8_t pg = 0; pg < KIT_PAGES; ++pg) {
    usbWait(160);
    Serial.printf("  %-4s", kMnmPages[pg].name);
    for (uint8_t sl = 0; sl < 8; ++sl) {
      char v[8];
      paramFormat(pg, sl, t, kitGet(t, pg, sl), v);
      Serial.printf(" %4s %-5s%s", paramName(pg, sl, t), v,
                    kitLearned(t, pg, sl) ? "*" : " ");
    }
    Serial.println();
  }
}

static void handleCommand(const char* c) {
  long a;
  // Pattern-generator commands get first look. They own their own vocabulary
  // (`gen ...`, `pat ...`), and returning true short-circuits the LFO dispatch
  // so a typo in a `gen` command cannot accidentally hit an LFO knob.
  if (patHandleCommand(c)) return;

  if (isCmd(c, "?") || isCmd(c, "h") || isCmd(c, "help")) {
    printHelp(); patPrintHelp(); return; }
  if (isCmd(c, "s")) { printStatus(); return; }
  if (isCmd(c, "m")) { monitorOn = !monitorOn;
                       Serial.printf("monitor %s\n", monitorOn ? "on" : "off"); return; }
  if (isCmd(c, "e")) { lfo.p[0].enabled = !lfo.p[0].enabled;
                       Serial.printf("LFO 1 %s\n", lfo.p[0].enabled ? "on" : "off");
                       return; }
  if (isCmd(c, "v")) {
    const char* arg = c; while (*arg && *arg != ' ') arg++; while (*arg == ' ') arg++;
    if (arg[0] == 'o') { manualVal = -1; rampMode = 0;
                         Serial.println("manual off, LFO driving"); }
    else { a = parseArg(c, false);
           if (a >= 0 && a <= 127) { manualVal = (int16_t)a; rampMode = 0;
             Serial.printf("sending fixed value %ld to %s %u on ch %u\n",
                           a, txMode == 3 ? "CC" : "yy", activeYY(), txChannel); } }
    return;
  }
  if (isCmd(c, "ramp")) {
    manualVal = 0; rampMode = 1; rampNextMs = millis();
    Serial.println("ramping 0 -> 127 -> 0 (runs in the background; 'v off' aborts)");
    return; }
  if (isCmd(c, "hn")) { huntMode = 1; huntPos = 0; huntNextMs = millis();
    if (txMode == 3) txMode = 0;
    Serial.println(F("HUNT NRPN: walking yy 0x00..0x1F, 2 s each.\n"
                     "Watch the Monomachine - whichever value moves is your target.\n"
                     "This writes to 32 parameters, so use a scratch pattern. 'hx' stops."));
    return; }
  if (isCmd(c, "hc")) { huntMode = 2; huntPos = 0; huntNextMs = millis(); txMode = 3;
    Serial.println(F("HUNT CC: walking CC 0..127, 1.2 s each, mode forced to CC.\n"
                     "Watch the Monomachine. 'hx' stops."));
    return; }
  if (isCmd(c, "hx")) { huntMode = 0; Serial.println("hunt stopped"); return; }
  if (isCmd(c, "save")) { a = parseArg(c, false);
    if (a >= 1 && a <= EE_PRESET_SLOTS) presetSlot = (uint8_t)a;
    if (Store::saveTo((uint8_t)(presetSlot - 1)))
      Serial.printf("saving slot %u (%u bytes, non-blocking)\n",
                    presetSlot, (unsigned)sizeof(PresetP));
    else Serial.printf("not saved: %s\n", Store::busy() ? "store busy - try again" : storeMsg);
    return; }
  if (isCmd(c, "load")) { a = parseArg(c, false);
    if (a >= 1 && a <= EE_PRESET_SLOTS) presetSlot = (uint8_t)a;
    if (Store::loadFrom((uint8_t)(presetSlot - 1)))
      Serial.printf("loading slot %u\n", presetSlot);
    else Serial.printf("not loaded: %s\n", Store::busy() ? "store busy - try again" : storeMsg);
    return; }
  if (isCmd(c, "wipe")) {
    // Clears the globals only, so the next boot runs the first-run wizard. The
    // preset slots themselves are left alone - "show me the wizard again" and
    // "destroy my presets" are very different requests.
    // v1.18: never mid-operation. The EEPROM driver refuses a second write
    // while one is running, so 'wipe' during a save printed "cleared", wrote
    // nothing, and threw away the save's own globals update.
    if (Store::busy() || !HalEeprom::present()) {
      Serial.println(Store::busy() ? "store busy - try again" : "no EEPROM");
      return; }
    memset(&gGlobal, 0, sizeof(gGlobal));
    Store::writeGlobals();
    Serial.println(F("globals cleared - next boot runs the first-run wizard.\n"
                     "Preset slots are untouched."));
    return; }
  if (isCmd(c, "invert")) {
    const long a2 = parseArg(c, false);
    if (a2 == 0 || a2 == 1) uiSetInvert(a2 != 0);
    Serial.printf("invert %s - %s\n", uiInvert ? "ON" : "OFF",
                  uiInvert ? "black ink on a lit field, the whole OS"
                           : "lit ink on black, the original");
    Serial.printf("       contrast now 0x%02X (uiContrast 0x%02X, field 0x%02X)\n",
                  (unsigned)(uiInvert ? OLED_CONTRAST_FIELD : uiContrast),
                  (unsigned)uiContrast, (unsigned)OLED_CONTRAST_FIELD);
    Serial.println(F("       Reversed video lights 70-85% of the panel where the\n"
                     "       original lights 15-30%, so it runs at the lower field\n"
                     "       contrast to keep the 3V3 rail near where it was. If it\n"
                     "       reads too dim on the bench, raise it with 'bright' - and\n"
                     "       if the panel then flickers or bands, the rail is the\n"
                     "       reason and the contrast is the thing to give back.\n"
                     "       'save' keeps this setting across a power cycle."));
    return; }
  if (isCmd(c, "style")) {
    const long a2 = parseArg(c, false);
    if (a2 == 0 || a2 == 1) { uiChipStyle = (a2 != 0); uiTouch(); g_shadowValid = false; }
    Serial.printf("style %s - %s\n", uiChipStyle ? "CHIP" : "LINE",
                  uiChipStyle ? "inverted blocks, as drawn in the mockup"
                              : "same layout, bright strokes on black");
    Serial.println(F("       Both are the same geometry; only the fill inverts.\n"
                     "       Judge it on the panel with a room light on - a\n"
                     "       passive-matrix OLED blooms into knocked-out text."));
    return; }
  if (isCmd(c, "wiz")) { bootActive = false; wizState = WIZ_SPLASH;
    sysState = SYS_WIZARD; uiTouch();
    Serial.println("wizard opened"); return; }
  if (isCmd(c, "mode")) { a = parseArg(c, false);
    if (a >= 0 && a <= 3) { txMode = (uint8_t)a; mnmOut.resend();
      Serial.printf("mode %ld  %s\n", a, kTxModeName[a]); } return; }
  if (isCmd(c, "ch")) { a = parseArg(c, false);
    if (a >= 1 && a <= 16) { setBaseChannel((uint8_t)a);
      Serial.printf("MIDI channel %ld\n", a); } return; }
  if (isCmd(c, "tr")) { a = parseArg(c, false);
    if (a >= 1 && a <= 6) { lfo.p[0].track = (uint8_t)(a - 1); lfoRetarget(0);
      mnmOut.resend(); Serial.printf("track %ld\n", a); } return; }
  if (isCmd(c, "yy")) { a = parseArg(c, true);
    if (a >= 0 && a <= 127) { yyOverride = (int16_t)a; mnmOut.resend();
      Serial.printf("raw yy 0x%02lX\n", a); } return; }
  if (isCmd(c, "cc")) { a = parseArg(c, false);
    if (a >= 0 && a <= 127) { txCC = (uint8_t)a; yyOverride = (int16_t)a;
      mnmOut.resend(); Serial.printf("raw CC %ld\n", a); } return; }
  if (isCmd(c, "pt")) { a = parseArg(c, false);
    txPerTrack = (a != 0); mnmOut.resend();
    Serial.printf("channel = base%s\n", txPerTrack ? " + track" : " only");
    return; }
  if (isCmd(c, "pg")) { a = parseArg(c, false);
    if (a >= 0 && a < PAGE_COUNT) { lfo.p[0].page = (uint8_t)a; yyOverride = -1;
      if (lfo.p[0].dest >= kMnmPages[a].count) lfo.p[0].dest = 0;
      lfoRetarget(0);
      mnmOut.resend(); Serial.printf("page %s\n", kMnmPages[a].name); } return; }
  if (isCmd(c, "ds")) { a = parseArg(c, false);
    if (a >= 0 && a < kMnmPages[lfo.p[0].page].count) { lfo.p[0].dest = (uint8_t)a;
      yyOverride = -1; lfoRetarget(0); mnmOut.resend();
      Serial.printf("dest %s  addr %u\n", mnmParamName(lfo.p[0].page, a),
                    activeYY()); } return; }
  if (isCmd(c, "w"))  { a = parseArg(c, false);
    if (a >= 0 && a < WAVE_COUNT) { lfo.p[0].wave = (uint8_t)a;
      Serial.printf("wave %s\n", kWaveNames[a]); } return; }
  if (isCmd(c, "sp")) { a = parseArg(c, false);
    if (a >= 0 && a <= 127) { lfo.p[0].spd = (uint8_t)a;
      Serial.printf("spd %ld\n", a); } return; }
  if (isCmd(c, "mu")) { a = parseArg(c, false);
    if (a >= 0 && a <= LFO_MULT_MAX) { lfo.p[0].mult = (uint8_t)a;
      Serial.printf("mult %s\n", kMultNames[a]); } return; }
  if (isCmd(c, "dp")) { a = parseArg(c, false);
    if (a >= 0 && a <= 127) { lfo.p[0].depth = (uint8_t)a;
      Serial.printf("depth %ld\n", a); } return; }
  if (isCmd(c, "bs")) { a = parseArg(c, false);
    if (a >= 0 && a <= 127) { lfo.p[0].baseValue = (uint8_t)a;
      Serial.printf("base %ld\n", a); } return; }
  if (isCmd(c, "lo")) { a = parseArg(c, false);
    if (a >= 0 && a <= 127) { lfo.p[0].lo = (uint8_t)a;
      Serial.printf("MIN %ld  (window %u..%u)\n", a, lfo.p[0].lo, lfo.p[0].hi); }
    return; }
  if (isCmd(c, "hi")) { a = parseArg(c, false);
    if (a >= 0 && a <= 127) { lfo.p[0].hi = (uint8_t)a;
      Serial.printf("MAX %ld  (window %u..%u)\n", a, lfo.p[0].lo, lfo.p[0].hi); }
    return; }
  if (isCmd(c, "cap")) {
    const char* arg = c; while (*arg && *arg != ' ') arg++; while (*arg == ' ') arg++;
    if (arg[0] == 'c') {
      for (uint8_t i = 0; i < LFO_COUNT; ++i) { lfo.s[i].hasCap = false;
                                                lfo.s[i].capture = 64; }
      Serial.println("captures cleared"); return; }
    Serial.println(F("captured patch values (from the Monomachine's CC out):"));
    for (uint8_t i = 0; i < LFO_COUNT; ++i)
      Serial.printf("  LFO %u  %-4s CC %-3u  window %u..%u  captured %s%u\n",
          i + 1, mnmParamName(lfo.p[i].page, lfo.p[i].dest),
          mnmCC(lfo.p[i].page, lfo.p[i].dest), lfo.p[i].lo, lfo.p[i].hi,
          lfo.s[i].hasCap ? "" : "(none) ", lfo.s[i].capture);
    Serial.printf("captures seen so far: %lu\n", (unsigned long)rxCapture);
    return; }
  if (isCmd(c, "mach")) {
    const char* w1 = cmdWord(c, 1);
    const char* w2 = cmdWord(c, 2);
    if (*w1 && *w2) {
      const long t = atol(w1);
      const uint8_t id = machineByText(w2);
      if (t < 1 || t > 6)  { Serial.println("mach <1-6> <id|name>"); return; }
      if (id == 0xFF)      { Serial.printf("no machine '%s' - type mach for the list\n", w2); return; }
      applyMachine((uint8_t)(t - 1), id);
      Serial.printf("T%ld = %s  (its SYNT page is back at init values)\n", t,
                    mnmMachineLabel(id));
      return;
    }
    printMachines();
    return; }
  if (isCmd(c, "kit")) {
    const char* w1 = cmdWord(c, 1);
    if (!strncmp(w1, "watch", 5)) {
      kitWatch = !kitWatch;
      Serial.printf("kit watch %s\n", kitWatch ? "ON - turn knobs on the Monomachine"
                                               : "off");
      return; }
    if (!strncmp(w1, "init", 4)) {
      const long t = atol(cmdWord(c, 2));
      for (uint8_t k = 0; k < 6; ++k)
        if (t < 1 || t > 6 || k == t - 1) kitResetPages(k, 0, (uint8_t)(KIT_PAGES - 1));
      for (uint8_t i = 0; i < 6; ++i) perfSeed(i);
      Serial.println(t >= 1 && t <= 6 ? "track back at the init kit"
                                      : "all tracks back at the init kit");
      uiTouch();
      return; }
    const long t = atol(w1);
    if (t >= 1 && t <= 6) printKit((uint8_t)(t - 1));
    else                  printKit(lfo.p[0].track < 6 ? lfo.p[0].track : 0);
    return; }
  if (isCmd(c, "tg")) { a = parseArg(c, false);
    if (a >= 0 && a < TRIG_COUNT) { lfo.p[0].trig = (uint8_t)a;
      Serial.printf("trig %s\n", kTrigNames[a]); } return; }
  if (isCmd(c, "font")) {
    // v1.24: retired. The Monomachine grid picks the face by role - 5x7 for
    // values, 3x5 for labels (stText) - so no page reads uiFont any more. The
    // byte stays in the globals so their layout does not move.
    Serial.println(F("font: retired in v1.24 - values are 5x7 and labels 3x5 on every\n"
                     "      page, with 3x5 only where a value will not fit its cell."));
    return; }
  if (isCmd(c, "page")) {
    const long a2 = parseArg(c, false);
    if (a2 >= 1 && a2 <= UPAGE_COUNT) {
      ui.page = (uint8_t)(a2 - 1); ui.inSub = false; uiTouch(); }
    const UiPageDef& pg = kUiPages[ui.page];
    Serial.printf("page %u/%u  %s%s%s\n", ui.page + 1, UPAGE_COUNT, pg.name,
                  ui.inSub ? " > " : "", ui.inSub ? uiSubName() : "");
    if (pg.subDraw) Serial.printf("        subpage '%s' - 'sub' here, or hold the"
                                  " board button 0.5 s\n", pg.subName);
    else            Serial.println(F("        this page has no subpage"));
    return; }
  if (isCmd(c, "sub")) {
    if (!uiHasSub()) { Serial.printf("page %s has no subpage\n",
                                     kUiPages[ui.page].name); return; }
    uiToggleSub();
    Serial.printf("%s %s\n", kUiPages[ui.page].name,
                  ui.inSub ? uiSubName() : "(top level)");
    return; }
  if (isCmd(c, "boot")) { bootMs = 0; bootActive = true;
    Serial.println("replaying the boot animation"); return; }
  if (isCmd(c, "turbo")) {
    // v1.19: the initiator. 'turbo' runs the handshake (tests at SPEED1, runs
    // at SPEED2); everything else here is status and diagnostics.
    const char* arg = c; while (*arg && *arg != ' ') arg++; while (*arg == ' ') arg++;
    const long n = parseArg(c, false);
    if (arg[0] == 0 || !strncmp(arg, "on", 2) ||
        n == (long)(kTmSpeeds[TURBO_SPEED1_CODE] / 31250u) ||
        n == (long)(kTmSpeeds[TURBO_SPEED2_CODE] / 31250u)) {
      turbo.start(millis());
      return;
    }
    if (!strncmp(arg, "off", 3) || n == 1) { turbo.stop(millis()); return; }
    if (!strncmp(arg, "loop", 4)) { turbo.startLoop(millis()); return; }     // turbo loop
    if (!strncmp(arg, "max", 3)) {                                          // turbo max <n>
      const long m = parseArg(arg, false);
      uint8_t code = 0;
      for (uint8_t k = 2; k <= TURBO_SPEED1_CODE; ++k)
        if ((long)(kTmSpeeds[k] / 31250u) == m) code = k;
      if (m >= 0 && !code) {
        Serial.println(F("turbo max takes 2 3 4 5 6 8 10 (the multiplier)"));
        return;
      }
      if (code) turbo.setMax(code);
      Serial.printf("turbo: ceiling %s - handshakes test at %s and run at %s%s\n",
                    kTmNames[turbo.maxCode()], kTmNames[turbo.speed1Code()],
                    kTmNames[turbo.speed2Code()], code ? "; 'save' keeps it" : "");
      return;
    }
    if (!strncmp(arg, "sw", 2)) { turbo.startSweep(millis()); return; }      // turbo sweep
    if (arg[0] == 'v') {                       // turbo v = toggle hex logging
      turboVerbose = !turboVerbose;
      Serial.printf("turbo hex logging %s\n", turboVerbose ? "on" : "off");
      return;
    }
    if (arg[0] == 'f') {                       // turbo f <n> = force, no handshake
      const long fm = parseArg(arg, false);
      const uint8_t fi = (fm == 1) ? 1 : (fm == 8) ? 7 : (fm == 10) ? 8 : 0;
      if (!fi) Serial.println(F("turbo f <1|8|10>  - switch with NO handshake (diagnostic)"));
      else     turbo.forceSpeed(fi, millis());
      return;
    }
    if (arg[0] == '?') {
      Serial.printf("turbo: %s at %s (%lu baud)\n", turbo.stateName(),
                    turbo.speedName(), (unsigned long)turbo.baud());
      Serial.printf("       ceiling %s: SPEED1 code %u = %s (test)   SPEED2 code %u = %s (run)\n",
                    kTmNames[turbo.maxCode()],
                    (unsigned)turbo.speed1Code(), kTmNames[turbo.speed1Code()],
                    (unsigned)turbo.speed2Code(), kTmNames[turbo.speed2Code()]);
      Serial.printf("       answers the machine's own handshake: %s\n",
                    turbo.answersMachine() ? "yes" : "no ('turbo' to allow)");
      Serial.printf("       keepalive %s, %lu FE sent; machine's own keepalive %s\n",
                    g_tmKeepalive ? "ON" : "off", (unsigned long)g_tmFeSent,
                    turbo.peerKeepalive() ? "seen" : "not seen");
      Serial.printf("       %lu locks, %lu reverts since boot\n",
                    (unsigned long)turbo.locks(), (unsigned long)turbo.reverts());
      Serial.printf("       peer device id %02X %02X%s\n",
                    sysex1.peerId(0), sysex1.peerId(1),
                    sysex1.peerKnown() ? " (learned from the machine)"
                                       : " (default - nothing heard yet)");
      Serial.printf("       log lines dropped %lu\n", (unsigned long)tmLogDrops);
      return;
    }
    Serial.println(F("turbo | turbo off | turbo ? | turbo loop | turbo max <n> | turbo v |\n"
                     "turbo sweep | turbo f <1|8|10>"));
    return;
  }
  if (isCmd(c, "clk")) {
    const char* arg = c; while (*arg && *arg != ' ') arg++; while (*arg == ' ') arg++;
    bool internal = (arg[0] == 'i');
    clk.setInternal(internal, micros());
    if (internal) lfo.onStart();
    Serial.printf("clock %s\n", internal ? "INTERNAL (running now)" : "EXTERNAL");
    return; }
  if (isCmd(c, "bpm")) { a = parseArg(c, false);
    if (a >= 20 && a <= 300) { clk.setBpm((float)a);
      Serial.printf("bpm %ld\n", a); } return; }
  if (isCmd(c, "disp")) { rulerMode = !rulerMode; g_shadowValid = false;
    Serial.println(F("\nPANEL RULER on. Read the LARGEST tick number and the\n"
                     "longest bar you can actually see, then type  uiw <that number>.\n"
                     "  ruler reaches 56/63  -> the panel is fine, the old page was\n"
                     "                          just drawing past the lit area\n"
                     "  ruler stops early    -> work through mux / stl / offs / rmap\n"
                     "                          BEFORE accepting a smaller uiw\n"
                     "'disp' again returns to the LFO page."));
    return; }
  if (isCmd(c, "uiw")) { a = parseArg(c, false);
    if (a >= 8 && a <= 64) { UI_W = (int16_t)a; g_shadowValid = false;
      Serial.printf("UI_W %ld - pages relaid out\n", a); }
    else Serial.println("uiw takes 8..64");
    return; }

  // ---- live display-init tuning ------------------------------------------
  if (isCmd(c, "mux"))  { a = parseArg(c, false);
    if (a >= 15 && a <= 63) { dispCmd2(0xA8, (uint8_t)a); g_shadowValid = false;
      Serial.printf("multiplex ratio %ld (%ld rows)\n", a, a + 1); }
    else Serial.println("mux takes 15..63 (rows-1; 63 = all 64)");
    return; }
  if (isCmd(c, "stl"))  { a = parseArg(c, false);
    if (a >= 0 && a <= 63) { dispCmd2(0xA1, (uint8_t)a); g_shadowValid = false;
      Serial.printf("display start line %ld\n", a); } return; }
  if (isCmd(c, "offs")) { a = parseArg(c, false);
    if (a >= 0 && a <= 63) { dispCmd2(0xA2, (uint8_t)a); g_shadowValid = false;
      Serial.printf("display offset %ld\n", a); } return; }
  if (isCmd(c, "rmap")) { a = parseArg(c, true);
    if (a >= 0 && a <= 255) { dispRemap = (uint8_t)a; dispCmd2(0xA0, (uint8_t)a); g_shadowValid = false;
      Serial.printf("re-map 0x%02lX  (bit0 col, bit1 nibble, bit2 vert inc,"
                    " bit4 COM remap, bit6 COM split)\n", a); } return; }
  if (isCmd(c, "bright")) { a = parseArg(c, false);
    if (a >= 0 && a <= 255) { setContrast((uint8_t)a);
      Serial.printf("contrast 0x%02lX\n", a); } return; }
  if (isCmd(c, "rows")) { rowTestMode = !rowTestMode; g_shadowValid = false;
    Serial.println(F(
      "\nROW TEST on: eight bands across the full 64 logical columns, each\n"
      "labelled with its first row number. Count the bands you can see - that\n"
      "is exactly which COM lines are dark, which is what mux/stl/offs act on.\n"
      "\n"
      "IF ABOUT HALF THE BANDS ARE MISSING, TRY THIS FIRST:   rmap 01\n"
      "\n"
      "The re-map byte is 0x41 today, and bit 6 of it enables COM split\n"
      "odd/even. A panel wired for sequential COM but told to split drives\n"
      "only half its rows. 0x01 keeps the column remap this panel needs\n"
      "(bit 1 stays clear - see dispRemap) and clears the split. If that\n"
      "fixes it, set dispRemap and the 0xA0 line of INIT_SEQ to 0x01 and\n"
      "raise UI_W to 64. If not: mux 63 -> stl 0 -> offs 0.\n"
      "'rows' again returns to the LFO page."));
    return; }
  if (isCmd(c, "led")) {
    const char* arg = c; while (*arg && *arg != ' ') arg++; while (*arg == ' ') arg++;
    if (arg[0] == 'a') { ledSet(0x1F); Serial.println("all 5 LEDs on"); }
    else if (arg[0] == 'o') { ledSet(0x00); Serial.println("LEDs off"); }
    else { a = parseArg(c, false);
      if (a >= 0 && a <= 4) { ledSet((uint8_t)(1u << a));
        Serial.printf("only GPB%ld on (that is D%s)\n", a,
                      a == 4 ? "1 - the tempo LED" : "2..D5, a page LED"); } }
    return; }
  if (isCmd(c, "wr")) { a = parseArg(c, false);
    if (a >= 8 && a <= 200) { WR_NOPS = (uint8_t)a; g_shadowValid = false;
      Serial.printf("/WR pulse = %ld nops. Higher = slower but safer on a\n"
                    "breadboard; if the screen speckles or tears, raise it.\n", a); }
    else Serial.printf("WR_NOPS is %u (8..200)\n", WR_NOPS);
    return; }
  if (isCmd(c, "fbdump")) {
    // The framebuffer exactly as drawn, panel order, 128 hex bytes per row.
    // For comparing what the firmware drew against what the panel shows.
    Serial.println("FBDUMP BEGIN");
    for (int r = 0; r < PANEL_H; ++r) {
      char line[PANEL_W + 2];
      for (int i = 0; i < PANEL_W / 2; ++i)
        snprintf(&line[i * 2], 3, "%02X", g_fb[r * (PANEL_W / 2) + i]);
      usbWait(PANEL_W + 2);
      Serial.println(line);
    }
    Serial.println("FBDUMP END");
    return; }
  if (isCmd(c, "fmode")) { a = parseArg(c, false);
    if (a == 0 || a == 1) { dispMode = (uint8_t)a; g_shadowValid = false; }
    Serial.printf("frame mode %u = %s\n", dispMode, dispMode
                  ? "stress-test stream (window once, /CS held)"
                  : "v1.08 chunks (window re-sent every 2 rows)");
    return; }
  if (isCmd(c, "click") || isCmd(c, "hold")) {       // v1.16: the board button
    if (c[0] == 'c') uiBoardClick(); else uiBoardHold();
    static const char* const kPg[UPAGE_COUNT] = {"PERF", "LFO", "PAT", "SET"};
    Serial.printf("%s  (%s%s", kPg[ui.page < UPAGE_COUNT ? ui.page : 0],
                  ui.inSub ? "SUB" : "MAIN",
                  ui.inSub ? "" : ")\n");
    if (ui.inSub && ui.page == UPAGE_LFO) Serial.printf(" EDIT L%u)\n", ui.sel + 1);
    else if (ui.inSub)                    Serial.printf(" %s)\n", uiSubName());
    return; }
  if (isCmd(c, "joy")) {                             // v1.17
    const char* arg = c; while (*arg && *arg != ' ') arg++; while (*arg == ' ') arg++;
    const long n = parseArg(c, false);
    if      (!strncmp(arg, "all", 3)) g_joyMask = 0x3F;
    else if (!strncmp(arg, "off", 3)) g_joyMask = 0;
    else if (!strncmp(arg, "tog", 3)) {
      const long t = parseArg(arg, false);
      if (t >= 1 && t <= 6) g_joyMask = (uint8_t)(g_joyMask ^ (1u << (t - 1)));
    } else if (!strncmp(arg, "sweep", 5)) {
      long sec = parseArg(arg, false);
      if (sec < 1) sec = 3;
      if (sec > 20) sec = 20;
      g_joySweepReq = (uint32_t)sec * 1000u;
      Serial.printf("joy: dry-run sweep, %ld s - nothing is transmitted; 'joy' after it"
                    " for the result\n", sec);
      return;
    } else if (arg[0] == 'x' || arg[0] == 'y') {
      const long v = parseArg(arg, false);
      if (v >= 0 && v < (long)destCount()) {
        if (arg[0] == 'x') { g_joyDestX = (uint8_t)v; joyForget(true, false); }
        else               { g_joyDestY = (uint8_t)v; joyForget(false, true); }
      }
    } else if (n >= 1 && n <= 6) g_joyMask = (uint8_t)(1u << (n - 1));
    uiTouch();
    char lb[6]; joyMaskLabel(g_joyMask, lb);
    uint8_t pg = 0, sl = 0;
    Serial.printf("joy %s   X %u = ", lb, g_joyDestX);
    if (destAt(g_joyDestX, &pg, &sl)) Serial.printf("%s %s CC%u", kMnmPages[pg].name,
                                                    mnmParamName(pg, sl), mnmCC(pg, sl));
    Serial.printf("   Y %u = ", g_joyDestY);
    if (destAt(g_joyDestY, &pg, &sl)) Serial.printf("%s %s CC%u", kMnmPages[pg].name,
                                                    mnmParamName(pg, sl), mnmCC(pg, sl));
    Serial.println();
    for (uint8_t t = 0; t < 6; ++t)
      Serial.printf("  T%u ch%-2u %s  %lu msgs\n", t + 1,
                    (unsigned)(((txChannel - 1 + t) & 0x0F) + 1),
                    (g_joyMask & (1u << t)) ? "ON " : "off", (unsigned long)g_joyTrkN[t]);
    Serial.printf("  queue cap %u B, waited %lu times, worst lag %lu ms\n",
                  (unsigned)JOY_Q_CAP, (unsigned long)g_joyWaits,
                  (unsigned long)(g_joyLagMaxUs / 1000u));
    if (g_joySwMs) {
      joyMaskLabel(g_joySwMask, lb);
      Serial.printf("  last dry sweep: %lu ms to %s, %lu B (%lu%% of a 3125 B/s wire),"
                    " waited %lu, worst lag %lu ms\n   msgs per track:",
                    (unsigned long)g_joySwMs, lb, (unsigned long)g_joySwBytes,
                    (unsigned long)(g_joySwBytes * 100u / (g_joySwMs * 3125u / 1000u + 1u)),
                    (unsigned long)g_joySwWaits, (unsigned long)(g_joySwLagUs / 1000u));
      for (uint8_t t = 0; t < 6; ++t) Serial.printf(" %lu", (unsigned long)g_joySwN[t]);
      Serial.println();
      if (g_joySwTail >= 0)
        Serial.printf("   resting stick afterwards: %ld msgs would have gone %s\n",
                      (long)g_joySwTail, g_joySwTail ? "- LEAK" : "(correct: none)");
    }
    return; }
  if (isCmd(c, "trans")) { a = parseArg(c, false);   // v1.16, v1.17: two
    const char* arg = c; while (*arg && *arg != ' ') arg++; while (*arg == ' ') arg++;
    if (arg[0] == 's') { a = parseArg(arg, false);
                         if (a >= 0 && a < TS_COUNT) { uiTransSub = (uint8_t)a; uiTouch(); } }
    else if (a >= 0 && a < TS_COUNT) { uiTransStyle = (uint8_t)a; uiTouch(); }
    Serial.printf("pages %u = %s   sub %u = %s   (0 SLIDE 1 WIPE 2 DISSOLVE 3 FLASH 4 CUT)\n"
                  "last transition drew %u in-between frames\n",
                  uiTransStyle, kTransName[uiTransStyle], uiTransSub,
                  kTransName[uiTransSub], g_transFramesLast);
    return; }
  if (isCmd(c, "crash")) {                 // v1.15: why did the last reset happen?
    Serial.printf("this boot: %s  (SRSR 0x%08lX)\n", resetCause(g_resetSrsr),
                  (unsigned long)g_resetSrsr);
    if (CrashReport) Serial.print(CrashReport);
    else Serial.println("no CPU fault recorded");
    Serial.printf("watchdog %s (4 s)\n", g_wdtOn ? "armed" : "OFF");
    return; }
  if (isCmd(c, "wdtest")) {                // v1.15: prove the watchdog recovers
    Serial.println("hanging on purpose - the watchdog should reboot the XY6 in ~4 s");
    Serial.flush();
    for (;;) { }                           // interrupts stay on: USB still reflashes
  }
  if (isCmd(c, "redraw")) { g_shadowValid = false;
    Serial.println("full frame forced"); return; }
  if (isCmd(c, "heal")) { a = parseArg(c, false);
    if (a >= 0 && a <= 60000) { dispHealMs = (uint32_t)a; g_healAt = millis(); }
    if (dispHealMs)
      Serial.printf("display self-heal every %lu ms (%lu repaints so far)\n"
                    "       The row diff cannot see a byte that landed wrong on\n"
                    "       the panel - this is a write-only bus - so a one-byte\n"
                    "       glitch would otherwise stay on screen for ever. This\n"
                    "       forgets the shadow on a timer and repaints in full.\n"
                    "       Costs ~3.5 ms of bus time each, and it IS the worst\n"
                    "       loop pass, so LOOP US reads ~3500 by design.\n"
                    "       'heal 0' disables it; 'heal 250' if the panel is\n"
                    "       noisy enough to want it sooner.\n",
                    (unsigned long)dispHealMs, (unsigned long)g_healCount);
    else
      Serial.println(F("display self-heal OFF - a corrupted row will now stay\n"
                       "       corrupted until its content changes. 'heal 1000'\n"
                       "       puts it back."));
    return; }
  if (isCmd(c, "dumpraw")) { dumpRawOn = !dumpRawOn; dumpIn = false; dumpLen = 0;
    if (dumpRawOn)
      Serial.println(F("\nDUMPRAW armed. Now on the Monomachine, send a SysEx dump\n"
                       "(its SysEx Send menu -> Kit, or Global). The whole message\n"
                       "prints here as hex - copy ALL of it. STOP the sequencer\n"
                       "first so clock does not clutter the port. 'dumpraw' again\n"
                       "turns it off."));
    else Serial.println(F("dumpraw off"));
    return; }
  if (isCmd(c, "dumpshow")) {
    if (dumpLen) printDump();
    else Serial.println(F("nothing captured yet - arm 'dumpraw' and send a dump"));
    return; }
  if (isCmd(c, "rx")) { rxDump = !rxDump;
    Serial.printf("\nraw MIDI IN 1 dump %s\n", rxDump ? "on" : "off"); return; }
  if (isCmd(c, "p2")) { rx2Dump = !rx2Dump;
    Serial.printf("\nraw MIDI IN 2 dump %s\n", rx2Dump ? "on" : "off"); return; }
  if (isCmd(c, "loop")) {
    Serial.println(F("queuing B0 07 7F / B0 07 00 on OUT 1.\n"
                     "Patch OUT 1 into IN 2 and watch RX 2 climb - that proves the\n"
                     "transmit hardware works independently of the Monomachine."));
    const uint8_t t1[3] = {0xB0, 0x07, 0x7F}, t2[3] = {0xB0, 0x07, 0x00};
    for (int i = 0; i < 8; ++i) { qCtrl.push(t1, 3); qCtrl.push(t2, 3); }
    return; }
  Serial.println("? unknown - type ? for the list");
}

// Accepts a command however your terminal chooses to end a line — newline,
// carriage return, both, or NOTHING at all. The Arduino Serial Monitor has a
// line-ending dropdown, and with it set to "No line ending" a parser that waits
// for '\n' never runs a single command. So a short idle gap also counts as
// end-of-line.
static void pollSerial() {
  static uint32_t lastCharMs = 0;
  while (Serial.available()) {
    const char ch = (char)Serial.read();
    lastCharMs = millis();
    if (ch == '\n' || ch == '\r') {
      if (cmdLen) {
        cmdBuf[cmdLen] = 0;
        Serial.printf("> %s\n", cmdBuf);      // echo, so you can see it landed
        handleCommand(cmdBuf);
        cmdLen = 0;
      }
    } else if (cmdLen < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdLen++] = ch;
    }
  }
  if (cmdLen && (millis() - lastCharMs) > 150) {
    cmdBuf[cmdLen] = 0;
    Serial.printf("> %s\n", cmdBuf);
    handleCommand(cmdBuf);
    cmdLen = 0;
  }
}


// ================================ joystick ===================================
//
// ON since v1.10 (JOYSTICK_ENABLED in the config block). Set it to 0 and the
// whole section compiles away - no RAM, no CCs, no meters on the PERF page.
//
// On the review question "can analogRead() block the serial read": no, and it
// is not close. A Teensy 4 analogRead at the core's default 10-bit, 4x
// averaging is single-digit microseconds; two of them per update is well under
// one byte time even at 10x turbo (32 us). The thing that genuinely stalls this
// loop is a USB print into a host that has stopped reading, which the core will
// spin on for 120 ms - twenty thousand times worse. That is fixed above.
//
// It is still rate-limited, for a different and better reason: the wire. Every
// update is up to 5 bytes PER TRACK on a port the LFOs and the pattern
// generator are already sharing. One track at 8 ms (125 Hz) is 625 B/s of 3125
// at standard MIDI; all six moving flat out would want 3750 - more than the
// whole port. v1.17 handles that by never queueing a backlog: each track keeps
// the value it was last sent, only the newest value is ever queued, the queue
// is only topped up to JOY_Q_CAP, and the stick spends at most JOY_BUDGET_BPS.
// Under load the tracks simply update a little less often - never out of
// order, never piling up in front of an LFO or a note. Change detection still
// makes a resting stick cost nothing.
// (JOYSTICK_ENABLED and the JOY_* settings live in the config block up top.)

#if JOYSTICK_ENABLED
static uint32_t joyNextUs = 0;
static uint32_t joySent = 0;
static uint8_t  joyRr = 0;                 // track that goes first next pass
static uint32_t joyTokMb = JOY_BURST_B * 1000u, joyTokUs = 0;   // budget, milli-bytes
static uint32_t joyDueUs[6] = {0, 0, 0, 0, 0, 0};   // when each fell behind, 0 = not
// 'joy sweep': a dry run. The stick is swept by a pair of sines and every
// message goes to a pretend wire drained at 3125 B/s instead of the port.
static bool      joyDry = false, joyDryTail = false;
static uint32_t  joyDryTailN0 = 0;
static const uint32_t JOY_DRY_TAIL_MS = 2000;   // real stick, still dry, after
static uint32_t  joyDryT0 = 0, joyDryDur = 0, joyDryLastUs = 0, joyDryAcc = 0;
static uint32_t  joyDryBytes = 0, joyDryN0[6];
static uint8_t   joyDryKeepX[6], joyDryKeepY[6];
static MidiQueue qJoyDry;

// Raw ADC to 0..127 with 64 at rest, deadzone honoured. Same shape as
// mapJoystick() in MonomachineJoystick_v1_0_1.ino, minus the per-preset
// two-CC split, which belongs with the presets that define it.
// Per-axis state. Smoothing, the Schmitt deadzone and the send threshold all
// need memory, and duplicating three copies of that for X and Y is how the two
// axes end up behaving differently.
struct JoyAxis {
  uint16_t centre = 512;
  int32_t  filt   = 0;        // raw ADC, Q4, one-pole smoothed
  bool     primed = false;    // filt has a real reading in it
  bool     inDead = true;     // currently inside the centre band
  uint8_t  pend   = 64;       // most recent computed value
  uint32_t pendMs = 0;        // when pend last changed
  bool     calBad = false;    // boot centre was implausible, 512 used instead
  int16_t  lo = 0, hi = 1023; // learned extremes of the smoothed reading
  uint16_t bootCentre = 512;  // v1.17: the rest centre may drift, this far
  int16_t  held = 0;          // v1.17: reading after JOY_RAW_HYST
  int16_t  stillRef = 0;      // v1.17: where the stick last settled...
  uint32_t stillMs = 0;       //        ...and since when
  int16_t  raw1 = 0, raw2 = 0;   // v1.18: the two readings before this one

  // A METHOD, not a free function taking JoyAxis&. See the note by destName():
  // the IDE hoists a prototype for every free function to the very top of the
  // sketch, above every type this file defines, so a free function may not name
  // one in its signature. Member functions are not hoisted.
  //
  // Smooth and map one reading into pend.
  void step(int raw, uint32_t nowMs);
  // v1.17: is a track that was last sent `sentV` due the current value? What
  // was sent is per TRACK now (g_joySentX / Y), so this takes it as an input.
  bool due(uint8_t sentV, uint32_t nowMs) const;
  // The dry-run sweep sets the value directly.
  void force(uint8_t v, uint32_t nowMs) {
    if (v != pend) { pend = v; pendMs = nowMs; }
    inDead = (v == 64);
  }
};
static JoyAxis joyX, joyY;
static JoyAxis joyDryAxX, joyDryAxY;       // v1.17: the stick, as it was

// Every read of the stick goes through these, calibration included, so the
// centre and the live values can never disagree about which way is up.
static inline int joyReadX() { const int r = analogRead(JOY_PIN_X); return JOY_INVERT_X ? 1023 - r : r; }
static inline int joyReadY() { const int r = analogRead(JOY_PIN_Y); return JOY_INVERT_Y ? 1023 - r : r; }

static uint8_t joyMap(int raw, int centre, int lo, int hi, bool* inDead) {
  const int d  = raw - centre;
  const int ad = (d < 0) ? -d : d;

  // Schmitt band. analogReadAveraging(4) averages sample noise; it does nothing
  // about a stick parked exactly on the deadzone edge, where one ADC count
  // either way flips the output between 64 and 65 and - with plain change
  // detection - streams a CC every 8 ms forever.
  if (*inDead) { if (ad < JOY_DEADZONE) return 64; *inDead = false; }
  else if (ad < JOY_DEADZONE - JOY_HYST) { *inDead = true; return 64; }

  // Clamp the divisors. centre comes from eight raw reads at boot, so a
  // disconnected or failing pot puts it near 0 or 1023 - and at centre == 29
  // (low branch) or 994 (high branch) these were EXACTLY zero. Integer division
  // by zero is undefined behaviour in C, not a convenient infinity.
  // No "+ 1" in the divisors. That was only ever there to dodge a division by
  // zero, and it did not even manage that - it just moved the zero to
  // centre == 29 / 994. It also cost the endpoints: with it, a full-travel
  // sweep resolved to 1..126 and never reached 0 or 127, so the stick could not
  // completely close a filter or reach maximum. Clamping the divisor handles
  // the degenerate centre properly and gives back both ends of the range.
  int v;
  if (d < 0) {
    int den = centre - lo - JOY_DEADZONE - JOY_EDGE;
    if (den < 1) den = 1;
    v = 64 + (64 * (d + JOY_DEADZONE)) / den;
  } else {
    int den = hi - centre - JOY_DEADZONE - JOY_EDGE;
    if (den < 1) den = 1;
    v = 64 + (63 * (d - JOY_DEADZONE)) / den;
  }
  if (v < 0)   v = 0;
  if (v > 127) v = 127;
  return (uint8_t)v;
}

void JoyAxis::step(int raw, uint32_t nowMs) {
  if (!primed) { filt = (int32_t)raw << 4; primed = true;
                 held = stillRef = raw1 = raw2 = (int16_t)raw; stillMs = nowMs; }
#if JOY_FAST_CNT
  else {
    // v1.18: median of three, then a one-pole that speeds up for real motion.
    // See JOY_FAST_CNT.
    const int a = raw, b = raw1, c = raw2;
    const int med = (a > b) ? ((b > c) ? b : (a > c ? c : a))
                            : ((a > c) ? a : (b > c ? c : b));
    raw2 = raw1; raw1 = (int16_t)raw;
    const int32_t e  = ((int32_t)med << 4) - filt;
    const int32_t ae = (e < 0) ? -e : e;
    filt += (ae > ((int32_t)JOY_FAST_CNT << 4)) ? ((e * 3) >> 2) : (e >> 2);
  }
#else
  else         filt += (((int32_t)raw << 4) - filt) >> 2;   // one-pole
#endif
  const int sm = (int)(filt >> 4);
  // Learn the travel from the SMOOTHED reading, so one noisy sample cannot
  // stretch the range and leave the ends unreachable again.
  if (sm < lo) lo = (int16_t)sm;
  if (sm > hi) hi = (int16_t)sm;
  // v1.17: noise on a step boundary must not reach the mapping.
  if (sm - held >= JOY_RAW_HYST || held - sm >= JOY_RAW_HYST) held = (int16_t)sm;
  // v1.17: still, and near the centre: this is where the spring put it.
  if (sm - stillRef > JOY_STILL_CNT || stillRef - sm > JOY_STILL_CNT) {
    stillRef = (int16_t)sm; stillMs = nowMs;
  } else if ((uint32_t)(nowMs - stillMs) >= JOY_STILL_MS) {
    const int off = sm - (int)centre, drift = sm - (int)bootCentre;
    if (off > -JOY_REST_BAND && off < JOY_REST_BAND &&
        drift > -JOY_DRIFT_MAX && drift < JOY_DRIFT_MAX) {
      centre = (uint16_t)sm; held = (int16_t)sm; inDead = true;
    }
  }
  const uint8_t v = joyMap(held, (int)centre, lo, hi, &inDead);

  if (v != pend) { pend = v; pendMs = nowMs; }
}

bool JoyAxis::due(uint8_t sentV, uint32_t nowMs) const {
  // v1.10: nothing goes out until the stick has actually been MOVED. The old
  // "first ever send" transmitted 64 at every boot, overwriting whatever the
  // Monomachine parameter was set to - the exact thing this firmware promises
  // never to do (see the LFOs, which stay silent until they are driving).
  if (sentV == 0xFF) return !inDead;
  if (pend == sentV) return false;
  const int16_t delta = (int16_t)pend - (int16_t)sentV;
  const uint16_t ad   = (uint16_t)(delta < 0 ? -delta : delta);
  // A real move goes immediately. A one-count wobble goes only once it has held
  // still for 60 ms - so the resting value is always exact, but a shivering
  // stick cannot stream 375 bytes a second at it.
  if (ad >= 2) return true;
  return (uint32_t)(nowMs - pendMs) >= 60u;
}

static void joyBegin() {
  pinMode(JOY_PIN_X, INPUT);
  pinMode(JOY_PIN_Y, INPUT);
  analogReadResolution(10);          // pinned, so the timing above is a fact
  analogReadAveraging(4);            //   rather than a default that may move
  g_joyUiX = g_joyUiY = 64;
  (void)analogRead(JOY_PIN_X); (void)analogRead(JOY_PIN_Y);   // settle the mux
  uint32_t sx = 0, sy = 0;
  for (uint8_t i = 0; i < 8; ++i) { sx += joyReadX();
                                    sy += joyReadY(); }
  joyX.centre = (uint16_t)(sx / 8);
  joyY.centre = (uint16_t)(sy / 8);
  g_joyCx = joyX.centre; g_joyCy = joyY.centre;
  Serial.printf("joystick: centre X %u  Y %u\n", joyX.centre, joyY.centre);
  // A centre this far off means the stick was not at rest during calibration,
  // or the pot is not wired. The mapping still works - the divisors are clamped
  // - but the travel either side will be wildly asymmetric, so say so.
  // v1.10: a centre this far off means the stick was being held during boot
  // (or the pot is not wired). Calibrating to it would skew the whole range -
  // one side of travel crushed into a few counts - so use the nominal centre
  // instead and say so. A pot that is genuinely off-centre by that much is a
  // hardware fault the warning points at.
  if (joyX.centre < 200 || joyX.centre > 824) { joyX.centre = 512; joyX.calBad = true; }
  if (joyY.centre < 200 || joyY.centre > 824) { joyY.centre = 512; joyY.calBad = true; }
  g_joyCx = joyX.centre; g_joyCy = joyY.centre;
  for (JoyAxis* a : {&joyX, &joyY}) {
    a->lo = (int16_t)max(0,    (int)a->centre - JOY_SPAN_INIT);
    a->hi = (int16_t)min(1023, (int)a->centre + JOY_SPAN_INIT);
    a->bootCentre = a->centre;
  }
  if (joyX.calBad || joyY.calBad)
    Serial.println(F("joystick: WARNING - centre was far from 512, using 512. Leave\n"
                     "          the stick alone through boot, and check the pot wiring."));
}

// An LFO already DRIVING this exact parameter on this track: move its centre,
// as the PERF knobs do (v1.12), rather than send a CC it overwrites next tick.
static bool joyLfoCentre(uint8_t t, uint8_t pg, uint8_t sl, uint8_t v) {
  bool any = false;
  for (uint8_t i = 0; i < LFO_COUNT; ++i) {
    LfoParams& p = lfo.p[i];
    if (!lfo.driving(i) || p.page != pg || p.dest != sl) continue;
    if ((txPerTrack ? p.track : 0) != t) continue;       // same channel only
    p.baseValue = v; lfo.setCapture(i, v); any = true;
  }
  return any;
}
// A PERF slot on the same track and parameter shows what the stick sent - and
// the echo filter learns it, so the value coming back through a THRU is ours.
static void joyFollowPerf(uint8_t t, uint8_t pg, uint8_t sl, uint8_t v) {
  for (uint8_t c = 0; c < 6; ++c) {
    if (perfSlot[c].track != t || perfSlot[c].page != pg || perfSlot[c].dest != sl) continue;
    perfSlot[c].value = v; perfSlot[c].known = true; perfSent[c] = v;
    if (ui.page == UPAGE_PERF && !ui.inSub) uiTouch();
  }
}

// Every selected track that is behind gets the current X / Y, newest value
// only, as ONE message: status, CC X, value, CC Y, value - running status, so
// both axes cost 5 bytes rather than 6. Tracks are served round-robin from the
// one that waited last time, and the queue is only filled to JOY_Q_CAP.
static void joyTransmit(uint32_t nowMs, uint32_t nowUs) {
  uint8_t pgX = 0, slX = 0, pgY = 0, slY = 0;
  const uint8_t ccX = destAt(g_joyDestX, &pgX, &slX) ? mnmCC(pgX, slX) : 0xFF;
  const uint8_t ccY = destAt(g_joyDestY, &pgY, &slY) ? mnmCC(pgY, slY) : 0xFF;
  MidiQueue& q = joyDry ? qJoyDry : qCtrl;
  { uint32_t el = nowUs - joyTokUs; joyTokUs = nowUs;       // refill the budget
    if (el > 100000u) el = 100000u;
    joyTokMb += el * (uint32_t)JOY_BUDGET_BPS / 1000u;
    if (joyTokMb > JOY_BURST_B * 1000u) joyTokMb = JOY_BURST_B * 1000u; }
  const uint8_t first = joyRr;
  for (uint8_t k = 0; k < 6; ++k) {
    const uint8_t t = (uint8_t)((first + k) % 6);
    // v1.18: a track that is not under the stick forgets what it was sent. It
    // used to keep it, so a track taken off and put back later - its last value
    // 100, say, and the stick now resting - was due at once and got the resting
    // 64, overwriting the patch: the one thing 0xFF exists to prevent.
    if (!(g_joyMask & (1u << t))) {
      joyDueUs[t] = 0; g_joySentX[t] = g_joySentY[t] = 0xFF;
      continue;
    }
    bool dx = (ccX != 0xFF) && joyX.due(g_joySentX[t], nowMs);
    bool dy = (ccY != 0xFF) && joyY.due(g_joySentY[t], nowMs);
    const uint8_t vx = joyX.pend, vy = joyY.pend;
    if (!joyDry) {
      if (dx && joyLfoCentre(t, pgX, slX, vx)) { g_joySentX[t] = vx; dx = false; }
      if (dy && joyLfoCentre(t, pgY, slY, vy)) { g_joySentY[t] = vy; dy = false; }
    }
    if (!dx && !dy) { joyDueUs[t] = 0; continue; }
    if (!joyDueUs[t]) joyDueUs[t] = nowUs ? nowUs : 1u;

    uint8_t m[5], n = 0;
    m[n++] = (uint8_t)(0xB0 | ((txChannel - 1 + t) & 0x0F));
    if (dx) { m[n++] = ccX; m[n++] = vx; }
    if (dy) { m[n++] = ccY; m[n++] = vy; }
    const uint16_t used = (uint16_t)(qCtrl.pending() + (joyDry ? qJoyDry.pending() : 0));
    int32_t waited = (int32_t)(nowUs - joyDueUs[t]);
    if (waited < 0) waited = 0;
    const bool late = waited >= (int32_t)JOY_LATE_US;
    if (joyTokMb < (uint32_t)n * 1000u ||
        (!late && used + n + 1u > (uint16_t)JOY_Q_CAP) || !q.push(m, n)) {
      joyRr = t; g_joyWaits++;      // wire busy: this track goes first next pass
      return;
    }
    joyTokMb -= (uint32_t)n * 1000u;
    if (dx) g_joySentX[t] = vx;
    if (dy) g_joySentY[t] = vy;
    if ((uint32_t)waited > g_joyLagMaxUs) g_joyLagMaxUs = (uint32_t)waited;
    joyDueUs[t] = 0;
    g_joyTrkN[t]++; g_joyActMs[t] = nowMs ? nowMs : 1u;
    if (!joyDry) {
      joySent++; g_joySent += (uint32_t)dx + (uint32_t)dy;   // real CCs only
      if (dx) joyFollowPerf(t, pgX, slX, vx);
      if (dy) joyFollowPerf(t, pgY, slY, vy);
      // v1.23: the machine now holds it, so the kit model does too.
      if (dx) kitSet(t, pgX, slX, vx);
      if (dy) kitSet(t, pgY, slY, vy);
    }
  }
  joyRr = (uint8_t)((first + 1) % 6);    // all served: rotate who leads
}

static void joyService(uint32_t nowUs) {
  if ((int32_t)(nowUs - joyNextUs) < 0) return;
  joyNextUs = nowUs + JOY_INTERVAL_US;
  // Not while a speed change is on the wire - see the same guard in patTick.
  if (turboHoldsWire()) return;

  const uint32_t nowMs = millis();
  const uint8_t px = joyX.pend, py = joyY.pend;

  // ---- 'joy sweep': start, sweep, tail, finish ----
  // SWEEP: two sines drive the stick. TAIL: the real stick again, output still
  // dry - after a sweep a resting stick must want to send NOTHING, and the
  // tail counts what it would have sent, as a self-test.
  if (g_joySweepReq && !joyDry) {
    joyDry = true; joyDryTail = false; joyDryT0 = nowMs; joyDryDur = g_joySweepReq;
    g_joySweepReq = 0;
    memcpy(joyDryKeepX, g_joySentX, 6); memcpy(joyDryKeepY, g_joySentY, 6);
    memcpy(joyDryN0, g_joyTrkN, sizeof joyDryN0);
    joyDryAxX = joyX; joyDryAxY = joyY;
    g_joyWaits = 0; g_joyLagMaxUs = 0; g_joySwTail = -1;
    qJoyDry.reset(); joyDryLastUs = nowUs; joyDryAcc = 0; joyDryBytes = 0;
    for (uint8_t t = 0; t < 6; ++t) joyDueUs[t] = 0;
  }
  if (joyDry && !joyDryTail && (uint32_t)(nowMs - joyDryT0) >= joyDryDur) {
    g_joySwMs = joyDryDur; g_joySwBytes = joyDryBytes; g_joySwWaits = g_joyWaits;
    g_joySwLagUs = g_joyLagMaxUs; g_joySwMask = g_joyMask;
    joyDryTailN0 = 0;
    for (uint8_t t = 0; t < 6; ++t) { g_joySwN[t] = g_joyTrkN[t] - joyDryN0[t];
                                      joyDryTailN0 += g_joyTrkN[t]; joyDueUs[t] = 0; }
    // Nothing reached the machine, so what it was last sent is what it was -
    // and the stick goes back to exactly the state it was in, rest included.
    // (v1.17's first cut left both axes "moved" here, so a stick resting just
    // off centre went on sending its 63 for real.)
    memcpy(g_joySentX, joyDryKeepX, 6); memcpy(g_joySentY, joyDryKeepY, 6);
    joyX = joyDryAxX; joyY = joyDryAxY;
    joyDryTail = true;
  }
  if (joyDry && joyDryTail && (uint32_t)(nowMs - joyDryT0) >= joyDryDur + JOY_DRY_TAIL_MS) {
    uint32_t sum = 0;
    for (uint8_t t = 0; t < 6; ++t) { sum += g_joyTrkN[t]; joyDueUs[t] = 0; }
    g_joySwTail = (int32_t)(sum - joyDryTailN0);
    memcpy(g_joySentX, joyDryKeepX, 6); memcpy(g_joySentY, joyDryKeepY, 6);
    joyDry = false; joyDryTail = false;
  }
  if (joyDry && !joyDryTail) {
    const float ts = (float)(nowMs - joyDryT0) * 0.001f;
    joyX.force((uint8_t)(64.0f + 63.0f * sinf(6.2832f * 1.3f * ts)), nowMs);
    joyY.force((uint8_t)(64.0f + 63.0f * sinf(6.2832f * 0.9f * ts + 1.0f)), nowMs);
  } else {
    // step() does the smoothing, the Schmitt deadzone and the rest centre;
    // due() the send threshold - so a stick at rest costs nothing at all.
    joyX.step(joyReadX(), nowMs);
    joyY.step(joyReadY(), nowMs);
  }
  if (joyDry) {
    // The pretend wire: 3125 bytes a second, in milli-bytes. The gap is
    // clamped like the budget refill above (v1.18): a console command that
    // held the loop for 1.4 s would overflow the multiply.
    uint32_t gapUs = nowUs - joyDryLastUs; joyDryLastUs = nowUs;
    if (gapUs > 100000u) gapUs = 100000u;
    joyDryAcc += gapUs * 3125u / 1000u;
    for (uint8_t n; (n = qJoyDry.peekLen()) != 0 && joyDryAcc >= (uint32_t)n * 1000u; ) {
      qJoyDry.discard(); joyDryAcc -= (uint32_t)n * 1000u; joyDryBytes += n;
    }
    if (!qJoyDry.pending()) joyDryAcc = 0;          // an idle wire banks nothing
  }
  joyTransmit(nowMs, nowUs);
  // v1.14: activity. Off centre, or moved since the last pass, counts; the
  // meters stay up for JOY_HIDE_MS after the last of either. The PERF page is
  // static, so it is told when they come and go.
  { static uint32_t lastActiveMs = 0;
    if (joyX.pend != 64 || joyY.pend != 64 || joyX.pend != px || joyY.pend != py)
      lastActiveMs = nowMs ? nowMs : 1;
    const bool show = lastActiveMs && (uint32_t)(nowMs - lastActiveMs) < JOY_HIDE_MS;
    if (show != g_joyShow) {
      g_joyShow = show;
      if (ui.page == UPAGE_PERF && !ui.inSub) uiTouch();
    }
  }
  // The PERF page draws the stick. It is a static page, so it has to be told.
  g_joyUiX = joyX.pend; g_joyUiY = joyY.pend;
  g_joyCx = joyX.centre; g_joyCy = joyY.centre;          // v1.17: they move now
  g_joyRng[0] = joyX.lo; g_joyRng[1] = joyX.hi; g_joyRng[2] = joyY.lo; g_joyRng[3] = joyY.hi;
  if ((joyX.pend != px || joyY.pend != py) && ui.page == UPAGE_PERF && !ui.inSub)
    uiTouch();
  // v1.17: the JOY page shows the stick and each track's send light, and is a
  // static page too. Keep it drawing while a light is on so the last one
  // goes out on time.
  if (ui.page == UPAGE_PERF && ui.inSub && g_perfSub == 1) {
    bool lit = (joyX.pend != px || joyY.pend != py);
    for (uint8_t t = 0; t < 6 && !lit; ++t)
      lit = g_joyActMs[t] && (uint32_t)(nowMs - g_joyActMs[t]) < JOY_ACT_MS + 40u;
    if (lit) uiTouch();
  }
}

#else   // JOYSTICK_ENABLED == 0
// Declared away entirely rather than compiled and left unused, so a disabled
// joystick costs no RAM and produces no warnings to train your eye past.
static void joyBegin() {}
static void joyService(uint32_t) {}
#endif


void setup() {
  g_resetSrsr = SRC_SRSR;             // v1.15: why did we boot? (see resetCause)
  SRC_SRSR = g_resetSrsr;             //   write-1-to-clear, so the next boot is fresh
  Serial.begin(115200);

  dispBegin();          // also clears the panel and pushes a blank frame
  uiApplyContrast();    // contrast follows reversed video - see uiInvert
  encodersBegin();
  joyBegin();
  mcpBegin();
  // After Wire.begin(), which mcpBegin() does. The probe is one transaction and
  // decides between SYS_RUN and SYS_FAULT later.
  HalEeprom::begin();
  Store::begin();        // queues the globals read; the splash covers it
  gBtn.begin();
  // Walk all five LEDs once so you can see each one works and which bit drives
  // which. If one stays dark here it is wiring, not firmware. 60 ms a step
  // rather than 150 — still clearly a sweep, and it is 450 ms less dead time
  // before the panel comes up.
  for (uint8_t i = 0; i < 5; ++i) { ledSet((uint8_t)(1u << i)); delay(60); }
  ledSet(LED_PAGE2);   // page LED for the LFO page

  // Deeper UART buffers before the first begin(). At 10x (312500 baud) a byte
  // lands every 32 us, so the stock 64-byte RX buffer is only ~2 ms of slack -
  // survivable, but not with any margin for a full-frame display push. 1 KB of
  // RX is 32 ms of slack at 10x and costs nothing we have a shortage of. The
  // pointers set here survive later begin() calls, so the baud switch does not
  // need to re-register them.
  Serial1.addMemoryForRead(serial1RxBuf, sizeof(serial1RxBuf));
  Serial1.addMemoryForWrite(serial1TxBuf, sizeof(serial1TxBuf));
  Serial1.begin(31250);
  Serial2.begin(31250);
  qNote.reset();
  qCtrl.reset();
  // availableForWrite() on an idle port reports the whole buffer less one, and
  // that value is how the negotiator recognises "the UART has gone empty" -
  // and, since v1.18, how midiTxService() works out how much is in the ring.
  g_txCap = Serial1.availableForWrite();
  midiTxSetBaud(31250);
  turbo.begin(g_txCap);
  // SERIAL1_RX_BUFFER_SIZE in the core is 64; addMemoryForRead adds to it.
  rxCapacity = (uint16_t)(64u + sizeof(serial1RxBuf));

  clk.begin(micros(), 120.0f);
  lfo.begin((uint32_t)micros());
  mnmOut.begin();
  mnmOut.setBaud(31250);

  // v1.22: the kit model starts as the init kit - every parameter on every
  // track at the value a freshly loaded Monomachine kit holds, none of them
  // "learned" yet. A preset load or the machine's own CCs refine it from here.
  kitResetAll();
  for (uint8_t i = 0; i < 6; ++i) perfSeed(i);

  // Six LFOs across the AMP page of track 1, a different waveform on each, at
  // slightly different rates so they drift out of phase and the CC stream never
  // settles into a lull. If all six move smoothly on the Monomachine with no
  // stutter or stuck values, the clock and the output scheduler are holding
  // under the worst case.
  //
  //   AMP slot -> CC:  0 ATCK 56 | 1 HOLD 57 | 2 DEC 58
  //                    3 REL  59 | 4 DIST 60 | 5 VOL  61
  //
  // v1.17: all six are TRIG, so they start on play and lock to the bar. LFO 1
  // used to be FREE - a bring-up aid, so something moved with the sequencer
  // stopped - which meant that on any boot without a loadable preset it swept
  // track 1's AMP ATTACK on its own (and the PERF ATK fader with it) while the
  // other five correctly waited. None of them put a byte on the wire until
  // they are actually driving, so a cold boot never disturbs the loaded patch.
  //
  // v1.22: each swings around its destination's INIT-KIT value (ATCK 0,
  // HOLD 0, DEC 64, REL 64, DIST 0, VOL 64) rather than a blanket 64, so with a
  // default kit loaded the modulation is centred where the sound really is.
  static const struct LfoPreset {
    uint8_t track, page, dest, wave, trig, spd, mult, depth;
  } kPreset[] = {
    { 0, PAGE_AMP, 0, WAVE_TRI,   TRIG_TRIG, 64, 1, 127 },  // ATTACK  tri
    { 0, PAGE_AMP, 2, WAVE_EXP,   TRIG_TRIG, 72, 1, 127 },  // DECAY   exp fall
    { 0, PAGE_AMP, 1, WAVE_SAW,   TRIG_TRIG, 48, 1, 127 },  // HOLD    saw up
    { 0, PAGE_AMP, 3, WAVE_RMP,   TRIG_TRIG, 80, 1, 127 },  // RELEASE lin fall
    { 0, PAGE_AMP, 4, WAVE_TRI_M, TRIG_TRIG, 56, 2, 127 },  // DIST    itri 2x
    { 0, PAGE_AMP, 5, WAVE_RND,   TRIG_TRIG, 40, 1, 127 },  // VOL     random
  };
  const uint8_t nPreset = (uint8_t)(sizeof(kPreset) / sizeof(kPreset[0]));
  for (uint8_t i = 0; i < LFO_COUNT && i < nPreset; ++i) {
    LfoParams& q  = lfo.p[i];
    q.enabled     = true;
    q.track       = kPreset[i].track;
    q.page        = kPreset[i].page;
    q.dest        = kPreset[i].dest;
    q.wave        = kPreset[i].wave;
    q.trig        = kPreset[i].trig;
    q.spd         = kPreset[i].spd;
    q.mult        = kPreset[i].mult;   // spd 64 + mult 2x = one cycle per bar
    q.depth       = kPreset[i].depth;
    q.bars        = 1;
    lfoRetarget(i);                    // base = the init-kit value there
  }

  patGenerate(pat.genre, 1);   // so the pattern page has something to draw

  freeLastUs = micros();

  delay(300);                          // let the USB serial port come up
  Serial.println(F("\n\nREDOT XY6 - LFO + pattern OS"));
  Serial.printf("%u LFOs on track 1 AMP page, ch %u:\n", LFO_COUNT,
                (unsigned)MNM_BASE_CHANNEL);
  for (uint8_t i = 0; i < LFO_COUNT; ++i)
    Serial.printf("  LFO %u -> %-4s  CC %u  %s\n", i + 1,
                  mnmParamName(lfo.p[i].page, lfo.p[i].dest),
                  mnmCC(lfo.p[i].page, lfo.p[i].dest),
                  kTrigNames[lfo.p[i].trig]);
  Serial.println(F("All six LFOs are TRIG: press PLAY on the Monomachine and they\n"
                   "start, locked to the bar ('clk i' runs them with no machine)."));
  Serial.println(F("If nothing moves, type  s  for status, then  hn  to hunt."));
  Serial.println(F("\nSerial Monitor line ending can be anything - commands also run on a\n"
                   "short pause, so 'No line ending' works too."));
  printHelp();
  printStatus();

  // -------------------------------------------------------------------------
  // THE SPLASH CLOCK STARTS HERE, NOT AT POWER-ON
  // -------------------------------------------------------------------------
  // bootMs is an elapsedMillis, so it was constructed and started counting at
  // static-init time - before this function ran - and nothing ever zeroed it.
  // By the time loop() draws the first splash frame this much has already gone:
  //
  //     dispBegin() panel reset         290 ms   (10 + 20 + 200 + 60, v1.09)
  //     LED sweep, 5 x delay(60)        300 ms
  //     delay(300) for the USB port     300 ms
  //     printHelp() + printStatus()     serial, and printStatus alone makes
  //                                     three usbWait(128) calls of up to 50 ms
  //                                     each whenever a Serial Monitor is open
  //                                     but not draining
  //
  // So the first frame was drawn at e >= 625 ms on the best day and past a
  // second with the monitor attached - while the splash's opening ran 0..620 ms.
  // That opening was therefore not merely rough, it was UNREACHABLE: every boot
  // ever run has snapped straight into the middle of the sequence with the rule
  // already a third of the way open, continuing from mid-travel. And
  // because the overshoot depends on how fast the USB host is draining, it
  // started from a different point every time - which is what makes it read as
  // glitchy rather than just abrupt.
  //
  // Zeroing it here is the whole fix. The animation is a pure function of this
  // one number, so it now plays from frame 0 exactly as it was written, and it
  // plays the same on every boot whether or not anything is watching the port.
  bootMs = 0;

  // v1.15: why did this boot happen? A watchdog recovery is said out loud.
  if (g_resetSrsr & SRC_SRSR_WDOG3_RST_B)
    Serial.println(F("\n*** booted by the WATCHDOG: the firmware hung and recovered"));
  if (CrashReport) { Serial.println(F("\n*** CPU fault before this boot:")); Serial.print(CrashReport); }
  wdtBegin(4000);
}

void loop() {
  wdtFeed();                            // v1.15: a pass that never ends resets
  const uint32_t nowUs = micros();
  const uint32_t nowMs = millis();

  pumpMidi();
  midiTxService();
  tmLogService();          // deferred turbo log - writes only into free USB space
  pollSerial();

  // A captured SysEx prints from here, never from handleMidiByte(): that runs
  // from inside the display flush with /CS asserted, and two kilobytes of hex
  // to USB from there stalled the bus mid-frame.
  if (dumpReady) { dumpReady = false; printDump(); }
  if (dumpIn && (nowMs - dumpLastByteMs) > 500u) {
    dumpIn = false; rxSysex++;
    Serial.println(F("(no end-of-SysEx seen - printing what arrived)"));
    printDump();
  }

  // Transport events recorded by the byte handler, announced here where a
  // blocked USB write costs nothing but a late log line.
  if (pendingEvt && usbReady(64)) {
    const uint8_t e = pendingEvt; pendingEvt = 0;
    if (e & EVT_START) Serial.println(F("\n>> START from Monomachine"));
    if (e & EVT_CONT)  Serial.println(F("\n>> CONTINUE from Monomachine"));
    if (e & EVT_STOP2) Serial.println(F("\n>> STOP x2 - patch values restored,"
                                        " waiting for play"));
    else if (e & EVT_STOP) Serial.println(F("\n>> STOP from Monomachine"));
  }

  // A SysEx that simply stopped arriving leaves the parser mid-message with
  // nothing able to end it. Sweep it after a quarter second of silence.
  if (sysex1.tick(nowMs, 250u)) {
    rxSysexAbort++;
    if (usbReady(64)) Serial.println(F("\n>> incomplete SysEx timed out"));
  }

  // The turbo negotiator. Every state in it is a comparison against a deadline
  // or a port status, so this returns immediately whatever it is doing.
  turbo.service(nowMs, nowUs);

  clk.update(nowUs);

  // Free-running position, at the same tempo, always. This used to be a 64-bit
  // division executed on every pass of the main loop — of the order of a
  // hundred thousand a second, each one tens of cycles, for a quotient that
  // changes only when the tempo does. The clock now hands over the reciprocal
  // it already maintains, and this is a multiply and a shift.
  {
    uint32_t dt = nowUs - freeLastUs;
    freeLastUs = nowUs;
    if (dt > 100000u) dt = 100000u;
    freePos += (clk.rateQ40() * (uint64_t)dt) >> 8;
  }

  // ---- pattern generator: 16th-step firing + note-off sweep ----
  // Reads the beat period from the clock rather than a fixed rate, so slowing
  // the tempo lengthens its notes correctly. Note-offs are checked on every
  // pass so a short note at high tempo cannot overrun the next trig.
  {
    const uint32_t beatUs = (uint32_t)clk.period() * 24u;   // period is per-tick
    patTick(clk.position(), nowUs, beatUs ? beatUs : 500000u, clk.running());
  }

  // ---- LFO engine at 1 kHz ----
  static uint32_t lfoNext = 0;
  if ((int32_t)(nowUs - lfoNext) >= 0) {
    lfoNext = nowUs + 1000;
    lfo.update(clk.position(), freePos, clk.running());

    // Non-blocking ramp: one step every 25 ms, driven from here rather than
    // from a delay() loop inside the console handler.
    if (rampMode && (int32_t)(nowMs - rampNextMs) >= 0) {
      rampNextMs = nowMs + 25;
      if (rampMode == 1) {
        if (manualVal < 127) manualVal++;
        else rampMode = 2;
      } else {
        if (manualVal > 0) manualVal--;
        else { rampMode = 0; manualVal = -1;
               Serial.println("ramp done, LFO driving again"); }
      }
    }

    static bool prevDrive[LFO_COUNT] = {false};
    static bool restorePending[LFO_COUNT] = {false};
    for (uint8_t i = 0; i < LFO_COUNT; ++i) {
      uint8_t yy = (txMode == 3) ? mnmCC(lfo.p[i].page, lfo.p[i].dest)
                                 : mnmParamIndex(lfo.p[i].page, lfo.p[i].dest);
      // When two LFOs share a destination their outputs are already summed into
      // the owner's value, so only the owner writes to the wire. Without this
      // the two fight over the same CC and the far end sees whichever message
      // happened to land last.
      const bool drive = lfo.driving(i) && lfo.s[i].owner;
      if (drive) {
        uint8_t v = lfo.s[i].value;
        if (i == 0) {
          if (yyOverride >= 0) yy = (uint8_t)yyOverride;
          if (manualVal  >= 0) v  = (uint8_t)manualVal;
        }
        mnmOut.setParam(i, lfo.p[i].track, yy, v);
        restorePending[i] = false;
      } else {
        // Turned off, or waiting after a double-STOP: hand the destination its
        // captured patch value ONCE, then fall quiet so we do not keep writing
        // over a parameter the user is free to touch.
        //
        // Note the condition: we only restore something we were previously
        // DRIVING. A trig-mode LFO that has never fired was never on the wire,
        // so there is nothing to hand back — which is what stops a cold boot
        // from writing 64 into six AMP parameters before anything has been
        // captured from the machine.
        if (lfo.restoring() || (prevDrive[i] && !drive)) restorePending[i] = true;
        if (restorePending[i]) {
          const uint8_t rv = lfo.restValue(i);
          mnmOut.setParam(i, lfo.p[i].track, yy, rv);
          if (mnmOut.sentValue(i) == rv) restorePending[i] = false;
        } else {
          mnmOut.clearSlot(i);
        }
      }
      prevDrive[i] = drive;
    }
  }

  // ---- EEPROM + preset store at 1 kHz ----
  // One 32-byte chunk per ~5 ms device cycle, ACK-polled. A full preset save is
  // ~340 ms of device time spread over ~340 passes of this loop, and not one of
  // them blocks - which is why the LFOs keep sweeping while you hit SAVE.
  {
    static Deadline dlEe;
    if (dlEe.due(nowUs, 1000)) { HalEeprom::service(nowUs); Store::service(); }
  }

  // Spec: MIDI played on the Monomachine comes back and is recorded. Placed
  // here, ungated, because note-off timing cannot be quantised to 1 kHz.
  MIDI_HandleDirectPlayCapture(nowUs);

  // ---- the two states that are waiting on the EEPROM ----
  // Both have to be released HERE rather than inside Store::service, because
  // the store has no business knowing what the UI is showing - and because a
  // wizard left sitting on its SAVING screen with the write already finished is
  // a dead instrument that looks like a busy one.
  if (sysState == SYS_LOADING && !Store::busy()) { sysState = SYS_RUN; uiTouch(); }
  if (sysState == SYS_WIZARD && wizState == WIZ_SAVE && !Store::busy()) {
    wizState = WIZ_DONE;
    sysState = (storeOp == ST_ERR) ? SYS_FAULT : SYS_RUN;
    ui.page = UPAGE_PERF;      // spec: setup hands over to the encoder page
    ui.inSub = false;
    uiTouch();
  }

  // ---- output scheduler at 1 kHz ----
  // It has its own dt accounting, so calling it at full loop speed was correct
  // but wasteful: a 36-entry urgency scan a hundred thousand times a second to
  // decide whether three bytes may go out. Once a millisecond is far finer than
  // the 12 ms floor on the per-LFO rate cap.
  static uint32_t outNext = 0;
  if ((int32_t)(nowUs - outNext) >= 0) {
    outNext = nowUs + 1000;
    mnmOut.service(nowUs);
  }

  // ---- joystick, rate-limited to 125 Hz ----
  joyService(nowUs);

  // ---- encoders: drain the interrupt-filled accumulators at 1 kHz ----
  // The decoding happens in the ISRs now, so this only harvests whole detents.
  static uint32_t encNext = 0;
  if ((int32_t)(nowUs - encNext) >= 0) {
    encNext = nowUs + 1000;
    int8_t d[6];
    encodersRead(d);
    bool moved = false;
    for (uint8_t i = 0; i < 6; ++i) if (d[i]) { moved = true; break; }
    // Any input skips the rest of the boot sequence. It is decoration; the
    // hardware is already live behind it, so there is nothing to wait for.
    if (moved && bootActive) { bootFinish(); }
    else if (moved && sysState == SYS_WIZARD) { wizEncoders(d); uiTouch(); }
    else if (moved && sysState == SYS_RUN) {
      const UiPageDef& pg = kUiPages[ui.page];
      if (ui.inSub && pg.subEnc) pg.subEnc(d);
      else                       pg.enc(d);
      uiTouch();
    }
  }

  // ---- switches at 200 Hz ----
  static uint32_t swNext = 0;
  if ((int32_t)(nowUs - swNext) >= 0) {
    swNext = nowUs + 5000;
    buttonsScan();
    if (btnDown && bootActive) { bootFinish(); }

    // GPA6, the on-board tactile button, is the whole navigation model:
    //   short click   next top-level page
    //   hold 0.5 s    enter or leave this page's subpage
    //
    // The inline b6DownMs / b6Done block that used to live here is now
    // ButtonGesture, which adds two things it did not have: a progress read-out
    // for the on-screen bar, and an explicit `valid` input so a dropped I2C
    // transaction cannot be read as a release. buttonsScan() reports no edges
    // on a bus error, and gpaOk carries that through.
    {
      const bool down  = (btnLevel & (1 << 6)) != 0;
      const uint8_t g  = gBtn.update(down, gpaOk, nowMs);
      const uint8_t rng = gBtn.holdProgress(nowMs);
      if (rng != uiHoldRing) { uiHoldRing = rng; uiTouch(); }

      // v1.16 - the two-layer state machine. ButtonGesture already
      // guarantees: a CLICK only on release, never on the release that ends a
      // hold; a HOLD exactly once, while still down, at BTN_HOLD_MS; nothing
      // from a debounce glitch or a failed I2C read.
      //
      //            click                     hold 0.5 s
      //   MAIN     next page                 enter this page's sub-pages
      //   SUB      next sub-page (ring)      back to MAIN on this page
      if      (g == BTN_HOLD)  uiBoardHold();
      else if (g == BTN_CLICK) uiBoardClick();
    }

    // Encoder pushes fire on RELEASE, and only when the knob did not turn while
    // it was down - see encTurnedHeld. That is what makes the coarse gesture
    // usable: a push-and-sweep must not end in a button press.
    //
    // The arm bit is cleared on the PRESS edge as well as the release. If a
    // release edge is ever lost - one dropped I2C read, which buttonsScan
    // deliberately refuses to paper over - a bit left armed would silently
    // swallow the next clean click. Clearing on press makes it self-healing.
    encTurnedHeld &= (uint8_t)~(btnDown & 0x3F);
    for (uint8_t i = 0; i < 6; ++i) {
      const uint8_t bit = (uint8_t)(1u << i);
      if (!(btnUp & bit)) continue;
      if (!(encTurnedHeld & bit)) encClick |= bit;
      encTurnedHeld &= (uint8_t)~bit;
    }
    if (encClick) {
      if (bootActive) bootFinish();
      else if (sysState == SYS_RUN) {
        const UiPageDef& pg = kUiPages[ui.page];
        if (ui.inSub && pg.subBtn) pg.subBtn(encClick);
        else                       pg.btn(encClick);
      }
      encClick = 0;
      uiTouch();
    }
  }

  // ---- HUNT: walk the address space so you can SEE which one lands ----
  if (huntMode && (int32_t)(nowMs - huntNextMs) >= 0) {
    if (huntMode == 1) {                       // NRPN parameter index
      huntNextMs = nowMs + 2000;
      yyOverride = (int16_t)huntPos;
      mnmOut.resend();
      Serial.printf("HUNT  yy 0x%02X  (%s / %s)   <- moving on the MnM?\n",
          (unsigned)huntPos, kMnmPages[huntPos / 8].name,
          mnmParamName(huntPos / 8, huntPos % 8));
      huntPos = (uint16_t)((huntPos + 1) & 0x1F);
      if (huntPos == 0) { huntMode = 0; yyOverride = -1;
                          Serial.println("HUNT done - nothing? try mode 1, mode 2, or hc"); }
    } else {                                   // plain CC
      huntNextMs = nowMs + 1200;
      txCC = (uint8_t)huntPos;
      yyOverride = (int16_t)huntPos;
      mnmOut.resend();
      Serial.printf("HUNT  CC %u   <- moving on the MnM?\n", (unsigned)huntPos);
      huntPos++;
      if (huntPos > 127) { huntMode = 0;
                           Serial.println("HUNT done - no CC moved anything"); }
    }
  }

  // ---- live monitor ----
  if (monitorOn) {
    static uint32_t monNext = 0;
    if ((int32_t)(nowMs - monNext) >= 0 && usbReady(128)) {
      monNext = nowMs + 100;
      const uint64_t pos = clk.position();
      Serial.printf("%s pos %lu.%03lu  L1 %3u  addr %3u  tx %luB  rx %luB  clk %lu\n",
          clk.running() ? "RUN " : "STOP",
          (unsigned long)(pos >> 32),
          (unsigned long)(((pos & 0xFFFFFFFFull) * 1000ull) >> 32),
          (manualVal >= 0) ? (unsigned)manualVal : (unsigned)lfo.s[0].value,
          activeYY(), (unsigned long)txBytesOut,
          (unsigned long)rxBytes, (unsigned long)rxClock);
    }
  }

  // The settings page is the only one whose content moves without input, and
  // it is deliberately NOT marked animated - so it gets ticked here instead of
  // redrawing thirty times a second for digits nobody is watching that closely.
  if (sysState == SYS_RUN && ui.page == UPAGE_SETTINGS && !ui.inSub && !bootActive) {
    static uint32_t setTickMs = 0;
    if ((int32_t)(nowMs - setTickMs) >= 0) {
      setTickMs = nowMs + UI_SET_TICK_MS;
      uiTouch();
    }
  }

  // ---- display ----
  // 60 Hz through the boot animation, 30 Hz after. Neither number gates MIDI:
  // the flush pumps both directions every 256 bus bytes, and a skipped redraw
  // simply hands its slice back to the loop.
  // v1.08: a steady 60 Hz everywhere, scheduled drift-free (next = previous
  // deadline + period, not now + period), so frames arrive evenly spaced. Even
  // spacing matters as much as rate: 30 Hz with jitter reads as stutter, 60 Hz
  // on a fixed grid reads as motion. If the loop falls a whole period behind
  // it re-syncs instead of bursting frames to catch up.
  static uint32_t drawNext = 0, lastFrameUs = 0;
  if ((int32_t)(nowUs - drawNext) >= 0) {
    // v1.09: 30 Hz. Half the bus traffic of v1.08 on animated pages, and the
    // screen has been stable at this rate.
    // v1.16: 60 Hz WHILE A TRANSITION RUNS. At 30 Hz an eased 220 ms dissolve
    // showed ~3 in-between frames and read as a plain cut; the transition demo
    // runs at 60 and reads as a dissolve. Only for the few hundred ms of a
    // transition - MIDI is still pumped inside every flush.
    const uint32_t periodUs = g_transKind ? 16667u : 33333u;
    drawNext += periodUs;
    if ((int32_t)(nowUs - drawNext) >= 0) drawNext = nowUs + periodUs;
    // THE YIELD. Rendering is the only task in this loop with no deadline at
    // all: a dropped frame is a frame nobody saw, a dropped clock byte is
    // audible. So when the receive buffer is backing up - a kit dump, a burst
    // of clock at 10x - the renderer stands down for one frame and hands the
    // whole slice to MIDI. This is the piece that was missing.
    if (Serial1.available() > MIDI_BACKLOG_YIELD) {
      gStats.frameSkipsBacklog++;
    } else if (bootActive) {
      const uint32_t e = (uint32_t)bootMs;
      if (e >= BOOT_TOTAL_MS) { bootFinish(); }
      else { drawBoot(e); flushAll(); }
    } else if (sysState == SYS_WIZARD) {
      // v1.15: a completed dump moves the wizard on by itself, as it always
      // claimed to. SKIP still works for a machine that sends nothing.
      if (wizState == WIZ_SYSEX_WAIT && sxGotDump) { wizState = WIZ_SYSEX_OK; uiDirty = true; }
      // Only the "waiting for a dump" page animates; the rest are static and
      // have no business costing a draw and a flush thirty times a second.
      if (wizState == WIZ_SYSEX_WAIT || wizState == WIZ_SAVE || uiDirty) {
        uiDirty = false;
        drawWizard(nowMs);
        flushAll();
      }
    } else if (sysState == SYS_FAULT) {
      if (uiDirty) { drawFault(); flushAll(); uiDirty = false; }
    } else if (sysState == SYS_LOADING) {
      if (uiDirty) { gfx.clear();
                     gfx.textBold(0, 110, "LOAD", SH_ON);
                     gfx.meter(0, 126, UI_W, 7, (int16_t)HalEeprom::progress(),
                               (int16_t)sizeof(PresetP), SH_MID);
                     flushAll(); uiDirty = false; }
      uiTouch();                          // the bar is live, so keep it moving
    } else if (rowTestMode) {
      drawRowTest();                     // draws and pushes its own frame
    } else if (rulerMode) {
      drawRuler();                       // draws and pushes its own frame
    } else {
      const UiPageDef& pg = kUiPages[ui.page];
      const bool sub = ui.inSub && pg.subDraw != nullptr;
      // v1.12: PERF is a static page - except while an LFO drives one of its
      // slots, when the fader has to move with it. Unchanged frames are still
      // never sent, so this costs nothing when nothing moves.
      const bool anim = (sub ? pg.subAnimated : pg.animated) ||
                        (!sub && ui.page == UPAGE_PERF && perfAnyLfoLinked());

      // v1.08: an animated page draws every frame (60 Hz). A static page draws
      // when something changed OR while anything on it is still in motion - a
      // fader settling, the cursor gliding, a page sliding in - and then goes
      // quiet again. The old 250 ms hash floor is gone: it is what made the
      // scopes and playheads step instead of move.
      static uint8_t lastPage = 0xFF, lastSel = 0, lastPerfSub = 0;
      static bool    lastSub  = false;
      // v1.16: moving between LFO EDIT instances on the sub-page ring is a
      // page change to the eye, so it animates like one.
      const bool pageChg = (ui.page != lastPage), subChg = (ui.inSub != lastSub);
      const bool instChg = !pageChg && !subChg && ui.inSub &&
                           ((ui.page == UPAGE_LFO && ui.sel != lastSel) ||
                            (ui.page == UPAGE_PERF && g_perfSub != lastPerfSub));
      if (lastPage != 0xFF && (pageChg || subChg || instChg)) {
        // v1.17: main page to main page is PAGES; anything that starts or
        // ends on a sub-page - in, out, or along the ring - is SUB.
        const bool mainMove = pageChg && !ui.inSub && !lastSub;
        switch (mainMove ? uiTransStyle : uiTransSub) {
          case TS_SLIDE:
            if (mainMove) {
              const uint8_t d = (uint8_t)((ui.page + UPAGE_COUNT - lastPage) % UPAGE_COUNT);
              transBegin((d == UPAGE_COUNT - 1) ? -1 : +1, MT_SLIDE_MS);
            } else if (subChg) transBegin(2, MT_FADE_MS);
            else               transBegin(+1, MT_SLIDE_MS);
            break;
          case TS_WIPE:     transBegin(3, 120); break;
          case TS_DISSOLVE: transBegin(4, 240); break;   // smoothed: see compose
          case TS_FLASH:    transBegin(5, 47);  break;
          default:          g_transFramesLast = 0; break;   // TS_CUT: instant
        }
        if (g_transKind) drawNext = nowUs + 16667u;      // 60 Hz from frame one
      }
      lastPage = ui.page; lastSub = ui.inSub; lastSel = ui.sel;
      lastPerfSub = g_perfSub;

      const bool wantDraw = uiDirty || anim || g_motionBusy || (g_transKind != 0);
      if (wantDraw) {
        const uint32_t bpmX100 = (uint32_t)(clk.bpm() * 100.0f);
        const uint32_t step16  = (uint32_t)(clk.position() >> 30);
        // Frame delta for the motion layer. After an idle stretch the first
        // frame uses one nominal period, so a glide starts gently instead of
        // jumping the whole idle time in one step.
        float dt = lastFrameUs ? (float)(nowUs - lastFrameUs) * 1e-6f : (1.0f / 60.0f);
        if (dt > 0.05f || dt <= 0.0f) dt = 1.0f / 60.0f;
        g_dt = dt; lastFrameUs = nowUs;
        g_motionBusyNext = false;
        // Cleared BEFORE the flush, not after. flushAll() pumps MIDI from
        // inside its inner loop, so an incoming CC can call uiTouch() while we
        // are still pushing pixels. Clearing afterwards would swallow that
        // touch and leave the page stale until the next input.
        uiDirty = false;
        if (sub) pg.subDraw(step16, clk.running(), bpmX100);
        else     pg.draw(step16, clk.running(), bpmX100);
        // v1.16: hold feedback on the bottom row, only where the hold DOES
        // something (in a sub-page always; on a main page only if it has
        // sub-pages) - a bar that fills and then does nothing would lie.
        if (uiHoldRing && (ui.inSub || uiHasSub()))
          gfx.hLine(0, 255, (int16_t)(((uint32_t)uiHoldRing * 64u) / 255u), SH_ON);
        transCompose(nowUs);
        g_motionBusy = g_motionBusyNext;
        flushAll();
      }
    }
  }

  // ---- one-second TX rate, so a speed change shows up as a number ----
  {
    static uint32_t rateAt = 0, rateMark = 0;
    if ((int32_t)(nowMs - rateAt) >= 0) {
      rateAt = nowMs + 1000;
      txRate = txBytesOut - rateMark;
      rateMark = txBytesOut;
    }
  }

  // ---- tempo LED (D1, GPB4) ----
  // Reads freePos while stopped, which always advances at the current tempo,
  // and the locked position while running so the blink lands on the
  // Monomachine's own downbeat. Page LEDs follow the page, so the box tells you
  // which one you are on without looking at the screen.
  {
    const uint64_t src = clk.running() ? clk.position() : freePos;
    const uint32_t beat = (uint32_t)(src >> 32);
    static uint32_t lastBeat = 0xFFFFFFFFu;
    if (beat != lastBeat) { lastBeat = beat; ledOnUntil = nowMs + 120; }
    const bool want = (int32_t)(nowMs - ledOnUntil) < 0;
    // One LED per top-level page, in page order - D2 D3 D4 D5 on GPB0..GPB3.
    // In a subpage that LED blinks, so the panel tells you which LEVEL you are
    // on as well as which page, without spending a fifth LED on it.
    static const uint8_t kPageLed[UPAGE_COUNT] =
        {LED_PAGE1, LED_PAGE2, LED_PAGE3, LED_PAGE4};
    uint8_t pageBit = kPageLed[ui.page < UPAGE_COUNT ? ui.page : 0];
    if (ui.inSub && ((nowMs >> 8) & 1u)) pageBit = 0;
    ledSet((uint8_t)(pageBit | (want ? LED_TEMPO : 0)));
  }

  // Worst-case pass time. With the RX buffer high-water mark on the same page
  // these two are the whole health picture: if LOOP US climbs past ~1500 the
  // non-blocking argument has a hole in it, and the pass that did it is almost
  // always a full-frame display flush.
  gStats.note(micros() - nowUs);

#if DEBUG_PRINT
  static uint32_t dbgNext = 0;
  if (!monitorOn && (int32_t)(nowMs - dbgNext) >= 0 && usbReady(96)) {
    dbgNext = nowMs + 2000;
    Serial.print("bpm ");    Serial.print(clk.bpm(), 2);
    Serial.print(clk.locked() ? " lock" : " ----");
    Serial.print(clk.running() ? " RUN " : " STOP");
    Serial.print(" tx ");    Serial.print(txBytesOut);
    Serial.print("B  L1 ");  Serial.print((unsigned)lfo.s[0].value);
    Serial.print(clk.running() || lfo.driving(0) ? "" : " (idle)");
    Serial.println();
  }
#endif
}
