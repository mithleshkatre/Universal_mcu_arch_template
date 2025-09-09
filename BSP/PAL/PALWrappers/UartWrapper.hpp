#pragma once
#include "IVendorPolicy.hpp"
#include "ISerial.hpp"
#include <functional>
#include <memory>

class UartWrapper {
public:
  using RxByteCb = ISerial::RxByteCb;
  using RxBlockCb = ISerial::RxBlockCb;
  using TxDoneCb = ISerial::TxDoneCb;
  using ErrorCb = ISerial::ErrorCb;

  UartWrapper(IVendorPolicy& policy, std::unique_ptr<ISerial> impl, const UartCfg& cfg);
  ~UartWrapper();
  void init();

  uint8_t sendBlocking(const uint8_t* d, size_t l);
  uint8_t recvBlocking(uint8_t* d, size_t l);
  uint8_t sendIT(const uint8_t* d, size_t l);
  uint8_t recvIT(uint8_t* d, size_t l);
  uint8_t sendDma(const uint8_t* d, size_t l);
  uint8_t recvDma(uint8_t* d, size_t l);

  void setRxByteCb(RxByteCb cb);
  void setRxBlockCb(RxBlockCb cb);
  void setTxDoneCb(TxDoneCb cb);
  void setErrorCb(ErrorCb cb);

  UartInst inst() const;
  ISerial* implPtr();

private:
  IVendorPolicy& policy;
  std::unique_ptr<ISerial> impl;
  UartCfg cfg;
};
