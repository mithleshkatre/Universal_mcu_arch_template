#include "Stm32Uart.hpp"

static Stm32Uart::Reg registry_[4] = {{USART1,nullptr},{USART2,nullptr},{USART3,nullptr},{USART6,nullptr}};

Stm32Uart::Stm32Uart(USART_TypeDef* inst, DMA_Stream_TypeDef* txStream, uint32_t txChannel, DMA_Stream_TypeDef* rxStream, uint32_t rxChannel)
: txStream_(txStream), rxStream_(rxStream), txChannel_(txChannel), rxChannel_(rxChannel) {
  huart_.Instance = inst;
  htx_.Instance = txStream_;
  hrx_.Instance = rxStream_;
  registerInstance(inst, this);
}

void Stm32Uart::init(const UartConfig& cfg) {
  cfg_ = cfg;
  huart_.Init.BaudRate = cfg.baud;
  huart_.Init.WordLength = UART_WORDLENGTH_8B;
  huart_.Init.StopBits = UART_STOPBITS_1;
  huart_.Init.Parity = UART_PARITY_NONE;
  huart_.Init.Mode = UART_MODE_TX_RX;
  huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart_.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart_);

  if (txStream_) {
    htx_.Init.Channel = txChannel_;
    htx_.Init.Direction = DMA_MEMORY_TO_PERIPH;
    htx_.Init.PeriphInc = DMA_PINC_DISABLE;
    htx_.Init.MemInc = DMA_MINC_ENABLE;
    htx_.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    htx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    htx_.Init.Mode = DMA_NORMAL;
    htx_.Init.Priority = DMA_PRIORITY_LOW;
    htx_.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&htx_);
    __HAL_LINKDMA(&huart_, hdmatx, htx_);
  }
  if (rxStream_) {
    hrx_.Init.Channel = rxChannel_;
    hrx_.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hrx_.Init.PeriphInc = DMA_PINC_DISABLE;
    hrx_.Init.MemInc = DMA_MINC_ENABLE;
    hrx_.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hrx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hrx_.Init.Mode = DMA_CIRCULAR;
    hrx_.Init.Priority = DMA_PRIORITY_HIGH;
    hrx_.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hrx_);
    __HAL_LINKDMA(&huart_, hdmarx, hrx_);
  }
}

void Stm32Uart::transmitBlocking(const uint8_t* data, size_t len) { HAL_UART_Transmit(&huart_, const_cast<uint8_t*>(data), (uint16_t)len, HAL_MAX_DELAY); }
void Stm32Uart::receiveBlocking(uint8_t* data, size_t len) { HAL_UART_Receive(&huart_, data, (uint16_t)len, HAL_MAX_DELAY); }

void Stm32Uart::transmitIT(const uint8_t* data, size_t len) { HAL_UART_Transmit_IT(&huart_, const_cast<uint8_t*>(data), (uint16_t)len); }
void Stm32Uart::receiveIT(uint8_t* data, size_t len) { HAL_UART_Receive_IT(&huart_, data, (uint16_t)len); }

void Stm32Uart::transmitDMA(const uint8_t* data, size_t len) { HAL_UART_Transmit_DMA(&huart_, const_cast<uint8_t*>(data), (uint16_t)len); }
void Stm32Uart::receiveDMA(uint8_t* data, size_t len) { HAL_UART_Receive_DMA(&huart_, data, (uint16_t)len); }

void Stm32Uart::onRxByte(RxByteCb cb) { rxByteCb_ = std::move(cb); }
void Stm32Uart::onRxBlock(RxBlockCb cb) { rxBlockCb_ = std::move(cb); }
void Stm32Uart::onTxDone(TxDoneCb cb) { txDoneCb_ = std::move(cb); }
void Stm32Uart::onError(ErrorCb cb) { errCb_ = std::move(cb); }

void Stm32Uart::handleUartIrq() {
  HAL_UART_IRQHandler(&huart_);
  if (__HAL_UART_GET_FLAG(&huart_, UART_FLAG_RXNE) && rxByteCb_) {
    uint8_t b = static_cast<uint8_t>(huart_.Instance->DR & 0xFF);
    rxByteCb_(b);
    __HAL_UART_CLEAR_FLAG(&huart_, UART_FLAG_RXNE);
  }
}

void Stm32Uart::handleDmaTxIrq() { HAL_DMA_IRQHandler(&htx_); }
void Stm32Uart::handleDmaRxIrq() { HAL_DMA_IRQHandler(&hrx_); }

void Stm32Uart::onHalTxCplt() { if (txDoneCb_) txDoneCb_(); }
void Stm32Uart::onHalRxHalf() { if (rxBlockCb_ && rxBuf_) rxBlockCb_(rxBuf_, rxLen_/2); }
void Stm32Uart::onHalRxFull() { if (rxBlockCb_ && rxBuf_) rxBlockCb_(rxBuf_ + rxLen_/2, rxLen_/2); }
void Stm32Uart::onHalError(uint32_t e) { if (errCb_) errCb_(e); }

Stm32Uart* Stm32Uart::fromInstance(USART_TypeDef* i) {
  for (auto &r : registry_) if (r.inst==i) return r.self; return nullptr;
}
Stm32Uart* Stm32Uart::fromDma(DMA_Stream_TypeDef* s) {
  for (auto &r : registry_) if (r.self && (r.self->txStream_==s || r.self->rxStream_==s)) return r.self; return nullptr;
}
void Stm32Uart::registerInstance(USART_TypeDef* inst, Stm32Uart* self) {
  for (auto &r : registry_) if (r.inst==inst) { r.self = self; return; }
}


