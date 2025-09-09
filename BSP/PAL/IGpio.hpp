#pragma once
#include "Common.hpp"
#include "uart_config.hpp"

struct IGpio {
   ~IGpio() = default;
  virtual void init(const PinCfg& cfg) = 0;
  virtual void set(bool high) = 0;
  virtual bool read() = 0;
};
