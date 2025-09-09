#pragma once
#include <cstdint>
#include <cstddef>

enum class PinMode {
  Input,
  Output,
  Alt,
  Analog 
};

enum class PinPull { 
  None, 
  Up, 
  Down 
};

enum class PinSpeed { Low, 
  Med, 
  High, 
  VeryHigh 
};

enum class Port : uint8_t { A,B,C,D,E,F,G };

struct PinID { 
  Port port; 
  uint8_t idx; 
};

struct PinCfg { 
  PinID id; 
  PinMode mode; 
  PinPull pull; 
  PinSpeed speed; 
  uint8_t alt; 
};

constexpr bool operator==(PinID a, PinID b) { return a.port==b.port && a.idx==b.idx; }
