#pragma once
#include "HardwareConfig.hpp"
#include "Pin.hpp"
#include "UartWrapper.hpp"


namespace PAL {

inline void initAll()
{ 
  Hardware_InitAll(); 
}

inline bool pinSet(PinID id, bool v)
{
  if (auto* p = Hardware_GetPinById(id))
  {
      p->set(v);
      return true; 
  } 
  return false;
}

inline bool uartSendBlocking(UartInst inst, const uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->sendBlocking(d,l);
    return true; 
  } 
    return false;
}

inline bool uartSendIT(UartInst inst, const uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->sendIT(d,l);
    return true; 
  } 
    return false;
}

inline bool uartReceiveBlocking(UartInst inst,  uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->recvBlocking(d,l);
    return true; 
  } 
    return false;
}

inline bool uartReceiveIT(UartInst inst,  uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->recvIT(d,l);
    return true; 
  } 
    return false;
}

inline bool setRXByteCb(UartInst inst, UartWrapper::RxByteCb cb)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->setRxByteCb(cb);
    return true; 
  } 
    return false;
}

inline bool setTXDoneCb(UartInst inst, UartWrapper::TxDoneCb cb)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->setTxDoneCb(cb);
    return true; 
  } 
    return false;
}

inline bool setRXBlockCb(UartInst inst, UartWrapper::RxBlockCb cb)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->setRxBlockCb(cb);
    return true; 
  } 
    return false;
}

inline bool setUartErrorCb(UartInst inst, UartWrapper::ErrorCb cb)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->setErrorCb(cb);
    return true; 
  } 
    return false;
}

inline bool uartSendDma(UartInst inst, const uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->sendDma(d,l);
    return true;
  }
    return false;
}

inline bool uartRecvDma(UartInst inst,  uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->recvDma(d,l);
    return true;
  }
    return false;
}
} // namespace PAL
