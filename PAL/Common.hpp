#pragma once
#include <cstdint>
#include <cstddef>

enum class PinMode { Input, Output, Alt, Analog };
enum class PinPull { None, Up, Down };
enum class PinSpeed { Low, Med, High, VeryHigh };
enum class Port : uint8_t { A,B,C,D,E,F,G };

struct PinID { Port port; uint8_t idx; };
struct PinCfg { PinID id; PinMode mode; PinPull pull; PinSpeed speed; uint8_t alt; };

enum class UartInst : uint8_t { None=0, Uart1=1, Uart2=2, Uart3=3, Uart4=4, Uart5=5, Uart6=6 };
enum class StopBits : uint8_t { One, Two };
enum class Parity : uint8_t { None, Even, Odd };

struct UartCfg {
  UartInst inst;
  PinCfg tx;
  PinCfg rx;
  uint32_t baud = 115200;
  StopBits stop = StopBits::One;
  Parity parity = Parity::None;
  bool use_irq = true;
  bool use_dma = false;
};

constexpr bool operator==(PinID a, PinID b) { return a.port==b.port && a.idx==b.idx; }
