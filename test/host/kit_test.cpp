// v1.22: the Monomachine parameter map (XY6_LFO/mnm_params.h) and what the
// sketch does with it - SYNT names per machine, values as the machine shows
// them, 'mach', 'kit' and 'kit watch', and old presets' machine numbers.
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
std::string kfmt(uint8_t t, uint8_t pg, uint8_t sl, uint8_t raw) {
  char b[6]; kitFormat(t, pg, sl, raw, b); return b;
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

}  // namespace

int main() {
  headerChecks();

  sim::adc[JOY_PIN_X] = sim::adc[JOY_PIN_Y] = 512;
  setup();
  bootActive = false; sysState = SYS_RUN; ui.page = UPAGE_PERF; ui.inSub = false;
  run(200);

  // ---- names --------------------------------------------------------------
  check(!strcmp(mnmParamName(PAGE_EFFX, 6), "DBAS") && !strcmp(mnmParamName(PAGE_EFFX, 7), "DWID"),
        "names: EFFX 7-8 are DBAS / DWID, as surveyed");

  // ---- mach ---------------------------------------------------------------
  std::string o = cmd("mach");
  check(has(o, "SID-6581") && has(o, "FX-FLNG") && has(o, "22"), "mach: lists all 22 machines");
  o = cmd("mach 1 sid");
  check(machineSel[0] == 4 && has(o, "T1 = SID-6581") && has(o, "PWRS WAVE MOD"),
        "mach: 'mach 1 sid' sets the SID and names its SYNT page");
  cmd("mach 2 fmdyn");
  check(machineSel[1] == mnmFindMachine("FM+", "DYN"), "mach: 'fmdyn' finds FM+DYN");
  o = cmd("mach 3 fx");
  check(machineSel[2] == 0 && has(o, "no machine"), "mach: an ambiguous family is refused");
  o = cmd("mach 7 sid");
  check(has(o, "track 1-6"), "mach: track out of range is refused");
  cmd("mach 4 12");
  check(machineSel[3] == 12, "mach: by number");
  cmd("mach 4 none");
  check(machineSel[3] == 0, "mach: 'none' clears");

  // ---- the LFO page names its destination from the track's machine --------
  lfo.p[0].track = 0; lfo.p[0].page = PAGE_SYNT; lfo.p[0].dest = 3;
  check(!strcmp(destName(0), "WAVE"), "LFO dest: T1 SYNT slot 4 on the SID is WAVE");
  lfo.p[0].track = 1; lfo.p[0].dest = 0;
  check(!strcmp(destName(0), "1FRQ"), "LFO dest: T2 SYNT slot 1 on FM+DYN is 1FRQ");
  lfo.p[0].track = 2; lfo.p[0].dest = 3;
  check(!strcmp(destName(0), "SYN4"), "LFO dest: no machine chosen falls back to SYN4");
  lfo.p[0].track = 7;
  check(!strcmp(destName(0), "SYN4"), "LFO dest: a corrupt track index is safe");
  lfo.p[0].track = 0; lfo.p[0].page = PAGE_AMP; lfo.p[0].dest = 2;
  check(kfmt(0, PAGE_AMP, 6, 52) == "-12", "LFO value: PAN 52 shows as -12");
  check(kfmt(0, PAGE_SYNT, 3, listRaw(ML_SIDWAVE, 1)) == "SAW", "LFO value: SID WAVE shows its word");
  check(kfmt(2, PAGE_SYNT, 3, 99) == "99", "LFO value: unknown machine shows the number");

  // ---- kit watch ----------------------------------------------------------
  o = cmd("kit watch");
  check(g_kitWatch && has(o, "MNM_LIST_SPREAD"), "kit watch: on, with the open question explained");
  out();
  cc(0, 51, listRaw(ML_SIDWAVE, 2));                  // T1 SYNT slot 4
  cc(0, 62, 80);                                      // T1 AMP PAN
  cc(0, 88, listRaw(ML_LFOPAGE, 3));                  // T1 LFO1 PAGE = FILT
  cc(0, 89, listRaw(ML_LFODEST, 2));                  // T1 LFO1 DEST = 3rd FILT slot
  cc(1, 48, 64);                                      // T2 FM+DYN 1FRQ
  cc(0, 20, 5);                                       // a CC no page uses
  cc(7, 51, 9);                                       // channel 8: not a track
  run(20);
  o = out();
  check(has(o, "T1 SID-6581 SYNT WAVE  CC 51  raw ") && has(o, "= PULS"),
        "kit watch: SID WAVE named, word shown");
  check(has(o, "AMP  PAN   CC 62  raw  80 = 16"), "kit watch: PAN raw 80 = 16");
  check(has(o, "LFO1 PAGE") && has(o, "= FILT"), "kit watch: LFO1 PAGE = FILT");
  check(has(o, "LFO1 DEST") && has(o, "= HPQ"), "kit watch: LFO1 DEST named from its PAGE");
  check(has(o, "T2 FM+DYN   SYNT 1FRQ") && has(o, "= 1.0"), "kit watch: T2 FM+DYN 1FRQ = 1.0");
  check(has(o, "CC 20  raw   5"), "kit watch: an unmapped CC still prints");
  check(!has(o, "T8") && g_kitCc[0][51] == listRaw(ML_SIDWAVE, 2), "kit watch: other channels ignored");
  printf("---- kit watch sample ----\n%s--------------------------\n", o.c_str());

  // ---- kit sheet ----------------------------------------------------------
  o = cmd("kit 1");
  check(has(o, "T1  SID-6581") && has(o, "WAVE PULS") && has(o, " PAN 16") &&
        has(o, "DEST HPQ") && has(o, "PW ?"), "kit 1: named values, '?' for unheard");
  cc(0, 104, listRaw(ML_LFOPAGE, 1));                 // T1 LFO2 PAGE = SYNT
  cc(0, 105, listRaw(ML_LFODEST, 3));                 // T1 LFO2 DEST = SYNT slot 4 = WAVE
  o = cmd("kit 1");
  check(has(o, "DEST WAVE"), "kit 1: an LFO aimed at SYNT names the machine's parameter");
  o = cmd("kit");
  check(has(o, "T1  SID-6581") && has(o, "T6  ----"), "kit: all six tracks");
  cmd("kit watch");
  check(!g_kitWatch, "kit watch: off again");
  out(); cc(0, 51, 1); run(20);
  check(!has(out(), "WAVE"), "kit watch: nothing prints once off");
  cmd("kit clear");
  o = cmd("kit 1");
  check(has(o, "WAVE ?"), "kit clear: forgets");
  printf("---- kit 1 sample ----\n");
  cc(0, 51, listRaw(ML_SIDWAVE, 3)); cc(0, 60, 70); cc(0, 72, 90);
  fputs(cmd("kit 1").c_str(), stdout);
  printf("----------------------\n");

  // ---- presets: machines survive a save, old numbering is translated ------
  check(machineFromV121(1) == 4 && machineFromV121(9) == mnmFindMachine("VO-6", "VO6") &&
        machineFromV121(12) == mnmFindMachine("DPRO", "BBOX") && machineFromV121(0) == 0 &&
        machineFromV121(13) == 0 && machineFromV121(200) == 0,
        "presets: v1.21 machine numbers map to the real machines");
  // Re-seal gRec the way packInto() does: CRC computed with its field at 0.
  auto reseal = [] {
    gRec.hdr.crc = 0;
    gRec.hdr.crc = xy6Crc16((const uint8_t*)&gRec + 12, gRec.hdr.size);
  };
  Store::packInto(0);
  machineSel[0] = machineSel[1] = 0;
  check(Store::unpackFrom() && machineSel[0] == 4 && machineSel[1] == mnmFindMachine("FM+", "DYN"),
        "presets: a v1.22 preset keeps its machines");
  gRec.kit[0].flags &= (uint8_t)~2u; gRec.kit[0].machineId = 4;       // v1.21 FMSTATIC
  reseal();
  check(Store::unpackFrom() && machineSel[0] == mnmFindMachine("FM+", "STAT"),
        "presets: a v1.21 preset's FMSTATIC loads as FM+STAT");
  gRec.kit[0].flags |= 2; gRec.kit[0].machineId = 99;                // corrupt
  reseal();
  check(Store::unpackFrom() && machineSel[0] == 0, "presets: an out-of-range machine loads as none");

  printf(failures ? "FAIL (%d)\n" : "PASS\n", failures);
  return failures ? 1 : 0;
}
