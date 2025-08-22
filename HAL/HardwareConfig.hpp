#pragma once
#include "Common.hpp"

class UartWrapper;
class Pin;
class DmaWrapper;

#ifdef __cplusplus
extern "C" {
#endif

inline constexpr PinCfg kBoardPins[] = {
  {{Port::A,5}, PinMode::Output, PinPull::None, PinSpeed::Low, 0},
  {{Port::A,2}, PinMode::Alt, PinPull::Up, PinSpeed::High, 7},
  {{Port::A,3}, PinMode::Alt, PinPull::Up, PinSpeed::High, 7},
};

inline constexpr UartCfg kBoardUarts[] = {
  { UartInst::Uart2,
    {{Port::A,2}, PinMode::Alt, PinPull::Up, PinSpeed::High, 7},
    {{Port::A,3}, PinMode::Alt, PinPull::Up, PinSpeed::High, 7},
    115200, StopBits::One, Parity::None, true, true }
};


void Hardware_InitAll(void);
UartWrapper* Hardware_GetUartWrapper(UartInst inst);
Pin* Hardware_GetPinById(PinID id);
DmaWrapper* Hardware_GetDmaForUart(UartInst inst);

#ifdef __cplusplus
}
#endif
