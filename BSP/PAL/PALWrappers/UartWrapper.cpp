#include "UartWrapper.hpp"

UartWrapper::UartWrapper(IVendorPolicy& policy, std::unique_ptr<ISerial> impl, const UartCfg& cfg)
: policy(policy), impl(std::move(impl)), cfg(cfg) {}

UartWrapper::~UartWrapper() = default;

void UartWrapper::init(){ 
  if (!policy.supportsUart(cfg.inst)) 
  while(1) {
      // Error: Unsupported UART instance
  }
  policy.enableGpioClock(cfg.tx.id.port); 
  policy.applyPin(cfg.tx);
  policy.enableGpioClock(cfg.rx.id.port); 
  policy.applyPin(cfg.rx);
  policy.enableUartClock(cfg.inst);
  if (cfg.use_irq == true){
   policy.enableUartIrq(cfg.inst);
  } 
  if (cfg.use_dma == true) {
    policy.enableDmaForUart(cfg.inst);
  }

  UartConfig uc; uc.baud = cfg.baud;
  uc.mode = cfg.use_dma ? UartMode::DMA : (cfg.use_irq ? UartMode::IRQ : UartMode::Polling);
  uc.irq = { cfg.use_irq, false }; uc.dma = { cfg.use_dma, cfg.use_dma };
  impl->init(uc);
}

uint8_t UartWrapper::sendBlocking(const uint8_t* d, size_t l){ 
  return impl->transmitBlocking(d,l); 
}

uint8_t UartWrapper::recvBlocking(uint8_t* d, size_t l){ 
  return impl->receiveBlocking(d,l); 
}

uint8_t UartWrapper::sendIT(const uint8_t* d, size_t l){ 
  return impl->transmitIT(d,l); 
}

uint8_t UartWrapper::recvIT(uint8_t* d, size_t l){ 
  return impl->receiveIT(d,l); 
}

uint8_t UartWrapper::sendDma(const uint8_t* d, size_t l){ 
  return impl->transmitDMA(d,l); 
}

uint8_t UartWrapper::recvDma(uint8_t* d, size_t l){ 
  return impl->receiveDMA(d,l); 
}

void UartWrapper::setRxByteCb(RxByteCb cb){ 
  impl->onRxByte(cb); 
}

void UartWrapper::setRxBlockCb(RxBlockCb cb){ 
  impl->onRxBlock(cb); 
}

void UartWrapper::setTxDoneCb(TxDoneCb cb){ 
  impl->onTxDone(cb); 
}

void UartWrapper::setErrorCb(ErrorCb cb){ 
  impl->onError(cb); 
}

UartInst UartWrapper::inst() const{ return cfg.inst; }
ISerial* UartWrapper::implPtr(){ return impl.get(); }
