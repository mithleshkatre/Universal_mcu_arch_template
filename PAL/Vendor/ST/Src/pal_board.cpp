#include "pal_board.hpp"

namespace pal {

void Board::init() {
    // Reset all peripherals, init Flash interface & SysTick
    HAL_Init();
}

void Board::mspInit() {
    HAL_MspInit();
}

void Board::delay(uint32_t delayMs) {
    HAL_Delay(delayMs);
}

void Board::errorHandler() {
    __disable_irq();
    while (true) {
        // Optionally add debug trap or LED blink here
    }
}

} // namespace pal

// Optional: assert_failed equivalent for debugging
#ifdef USE_FULL_ASSERT
extern "C" void assert_failed(uint8_t* file, uint32_t line) {
    // User can add logging here
    (void)file;
    (void)line;
    while (true) {}
}
#endif
