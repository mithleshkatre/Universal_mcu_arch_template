#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Clock source selection */
typedef enum {
    PAL_CLK_SRC_HSI = 0,   /* Internal RC */
    PAL_CLK_SRC_HSE,       /* External crystal */
    PAL_CLK_SRC_PLL        /* PLL (derive from HSI or HSE) */
} pal_clk_source_t;

/* Simple dividers (match HAL RCC div IDs) */
typedef enum {
    PAL_AHB_DIV_1   = 1,
    PAL_AHB_DIV_2   = 2,
    PAL_AHB_DIV_4   = 4,
    PAL_AHB_DIV_8   = 8,
    PAL_AHB_DIV_16  = 16,
    PAL_AHB_DIV_64  = 64,
    PAL_AHB_DIV_128 = 128,
    PAL_AHB_DIV_256 = 256,
    PAL_AHB_DIV_512 = 512
} pal_ahb_div_t;

typedef enum {
    PAL_APB_DIV_1 = 1,
    PAL_APB_DIV_2 = 2,
    PAL_APB_DIV_4 = 4,
    PAL_APB_DIV_8 = 8,
    PAL_APB_DIV_16 = 16
} pal_apb_div_t;

/* Universal PLL config (set only if you use PAL_CLK_SRC_PLL) */
typedef struct {
    uint8_t  use_hse;     /* 0: HSI as PLL input, 1: HSE as PLL input */
    uint32_t hse_hz;      /* if use_hse=1, supply crystal frequency */
    /* Generic PLL factors (map to family-specific fields in BSP): */
    uint32_t m;           /* input divider  (e.g., F4/H7) or ignored if not present */
    uint32_t n;           /* VCO multiplier */
    uint32_t p;           /* main sysclk divider or post-div */
    uint32_t q;           /* optional (USB etc.), ignored if N/A */
    uint32_t r;           /* optional (some L4/H7), ignored if N/A */
} pal_pll_cfg_t;

/* Top-level clock config */
typedef struct {
    pal_clk_source_t source;   /* HSI, HSE, or PLL */
    pal_pll_cfg_t    pll;      /* Valid if source == PAL_CLK_SRC_PLL */
    pal_ahb_div_t    ahb_div;  /* AHB prescaler  */
    pal_apb_div_t    apb1_div; /* APB1 prescaler */
    pal_apb_div_t    apb2_div; /* APB2 prescaler (ignored on parts without APB2) */
    uint32_t         flash_latency; /* Provide explicit FLASH latency to avoid family differences */
} pal_clock_cfg_t;

/* Initialize system clocks as requested. Returns 0 on success, <0 on error. */
int pal_clock_init(const pal_clock_cfg_t *cfg);

#ifdef __cplusplus
}
#endif
