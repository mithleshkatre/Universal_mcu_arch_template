#pragma once
#include <cstdint>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"

// Clock source selection
enum class ClockSource : uint8_t {
    HSI = 0,   // Internal RC
    HSE,       // External crystal
    PLL        // PLL (derive from HSI or HSE)
};

// Simple dividers
enum class AhbDiv : uint16_t {
    DIV_1   = 1,
    DIV_2   = 2,
    DIV_4   = 4,
    DIV_8   = 8,
    DIV_16  = 16,
    DIV_64  = 64,
    DIV_128 = 128,
    DIV_256 = 256,
    DIV_512 = 512
};

enum class ApbDiv : uint8_t {
    DIV_1 = 1,
    DIV_2 = 2,
    DIV_4 = 4,
    DIV_8 = 8,
    DIV_16 = 16
};

// PLL configuration
struct PllConfig {
    bool     use_hse {false};   // 0: HSI as PLL input, 1: HSE as PLL input
    uint32_t hse_hz {0};        // if use_hse=1, supply crystal frequency
    uint32_t m {0};
    uint32_t n {0};
    uint32_t p {0};
    uint32_t q {0};
    uint32_t r {0};
};

// High-level clock configuration
struct ClockConfig {
    ClockSource source {ClockSource::HSI};
    PllConfig   pll {};
    AhbDiv      ahb_div {AhbDiv::DIV_1};
    ApbDiv      apb1_div {ApbDiv::DIV_1};
    ApbDiv      apb2_div {ApbDiv::DIV_1};
    uint32_t    flash_latency {0};
};

// C++ Clock Manager
class PalClock {
public:
    explicit PalClock(const ClockConfig& cfg);
    bool init();   // returns true on success, false on error

private:
    ClockConfig cfg_;

    static uint32_t mapAhbDiv(AhbDiv d);
    static uint32_t mapApbDiv(ApbDiv d);
};
