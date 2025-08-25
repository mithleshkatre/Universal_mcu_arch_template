
#include "Pin.hpp"
#include "UartWrapper.hpp"
#include "DmaWrapper.hpp"
#include "IVendorPolicy.hpp"
#include "HardwareConfig.hpp"
#include <vector>
#include <memory>
#include <array>


#include "Stm32F4Policy.hpp"
#include "Stm32IO.hpp"
#include "Stm32Uart.hpp"
#include "Stm32Dma.hpp"


static std::vector<std::unique_ptr<Pin>> g_pins;
static std::vector<std::unique_ptr<UartWrapper>> g_uarts;
static std::vector<std::unique_ptr<DmaWrapper>> g_dmas;
static std::array<UartWrapper*, 8> g_uarts_by_inst{};

Stm32Uart* g_uart2 = nullptr;

void Hardware_InitAll(void) {

  static stm32f4_policy::Stm32F4Policy policy;
  for (auto &p : kBoardPins) {
    auto igpio = std::make_unique<Stm32IO>();
    auto pin = std::make_unique<Pin>(policy, std::move(igpio), p);
    pin->init();
    g_pins.push_back(std::move(pin));
  }
  for (auto &u : kBoardUarts) {
    auto impl = std::make_unique<Stm32Uart>(u.inst==UartInst::Uart2?USART2:USART1,
                                            DMA1_Stream6, DMA_CHANNEL_4, DMA1_Stream5, DMA_CHANNEL_4);
    g_uart2 = impl.get();
    auto wrap = std::make_unique<UartWrapper>(policy, std::move(impl), u);
    
    wrap->init();
    g_uarts_by_inst[(int)u.inst] = wrap.get();
    g_uarts.push_back(std::move(wrap));
  }
  auto dma_impl = std::make_unique<Stm32Dma>(DMA1_Stream6);
  g_dmas.push_back(std::make_unique<DmaWrapper>(std::move(dma_impl)));

}

UartWrapper* Hardware_GetUartWrapper(UartInst inst) {
  auto i = static_cast<int>(inst);
  return (i >= 0 && i < (int)g_uarts_by_inst.size()) ? g_uarts_by_inst[i] : nullptr;
}

Pin* Hardware_GetPinById(PinID id) {
  for (auto &p : g_pins) if (p && p->cfg().id == id) return p.get();
  return nullptr;
}

DmaWrapper* Hardware_GetDmaForUart(UartInst) {
  if (g_dmas.empty()) return nullptr;
  return g_dmas[0].get();
}
