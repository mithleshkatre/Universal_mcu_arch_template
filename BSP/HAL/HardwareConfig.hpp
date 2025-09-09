#pragma once
#include "Common.hpp"
#include "uart_config.hpp"

class UartWrapper;
class Pin;
class DmaWrapper;

#ifdef __cplusplus
extern "C" {
#endif

inline constexpr PinCfg kBoardPins[] = {
  {{Port::A,5}, PinMode::Output, PinPull::None, PinSpeed::Low, 0},
};

inline constexpr UartCfg kBoardUarts[] = {
  { UartInst::Uart2,
    {{Port::A,2}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7},
    {{Port::A,3}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7},
    115200, StopBits::One, Parity::None, true, true,
    {USART2, DMA1_Stream6, DMA_CHANNEL_4, DMA1_Stream5, DMA_CHANNEL_4}},
    
    { UartInst::Uart3,
    {{Port::C,10}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7},
    {{Port::C,11}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7},
    115200, StopBits::One, Parity::None, true, false,
    {USART3, DMA1_Stream3, DMA_CHANNEL_4, DMA1_Stream1, DMA_CHANNEL_4} }
};



void Hardware_InitAll(void);
UartWrapper* Hardware_GetUartWrapper(UartInst inst);
Pin* Hardware_GetPinById(PinID id);
DmaWrapper* Hardware_GetDmaForUart(UartInst inst);

#ifdef __cplusplus
}
#endif
