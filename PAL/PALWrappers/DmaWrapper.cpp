#include "DmaWrapper.hpp"

DmaWrapper::DmaWrapper(std::unique_ptr<IDma> impl): impl(std::move(impl)) {}

DmaWrapper::~DmaWrapper() = default;

void DmaWrapper::startTx(const uint8_t* d, size_t l){ 
    impl->startTx(d,l); 
}

void DmaWrapper::startRx(uint8_t* d, size_t l){ 
    impl->startRx(d,l); 
}

bool DmaWrapper::txBusy(){ 
    return impl->isTxBusy(); 
}

bool DmaWrapper::rxBusy(){ 
    return impl->isRxBusy(); 
}
