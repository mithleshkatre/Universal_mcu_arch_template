#pragma once
#include "HardwareConfig.hpp"
#include "Pin.hpp"
#include "UartWrapper.hpp"


namespace PAL {
inline void initAll(){ Hardware_InitAll(); }
inline bool pinSet(PinID id, bool v){
  if (auto* p = Hardware_GetPinById(id)){ p->set(v); return true; } return false;
}
inline bool uartSendBlocking(UartInst inst, const uint8_t* d, size_t l){
  if (auto* u = Hardware_GetUartWrapper(inst)){ u->sendBlocking(d,l); return true; } return false;
}
inline bool uartSendDma(UartInst inst, const uint8_t* d, size_t l){
  if (auto* u = Hardware_GetUartWrapper(inst)){ u->sendDma(d,l); return false; } return false;
}
} // namespace PAL
