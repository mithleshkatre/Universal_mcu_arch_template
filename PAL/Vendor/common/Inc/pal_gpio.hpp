#ifndef PAL_GPIO_HPP
#define PAL_GPIO_HPP

#pragma once
#include <cstdint>

namespace pal {

class Gpio {
public:
    enum class Mode { Input, Output, Alternate, Analog };
    enum class Pull { NoPull, Up, Down };
    enum class Speed : uint32_t {
        Low        = 0x00000000U,
        Medium     = 0x00000001U,
        High       = 0x00000002U,
        VeryHigh   = 0x00000003U
    };

    Gpio(uint8_t port, uint8_t pin);

    bool config(Mode mode, Pull pull, Speed speed, uint8_t af = 0);
    void write(bool value);
    bool read() const;

private:
    uint8_t portIndex;
    uint8_t pinNumber;
};

} // namespace pal

#endif
