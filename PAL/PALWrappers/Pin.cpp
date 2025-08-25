#include "Pin.hpp"

Pin::Pin(IVendorPolicy& policy, std::unique_ptr<IGpio> igpio, const PinCfg& cfg)
: policy(policy), igpio(std::move(igpio)), cfg_(cfg) {}

Pin::~Pin() = default;

void Pin::init() {
  if (cfg_.mode == PinMode::Alt && !policy.isValidAlt(cfg_.id, cfg_.alt)) {
    // throw std::runtime_error("Invalid alternate function for the pin");
  }
  policy.enableGpioClock(cfg_.id.port);
  // policy.applyPin(cfg_);
  igpio->init(cfg_);
}

void Pin::set(bool high){ 
  igpio->set(high); 
}

bool Pin::read(){ 
  return igpio->read(); 
}

void Pin::reconfig(const PinCfg& newCfg){ 
  cfg_ = newCfg; 
  init(); 
}

PinCfg Pin::cfg() const{ 
  return cfg_; 
}
