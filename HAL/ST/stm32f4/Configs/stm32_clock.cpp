#include "stm32_clock.hpp"

PalClock::PalClock(const ClockConfig& cfg) : cfg_(cfg) {}

uint32_t PalClock::mapAhbDiv(AhbDiv d) {
    switch (d) {
        case AhbDiv::DIV_1:   return RCC_SYSCLK_DIV1;
        case AhbDiv::DIV_2:   return RCC_SYSCLK_DIV2;
        case AhbDiv::DIV_4:   return RCC_SYSCLK_DIV4;
        case AhbDiv::DIV_8:   return RCC_SYSCLK_DIV8;
        case AhbDiv::DIV_16:  return RCC_SYSCLK_DIV16;
        case AhbDiv::DIV_64:  return RCC_SYSCLK_DIV64;
        case AhbDiv::DIV_128: return RCC_SYSCLK_DIV128;
        case AhbDiv::DIV_256: return RCC_SYSCLK_DIV256;
        case AhbDiv::DIV_512: return RCC_SYSCLK_DIV512;
        default:              return RCC_SYSCLK_DIV1;
    }
}

uint32_t PalClock::mapApbDiv(ApbDiv d) {
    switch (d) {
        case ApbDiv::DIV_1:  return RCC_HCLK_DIV1;
        case ApbDiv::DIV_2:  return RCC_HCLK_DIV2;
        case ApbDiv::DIV_4:  return RCC_HCLK_DIV4;
        case ApbDiv::DIV_8:  return RCC_HCLK_DIV8;
        case ApbDiv::DIV_16: return RCC_HCLK_DIV16;
        default:             return RCC_HCLK_DIV1;
    }
}

bool PalClock::init() {
#if defined(__HAL_RCC_PWR_CLK_ENABLE)
    __HAL_RCC_PWR_CLK_ENABLE();
#endif

#if defined(PWR_REGULATOR_VOLTAGE_SCALE1)
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
#elif defined(__HAL_PWR_VOLTAGESCALING_CONFIG)
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
#endif

    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
#if defined(RCC_CLOCKTYPE_PCLK2)
    clk.ClockType |= RCC_CLOCKTYPE_PCLK2;
#endif

    switch (cfg_.source) {
        case ClockSource::HSI:
            osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
            osc.HSIState = RCC_HSI_ON;
            osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
            osc.PLL.PLLState = RCC_PLL_NONE;
            if (HAL_RCC_OscConfig(&osc) != HAL_OK) return false;
            clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
            break;

        case ClockSource::HSE:
            osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
            osc.HSEState = RCC_HSE_ON;
            osc.PLL.PLLState = RCC_PLL_NONE;
            if (HAL_RCC_OscConfig(&osc) != HAL_OK) return false;
            clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
            break;

        case ClockSource::PLL: {
            if (cfg_.pll.use_hse) {
                osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
                osc.HSEState = RCC_HSE_ON;
            } else {
                osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
                osc.HSIState = RCC_HSI_ON;
                osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
            }

            osc.PLL.PLLState   = RCC_PLL_ON;
            osc.PLL.PLLSource  = cfg_.pll.use_hse ? RCC_PLLSOURCE_HSE : RCC_PLLSOURCE_HSI;
            osc.PLL.PLLM       = cfg_.pll.m;
            osc.PLL.PLLN       = cfg_.pll.n;
            osc.PLL.PLLP       = cfg_.pll.p;
            osc.PLL.PLLQ       = cfg_.pll.q;
            osc.PLL.PLLR       = cfg_.pll.r;

            if (HAL_RCC_OscConfig(&osc) != HAL_OK) return false;
            clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        } break;
    }

    clk.AHBCLKDivider  = mapAhbDiv(cfg_.ahb_div);
    clk.APB1CLKDivider = mapApbDiv(cfg_.apb1_div);
#if defined(RCC_CLOCKTYPE_PCLK2)
    clk.APB2CLKDivider = mapApbDiv(cfg_.apb2_div);
#endif

    if (HAL_RCC_ClockConfig(&clk, cfg_.flash_latency) != HAL_OK) return false;

    return true;
}
