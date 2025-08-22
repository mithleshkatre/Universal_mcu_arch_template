#pragma once
#include <cstdint>
#include <cstddef>
#include <functional>

enum class UartMode { Polling, IRQ, DMA };
struct UartIrqConfig { bool rx_irq{false}; bool tx_irq{false}; };
struct UartDmaConfig { bool tx_dma{false}; bool rx_dma{false}; };
struct UartConfig { uint32_t baud{115200}; UartMode mode{UartMode::Polling}; UartIrqConfig irq{}; UartDmaConfig dma{}; };

class ISerial {
public:
  using RxByteCb = std::function<void(uint8_t)>;
  using RxBlockCb = std::function<void(const uint8_t*, size_t)>;
  using TxDoneCb = std::function<void()>;
  using ErrorCb  = std::function<void(uint32_t)>;
  virtual ~ISerial() = default;

  virtual void init(const UartConfig& cfg) = 0;

  virtual void transmitBlocking(const uint8_t* data, size_t len) = 0;
  virtual void receiveBlocking(uint8_t* data, size_t len) = 0;

  virtual void transmitIT(const uint8_t* data, size_t len) = 0;
  virtual void receiveIT(uint8_t* data, size_t len) = 0;

  virtual void transmitDMA(const uint8_t* data, size_t len) = 0;
  virtual void receiveDMA(uint8_t* data, size_t len) = 0;

  virtual void onRxByte(RxByteCb cb) = 0;
  virtual void onRxBlock(RxBlockCb cb) = 0;
  virtual void onTxDone(TxDoneCb cb) = 0;
  virtual void onError(ErrorCb cb) = 0;
};
