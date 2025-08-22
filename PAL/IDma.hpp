#pragma once
#include <cstddef>
#include <cstdint>

class IDma {
public:
  virtual ~IDma() = default;
  virtual void startTx(const uint8_t* data, size_t len) = 0;
  virtual void startRx(uint8_t* data, size_t len) = 0;
  virtual bool isTxBusy() = 0;
  virtual bool isRxBusy() = 0;
};
