#include "pal_clock.h"
#include "stm32f4xx_hal.h"

/* One family-neutral HAL include: define STM32 series in your project and
 * have your HAL bring in the right headers. The alias below is common in recent HALs.
 * If your environment uses a family header directly, include that instead. */
#include "stm32f4xx_hal_rcc.h"

/* Map PAL dividers to HAL RCC macros at runtime */
static uint32_t map_ahb_div(pal_ahb_div_t d) {
    switch (d) {
        case PAL_AHB_DIV_1:   return RCC_SYSCLK_DIV1;
        case PAL_AHB_DIV_2:   return RCC_SYSCLK_DIV2;
        case PAL_AHB_DIV_4:   return RCC_SYSCLK_DIV4;
        case PAL_AHB_DIV_8:   return RCC_SYSCLK_DIV8;
        case PAL_AHB_DIV_16:  return RCC_SYSCLK_DIV16;
        case PAL_AHB_DIV_64:  return RCC_SYSCLK_DIV64;
        case PAL_AHB_DIV_128: return RCC_SYSCLK_DIV128;
        case PAL_AHB_DIV_256: return RCC_SYSCLK_DIV256;
        case PAL_AHB_DIV_512: return RCC_SYSCLK_DIV512;
        default:              return RCC_SYSCLK_DIV1;
    }
}
static uint32_t map_apb_div(pal_apb_div_t d) {
    switch (d) {
        case PAL_APB_DIV_1:  return RCC_HCLK_DIV1;
        case PAL_APB_DIV_2:  return RCC_HCLK_DIV2;
        case PAL_APB_DIV_4:  return RCC_HCLK_DIV4;
        case PAL_APB_DIV_8:  return RCC_HCLK_DIV8;
        case PAL_APB_DIV_16: return RCC_HCLK_DIV16;
        default:             return RCC_HCLK_DIV1;
    }
}

int pal_clock_init(const pal_clock_cfg_t *cfg)
{
    if (!cfg) return -1;

    /* PWR clock & voltage scaling when available */
#if defined(__HAL_RCC_PWR_CLK_ENABLE)
    __HAL_RCC_PWR_CLK_ENABLE();
#endif

#if defined(PWR_REGULATOR_VOLTAGE_SCALE1)
    /* Default to Scale1 unless you *know* you’re running slow: Scale3 is fine for <= 16 MHz on some F4 */
    HAL_StatusTypeDef ps = HAL_OK;
    ps = HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
    if (ps != HAL_OK) { /* Some older families don’t support this API */ }
#elif defined(__HAL_PWR_VOLTAGESCALING_CONFIG)
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
#endif

    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    /* We’ll always set HCLK/SYSCLK/PCLKs */
    clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
#if defined(RCC_CLOCKTYPE_PCLK2)
    clk.ClockType |= RCC_CLOCKTYPE_PCLK2;
#endif

    /* ——— Oscillator setup ——— */
    switch (cfg->source) {
        case PAL_CLK_SRC_HSI:
            osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
#if defined(RCC_HSI_ON)
            osc.HSIState = RCC_HSI_ON;
#endif
#if defined(RCC_HSICALIBRATION_DEFAULT)
            osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
#endif
            osc.PLL.PLLState = RCC_PLL_NONE;
            if (HAL_RCC_OscConfig(&osc) != HAL_OK) return -2;
            clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
            break;

        case PAL_CLK_SRC_HSE:
            osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
#if defined(RCC_HSE_ON)
            osc.HSEState = RCC_HSE_ON;
#endif
            osc.PLL.PLLState = RCC_PLL_NONE;
            if (HAL_RCC_OscConfig(&osc) != HAL_OK) return -3;
#if defined(RCC_SYSCLKSOURCE_HSE)
            clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
#else
            /* Some families do not allow direct HSE SYSCLK without PLL; fall back if needed */
            return -31;
#endif
            break;

        case PAL_CLK_SRC_PLL: {
            /* First enable the chosen input (HSI/HSE) */
            if (cfg->pll.use_hse) {
                osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
#if defined(RCC_HSE_ON)
                osc.HSEState = RCC_HSE_ON;
#endif
            } else {
                osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
#if defined(RCC_HSI_ON)
                osc.HSIState = RCC_HSI_ON;
#endif
#if defined(RCC_HSICALIBRATION_DEFAULT)
                osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
#endif
            }

            /* Family-agnostic PLL field fill (guards where fields exist) */
            osc.PLL.PLLState = RCC_PLL_ON;

#if defined(RCC_PLLSOURCE_HSE) && defined(RCC_PLLSOURCE_HSI)
            osc.PLL.PLLSource = cfg->pll.use_hse ? RCC_PLLSOURCE_HSE : RCC_PLLSOURCE_HSI;
#elif defined(RCC_PLLSOURCE_HSE)
            osc.PLL.PLLSource = RCC_PLLSOURCE_HSE; /* Assume HSE-only families */
#elif defined(RCC_PLLSOURCE_HSI)
            osc.PLL.PLLSource = RCC_PLLSOURCE_HSI; /* Assume HSI-only families */
#endif

#if defined(RCC_PLL_M)
            osc.PLL.PLLM = cfg->pll.m;
#endif
#if defined(RCC_PLL_N)
            osc.PLL.PLLN = cfg->pll.n;
#endif
#if defined(RCC_PLL_P)
            /* Some families encode P as divider/2 bits; HAL handles the enum/val mapping */
            osc.PLL.PLLP = cfg->pll.p;
#endif
#if defined(RCC_PLL_Q)
            osc.PLL.PLLQ = cfg->pll.q;
#endif
#if defined(RCC_PLL_R)
            osc.PLL.PLLR = cfg->pll.r;
#endif

            if (HAL_RCC_OscConfig(&osc) != HAL_OK) return -4;

#if defined(RCC_SYSCLKSOURCE_PLLCLK)
            clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
#else
            return -32; /* No PLL as SYSCLK on this part */
#endif
        } break;

        default:
            return -5;
    }

    /* ——— Bus prescalers ——— */
    clk.AHBCLKDivider  = map_ahb_div(cfg->ahb_div);
    clk.APB1CLKDivider = map_apb_div(cfg->apb1_div);
#if defined(RCC_CLOCKTYPE_PCLK2)
    clk.APB2CLKDivider = map_apb_div(cfg->apb2_div);
#endif

    /* You supply flash latency in cfg to avoid family-specific tables */
    if (HAL_RCC_ClockConfig(&clk, cfg->flash_latency) != HAL_OK) return -6;

    return 0;
}
