#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include"stm32f4xx.h"

void uartPrintf(const char* fmt, ...); 
#define OTA_FLAG_ADDR   0x08060000   // Sector 7 start address
#define OTA_FLAG_VALUE  0x1234ABCD




#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
