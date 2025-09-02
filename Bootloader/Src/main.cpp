#include "main.h"
#include "stm32_clock.hpp"
#include "stm32_hal_init.hpp"
#include "pal.hpp"
#include <cstring>
#include <cstdio>
#include <cstdarg>

static void SystemClock_Config(void);
static void goto_application(void);

PinID led{Port::A,5};

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
  PAL::pinSet(led, true);

  Board::delay(2000);
  uartPrintf("System initialized.\r\n");
  goto_application();

while (1)
  {
      // PAL::pinSet(led, true);
      // Board::delay(500);
      // PAL::pinSet(led, false);
      // Board::delay(500);

  }
}


static void goto_application(void)
{
    uartPrintf("Gonna Jump to Application\n");
    // 1. Get stack pointer value from application vector table
    uint32_t app_stack = *((volatile uint32_t*) 0x08010000);

    // 2. Get reset handler address (vector table + 4)
    uint32_t app_reset_addr = *((volatile uint32_t*) (0x08010000 + 4U));

    // 3. Define function pointer with correct cast
    void (*app_reset_handler)(void) = reinterpret_cast<void (*)(void)>(app_reset_addr);

    // 4. Set main stack pointer
    __set_MSP(app_stack);

    // 5. Jump to application
    app_reset_handler();
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void)
{
    // Example: target ~120 MHz SYSCLK
    ClockConfig cfg;
    cfg.source = ClockSource::PLL;
    cfg.pll.use_hse = false;      // use HSI (16 MHz)
    cfg.pll.m = 8;
    cfg.pll.n = 120;
    cfg.pll.p = 2;
    cfg.pll.q = 2;
    cfg.pll.r = 2;
    cfg.flash_latency = FLASH_LATENCY_3;

    cfg.ahb_div  = AhbDiv::DIV_1;
    cfg.apb1_div = ApbDiv::DIV_4;
    cfg.apb2_div = ApbDiv::DIV_2;

    PalClock clock(cfg);
    if (!clock.init()) {
        while (1); // error
    }

    // cpu = clock.getSysClkHz();
    // hclk = clock.getHclkHz();
    // apb1 = clock.getApb1Hz();
    // apb2 = clock.getApb2Hz();
   
}

// ---------- Single helper function ----------
void uartPrintf(const char* fmt, ...) {
    char buffer[128];   // adjust size if you need longer messages
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    PAL::uartSendBlocking(UartInst::Uart2,
                          reinterpret_cast<const uint8_t*>(buffer),
                          strlen(buffer));
}


