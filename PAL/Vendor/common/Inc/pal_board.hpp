#pragma once
#include <cstdint>
#include "stm32f4xx_hal.h"

namespace pal {

class Board {
public:
    static void init();
    static void mspInit();
    static void delay(uint32_t delayMs);
    static void errorHandler();
};

} // namespace pal
