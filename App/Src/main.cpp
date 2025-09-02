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
#include "ei_run_classifier.h"

#define APP_START_ADDRESS  0x08010000

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
void thread0_entry(ULONG thread_input);
void thread1_entry(ULONG thread_input);

/* Thread control block and stack */
TX_THREAD thread_0;
TX_THREAD thread_1;
UCHAR thread_0_stack[2096];
UCHAR thread_1_stack[2096];

// ThreadX queue for UART bytes
TX_QUEUE uartQueue;
#define UART_QUEUE_SIZE 16
uint8_t uartQueueBuffer[UART_QUEUE_SIZE];
TX_EVENT_FLAGS_GROUP app_events;
#define EVENT_START_CLASSIFIER   0x01
TX_SEMAPHORE sem_trigger;
TX_SEMAPHORE sem_done;

// Message to send to PC
const char msg[] = "Hello from STM32 Board!\r\n";

uint32_t cpu;
uint32_t hclk ;
uint32_t apb1;
uint32_t apb2;

#define RX_BUFFER_SIZE 1024
uint8_t rxData;          // single byte buffer
char rxBuffer[RX_BUFFER_SIZE]={};   // DMA receive buffer
char txBuffer[RX_BUFFER_SIZE]={};   // DMA transmit buffer
volatile uint16_t rxIndex = 0;      // current position in buffer
uint8_t rxByte;                     // temp byte
PinID led{Port::A,5};
uint8_t TxData[64];
int isSent = 1;
int countloop = 0;
int countinterrupt = 0;
volatile bool txBusy = false;



// Example raw features (replace with your actual copied raw features)
static float input_buf[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {-0.0302, -0.9898, 0.5202, 0.0365, -1.0355, 0.4656, 0.1876, -1.0607, 0.4742, 0.1598, -0.3744, 0.4760, 0.2776, -0.9267, 0.4609, 0.2744, -0.9788, 0.5310, 0.2949, -0.9748, 0.4998, 0.4208, -1.0078, 0.5534, 0.4539, -0.9691, 0.4952, 0.5990, -0.9357, 0.5597, 0.5755, -0.9768, 0.6215, 0.6598, -0.8641, 0.4362, 0.6231, -0.9155, 0.4812, 0.7938, -0.9077, 0.5173, 0.7998, -0.9822, 0.4188, 0.7571, -0.7619, 0.4714, 0.7589, -0.8363, 0.5146, 0.8811, -0.9012, 0.4573, 0.8980, -0.8583, 0.4987, 0.9206, -0.8309, 0.5240, 0.9863, -0.8791, 0.4889, 0.9729, -0.8811, 0.5136, 1.0226, -0.8193, 0.4923, 0.9598, -0.7778, 0.5490, 0.9844, -0.6933, 0.5223, 1.0028, -0.6903, 0.5189, 0.9749, -0.7441, 0.5482, 0.9786, -0.6119, 0.3894, 0.9481, -0.6180, 0.5166, 0.9925, -0.5485, 0.4976, 1.0352, -0.6631, 0.5415, 0.9665, -0.5621, 0.5736, 0.9674, -0.5696, 0.5535, 0.8846, -0.4789, 0.4161, 0.8084, -0.4802, 0.4799, 0.7938, -0.4167, 0.5220, 0.7549, -0.3881, 0.4080, 0.8219, -0.4495, 0.5030, 0.7140, -0.3373, 0.4600, 0.6782, -0.3418, 0.5328, 0.5855, -0.3264, 0.4752, 0.5280, -0.2546, 0.4166, 0.4547, -0.3228, 0.5148, 0.4005, -0.2228, 0.4575, 0.2668, -0.1837, 0.4518, 0.2604, -0.1014, 0.4641, 0.2618, -0.1958, 0.5414, 0.2581, -0.1398, 0.5260, 0.0973, -0.0383, 0.4869, 0.0802, -0.0445, 0.4708, -0.0478, 0.0352, 0.4674, -0.1334, 0.0484, 0.3329, -0.0520, 0.0287, 0.4819, -0.1922, 0.0235, 0.4716, -0.2150, 0.0947, 0.5137, -0.3177, 0.2661, 0.4406, -0.3151, 0.1531, 0.4911, -0.4157, 0.1587, 0.5066, -0.4524, 0.2112, 0.4103, -0.5426, 0.1889, 0.4613, -0.5753, 0.3993, 0.4721, -0.5921, 0.3732, 0.4933, -0.7071, 0.3851, 0.3719, -0.6807, 0.3806, 0.5713, -0.7123, 0.3358, 0.5181, -0.8434, 0.3840, 0.4343, -0.8404, 0.5330, 0.4827, -0.9005, 0.5681, 0.5524, -0.8994, 0.5326, 0.4854, -0.9681, 0.5586, 0.3923, -0.9824, 0.5994, 0.2960, -0.9380, 0.6531, 0.3970, -0.9580, 0.6377, 0.4547, -1.0637, 0.7114, 0.4003, -0.9575, 0.7290, 0.3718, -0.9700, 0.6502, 0.3195, -0.9452, 0.7424, 0.3769, -0.9756, 0.7232, 0.3853, -0.9723, 0.7362, 0.3455, -0.9707, 0.8449, 0.3789, -1.0519, 0.7776, 0.3697, -1.0264, 0.8577, 0.3864, -0.8014, 0.9061, 0.4371, -0.8835, 0.8566, 0.3237, -0.8564, 0.9556, 0.4007, -0.9672, 0.9023, 0.3877, -0.8224, 0.9388, 0.3521, -0.7147, 0.8749, 0.3421, -0.5690, 0.8148, 0.3303, -0.6631, 0.9096, 0.3240, -0.5736, 0.9605, 0.2905, -0.5137, 0.9446, 0.4163, -0.4433, 0.9402, 0.2833, -0.4344, 1.0324, 0.3384, -0.3200, 0.9814, 0.3329, -0.3351, 1.0288, 0.3661, -0.1686, 0.9322, 0.2972, -0.1992, 1.8419, 0.2649, -0.1505, 1.0055, 0.2608, -0.0633, 0.9833, 0.1926};

static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (input_buf + offset)[i];
    }
    return EIDSP_OK;
}


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

    /* Relocate vector table for app */
  SCB->VTOR = APP_START_ADDRESS;

  PAL::initAll();
  PAL::pinSet(led, false);

  uartPrintf("System initialized.\r\n");

#if 1
  PAL::setRXByteCb(UartInst::Uart2,onUartByte);
  PAL::setTXDoneCb(UartInst::Uart2,onUartTxByte);
  PAL::uartReceiveIT(UartInst::Uart2,  &rxData, 1);
  // PAL::setUartErrorCb(UartInst::Uart2,onUartErrorByte);
  // PAL::setRXBlockCb(UartInst::Uart2,onUartBlock);
  // PAL::uartRecvDma(UartInst::Uart2, reinterpret_cast<uint8_t*>(rxBuffer), RX_BUFFER_SIZE);
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
    tx_thread_create(&thread_0, (CHAR*)"thread 0",
                     thread0_entry, 0,
                     thread_0_stack, sizeof(thread_0_stack),
                     2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);

        /* Create a thread */
    tx_thread_create(&thread_1, (CHAR*)"thread 1",
                     thread1_entry, 0,
                     thread_1_stack, sizeof(thread_1_stack),
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

        // Create UART queue
    tx_queue_create(&uartQueue, (CHAR*)"UART Queue", 1,
                    uartQueueBuffer, sizeof(uartQueueBuffer));
    
    tx_event_flags_create(&app_events, "App Events");

    tx_semaphore_create(&sem_trigger, "Classifier Sem", 0);
    tx_semaphore_create(&sem_done, "Done Sem", 0);
}

/* Thread entry */
void thread0_entry(ULONG thread_input)
{
    signal_t signal;
    ei_impulse_result_t result;

    while (1)
    {
        // Wait until Thread1 gives a trigger
        tx_semaphore_get(&sem_trigger, TX_WAIT_FOREVER);

        // === Run classifier ===
        signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
        signal.get_data = &get_signal_data;

        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
        uartPrintf("run_classifier returned: %d\n", res);

        for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
            uartPrintf("  %s: %.5f\n",
                       ei_classifier_inferencing_categories[i],
                       result.classification[i].value);
        }

    #if EI_CLASSIFIER_HAS_ANOMALY == 1
        uartPrintf("Anomaly: %.3f\n", result.anomaly);
    #endif

        uartPrintf("Predictions: from Mithlesh Katre\n");

        // Notify Thread1 that we're done
        tx_semaphore_put(&sem_done);
    }
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

        // Trigger classifier when 'm' or 'M' received
        if (rx == 'm' || rx == 'M') {
        if (tx_semaphore_ceiling_put(&sem_trigger, 1) != TX_SUCCESS) {
            // Already queued, do nothing
        }
        tx_semaphore_get(&sem_done, TX_WAIT_FOREVER);
        uartPrintf("Classifier finished, ready for next input\n");
        }

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

    cpu = clock.getSysClkHz();
    hclk = clock.getHclkHz();
    apb1 = clock.getApb1Hz();
    apb2 = clock.getApb2Hz();
   
}


