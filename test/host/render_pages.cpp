// Draws every page of the real sketch, in a busy state, and writes them side
// by side into one PNG - so a layout can be checked, pixel for pixel, without
// flashing the board. Also a smoke test: every page's draw code runs.
//
//   ./run.sh builds and runs it; or by hand:
//   g++ -std=gnu++17 -O1 -Imock render_pages.cpp -o render_pages
//   ./render_pages pages.png [scale]
//
// The PNG shows the framebuffer's 16 greys as they are, 0..15 -> 0..255.
// Reversed video ('invert 1') is applied by the panel driver, not here.
#ifndef SKETCH
#define SKETCH "../../XY6_LFO/XY6_LFO.ino"
#endif
#include SKETCH
#include <stdlib.h>

namespace {

void run(uint32_t ms) {
  const uint64_t end = sim::nowUs() + (uint64_t)ms * 1000u;
  while (sim::nowUs() < end) { loop(); sim::nowNs += 20000; }
}

std::vector<std::vector<uint8_t>> pages;
std::vector<std::string> names;
void grab(const char* name) {
  std::vector<uint8_t> px(64 * 256);
  for (int y = 0; y < 256; ++y)
    for (int x = 0; x < 64; ++x) px[y * 64 + x] = gfx.get((int16_t)x, (int16_t)y);
  pages.push_back(px);
  names.push_back(name);
}

// ---- a minimal PNG writer: 8-bit greyscale, stored (uncompressed) deflate ----
uint32_t crcTab[256];
uint32_t crc32(const uint8_t* p, size_t n, uint32_t c = 0xFFFFFFFFu) {
  for (size_t i = 0; i < n; ++i) c = crcTab[(c ^ p[i]) & 0xFF] ^ (c >> 8);
  return c;
}
void be32(std::vector<uint8_t>& v, uint32_t x) {
  v.push_back((uint8_t)(x >> 24)); v.push_back((uint8_t)(x >> 16));
  v.push_back((uint8_t)(x >> 8));  v.push_back((uint8_t)x);
}
void chunk(FILE* f, const char* type, const std::vector<uint8_t>& data) {
  std::vector<uint8_t> c;
  be32(c, (uint32_t)data.size());
  c.insert(c.end(), type, type + 4);
  c.insert(c.end(), data.begin(), data.end());
  be32(c, crc32(c.data() + 4, c.size() - 4) ^ 0xFFFFFFFFu);
  fwrite(c.data(), 1, c.size(), f);
}
bool writePng(const char* path, int w, int h, const std::vector<uint8_t>& grey) {
  for (uint32_t i = 0; i < 256; ++i) {
    uint32_t c = i;
    for (int k = 0; k < 8; ++k) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
    crcTab[i] = c;
  }
  std::vector<uint8_t> raw;                        // filter byte 0 + a row
  for (int y = 0; y < h; ++y) {
    raw.push_back(0);
    raw.insert(raw.end(), grey.begin() + (size_t)y * w, grey.begin() + (size_t)(y + 1) * w);
  }
  std::vector<uint8_t> z = {0x78, 0x01};
  uint32_t a = 1, b = 0;
  for (uint8_t v : raw) { a = (a + v) % 65521u; b = (b + a) % 65521u; }
  for (size_t off = 0; off < raw.size() || off == 0;) {
    const size_t n = std::min<size_t>(65535, raw.size() - off);
    z.push_back(off + n >= raw.size() ? 1 : 0);
    z.push_back((uint8_t)n); z.push_back((uint8_t)(n >> 8));
    z.push_back((uint8_t)~n); z.push_back((uint8_t)(~n >> 8));
    z.insert(z.end(), raw.begin() + off, raw.begin() + off + n);
    off += n;
    if (!n) break;
  }
  be32(z, (b << 16) | a);
  FILE* f = fopen(path, "wb");
  if (!f) return false;
  static const uint8_t sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
  fwrite(sig, 1, 8, f);
  std::vector<uint8_t> ihdr;
  be32(ihdr, (uint32_t)w); be32(ihdr, (uint32_t)h);
  ihdr.push_back(8); ihdr.push_back(0); ihdr.push_back(0); ihdr.push_back(0); ihdr.push_back(0);
  chunk(f, "IHDR", ihdr);
  chunk(f, "IDAT", z);
  chunk(f, "IEND", {});
  fclose(f);
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  const char* out = argc > 1 ? argv[1] : "pages.png";
  const int S = argc > 2 ? atoi(argv[2]) : 3;

  // A busy box: three machines, something learned, LFOs running on the
  // internal clock, a scene playing, the stick pushed.
  sim::adc[JOY_PIN_X] = sim::adc[JOY_PIN_Y] = 512;
  setup();
  bootActive = false; sysState = SYS_RUN;
  run(100);
  applyMachine(0, mnmFindMachine("SID", "6581"));
  applyMachine(1, mnmFindMachine("FM+", "DYN"));
  applyMachine(2, mnmFindMachine("SWAVE", "PULS"));
  kitSet(0, PAGE_AMP, 6, 50);
  kitSet(0, PAGE_FILT, 0, 90);
  for (uint8_t i = 0; i < 6; ++i) perfSeed(i);
  perfSlot[1].value = 90; perfSlot[1].known = true;
  perfSlot[3].value = 70; perfSlot[3].known = true;
  lfo.p[1].page = PAGE_FILT; lfo.p[1].dest = 0; lfoRetarget(1);
  lfo.p[0].page = PAGE_SYNT; lfo.p[0].dest = 3; lfoRetarget(0);
  lfo.p[0].trig = TRIG_FREE; lfo.p[0].wave = WAVE_SQR; lfo.p[0].depth = 90;
  lfo.p[3].enabled = false;
  lfo.p[4].wave = WAVE_EXP_M;
  for (uint8_t k = 0; k < 16; k += 3) {
    lfo.p[0].steps[k].on = 1;
    lfo.p[0].steps[k].prob = (uint8_t)(1 + k % 8);
  }
  handleCommand("clk i");
  pat.engineOn = true;
  patSelectGenre(PAT_FOG, 7);
  sim::adc[JOY_PIN_X] = 300;
  run(1000);
  Serial.captured.clear();

  const uint32_t st  = (uint32_t)(clk.position() >> 30);
  const uint32_t bpm = (uint32_t)(clk.bpm() * 100.0f);
  ui.page = UPAGE_PERF; ui.inSub = false; perfCursor = 1;
  drawPerform(st, true, bpm);            grab("PERF");
  g_perfSub = 0; drawPerfDest(st, true, bpm);  grab("DEST");
  g_perfSub = 1; drawPerfJoy(st, true, bpm);   grab("JOY");
  ui.page = UPAGE_LFO; ui.sel = 0;
  drawOverview(true, "", bpm);           grab("LFO");
  ui.paramRow = 3; ui.stepCur = 2;
  drawEdit(st, true);                    grab("EDIT");
  drawPattern(st, true, bpm);            grab("PAT");
  setCur = 11; drawSettings(st, true, bpm);   grab("SET");
  wizState = WIZ_MACHINE_SELECT; wizCursor = 1;
  drawWizard(millis());                  grab("WIZARD");
  drawFault();                           grab("FAULT");
  drawBoot(2600);                        grab("BOOT");

  const int G = 8, pw = 64 * S, ph = 256 * S;
  const int W = (int)pages.size() * (pw + G) + G, H = ph + 2 * G;
  std::vector<uint8_t> img((size_t)W * H, 40);
  for (size_t i = 0; i < pages.size(); ++i)
    for (int y = 0; y < ph; ++y)
      for (int x = 0; x < pw; ++x)
        img[(size_t)(G + y) * W + G + i * (pw + G) + x] =
            (uint8_t)(pages[i][(y / S) * 64 + x / S] * 17);
  if (!writePng(out, W, H, img)) { fprintf(stderr, "cannot write %s\n", out); return 1; }
  printf("%s: %zu pages at x%d (", out, pages.size(), S);
  for (size_t i = 0; i < names.size(); ++i) printf(i ? " %s" : "%s", names[i].c_str());
  printf(")\n");
  return 0;
}
