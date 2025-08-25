#pragma once

#include "stm32f4xx_hal.h"
#include "IGpio.hpp"
#include "Stm32F4Policy.hpp"
using namespace stm32f4_policy_internal;

class Stm32IO : public IGpio {
public:
  Stm32IO() = default;
  void init(const PinCfg& cfg) ;
  void set(bool high) ;
  bool read() ;
private:
  PinCfg cfg_{ {Port::A,0}, PinMode::Input, PinPull::None, PinSpeed::Low, 0 };
};
