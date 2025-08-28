#include "gtest/gtest.h"
#include "gmock/gmock.h"


#include "uart_mock.hpp"
#include "Pin.hpp"
#include "UartWrapper.hpp"

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>


using namespace std::chrono_literals;

static MockPolicy policy;

class UartTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup one UART and one Pin
        UartCfg cfg{ UartInst::Uart2,
        { {Port::A,2}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7 },
        { {Port::A,3}, PinMode::Alt, PinPull::Up, PinSpeed::Med, 7 },
        115200, StopBits::One, Parity::None, true, true
        };

        auto impl = std::make_unique<MockUart>();
        uart = std::make_unique<UartWrapper>(policy, std::move(impl), cfg);
        uart->init();


        PinCfg ledCfg{ {Port::A,5}, PinMode::Output, PinPull::None, PinSpeed::Low, 0 };
        auto igpio = std::make_unique<MockPin>();
        led = std::make_unique<Pin>(policy, std::move(igpio), ledCfg);
        led->init();
    }

    void TearDown() override {
        // if (uart) {
            // auto impl = dynamic_cast<MockUart*>(uart->impl.get());
            // if (impl) impl->stop();
        // }
    }

    std::unique_ptr<UartWrapper> uart;
    std::unique_ptr<Pin> led;
};

TEST_F(UartTest, PinSetAndRead) {
    ASSERT_TRUE(led != nullptr);
    led->set(true);   // should print
    led->set(false);  // should print
    SUCCEED(); // if no crash, test passes
}

TEST_F(UartTest, ReceiveByteITCallback) {
    ASSERT_TRUE(uart != nullptr);

    uint8_t lastByte = 0;
    uart->setRxByteCb([&](uint8_t b){ lastByte = b; });

    uint8_t rx;
    uart->recvIT(&rx, 1);

    std::this_thread::sleep_for(120ms); // allow MockUart thread to generate 'x'

    EXPECT_EQ(lastByte, 'x');

}

TEST_F(UartTest, TransmitDMAInvokesTxDone) {
    ASSERT_TRUE(uart != nullptr);

    bool txDoneCalled = false;
    uart->setTxDoneCb([&](){ txDoneCalled = true; });

    const char msg[] = "Hello DMA\n";
    uart->sendDma(reinterpret_cast<const uint8_t*>(msg), sizeof(msg));

    std::this_thread::sleep_for(20ms);

    EXPECT_TRUE(txDoneCalled);
}

TEST_F(UartTest, ReceiveBlockDMA) {
    ASSERT_TRUE(uart != nullptr);

    bool blockReceived = false;
    uart->setRxBlockCb([&](const uint8_t* data, size_t len) {
        blockReceived = true;
    });

    uint8_t rxBuf[8];
    uart->recvDma(rxBuf, sizeof(rxBuf));

    std::this_thread::sleep_for(120ms); // allow background thread to invoke callback

    EXPECT_TRUE(blockReceived);
}
