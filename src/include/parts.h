//
// Created by Zachary Mahan on 5/18/25.
//

#ifndef PARTS_H
#define PARTS_H
#include "isa.h"
#include <array>
#include <cstdint>

namespace parts {
class Clock {
    // reference to board memory
    std::array<uint8_t, isa::SRAM_SIZE>& sram;
    // cum cycles
    uint64_t cycles = 0;
    // values for mem-mapped time
    uint8_t frames = 0;
    uint8_t seconds = 0;
    uint8_t minutes = 0;
    uint8_t hours = 0;
    uint8_t days = 0;
    uint8_t months = 0;
    uint16_t years = 0;
    uint16_t continuous_frames = 0;
    // map for days in months
    static constexpr std::array<uint8_t, 12> daysInMonths = {31, 28, 31, 30, 31, 30,
                                                             31, 31, 30, 31, 30, 31};

  public:
    void initMemMappedTime();
    void incMemMappedTime();
    [[nodiscard]] uint64_t getCycles() const { return cycles; }
    void resetCycles() { cycles = 0; }
    bool frozen = false;
    explicit Clock(std::array<uint8_t, isa::SRAM_SIZE>& sram);
    void freeze();
    void unfreeze();
    void tick();
    void tick(int delayMillis);
};
class Instruction {
  public:
    uint16_t opcode;
    uint16_t src1;
    uint16_t src2;
    uint16_t dest;
    uint8_t immFlags;  // top 2 bits of Opcode
    uint16_t opCode14; // bottom 2 bits of Opcode
    uint64_t val;      // 64 bit val

    explicit Instruction(uint64_t raw);
    Instruction() = default;
    [[nodiscard]] uint64_t toRaw() const;
};
class FlagRegister {
  public:
    bool carry{};
    bool zero{};
    bool negative{};
    bool overflow{};

    void setCarry(bool carry);
    void setZero(bool zero);
    void setNegative(bool neg);
    void setOverflow(bool over);
    void reset();
    FlagRegister() = default;
};
class GenRegister {
  public:
    uint16_t val = 0;
    void set(uint16_t val);
    void operator-=(uint16_t decAmount);
    void operator+=(uint16_t incAmount);
    void reset();
    explicit GenRegister(int startingVal) { val = startingVal; }
    GenRegister() = default;
};
} // namespace parts
#endif // PARTS_H
