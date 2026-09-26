// v1.22: the Monomachine parameter map (in the sketch since v1.24) and the kit
// model built on it - SYNT names per machine, values as the machine shows
// them, init-kit centres and ranges, 'mach', 'kit' and 'kit watch', and old
// presets (machine numbers, wave order, MULT) translated on load.
// run.sh builds this twice: as is, and with -DMNM_LIST_SPREAD=1, because the
// open question in the header is which of the two the machine uses.
// Exit code 0 when every check passes.
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

void run(uint32_t ms) {
  const uint64_t end = sim::nowUs() + (uint64_t)ms * 1000u;
  while (sim::nowUs() < end) { loop(); sim::nowNs += 20000; }
}

// Everything printed since the last call.
std::string out() { std::string s = Serial.captured; Serial.captured.clear(); return s; }
bool has(const std::string& s, const char* what) { return s.find(what) != std::string::npos; }

std::string cmd(const char* c) { out(); handleCommand(c); run(20); return out(); }

// A CC from the Monomachine on 0-based channel ch, then time for the parser.
void cc(uint8_t ch, uint8_t n, uint8_t v) {
  Serial1.inject((uint8_t)(0xB0 | ch)); Serial1.inject(n); Serial1.inject(v);
  run(5);
}

std::string fmt(const MnmParam& p, uint8_t raw) { char b[6]; mnmFormat(p, raw, b); return b; }
std::string pfmt(uint8_t pg, uint8_t sl, uint8_t t, uint8_t raw) {
  char b[8]; paramFormat(pg, sl, t, raw, b); return b;
}
uint8_t listRaw(uint8_t list, uint8_t idx) { return mnmListRaw(idx, kMnmLists[list].n); }

void headerChecks() {
  bool ok = true;
  for (uint8_t l = 0; l < ML_COUNT; ++l)
    for (uint8_t i = 0; i < kMnmLists[l].n; ++i) if (!kMnmLists[l].items[i][0]) ok = false;
  check(ok, "map: no empty entry in any list");
  check(kMnmLists[ML_FMRATIO].n == 24 && kMnmLists[ML_DYNRATIO].n == 128 &&
        kMnmLists[ML_INTERVAL].n == 33 && kMnmLists[ML_PHONEME].n == 21 &&
        kMnmLists[ML_LFOPAGE].n == 9 && kMnmLists[ML_LFOWAVE].n == 11 &&
        kMnmLists[ML_LFOMULT].n == 7 && kMnmLists[ML_LFOTRIG].n == 5,
        "map: list sizes as surveyed");
  check(!strcmp(kMnmInterval[MNM_INTERVAL_OFF], "OFF"), "map: interval OFF is the middle entry");
  check(MNM_MACHINE_COUNT == 22, "map: 22 machines");

  // FM+DYN ratios: raw/64, except raw 127 = 2.0.
  ok = true;
  for (int r = 0; r < 128; ++r) {
    const char* s = kMnmDynRatio[r];
    double v; int a, b;
    if (sscanf(s, "%d/%d", &a, &b) == 2) v = (double)a / b; else v = atof(s);
    if (fabs(v - (r == 127 ? 2.0 : r / 64.0)) > 0.011) { ok = false; printf("  dyn %d = %s\n", r, s); }
  }
  check(ok, "map: every FM+DYN ratio is raw/64");

  // Defaults inside their ranges; list raw <-> index round-trips; labels unique.
  ok = true;
  const MnmParam* pages[] = {kMnmAmpP, kMnmFiltP, kMnmEffxP, kMnmLfoP};
  auto defOk = [&](const MnmParam& p) {
    const uint8_t n = mnmParamCount(p);
    if (n ? p.def >= n : p.def > 127) { ok = false; printf("  default %s %u\n", p.name, p.def); }
  };
  for (const MnmParam* pg : pages) for (int s = 0; s < 8; ++s) defOk(pg[s]);
  for (uint8_t m = 0; m < MNM_MACHINE_COUNT; ++m) for (int s = 0; s < 8; ++s) defOk(kMnmMachines[m].p[s]);
  check(ok, "map: every default is inside its range");

  ok = true;
  for (uint8_t l = 0; l < ML_COUNT; ++l) {
    const uint8_t n = kMnmLists[l].n;
    for (uint8_t i = 0; i < n; ++i) if (mnmListIndex(mnmListRaw(i, n), n) != i) ok = false;
    uint8_t prev = 0;
    for (int r = 0; r < 128; ++r) {                 // every raw lands in the list, in order
      const uint8_t i = mnmListIndex((uint8_t)r, n);
      if (i >= n || i < prev) ok = false;
      prev = i;
    }
  }
  check(ok, "map: list index <-> raw round-trips, and no raw falls off a list");

  ok = true;
  for (uint8_t a = 0; a < MNM_MACHINE_COUNT; ++a)
    for (uint8_t b = (uint8_t)(a + 1); b < MNM_MACHINE_COUNT; ++b)
      if (!strcmp(kMnmMachines[a].label, kMnmMachines[b].label) ||
          !strcmp(kMnmMachines[a].name, kMnmMachines[b].name)) ok = false;
  check(ok, "map: machine labels and names are unique");
  check(mnmFindMachine("FM+", "DYN") == 10 && mnmFindMachine(nullptr, "6581") == 4 &&
        mnmFindMachine("FX", "6581") == 0, "map: mnmFindMachine");

  // Formatting.
  check(fmt(kMnmAmpP[6], 0) == "-64" && fmt(kMnmAmpP[6], 64) == "0" &&
        fmt(kMnmAmpP[6], 127) == "63", "format: bipolar PAN is raw-64");
  check(fmt(kMnmAmpP[5], 127) == "127" && fmt(kMnmAmpP[5], 0) == "0", "format: plain VOL");
  const MnmMachine& sid = kMnmMachines[mnmFindMachine("SID", "6581") - 1];
  check(fmt(sid.p[3], listRaw(ML_SIDWAVE, 2)) == "PULS" &&
        fmt(sid.p[3], listRaw(ML_SIDWAVE, 4)) == "NOIS", "format: SID WAVE by list");
  check(fmt(sid.p[2], listRaw(ML_ONOFF, 1)) == "ON", "format: on/off");
  const MnmMachine& ddrw = kMnmMachines[mnmFindMachine("DPRO", "DDRW") - 1];
  check(fmt(ddrw.p[0], mnmListRaw(0, 64)) == "D01" && fmt(ddrw.p[0], mnmListRaw(63, 64)) == "D64",
        "format: DDRW wave slots D01..D64");
  const MnmMachine& dyn = kMnmMachines[mnmFindMachine("FM+", "DYN") - 1];
  check(fmt(dyn.p[0], 64) == "1.0" && fmt(dyn.p[0], 127) == "2.0", "format: FM+DYN ratio");
  check(mnmDefaultRaw(dyn.p[4]) == 85 && fmt(dyn.p[4], mnmDefaultRaw(dyn.p[4])) == "1.32",
        "format: FM+DYN 2FRQ default");
  check(fmt(kMnmLfoP[0], listRaw(ML_LFOPAGE, 8)) == "MIDI", "format: LFO PAGE");
}


// Re-seal gRec the way packInto() does: CRC computed with its field at 0.
void reseal() {
  gRec.hdr.crc = 0;
  gRec.hdr.crc = xy6Crc16((const uint8_t*)&gRec + 12, gRec.hdr.size);
}

}  // namespace

int main() {
  headerChecks();

  sim::adc[JOY_PIN_X] = sim::adc[JOY_PIN_Y] = 512;
  setup();
  bootActive = false; sysState = SYS_RUN; ui.page = UPAGE_PERF; ui.inSub = false;
  run(200);
  const uint8_t SID = mnmFindMachine("SID", "6581"), DYN = mnmFindMachine("FM+", "DYN");

  // ---- names --------------------------------------------------------------
  check(!strcmp(mnmParamName(PAGE_EFFX, 6), "DBAS") && !strcmp(mnmParamName(PAGE_EFFX, 7), "DWID"),
        "names: EFFX 7-8 are DBAS / DWID, as surveyed");

  // ---- the init kit ---------------------------------------------------------
  check(kitGet(0, PAGE_AMP, 0) == 0 && kitGet(0, PAGE_FILT, 1) == 127 &&
        kitGet(0, PAGE_EFFX, 5) == 28 && kitGet(0, PAGE_AMP, 6) == 64 &&
        !kitLearned(0, PAGE_FILT, 1), "kit: boots at the init kit (ATCK 0, WDTH 127, DFB 28), none learned");
  check(lfo.p[0].page == PAGE_AMP && lfo.p[0].dest == 0 && lfo.restValue(0) == 0 &&
        lfo.p[1].dest == 2 && lfo.restValue(1) == 64,
        "LFO centre: the boot LFOs swing around ATCK 0 / DEC 64, not a blanket 64");

  // ---- mach ---------------------------------------------------------------
  std::string o = cmd("mach");
  check(has(o, "SID-6581") && has(o, "FX-FLNG") && has(o, "22 FX-FLNG"), "mach: lists all 22 machines");
  o = cmd("mach 1 sid-6581");
  check(machineSel[0] == SID && has(o, "T1 = SID-6581"), "mach: 'mach 1 sid-6581' sets the SID");
  cmd("mach 2 dyn");
  check(machineSel[1] == DYN, "mach: by short name, any case");
  o = cmd("mach 3 fx");
  check(machineSel[2] == 0 && has(o, "no machine"), "mach: a family name alone is refused");
  o = cmd("mach 7 sid-6581");
  check(has(o, "mach <1-6>"), "mach: track out of range is refused");
  cmd("mach 4 12");
  check(machineSel[3] == 12, "mach: by number");
  cmd("mach 4 0");
  check(machineSel[3] == 0, "mach: 0 clears");
  check(kitGet(0, PAGE_SYNT, 2) == listRaw(ML_ONOFF, 1) && !kitLearned(0, PAGE_SYNT, 2),
        "mach: the SYNT page takes the machine's init values (SID PWRS ON)");

  // ---- names and values per machine ---------------------------------------
  lfo.p[5].track = 0; lfo.p[5].page = PAGE_SYNT; lfo.p[5].dest = 3;
  check(!strcmp(destName(5), "WAVE"), "LFO dest: T1 SYNT slot 4 on the SID is WAVE");
  lfo.p[5].track = 1; lfo.p[5].dest = 0;
  check(!strcmp(destName(5), "1FRQ"), "LFO dest: T2 SYNT slot 1 on FM+DYN is 1FRQ");
  lfo.p[5].track = 2; lfo.p[5].dest = 3;
  check(!strcmp(destName(5), "SYN4"), "LFO dest: no machine chosen falls back to SYN4");
  lfo.p[5].track = 7;
  check(!strcmp(destName(5), "SYN4"), "LFO dest: a corrupt track index is safe");
  lfo.p[5].track = 0; lfo.p[5].page = PAGE_AMP; lfo.p[5].dest = 5;
  check(pfmt(PAGE_AMP, 6, 0, 52) == "-12", "value: PAN 52 shows as -12");
  check(pfmt(PAGE_SYNT, 3, 0, listRaw(ML_SIDWAVE, 1)) == "SAW", "value: SID WAVE shows its word");
  check(pfmt(PAGE_SYNT, 3, 2, 99) == "99", "value: no machine shows the number");
  check(pfmt(PAGE_SYNT, 0, 1, 64) == "1.0", "value: FM+DYN 1FRQ 64 is 1.0");
  check(paramRawMax(PAGE_SYNT, 3, 0) == listRaw(ML_SIDWAVE, 4) && paramRawMax(PAGE_AMP, 5, 0) == 127,
        "range: SID WAVE ends at its fifth entry, VOL at 127");

  // ---- kit watch: CCs from the machine --------------------------------------
  o = cmd("kit watch");
  check(kitWatch, "kit watch: on");
  out();
  cc(0, 51, listRaw(ML_SIDWAVE, 2));                  // T1 SYNT slot 4
  cc(0, 62, 80);                                      // T1 AMP PAN
  cc(0, 88, listRaw(ML_LFOPAGE, 3));                  // T1 LFO1 PAGE = FILT
  cc(0, 89, listRaw(ML_LFODEST, 2));                  // T1 LFO1 DEST = 3rd FILT slot
  cc(1, 48, 64);                                      // T2 FM+DYN 1FRQ
  cc(7, 51, 9);                                       // channel 8: not a track
  run(30);
  o = out();
  check(has(o, "kit T1 SYNT WAVE") && has(o, "= PULS"), "kit watch: SID WAVE named, word shown");
  check(has(o, "kit T1 AMP  PAN") && has(o, "= 16"), "kit watch: PAN raw 80 = 16");
  check(has(o, "LFO1 PAGE") && has(o, "= FILT"), "kit watch: LFO1 PAGE = FILT");
  check(has(o, "LFO1 DEST") && has(o, "= HPQ"), "kit watch: LFO1 DEST named from its PAGE");
  check(has(o, "kit T2 SYNT 1FRQ") && has(o, "= 1.0"), "kit watch: T2 FM+DYN 1FRQ = 1.0");
  check(!has(o, "kit T8") && kitGet(0, PAGE_SYNT, 3) == listRaw(ML_SIDWAVE, 2) &&
        kitLearned(0, PAGE_SYNT, 3), "kit: learned from the machine; other channels ignored");
  printf("---- kit watch sample ----\n%s--------------------------\n", o.c_str());
  cmd("kit watch");
  check(!kitWatch, "kit watch: off again");

  // ---- kit sheet ----------------------------------------------------------
  o = cmd("kit 1");
  check(has(o, "T1  SID-6581") && has(o, "WAVE PULS *") && has(o, " PAN 16   *") &&
        has(o, "DEST HPQ  *") && has(o, "WDTH 127   ") && !has(o, "WDTH 127  *"),
        "kit 1: named values, * on the learned ones only");
  cc(0, 104, listRaw(ML_LFOPAGE, 1));                 // T1 LFO2 PAGE = SYNT
  cc(0, 105, listRaw(ML_LFODEST, 3));                 // T1 LFO2 DEST = SYNT slot 4
  check(pfmt(PAGE_LFO2, 1, 0, listRaw(ML_LFODEST, 3)) == "WAVE",
        "value: an MnM LFO aimed at SYNT names the machine's parameter");
  kitSet(0, PAGE_LFO3, 0, listRaw(ML_LFOPAGE, 0));
  check(pfmt(PAGE_LFO3, 1, 0, listRaw(ML_LFODEST, 7)) == "160CT", "value: PTCH-page LFO dest");

  // ---- echoes: an LFO's own swing must not land in the kit ------------------
  {
    kitResetAll();
    LfoParams& L = lfo.p[4];
    L.enabled = true; L.trig = TRIG_FREE; L.track = 0; L.page = PAGE_FILT; L.dest = 0;
    L.depth = 60; L.spd = 127; L.mult = 3;
    lfoRetarget(4);
    run(200);
    const uint8_t sent = mnmOut.sentValue(4);
    check(sent != 0xFF, "echo: the LFO is on the wire");
    cc(0, 72, sent);                                   // our own value, echoed
    check(!kitLearned(0, PAGE_FILT, 0), "echo: an echo of the LFO does not become the patch");
    cc(0, 72, (uint8_t)((sent + 40) & 0x7F));          // a person at the machine
    check(kitLearned(0, PAGE_FILT, 0), "echo: a real knob move does");
    L.enabled = false; run(100);
  }
  {
    txMode = 0;                                        // NRPN: nothing comes back as CC
    cc(2, 61, 99);
    check(kitGet(2, PAGE_AMP, 5) == 99 && kitLearned(2, PAGE_AMP, 5), "kit: NRPN mode learns every CC");
    txMode = 3;
  }
  o = cmd("kit init");
  check(!kitLearned(0, PAGE_FILT, 0) && !kitLearned(2, PAGE_AMP, 5) && kitGet(2, PAGE_AMP, 5) == 64,
        "kit init: back to the init kit");

  // ---- LFO centre follows the destination ----------------------------------
  {
    lfo.setCapture(0, 99);                            // a capture from the OLD destination
    cmd("pg 2"); cmd("ds 1");                         // LFO 1 -> FILT WDTH
    check(lfo.restValue(0) == 127 && !lfo.s[0].hasCap,
          "retarget: WDTH swings around 127, the old capture dropped");
    cmd("ds 5");                                      // FILT DEC, init 32
    check(lfo.p[0].baseValue == 32, "retarget: a new destination drops the old centre");
    kitSet(0, PAGE_FILT, 0, 90);
    cmd("ds 0");
    check(lfo.restValue(0) == 90 && lfo.s[0].hasCap, "retarget: a learned value is the centre");
  }
  {
    // A list destination: the output never passes its last entry.
    LfoParams& L = lfo.p[3];
    L.enabled = true; L.trig = TRIG_FREE; L.track = 0; L.page = PAGE_SYNT; L.dest = 3;
    L.depth = 127; L.spd = 127; L.mult = 4; L.lo = 0; L.hi = 127;
    lfoRetarget(3);
    uint8_t mx = 0;
    for (int k = 0; k < 200; ++k) { run(2); if (lfo.s[3].value > mx) mx = lfo.s[3].value; }
    check(mx == paramRawMax(PAGE_SYNT, 3, 0), "range: an LFO on SID WAVE stops at NOIS");
    L.enabled = false; run(100);
  }

  // ---- PERF -----------------------------------------------------------------
  {
    perfSlot[0].page = PAGE_AMP; perfSlot[0].dest = 6; perfSlot[0].track = 0;
    perfSlot[0].value = 10; perfSlot[0].known = true;
    btnPerform(0x01);
    check(perfSlot[0].value == 64, "PERF: a push returns PAN to centre, not hard left");
    perfSlot[0].page = PAGE_SYNT; perfSlot[0].dest = 3; perfSeed(0);
    const int8_t d[6] = {127, 0, 0, 0, 0, 0};
    encPerform(d);
    check(perfSlot[0].value == paramRawMax(PAGE_SYNT, 3, 0), "PERF: a list knob stops at its last entry");
    check(kitLearned(0, PAGE_SYNT, 3), "PERF: what we send is what the kit model holds");
    run(50);
  }

  // ---- machine change re-reads the SYNT page ---------------------------------
  {
    kitSet(0, PAGE_SYNT, 0, 77);
    lfo.p[2].track = 0; lfo.p[2].page = PAGE_SYNT; lfo.p[2].dest = 0;
    applyMachine(0, DYN);
    check(!kitLearned(0, PAGE_SYNT, 0) && kitGet(0, PAGE_SYNT, 0) == 64 &&
          lfo.p[2].baseValue == 64, "machine change: SYNT back to the new machine's init values");
    applyMachine(0, SID);
  }

  // ---- scenes tell the kit model what they sent -------------------------------
  {
    patSelectGenre(PAT_FROST, 1);
    sceneApply();
    check(kitGet(0, PAGE_FILT, 0) == 30 && kitLearned(0, PAGE_FILT, 0), "scene: its sound lands in the kit model");
    run(50);
  }

  // ---- the stick and pattern locks tell the kit model what they sent ---------
  {
    kitResetAll();
    // No LFO on FILT BASE (the FROST scene put one there): a driving LFO takes
    // the stick as its centre instead, and nothing is sent.
    for (uint8_t i = 0; i < LFO_COUNT; ++i) lfo.p[i].enabled = false;
    run(100);
    g_joyMask = 0x01;                                   // T1, X = FILT BASE
    sim::adc[JOY_PIN_X] = 150;                          // push (inverted axis)
    run(300);
    const uint8_t sx = g_joySentX[0];
    check(sx != 0xFF && kitLearned(0, PAGE_FILT, 0) && kitGet(0, PAGE_FILT, 0) == sx,
          "stick: what it sends is what the kit model holds");
    sim::adc[JOY_PIN_X] = 512;
    run(1500);
    check(kitGet(0, PAGE_FILT, 0) == g_joySentX[0], "stick: back at rest, the kit follows");
    g_joyMask = 0;
    patSendParam(2, PAGE_EFFX, 4, 99);                 // a lock...
    check(kitGet(2, PAGE_EFFX, 4) == 99, "lock: the locked value is held");
    patSendParam(2, PAGE_EFFX, 4, 12);                 // ...and its restore
    check(kitGet(2, PAGE_EFFX, 4) == 12, "lock: the restore puts the kit back on the base");
    run(50);
  }

  // ---- presets ----------------------------------------------------------------
  check(legacyMachineId(1) == SID && legacyMachineId(4) == mnmFindMachine("FM+", "STAT") &&
        legacyMachineId(9) == mnmFindMachine("VO-6", "VO6") &&
        legacyMachineId(12) == mnmFindMachine("DPRO", "BBOX") && legacyMachineId(0) == 0 &&
        legacyMachineId(13) == 0 && legacyMachineId(200) == 0,
        "presets: old machine numbers map to the real machines");
  {
    kitResetAll();
    kitSet(0, PAGE_FILT, 0, 33);
    lfo.p[0].wave = WAVE_RMP; lfo.p[0].mult = 5;
    Store::packInto(0);
    kitResetAll(); machineSel[0] = machineSel[1] = 0; lfo.p[0].wave = WAVE_TRI;
    check(Store::unpackFrom(), "presets: a saved preset loads");
    check(machineSel[0] == SID && machineSel[1] == DYN, "presets: machines come back");
    check(kitGet(0, PAGE_FILT, 0) == 33 && kitLearned(0, PAGE_FILT, 0) &&
          !kitLearned(0, PAGE_FILT, 1) && kitGet(0, PAGE_FILT, 1) == 127,
          "presets: learned values come back, the rest stay init defaults");
    check(lfo.p[0].wave == WAVE_RMP && lfo.p[0].mult == 5, "presets: a v1.22 wave loads as itself");

    // The same record as a v1.21 build wrote it.
    Store::packInto(0);
    for (uint8_t t = 0; t < EE_TRACKS; ++t) gRec.kit[t].flags = 0;
    gRec.kit[0].machineId = 4;                         // v1.21 FMSTATIC
    gRec.lfo[0].flags &= (uint8_t)~LFO_F_V122_WAVE;
    gRec.lfo[0].wave = 6;                              // v1.21 numbering: RMP
    gRec.lfo[1].flags &= (uint8_t)~LFO_F_V122_WAVE;
    gRec.lfo[1].wave = 9;                              // v1.21: EXP mirrored
    gRec.lfo[2].mult = 11;                             // 2048X
    reseal();
    check(Store::unpackFrom(), "presets: a v1.21 record loads");
    check(machineSel[0] == mnmFindMachine("FM+", "STAT"), "presets: v1.21 FMSTATIC loads as FM+STAT");
    check(lfo.p[0].wave == WAVE_RMP && lfo.p[1].wave == WAVE_EXP_M, "presets: v1.21 waves renumbered");
    check(lfo.p[2].mult == LFO_MULT_MAX, "presets: MULT past 64X loads as 64X");
    check(!kitLearned(0, PAGE_FILT, 0), "presets: a v1.21 record carries no kit values");
  }

  printf("---- kit 1 sample ----\n");
  kitResetAll(); applyMachine(0, SID);
  cc(0, 51, listRaw(ML_SIDWAVE, 3)); cc(0, 60, 70); cc(0, 72, 90);
  fputs(cmd("kit 1").c_str(), stdout);
  printf("----------------------\n");

  printf(failures ? "FAIL (%d)\n" : "PASS\n", failures);
  return failures ? 1 : 0;
}
