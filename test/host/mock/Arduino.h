// Host-side stand-in for the parts of the Teensy 4 / Arduino API that
// XY6_LFO.ino uses, so the sketch can be compiled and exercised on a PC with
// no Teensy toolchain. It is a model, not an emulator:
//
//   * time is simulated (sim::nowNs) and every micros() call costs 1 us, so
//     the sketch's own polling loops always make progress;
//   * Serial1's TX ring is modelled byte-accurately - the Teensy core's ring
//     size and availableForWrite() arithmetic, draining onto the wire at
//     baud / 10 bytes a second - because MIDI output latency is what the
//     host test measures;
//   * the LPUART's transmit-complete flag is modelled too (IMXRT_LPUART6.STAT
//     TC), so the sketch's own Teensy 4 code path decides when the UART is
//     idle, and a baud change with a byte still going out is counted;
//   * I2C always ACKs, reads back 0xFF, and analogRead() returns whatever the
//     test puts in sim::adc[].
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <algorithm>
#include <deque>
#include <vector>

// ---- simulated time ---------------------------------------------------------
namespace sim {
inline uint64_t nowNs = 0;                // the one clock; micros() reads it
inline uint64_t nowUs() { return nowNs / 1000u; }
inline uint16_t adc[64] = {0};
// Every byte Serial1 was handed: value, when it was written into the ring,
// when its stop bit left the pin, and the baud it went out at.
struct WireByte { uint8_t b; uint64_t writeUs, doneUs; uint32_t baud; };
inline std::vector<WireByte> wire;
inline uint32_t txOverruns = 0;   // write() into a full ring (would block on HW)
// begin() called while a byte was still being shifted out - on the board that
// byte is cut off mid-frame.
inline uint32_t txTruncations = 0;
struct BaudChange { uint64_t us; uint32_t baud; };
inline std::vector<BaudChange> baudChanges;   // every Serial1.begin()
}

// Rough costs, so busy code takes time: a micros() call 1 us (pessimistic),
// a GPIO write 50 ns - which makes an 8 KB display push ~5 ms, as on the board.
inline uint32_t micros() { sim::nowNs += 1000u; return (uint32_t)sim::nowUs(); }
inline uint32_t millis() { return (uint32_t)(sim::nowNs / 1000000u); }
inline void delay(uint32_t ms) { sim::nowNs += (uint64_t)ms * 1000000u; }
inline void delayMicroseconds(uint32_t us) { sim::nowNs += (uint64_t)us * 1000u; }
inline void yield() {}

class elapsedMillis {
 public:
  elapsedMillis() : t0_(millis()) {}
  operator uint32_t() const { return millis() - t0_; }
  elapsedMillis& operator=(uint32_t v) { t0_ = millis() - v; return *this; }
 private:
  uint32_t t0_;
};

// ---- GPIO / ADC / interrupts ------------------------------------------------
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define CHANGE 4
inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWriteFast(uint8_t, uint8_t) { sim::nowNs += 50u; }
inline uint8_t digitalReadFast(uint8_t) { return 1; }
inline int digitalPinToInterrupt(uint8_t p) { return p; }
inline void attachInterrupt(int, void (*)(), int) {}
inline void noInterrupts() {}
inline void interrupts() {}
inline void __disable_irq() {}
inline void __enable_irq() {}
inline int analogRead(uint8_t pin) { return sim::adc[pin & 63]; }
inline void analogReadResolution(unsigned) {}
inline void analogReadAveraging(unsigned) {}

template <class A, class B> inline auto max(A a, B b) { return a > b ? a : b; }
template <class A, class B> inline auto min(A a, B b) { return a < b ? a : b; }

// ---- i.MX RT registers the sketch touches -----------------------------------
inline uint32_t SRC_SRSR = 1;                     // power-on
#define SRC_SRSR_IPP_RESET_B        (1u << 0)
#define SRC_SRSR_LOCKUP_SYSRESETREQ (1u << 2)
#define SRC_SRSR_WDOG3_RST_B        (1u << 7)
inline volatile uint32_t WDOG3_CNT = 0, WDOG3_TOVAL = 0, WDOG3_WIN = 0;
inline volatile uint32_t WDOG3_CS = (1u << 10) | (1u << 11);   // RCS, ULK: ready
inline uint32_t CCM_CCGR5 = 0;
#define CCM_CCGR5_WDOG3(n) ((uint32_t)(n) << 20)

// ---- Print / USB serial -----------------------------------------------------
class __FlashStringHelper;
#define F(s) ((const __FlashStringHelper*)(s))
#define HEX 16
#define DEC 10

// CrashReport: never set on the host.
struct CrashReportClass { explicit operator bool() const { return false; } };

class Print {
 public:
  virtual ~Print() {}
  virtual size_t write(uint8_t b) = 0;
  size_t write(const char* s) { size_t n = 0; while (*s) n += write((uint8_t)*s++); return n; }
  size_t print(const char* s) { return write(s); }
  size_t print(const __FlashStringHelper* s) { return write((const char*)s); }
  size_t print(char c) { return write((uint8_t)c); }
  size_t print(const CrashReportClass&) { return 0; }
  size_t print(long v, int base = DEC) { return fmt(base == HEX ? "%lX" : "%ld", v); }
  size_t print(unsigned long v, int base = DEC) { return fmt(base == HEX ? "%lX" : "%lu", v); }
  size_t print(int v, int base = DEC) { return print((long)v, base); }
  size_t print(unsigned v, int base = DEC) { return print((unsigned long)v, base); }
  size_t print(uint8_t v, int base = DEC) { return print((unsigned long)v, base); }
  size_t print(double v, int digits = 2) { return fmt("%.*f", digits, v); }
  template <class T> size_t println(T v) { size_t n = print(v); return n + print("\n"); }
  template <class T> size_t println(T v, int b) { size_t n = print(v, b); return n + print("\n"); }
  size_t println() { return print("\n"); }
  __attribute__((format(printf, 2, 3))) size_t printf(const char* f, ...) {
    char b[1024]; va_list ap; va_start(ap, f); vsnprintf(b, sizeof b, f, ap); va_end(ap);
    return write(b);
  }
 private:
  __attribute__((format(printf, 2, 3))) size_t fmt(const char* f, ...) {
    char b[64]; va_list ap; va_start(ap, f); vsnprintf(b, sizeof b, f, ap); va_end(ap);
    return write(b);
  }
};

class usb_serial_class : public Print {
 public:
  void begin(long) {}
  int available() { return 0; }
  int read() { return -1; }
  int availableForWrite() { return 4096; }
  void flush() {}
  using Print::write;
  size_t write(uint8_t b) override { if (echo) fputc(b, stdout); return 1; }
  bool echo = false;
};
inline usb_serial_class Serial;

inline CrashReportClass CrashReport;

// ---- hardware UART ------------------------------------------------------------
// Mirrors the Teensy 4 HardwareSerialIMXRT arithmetic: a ring of
// (64 + added memory) bytes, availableForWrite() = size - 1 - used. Behind the
// ring sit the 4-byte LPUART FIFO and the shift register; the ISR keeps them
// topped up, so a byte leaves the ring as soon as there is FIFO space.
class HardwareSerial : public Print {
 public:
  explicit HardwareSerial(bool modelTx) : modelTx_(modelTx) {}
  void begin(uint32_t baud) {
    drain();
    if (modelTx_) {
      if (!pend_.empty()) sim::txTruncations++;
      sim::baudChanges.push_back({sim::nowUs(), baud});
    }
    baud_ = baud; pend_.clear(); rx_.clear();
  }
  void addMemoryForRead(void*, size_t) {}
  void addMemoryForWrite(void*, size_t n) { txSize_ = 64 + (int)n; }
  int available() { return (int)rx_.size(); }
  int read() { if (rx_.empty()) return -1; int b = rx_.front(); rx_.pop_front(); return b; }
  int availableForWrite() {
    drain();
    int inRing = (int)pend_.size() - kHwDepth;   // what the FIFO + shifter do not hold
    if (inRing < 0) inRing = 0;
    return txSize_ - 1 - inRing;
  }
  using Print::write;
  size_t write(uint8_t b) override {
    if (!modelTx_) return 1;
    if (availableForWrite() <= 0) sim::txOverruns++;
    const uint64_t byteUs = 10000000ull / baud_;
    const uint64_t now = sim::nowUs();
    const uint64_t start = pend_.empty() ? now : std::max(now, pend_.back());
    pend_.push_back(start + byteUs);
    sim::wire.push_back({b, now, start + byteUs, baud_});
    return 1;
  }
  void inject(uint8_t b) { rx_.push_back(b); }
  int txSize() const { return txSize_; }
  uint32_t baud() const { return baud_; }
  // FIFO and shift register both empty: the LPUART's TC flag.
  bool txComplete() { drain(); return pend_.empty(); }
 private:
  static const int kHwDepth = 5;                  // 4-deep FIFO + shift register
  void drain() { while (!pend_.empty() && pend_.front() <= sim::nowUs()) pend_.pop_front(); }
  bool modelTx_;
  uint32_t baud_ = 31250;
  int txSize_ = 64;
  std::deque<uint64_t> pend_;    // completion time of every byte not yet sent
  std::deque<uint8_t> rx_;
};
inline HardwareSerial Serial1(true);
inline HardwareSerial Serial2(false);

// ---- LPUART6, the register block behind Serial1 -------------------------------
// Only what the sketch's tmTxIdle() / tmPortRestart() touch. Defining the
// chip's macro puts the sketch on its real Teensy 4 code path.
#define __IMXRT1062__ 1
#define LPUART_STAT_TC      (1u << 22)
#define LPUART_STAT_OR      (1u << 19)
#define LPUART_STAT_NF      (1u << 18)
#define LPUART_STAT_FE      (1u << 17)
#define LPUART_STAT_PF      (1u << 16)
#define LPUART_FIFO_TXFLUSH (1u << 15)
#define LPUART_FIFO_RXFLUSH (1u << 14)
struct MockLpuart {
  struct Stat {
    operator uint32_t() const { return Serial1.txComplete() ? LPUART_STAT_TC : 0u; }
    Stat& operator=(uint32_t) { return *this; }     // write-1-to-clear flags: no-op
  } STAT;
  uint32_t FIFO = 0;
};
inline MockLpuart IMXRT_LPUART6;
