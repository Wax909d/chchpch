// mnm_params.h — Monomachine parameter map for the REDOT XY6        (v1.22)
//
// Transcribed from Redot's hardware survey (Freeform board "Xy6", box
// "Claude", Sep 2026): every parameter on every page and machine, its
// display range, its init-kit default, and every value list, read off a
// real Monomachine. Survey notation: "(x)" = value after loading a default
// kit, "0-127" / "-64-63" / "on/off" = the parameter's range.
//
// Header-only on purpose: the Arduino IDE generates prototypes for .ino
// files but not for headers, so nothing here trips the prototype-hoist bug.
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
#pragma once
#include <stdint.h>
#include <string.h>

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
