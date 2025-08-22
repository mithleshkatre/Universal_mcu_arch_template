#pragma once
#include "IDma.hpp"
#include <memory>

class DmaWrapper {
public:
  DmaWrapper(std::unique_ptr<IDma> impl);
  ~DmaWrapper();
  void startTx(const uint8_t* d, size_t l);
  void startRx(uint8_t* d, size_t l);
  bool txBusy();
  bool rxBusy();
private:
  std::unique_ptr<IDma> impl;
};
