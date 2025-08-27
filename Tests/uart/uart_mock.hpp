#pragma once
#include "IVendorPolicy.hpp"
#include "IGpio.hpp"
#include "ISerial.hpp"
#include <cstdio>
#include <cstring>
#include <thread>
#include <chrono>
#include <atomic>
#include <algorithm>

struct MockPolicy : public IVendorPolicy {
  void enableGpioClock(Port) override {}
  bool isValidAlt(PinID, uint8_t) override { return true; }
  void applyPin(const PinCfg& cfg) override {
    std::printf("[MOCK] apply pin P%c%u mode=%d alt=%u\n", 'A'+(int)cfg.id.port, cfg.id.idx, (int)cfg.mode, cfg.alt);
  }
  bool supportsUart(UartInst u) override { return u!=UartInst::None; }
  bool isValidUartAF(UartInst, PinID, uint8_t) override { return true; }
  void enableUartClock(UartInst) override {}
  void enableUartIrq(UartInst) override {}
  void enableDmaForUart(UartInst) override {}
};

struct MockPin : public IGpio {
  PinCfg cfg{};
  void init(const PinCfg& c) override { cfg=c; }
  void set(bool high) override { std::printf("[MOCK] GPIO set P%c%u=%d\n", 'A'+(int)cfg.id.port, cfg.id.idx, (int)high); }
  bool read() override { return false; }
};


struct MockUart : public ISerial {
  UartConfig cfg{};
  UartIrqConfig irq{};
  UartDmaConfig dma{};
  std::atomic<bool> runRx{false};
  std::thread rxThread;
  RxByteCb rxByteCb{};
  RxBlockCb rxBlkCb{};
  TxDoneCb txCb{};
  ErrorCb errCb{};
  uint8_t* rxBuf{nullptr};
  size_t rxLen{0};

  void init(const UartConfig& c) override { cfg=c; std::printf("[MOCK] UART init baud=%u mode=%d\n", (unsigned)c.baud, (int)c.mode); }

  void transmitBlocking(const uint8_t* data, size_t len) override { fwrite(data,1,len,stdout); fflush(stdout); }
  void receiveBlocking(uint8_t* data, size_t len) override {
    const char* demo = "mock-rx\n";
    size_t n = std::min(len, strlen(demo));
    memcpy(data, demo, n);
  }

  void transmitIT(const uint8_t* data, size_t len) override {
    if (!irq.tx_irq) return;
    std::thread([data,len,this](){ std::this_thread::sleep_for(std::chrono::milliseconds(5)); fwrite(data,1,len,stdout); fflush(stdout); if (txCb) txCb(); }).detach();
  }
  void receiveIT(uint8_t* data, size_t len) override {
    if (!irq.rx_irq) return;
    runRx = true;
    rxThread = std::thread([this](){
      while (runRx) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); if (rxByteCb) rxByteCb('x'); }
    });
  }

  void transmitDMA(const uint8_t* data, size_t len) override {
    if (!dma.tx_dma) return;
    std::thread([data,len,this](){ std::this_thread::sleep_for(std::chrono::milliseconds(5)); fwrite(data,1,len,stdout); fflush(stdout); if (txCb) txCb(); }).detach();
  }
  void receiveDMA(uint8_t* data, size_t len) override {
    if (!dma.rx_dma) return;
    rxBuf = data; rxLen = len;
    runRx = true;
    rxThread = std::thread([this](){
      while (runRx) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (rxBlkCb && rxBuf && rxLen>=2) rxBlkCb(rxBuf, rxLen/2);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (rxBlkCb && rxBuf && rxLen>=2) rxBlkCb(rxBuf + rxLen/2, rxLen/2);
      }
    });
  }

  void onRxByte(RxByteCb cb) override { rxByteCb = std::move(cb); }
  void onRxBlock(RxBlockCb cb) override { rxBlkCb = std::move(cb); }
  void onTxDone(TxDoneCb cb) override { txCb = std::move(cb); }
  void onError(ErrorCb cb) override { errCb = std::move(cb); }

  void stop(){ runRx = false; if (rxThread.joinable()) rxThread.join(); }
};
