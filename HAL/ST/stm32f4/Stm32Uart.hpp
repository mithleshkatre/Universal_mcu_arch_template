#pragma once

#include "stm32f4xx_hal.h"
#include <atomic>
#include <cstring>
#include "ISerial.hpp"

class Stm32Uart : public ISerial {
public:
  struct Reg { USART_TypeDef* inst; Stm32Uart* self; };

  explicit Stm32Uart(USART_TypeDef* inst,
                     DMA_Stream_TypeDef* txStream=nullptr, uint32_t txChannel=0,
                     DMA_Stream_TypeDef* rxStream=nullptr, uint32_t rxChannel=0);

  void init(const UartConfig& cfg) override;

  // Blocking
  void transmitBlocking(const uint8_t* data, size_t len) override;
  void receiveBlocking(uint8_t* data, size_t len) override;

  // IRQ
  void transmitIT(const uint8_t* data, size_t len) override;
  void receiveIT(uint8_t* data, size_t len) override;

  // DMA
  void transmitDMA(const uint8_t* data, size_t len) override;
  void receiveDMA(uint8_t* data, size_t len) override;

  // Callbacks
  void onRxByte(RxByteCb cb) override;
  void onRxBlock(RxBlockCb cb) override;
  void onTxDone(TxDoneCb cb) override;
  void onError(ErrorCb cb) override;

  // IRQ forwarders
  void handleUartIrq();
  void handleDmaTxIrq();
  void handleDmaRxIrq();

  // HAL event hooks called from C callbacks
  void onHalTxCplt();
  void onHalRxHalf();
  void onHalRxFull();
  void onHalError(uint32_t e);

  static Stm32Uart* fromInstance(USART_TypeDef* i);
  static Stm32Uart* fromDma(DMA_Stream_TypeDef* s);

private:
  static void registerInstance(USART_TypeDef* inst, Stm32Uart* self);

  UART_HandleTypeDef huart_{};
  DMA_HandleTypeDef  htx_{};
  DMA_HandleTypeDef  hrx_{};
  DMA_Stream_TypeDef* txStream_{nullptr};
  DMA_Stream_TypeDef* rxStream_{nullptr};
  uint32_t txChannel_{0};
  uint32_t rxChannel_{0};

  uint8_t* rxBuf_{nullptr};
  size_t rxLen_{0};

  UartConfig cfg_{};
  UartIrqConfig irqCfg_{};
  UartDmaConfig dmaCfg_{};

  RxByteCb rxByteCb_{};
  RxBlockCb rxBlockCb_{};
  TxDoneCb txDoneCb_{};
  ErrorCb errCb_{};
};

