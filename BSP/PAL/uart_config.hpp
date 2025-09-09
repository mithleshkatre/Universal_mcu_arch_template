#pragma once
#include "Common.hpp"
#include "Stm32Uart.hpp"

enum class UartInst : uint8_t { 
  None=0, 
  Uart1=1, 
  Uart2=2, 
  Uart3=3, 
  Uart4=4, 
  Uart5=5, 
  Uart6=6 
};

enum class StopBits : uint8_t { 
  One, 
  Two 
};

enum class Parity : uint8_t { 
  None, 
  Even, 
  Odd 
};

struct UartCfg {
  UartInst inst;
  PinCfg tx;
  PinCfg rx;
  uint32_t baud = 115200;
  StopBits stop = StopBits::One;
  Parity parity = Parity::None;
  bool use_irq = true;
  bool use_dma = false;
  UartHwCfg dmaConfig;
};