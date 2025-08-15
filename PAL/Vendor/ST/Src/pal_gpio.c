#include "pal_gpio.h"
#include "stm32f4xx_hal.h"  // This will be aliased by CubeIDE to the correct family header

/* Port mapping — works for any STM32 family */
static GPIO_TypeDef* port_table[] = {
    GPIOA, GPIOB, GPIOC, GPIOD,
#if defined(GPIOE)
    GPIOE,
#endif
#if defined(GPIOF)
    GPIOF,
#endif
#if defined(GPIOG)
    GPIOG,
#endif
#if defined(GPIOH)
    GPIOH,
#endif
#if defined(GPIOI)
    GPIOI,
#endif
#if defined(GPIOJ)
    GPIOJ,
#endif
#if defined(GPIOK)
    GPIOK,
#endif
};

/* Enable GPIO clock dynamically based on port index */
static void gpio_clock_enable(uint8_t port_index)
{
    switch (port_index) {
        case 0: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
        case 1: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
        case 2: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
        case 3: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
#if defined(GPIOE)
        case 4: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
#endif
#if defined(GPIOF)
        case 5: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
#endif
#if defined(GPIOG)
        case 6: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
#endif
#if defined(GPIOH)
        case 7: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
#endif
#if defined(GPIOI)
        case 8: __HAL_RCC_GPIOI_CLK_ENABLE(); break;
#endif
#if defined(GPIOJ)
        case 9: __HAL_RCC_GPIOJ_CLK_ENABLE(); break;
#endif
#if defined(GPIOK)
        case 10: __HAL_RCC_GPIOK_CLK_ENABLE(); break;
#endif
        default: break;
    }
}

int pal_gpio_config(pal_gpio_t io,
                    pal_gpio_mode_t mode,
                    pal_gpio_pull_t pull,
                    pal_gpio_speed_t speed,
                    uint8_t af)
{
    if (io.port >= (sizeof(port_table) / sizeof(port_table[0]))) {
        return -1; // Invalid port
    }

    gpio_clock_enable(io.port);

    GPIO_InitTypeDef cfg = {0};
    cfg.Pin = (1u << io.pin);

    // Mode mapping
    switch (mode) {
        case PAL_GPIO_OUTPUT: cfg.Mode = GPIO_MODE_OUTPUT_PP; break;
        case PAL_GPIO_INPUT:  cfg.Mode = GPIO_MODE_INPUT; break;
        case PAL_GPIO_AF:     cfg.Mode = GPIO_MODE_AF_PP; break;
        case PAL_GPIO_ANALOG: cfg.Mode = GPIO_MODE_ANALOG; break;
        default: return -1;
    }

    // Pull mapping
    switch (pull) {
        case PAL_GPIO_NOPULL:  cfg.Pull = GPIO_NOPULL; break;
        case PAL_GPIO_PULLUP:  cfg.Pull = GPIO_PULLUP; break;
        case PAL_GPIO_PULLDOWN:cfg.Pull = GPIO_PULLDOWN; break;
        default: cfg.Pull = GPIO_NOPULL; break;
    }

    // Speed mapping — directly maps to HAL macros
    cfg.Speed = (uint32_t)speed;

    // Alternate function number
    cfg.Alternate = af;

    HAL_GPIO_Init(port_table[io.port], &cfg);
    return 0;
}

int pal_gpio_write(pal_gpio_t io, int value)
{
    if (io.port >= (sizeof(port_table) / sizeof(port_table[0]))) {
        return -1;
    }
    HAL_GPIO_WritePin(port_table[io.port], (1u << io.pin),
                      value ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return 0;
}

int pal_gpio_read(pal_gpio_t io)
{
    if (io.port >= (sizeof(port_table) / sizeof(port_table[0]))) {
        return -1;
    }
    return (HAL_GPIO_ReadPin(port_table[io.port], (1u << io.pin)) == GPIO_PIN_SET) ? 1 : 0;
}
