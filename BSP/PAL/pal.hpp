#pragma once
#include "HardwareConfig.hpp"
#include "Pin.hpp"
#include "UartWrapper.hpp"

namespace PAL {
void initAll(void);
bool pinSet(PinID id, bool v);
uint8_t uartSendBlocking(UartInst inst, const uint8_t* d, size_t l);
uint8_t uartSendIT(UartInst inst, const uint8_t* d, size_t l);
uint8_t uartReceiveBlocking(UartInst inst,  uint8_t* d, size_t l);
uint8_t uartReceiveIT(UartInst inst,  uint8_t* d, size_t l);
bool setRXByteCb(UartInst inst, UartWrapper::RxByteCb cb);
bool setTXDoneCb(UartInst inst, UartWrapper::TxDoneCb cb);
bool setRXBlockCb(UartInst inst, UartWrapper::RxBlockCb cb);
bool setUartErrorCb(UartInst inst, UartWrapper::ErrorCb cb);
uint8_t uartSendDma(UartInst inst, const uint8_t* d, size_t l);
uint8_t uartRecvDma(UartInst inst,  uint8_t* d, size_t l);

} // namespace PAL