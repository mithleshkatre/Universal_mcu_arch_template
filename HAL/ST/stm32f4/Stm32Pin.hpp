#pragma once

#include "stm32f4xx_hal.h"
#include "IGpio.hpp"
#include "Stm32F4Policy.hpp"
using namespace stm32f4_policy_internal;

class Stm32Pin : public IGpio {
public:
  Stm32Pin() = default;
  void init(const PinCfg& cfg) override;
  void set(bool high) override;
  bool read() override;
private:
  PinCfg cfg_{ {Port::A,0}, PinMode::Input, PinPull::None, PinSpeed::Low, 0 };
};
