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

  void sendBlocking(const uint8_t* d, size_t l);
  void recvBlocking(uint8_t* d, size_t l);
  void sendIT(const uint8_t* d, size_t l);
  void recvIT(uint8_t* d, size_t l);
  void sendDma(const uint8_t* d, size_t l);
  void recvDma(uint8_t* d, size_t l);

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
