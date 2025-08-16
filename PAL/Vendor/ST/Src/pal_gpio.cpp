#include "pal_gpio.hpp"
#include "stm32f4xx_hal.h"

namespace pal
{

// This table maps port indexes (0 = A, 1 = B, ...) to GPIO_TypeDef*
static GPIO_TypeDef* portTable[] = {
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH
};

// Enable clock for a given GPIO port index
static void gpioClockEnable(uint8_t portIndex)
{
    switch (portIndex)
    {
        case 0: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
        case 1: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
        case 2: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
        case 3: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
        case 4: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
        case 5: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
        case 6: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
        case 7: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
        default: break;
    }
}

Gpio::Gpio(uint8_t port, uint8_t pin)
    : portIndex(port), pinNumber(pin)
{
}

bool Gpio::config(Mode mode, Pull pull, Speed speed, uint8_t af)
{
    if (portIndex >= (sizeof(portTable) / sizeof(portTable[0])))
        return false;

    gpioClockEnable(portIndex);

    GPIO_InitTypeDef cfg = {};
    cfg.Pin = (1u << pinNumber);

    switch (mode)
    {
        case Mode::Output:    cfg.Mode = GPIO_MODE_OUTPUT_PP; break;
        case Mode::Input:     cfg.Mode = GPIO_MODE_INPUT; break;
        case Mode::Alternate: cfg.Mode = GPIO_MODE_AF_PP; break;
        case Mode::Analog:    cfg.Mode = GPIO_MODE_ANALOG; break;
    }

    switch (pull)
    {
        case Pull::NoPull: cfg.Pull = GPIO_NOPULL; break;
        case Pull::Up:     cfg.Pull = GPIO_PULLUP; break;
        case Pull::Down:   cfg.Pull = GPIO_PULLDOWN; break;
    }

    cfg.Speed = static_cast<uint32_t>(speed);
    cfg.Alternate = af;

    HAL_GPIO_Init(portTable[portIndex], &cfg);
    return true;
}

void Gpio::write(bool value)
{
    HAL_GPIO_WritePin(portTable[portIndex], (1u << pinNumber),
                      value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool Gpio::read() const
{
    return (HAL_GPIO_ReadPin(portTable[portIndex], (1u << pinNumber)) == GPIO_PIN_SET);
}

} // namespace pal
