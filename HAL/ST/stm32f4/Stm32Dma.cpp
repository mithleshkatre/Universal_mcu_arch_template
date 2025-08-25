#include "Stm32Dma.hpp"

Stm32Dma::Stm32Dma(DMA_Stream_TypeDef* stream): stream_(stream) {}

Stm32Dma::~Stm32Dma(){}

void Stm32Dma::startTx(const uint8_t* data, size_t len) { 
    (void)stream_; 
    (void)data; 
    (void)len; 
}

void Stm32Dma::startRx(uint8_t* data, size_t len) { 
    (void)stream_; 
    (void)data; 
    (void)len; 
}

bool Stm32Dma::isTxBusy(){ 
    return false; 
}

bool Stm32Dma::isRxBusy(){ 
    return false; 
}
