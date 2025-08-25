#pragma once
#include "Common.hpp"

struct IVendorPolicy {
  virtual ~IVendorPolicy() = default;

  virtual void enableGpioClock(Port) = 0;
  virtual bool isValidAlt(PinID, uint8_t) = 0;
  virtual void applyPin(const PinCfg& cfg) = 0;

  virtual bool supportsUart(UartInst) = 0;
  virtual bool isValidUartAF(UartInst, PinID, uint8_t) = 0;
  virtual void enableUartClock(UartInst) = 0;
  virtual void enableUartIrq(UartInst) = 0;
  virtual void enableDmaForUart(UartInst) = 0;
};
