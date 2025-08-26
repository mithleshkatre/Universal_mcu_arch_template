#include "main.h"
#include "stm32_clock.hpp"
#include "stm32_hal_init.hpp"
#include "pal.hpp"
#include <cstring>
#include <cstdio>
#include <cstdarg>
extern "C" {
  #include <string.h>
  #include <stdio.h>
}

#include"Pin.hpp"
#include"Stm32IO.hpp"
#include "Stm32F4Policy.hpp"
#include "Stm32Uart.hpp"


static void SystemClock_Config(void);
void uartPrintf(const char* fmt, ...); 
void onUartByte(uint8_t byte) ;
void onUartTxByte(void) ;

UART_HandleTypeDef huart2;
// Message to send to PC
const char msg[] = "Hello from STM32 Board!\r\n";

uint32_t cpu;
uint32_t hclk ;
uint32_t apb1;
uint32_t apb2;

#define RX_BUFFER_SIZE 64
uint8_t rxData;          // single byte buffer
char rxBuffer[RX_BUFFER_SIZE];   // DMA receive buffer
uint8_t txBuffer[RX_BUFFER_SIZE];   // DMA transmit buffer
volatile uint16_t rxIndex = 0;      // current position in buffer
uint8_t rxByte;                     // temp byte
stm32f4_policy::Stm32F4Policy policy;

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



#if 0
  auto ledGpio = std::make_unique<Stm32IO>();
  PinCfg ledCfg{{Port::A,5}, PinMode::Output, PinPull::None, PinSpeed::Low, 0}; // output, no pull-up

  Pin led(policy, std::move(ledGpio), ledCfg);

  led.init();

  auto tx = std::make_unique<Stm32IO>();
  PinCfg ledCfgTX{{Port::A,2}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7}; 
  Pin Tx(policy, std::move(tx), ledCfgTX);
  Tx.init();
  auto rx = std::make_unique<Stm32IO>();
  PinCfg ledCfgRX {{Port::A,3}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7}; 
  Pin Rx(policy, std::move(rx), ledCfgRX);
  Rx.init();



  UartCfg uartCfg  { UartInst::Uart2,
    {{Port::A,2}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7},
    {{Port::A,3}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7},
    115200, StopBits::One, Parity::None, true, false };

  auto impl = std::make_unique<Stm32Uart>(uartCfg.inst==UartInst::Uart2?USART2:USART1,
                                             DMA1_Stream6, DMA_CHANNEL_4, DMA1_Stream5, DMA_CHANNEL_4);

    g_uart2 = impl.get();
    UartWrapper UartWrapper(policy, std::move(impl), uartCfg);
    UartWrapper.init();

    // UartWrapper.sendBlocking(reinterpret_cast<const uint8_t*>(msg), strlen(msg));
    UartWrapper.recvIT(&rxData, 1);  
    // HAL_UART_Receive_IT(&huart2, &rxData, 1);
    
#endif

  PAL::initAll();
  PAL::pinSet(led, false);

  char buffer[100];
  int len = snprintf(buffer, sizeof(buffer),
                    "CPU=%lu, HCLK=%lu, APB1=%lu, APB2=%lu\r\n",
                    cpu, hclk, apb1, apb2);

  uartPrintf("System initialized.\r\n");
  uartPrintf("Frequencies: CPU=%lu, HCLK=%lu, APB1=%lu, APB2=%lu\r\n", cpu, hclk, apb1, apb2);


  PAL::setRXByteCb(UartInst::Uart2,onUartByte);
  PAL::setTXDoneCb(UartInst::Uart2,onUartTxByte);
  PAL::uartReceiveIT(UartInst::Uart2,  &rxData, 1);
 
while (1)
  {


  }
}
// Application-level callback
void onUartByte(uint8_t byte) {
    if (byte == '\r' || byte == '\n' || rxIndex >= sizeof(rxBuffer) - 1) {
        rxBuffer[rxIndex] = '\0';   // null-terminate the string

        // uartPrintf("Received: %s\r\n", rxBuffer);  // print received line
        PAL::uartSendIT(UartInst::Uart2, reinterpret_cast<const uint8_t*>(rxBuffer),
                          strlen(rxBuffer));

        rxIndex = 0;  // reset for next line
    } else {
        rxBuffer[rxIndex++] = byte; // store received byte
    }

    // Re-arm reception for next byte
    PAL::uartReceiveIT(UartInst::Uart2, &rxData, 1);
}


void onUartTxByte(void) {

    PAL::pinSet(led, true);
    uartPrintf("\r\nTransmission complete.\r\n");
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

    cpu = clock.getSysClkHz();
    hclk = clock.getHclkHz();
    apb1 = clock.getApb1Hz();
    apb2 = clock.getApb2Hz();
   
}


