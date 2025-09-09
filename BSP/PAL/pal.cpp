#include "pal.hpp"


namespace PAL {
void initAll()
{ 
  Hardware_InitAll(); 
}

bool pinSet(PinID id, bool v)
{
  if (auto* p = Hardware_GetPinById(id))
  {
      p->set(v);
      return true; 
  } 
  return false;
}

uint8_t uartSendBlocking(UartInst inst, const uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    return u->sendBlocking(d,l);
  } 
    return -1;
}

uint8_t uartSendIT(UartInst inst, const uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    return u->sendIT(d,l);
  } 
    return -1;
}

uint8_t uartReceiveBlocking(UartInst inst,  uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    return u->recvBlocking(d,l);
  } 
    return -1;
}

uint8_t uartReceiveIT(UartInst inst,  uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    return u->recvIT(d,l);
  } 
    return -1;
}

bool setRXByteCb(UartInst inst, UartWrapper::RxByteCb cb)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->setRxByteCb(cb);
    return true; 
  } 
    return false;
}

bool setTXDoneCb(UartInst inst, UartWrapper::TxDoneCb cb)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->setTxDoneCb(cb);
    return true; 
  } 
    return false;
}

bool setRXBlockCb(UartInst inst, UartWrapper::RxBlockCb cb)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->setRxBlockCb(cb);
    return true; 
  } 
    return false;
}

bool setUartErrorCb(UartInst inst, UartWrapper::ErrorCb cb)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    u->setErrorCb(cb);
    return true; 
  } 
    return false;
}

uint8_t uartSendDma(UartInst inst, const uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
   return  u->sendDma(d,l);
  }
    return -1;
}

uint8_t uartRecvDma(UartInst inst,  uint8_t* d, size_t l)
{
  if (auto* u = Hardware_GetUartWrapper(inst))
  { 
    return u->recvDma(d,l);
  }
    return -1;
}

} // namespace PAL
