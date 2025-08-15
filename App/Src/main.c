#include "main.h"
#include "pal_clock.h"
#include "pal_gpio.h"
#include "pal_board.h"

static void SystemClock_Config(void);
pal_gpio_t LED = { .port = 0, .pin = 5 }; // GPIOA pin 5

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /*Board init*/
  pal_board_init();

  /* Configure the system clock */
  SystemClock_Config();

  pal_gpio_config(LED, PAL_GPIO_OUTPUT, PAL_GPIO_NOPULL, PAL_GPIO_SPEED_VERY_HIGH, 0);

  while (1)
  {
        pal_gpio_write(LED, 1);
        pal_delay(200); // 100 ms delay
        pal_gpio_write(LED, 0);
        pal_delay(200);

  }

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void)
{
    pal_clock_cfg_t c = {
        .source         = PAL_CLK_SRC_HSI,
        .ahb_div        = PAL_AHB_DIV_1,
        .apb1_div       = PAL_APB_DIV_1,
        .apb2_div       = PAL_APB_DIV_1,
        .flash_latency  = 0  /* 16 MHz -> latency 0 on most families; adjust if needed */
    };
    //     pal_clock_cfg_t c = {
    //     .source   = PAL_CLK_SRC_PLL,
    //     .pll = {
    //         .use_hse = 1,
    //         .hse_hz  = 8000000u,
    //         .m = 8,
    //         .n = 336,
    //         .p = 4,
    //         .q = 7,   /* USB 48 MHz (336/7=48) on F4 */
    //         .r = 0
    //     },
    //     .ahb_div       = PAL_AHB_DIV_1,
    //     .apb1_div      = PAL_APB_DIV_2,  /* APB1 max 42 MHz on F4 */
    //     .apb2_div      = PAL_APB_DIV_1,
    //     .flash_latency = 0x00000002U /* 84 MHz on F4 typically needs 2 WS; check your datasheet */
    // };
    if (pal_clock_init(&c) != 0) {
        Error_Handler();
    }
}


