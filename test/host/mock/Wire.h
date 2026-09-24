// Host stand-in for the Teensy Wire library: every transaction ACKs and every
// read returns 0xFF (no buttons pressed on the MCP23017, an erased EEPROM).
#pragma once
#include "Arduino.h"
class TwoWire {
 public:
  void begin() {}
  void setClock(uint32_t) {}
  void beginTransmission(uint8_t) {}
  size_t write(uint8_t) { return 1; }
  uint8_t endTransmission(bool = true) { return 0; }
  uint8_t requestFrom(int, int n) { avail_ = n; return (uint8_t)n; }
  int available() { return avail_; }
  int read() { if (avail_ > 0) avail_--; return 0xFF; }
 private:
  int avail_ = 0;
};
inline TwoWire Wire;
