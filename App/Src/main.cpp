#include "main.h"
#include "pal_clock.hpp"
#include "pal_gpio.hpp"
#include "pal_board.hpp"


using namespace pal;
static void SystemClock_Config(void);
// pal_gpio_t LED = { .port = 0, .pin = 5 }; // GPIOA pin 5
 Gpio led(0, 5); // Port A, Pin 5

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /*Board init*/
   pal::Board::init();

  /* Configure the system clock */
  SystemClock_Config();

  // pal_gpio_config(LED, PAL_GPIO_OUTPUT, PAL_GPIO_NOPULL, PAL_GPIO_SPEED_VERY_HIGH, 0);
 
  led.config(Gpio::Mode::Output, Gpio::Pull::NoPull, Gpio::Speed::High);

  while (1)
  {

        led.write(true);
        pal::Board::delay(1000);
        led.write(false);
        pal::Board::delay(1000);

  }

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void)
{
    ClockConfig cfg;
    cfg.source = ClockSource::HSE;
    cfg.ahb_div = AhbDiv::DIV_1;
    cfg.apb1_div = ApbDiv::DIV_1;
    cfg.apb2_div = ApbDiv::DIV_1;
    cfg.flash_latency = 0;

    PalClock clock(cfg);
    if (!clock.init()) {
        // handle error
        while (1);
    }
}


