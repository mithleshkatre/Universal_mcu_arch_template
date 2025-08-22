#include "Stm32Pin.hpp"
using namespace stm32f4_policy_internal;
void Stm32Pin::init(const PinCfg& cfg) {
  GPIO_InitTypeDef init{};
  init.Pin = (uint16_t)(1u << cfg.id.idx);
  init.Mode = halMode(cfg.mode);
  init.Pull = halPull(cfg.pull);
  init.Speed = halSpeed(cfg.speed);
  init.Alternate = cfg.alt;
  HAL_GPIO_Init(gpioBase(cfg.id.port), &init);
  cfg_ = cfg;
}
void Stm32Pin::set(bool high) {
  HAL_GPIO_WritePin(gpioBase(cfg_.id.port), (uint16_t)(1u << cfg_.id.idx), high?GPIO_PIN_SET:GPIO_PIN_RESET);
}
bool Stm32Pin::read() {
  return HAL_GPIO_ReadPin(gpioBase(cfg_.id.port), (uint16_t)(1u << cfg_.id.idx)) == GPIO_PIN_SET;
}

