#pragma once

#include "IVendorPolicy.hpp"
#include "stm32f4xx_hal.h"
namespace stm32f4_policy_internal {
  inline GPIO_TypeDef* gpioBase(Port p){
    switch(p){ case Port::A: return GPIOA; case Port::B: return GPIOB; case Port::C: return GPIOC;
      case Port::D: return GPIOD; case Port::E: return GPIOE; case Port::F: return GPIOF; case Port::G: return GPIOG;
      default: return GPIOA;
    }
  }
  inline void enableGpioClk(Port p){
    switch(p){ case Port::A: __HAL_RCC_GPIOA_CLK_ENABLE(); break; case Port::B: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
      case Port::C: __HAL_RCC_GPIOC_CLK_ENABLE(); break; case Port::D: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
      case Port::E: __HAL_RCC_GPIOE_CLK_ENABLE(); break; case Port::F: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
      case Port::G: __HAL_RCC_GPIOG_CLK_ENABLE(); break; default: break;
    }
  }
  inline uint32_t halMode(PinMode m){
    switch(m){ case PinMode::Input: return GPIO_MODE_INPUT; case PinMode::Output: return GPIO_MODE_OUTPUT_PP;
      case PinMode::Alt: return GPIO_MODE_AF_PP; case PinMode::Analog: return GPIO_MODE_ANALOG; }
    return GPIO_MODE_INPUT;
  }
  inline uint32_t halPull(PinPull p){
    switch(p){ case PinPull::None: return GPIO_NOPULL; case PinPull::Up: return GPIO_PULLUP; case PinPull::Down: return GPIO_PULLDOWN; }
    return GPIO_NOPULL;
  }
  inline uint32_t halSpeed(PinSpeed s){
    switch(s){ case PinSpeed::Low: return GPIO_SPEED_FREQ_LOW; case PinSpeed::Med: return GPIO_SPEED_FREQ_MEDIUM;
      case PinSpeed::High: return GPIO_SPEED_FREQ_HIGH; case PinSpeed::VeryHigh: return GPIO_SPEED_FREQ_VERY_HIGH; }
    return GPIO_SPEED_FREQ_LOW;
  }
  inline USART_TypeDef* uartBase(UartInst u){
    switch(u){ case UartInst::Uart1: return USART1; case UartInst::Uart2: return USART2; case UartInst::Uart3: return USART3;
      case UartInst::Uart6: return USART6; default: return nullptr;
    }
  }
}

namespace stm32f4_policy {
class Stm32F4Policy : public IVendorPolicy {
public:
  void enableGpioClock(Port p) override { stm32f4_policy_internal::enableGpioClk(p); }
  bool isValidAlt(PinID, uint8_t) override { return true; }
  void applyPin(const PinCfg& cfg) override {
    GPIO_InitTypeDef init{};
    init.Pin = (uint16_t)(1u << cfg.id.idx);
    init.Mode = stm32f4_policy_internal::halMode(cfg.mode);
    init.Pull = stm32f4_policy_internal::halPull(cfg.pull);
    init.Speed = stm32f4_policy_internal::halSpeed(cfg.speed);
    init.Alternate = cfg.alt;
    HAL_GPIO_Init(stm32f4_policy_internal::gpioBase(cfg.id.port), &init);
  }
  bool supportsUart(UartInst u) override { return stm32f4_policy_internal::uartBase(u) != nullptr; }
  bool isValidUartAF(UartInst, PinID, uint8_t) override { return true; }
  void enableUartClock(UartInst u) override {
    switch (u) {
      case UartInst::Uart1: __HAL_RCC_USART1_CLK_ENABLE(); break;
      case UartInst::Uart2: __HAL_RCC_USART2_CLK_ENABLE(); break;
      case UartInst::Uart3: __HAL_RCC_USART3_CLK_ENABLE(); break;
      case UartInst::Uart6: __HAL_RCC_USART6_CLK_ENABLE(); break;
      default: break;
    }
  }
  void enableUartIrq(UartInst u) override {
    if (u == UartInst::Uart2) { HAL_NVIC_SetPriority(USART2_IRQn, 5, 0); HAL_NVIC_EnableIRQ(USART2_IRQn); }
  }
  void enableDmaForUart(UartInst) override {
    __HAL_RCC_DMA1_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 6, 0); HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 6, 0); HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  }
};
} // namespace stm32f4_policy


