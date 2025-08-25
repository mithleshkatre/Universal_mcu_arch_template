#pragma once

#include "stm32f4xx_hal.h"
#include "IDma.hpp"

class Stm32Dma : public IDma {
public:
  Stm32Dma(DMA_Stream_TypeDef* stream);
  ~Stm32Dma();
  void startTx(const uint8_t* data, size_t len) override;
  void startRx(uint8_t* data, size_t len) override;
  bool isTxBusy() override;
  bool isRxBusy() override;
private:
  DMA_Stream_TypeDef* stream_;
};
