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
#include"tx_api.h"


static void SystemClock_Config(void);
void uartPrintf(const char* fmt, ...); 
void onUartByte(uint8_t byte) ;
void onUartTxByte(void) ;
void onUartErrorByte(void) ;
void onUartBlock(const uint8_t* data, size_t len);


/* ThreadX byte pool */
TX_BYTE_POOL byte_pool_0;
#define BYTE_POOL_SIZE 16*1024
UCHAR byte_pool_memory[BYTE_POOL_SIZE];

/* Forward declarations */
void tx_application_define(void *first_unused_memory);
void thread1_entry(ULONG thread_input);
void thread0_entry(ULONG thread_input);

/* Thread control block and stack */
TX_THREAD thread_1;
TX_THREAD thread_0;
UCHAR thread_1_stack[2096];
UCHAR thread_0_stack[2096];

// ThreadX queue for UART bytes
TX_QUEUE uartQueue;
#define UART_QUEUE_SIZE 16
uint8_t uartQueueBuffer[UART_QUEUE_SIZE];



// Message to send to PC
const char msg[] = "Hello from STM32 Board!\r\n";


#define RX_BUFFER_SIZE 1024
uint8_t rxData;          // single byte buffer
char rxBuffer[RX_BUFFER_SIZE]={};   // DMA receive buffer
char txBuffer[RX_BUFFER_SIZE]={};   // DMA transmit buffer
volatile uint16_t rxIndex = 0;      // current position in buffer
uint8_t rxByte;                     // temp byte
PinID led{Port::A,5};
uint8_t TxData[64];
volatile bool txBusy = false;
volatile bool dmaflag = false;



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
  PAL::pinSet(led, false);

  uartPrintf("System initialized.\r\n");

#if 1
  PAL::setRXByteCb(UartInst::Uart2,onUartByte);
  PAL::setTXDoneCb(UartInst::Uart2,onUartTxByte);
  PAL::uartReceiveIT(UartInst::Uart2,  &rxData, 1);
  PAL::setUartErrorCb(UartInst::Uart2,onUartErrorByte);
//   PAL::setRXBlockCb(UartInst::Uart2,onUartBlock);
//   PAL::uartRecvDma(UartInst::Uart2, reinterpret_cast<uint8_t*>(rxBuffer), RX_BUFFER_SIZE);
  // PAL::uartSendDma(UartInst::Uart2, reinterpret_cast<const uint8_t*>(TxData), sizeof(TxData));
#endif

  // Initialize ThreadX
  tx_kernel_enter();

while (1)
  {


  }
}

/* Define application objects */
void tx_application_define(void *first_unused_memory)
{
    /* Create byte pool from linker section if preferred */
    tx_byte_pool_create(&byte_pool_0, (CHAR*)"byte pool 0",
                        byte_pool_memory, BYTE_POOL_SIZE);

            /* Create a thread */
    // tx_thread_create(&thread_0, (CHAR*)"thread 0",
    //                  thread0_entry, 0,
    //                  thread_0_stack, sizeof(thread_1_stack),
    //                  1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
        /* Create a thread */
    tx_thread_create(&thread_1, (CHAR*)"thread 1",
                     thread1_entry, 0,
                     thread_1_stack, sizeof(thread_1_stack),
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

        // Create UART queue
    tx_queue_create(&uartQueue, (CHAR*)"UART Queue", 1,
                    uartQueueBuffer, sizeof(uartQueueBuffer));
    
}


void thread1_entry(ULONG thread_input)
{
    uint8_t rx;
    uint8_t msgBuffer[64];
    uint8_t msgIndex = 0;

    while (1)
    {
        tx_queue_receive(&uartQueue, &rx, TX_WAIT_FOREVER);
        msgBuffer[msgIndex++] = rx;
        // End of message (your existing UART echo logic)
        if (rx == '\r' || msgIndex >= sizeof(msgBuffer))
        {
            while (txBusy) tx_thread_sleep(1);
            memcpy(TxData, msgBuffer, msgIndex);
            txBusy = true;
            PAL::uartSendIT(UartInst::Uart2, TxData, msgIndex);
            msgIndex = 0;
        }
    }
}

// Application-level callback
void onUartByte(uint8_t byte) {
#if 0
    if (byte == '\r' || byte == '\n' || rxIndex >= sizeof(rxBuffer) - 1) {
        rxBuffer[rxIndex] = '\0';   // null-terminate the string

        // uartPrintf("Received: %s\r\n", rxBuffer);  // print received line

        // PAL::uartSendIT(UartInst::Uart2, reinterpret_cast<const uint8_t*>(rxBuffer),
                          // strlen(rxBuffer));

        rxIndex = 0;  // reset for next line
    } else {
        rxBuffer[rxIndex++] = byte; // store received byte
    }

    // Re-arm reception for next byte
    PAL::uartReceiveIT(UartInst::Uart2, &rxData, 1);
#endif
        tx_queue_send(&uartQueue, &byte, TX_NO_WAIT);

        // // Restart UART interrupt reception
         PAL::uartReceiveIT(UartInst::Uart2, &rxData, 1);
    
}

void onUartBlock(const uint8_t* data, size_t len) {

  uartPrintf("\r\n.........................\r\n");
  // PAL::uartSendBlocking(UartInst::Uart2, data, len);
  PAL::uartSendDma(UartInst::Uart2, const_cast<uint8_t*>(data), len);
  uartPrintf("\r\n##########################\r\n");
  // PAL::uartRecvDma(UartInst::Uart2, (data), len);
  // PAL::uartSendDma(UartInst::Uart2, const_cast<uint8_t*>(data), len);
}



void onUartTxByte(void) {
    txBusy = false; 
    // PAL::pinSet(led, true);
    // uartPrintf("\r\nTransmission complete.\r\n");
}

void onUartErrorByte(void){

    PAL::pinSet(led, false);
    uartPrintf("\r\nUART Error occurred.\r\n");
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
}
