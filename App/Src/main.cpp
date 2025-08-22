#include "main.h"
#include "stm32_clock.hpp"
#include "stm32_hal_init.hpp"
#include "pal.hpp"


static void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /*Board init*/
   Board::init();

  /* Configure the system clock */
  SystemClock_Config();
 
  PAL::initAll();

  PinID led{Port::A,5};
  PAL::pinSet(led, false);


  while (1)
  {
    PAL::pinSet(led, true);
    Board::delay(500);
    PAL::pinSet(led, false);
    Board::delay(500);

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


