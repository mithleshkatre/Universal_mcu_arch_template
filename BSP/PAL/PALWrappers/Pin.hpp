#pragma once
#include "IVendorPolicy.hpp"
#include "IGpio.hpp"
#include <memory>

class Pin {
public:
  Pin(IVendorPolicy& policy, std::unique_ptr<IGpio> igpio, const PinCfg& cfg);
  ~Pin();
  void init();
  void set(bool high);
  bool read();
  void reconfig(const PinCfg& newCfg);
  PinCfg cfg() const;
private:
  IVendorPolicy& policy;
  std::unique_ptr<IGpio> igpio;
  PinCfg cfg_;
};
