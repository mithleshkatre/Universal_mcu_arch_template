#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PAL_GPIO_INPUT = 0,
    PAL_GPIO_OUTPUT,
    PAL_GPIO_AF,
    PAL_GPIO_ANALOG
} pal_gpio_mode_t;

typedef enum {
    PAL_GPIO_NOPULL = 0,
    PAL_GPIO_PULLUP,
    PAL_GPIO_PULLDOWN
} pal_gpio_pull_t;

typedef enum {
    PAL_GPIO_SPEED_LOW        = 0x00000000U,
    PAL_GPIO_SPEED_MEDIUM     = 0x00000001U,
    PAL_GPIO_SPEED_HIGH       = 0x00000002U,
    PAL_GPIO_SPEED_VERY_HIGH  = 0x00000003U
} pal_gpio_speed_t;

typedef struct {
    uint8_t port;  // Logical port index (0 = Port A for STM32, 0 = P0 for Infineon)
    uint8_t pin;   // Pin number within the port
} pal_gpio_t;

/**
 * Configure a GPIO pin
 */
int pal_gpio_config(pal_gpio_t io,
                    pal_gpio_mode_t mode,
                    pal_gpio_pull_t pull,
                    pal_gpio_speed_t speed,
                    uint8_t af);

/**
 * Write a logic value to the GPIO pin
 */
int pal_gpio_write(pal_gpio_t io, int value);

/**
 * Read a logic value from the GPIO pin
 */
int pal_gpio_read(pal_gpio_t io);

#ifdef __cplusplus
}
#endif
