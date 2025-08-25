#include "stm32_clock.hpp"

PalClock::PalClock(const ClockConfig& cfg) : cfg_(cfg) {}

bool PalClock::init() {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // --- Oscillator Config ---
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    if (cfg_.source == ClockSource::PLL) {
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = cfg_.pll.use_hse ? RCC_PLLSOURCE_HSE : RCC_PLLSOURCE_HSI;
        RCC_OscInitStruct.PLL.PLLM = cfg_.pll.m;
        RCC_OscInitStruct.PLL.PLLN = cfg_.pll.n;
        RCC_OscInitStruct.PLL.PLLP = cfg_.pll.p;
        RCC_OscInitStruct.PLL.PLLQ = cfg_.pll.q;
        RCC_OscInitStruct.PLL.PLLR = cfg_.pll.r;
    }

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        return false;
    }

    // --- Clock Config ---
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = (cfg_.source == ClockSource::PLL) ? RCC_SYSCLKSOURCE_PLLCLK
                                                                      : RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = mapAhbDiv(cfg_.ahb_div);
    RCC_ClkInitStruct.APB1CLKDivider = mapApbDiv(cfg_.apb1_div);
    RCC_ClkInitStruct.APB2CLKDivider = mapApbDiv(cfg_.apb2_div);

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, cfg_.flash_latency) != HAL_OK) {
        return false;
    }

    // --- Compute SYSCLK Frequency ---
    if (cfg_.source == ClockSource::HSI) {
        sysclk_hz_ = HSI_VALUE;
    } else if (cfg_.source == ClockSource::HSE) {
        sysclk_hz_ = cfg_.pll.hse_hz;
    } else if (cfg_.source == ClockSource::PLL) {
        uint32_t fin = cfg_.pll.use_hse ? cfg_.pll.hse_hz : HSI_VALUE;
        sysclk_hz_ = (fin / cfg_.pll.m) * cfg_.pll.n / cfg_.pll.p;
    }

    return true;
}

uint32_t PalClock::getSysClkHz() const { return sysclk_hz_; }
uint32_t PalClock::getHclkHz()   const { return sysclk_hz_ / static_cast<uint32_t>(cfg_.ahb_div); }
uint32_t PalClock::getApb1Hz()   const { return getHclkHz() / static_cast<uint32_t>(cfg_.apb1_div); }
uint32_t PalClock::getApb2Hz()   const { return getHclkHz() / static_cast<uint32_t>(cfg_.apb2_div); }

uint32_t PalClock::mapAhbDiv(AhbDiv d) {
    switch (d) {
        case AhbDiv::DIV_1: return RCC_SYSCLK_DIV1;
        case AhbDiv::DIV_2: return RCC_SYSCLK_DIV2;
        case AhbDiv::DIV_4: return RCC_SYSCLK_DIV4;
        case AhbDiv::DIV_8: return RCC_SYSCLK_DIV8;
        case AhbDiv::DIV_16: return RCC_SYSCLK_DIV16;
        case AhbDiv::DIV_64: return RCC_SYSCLK_DIV64;
        case AhbDiv::DIV_128: return RCC_SYSCLK_DIV128;
        case AhbDiv::DIV_256: return RCC_SYSCLK_DIV256;
        case AhbDiv::DIV_512: return RCC_SYSCLK_DIV512;
        default: return RCC_SYSCLK_DIV1;
    }
}

uint32_t PalClock::mapApbDiv(ApbDiv d) {
    switch (d) {
        case ApbDiv::DIV_1: return RCC_HCLK_DIV1;
        case ApbDiv::DIV_2: return RCC_HCLK_DIV2;
        case ApbDiv::DIV_4: return RCC_HCLK_DIV4;
        case ApbDiv::DIV_8: return RCC_HCLK_DIV8;
        case ApbDiv::DIV_16: return RCC_HCLK_DIV16;
        default: return RCC_HCLK_DIV1;
    }
}
