//
// Created by Zachary Mahan on 5/18/25.
//

#ifndef PARTS_H
#define PARTS_H
#include <cstdint>
#include <thread>

namespace parts {
    class Clock {
        uint64_t cycles = 0;
        uint16_t bit = 0;
        uint16_t lastBit = 0;
        bool frozen = false;
    public:
        Clock() = default;
        void freeze();
        void unfreeze();
        void tick();
        void tick(auto delayMillis);
    };
    class Instruction {
    public:
        uint16_t opcode;
        uint16_t src1;
        uint16_t src2;
        uint16_t dest;
        uint8_t  immFlags; //top 2 bits of Opcode
        uint16_t opCode14; //bottom 2 bits of Opcode

        explicit Instruction(uint64_t raw);
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
        void setOverflow(bool v);
        void reset();
        FlagRegister() = default;
    };
    class GenRegister {
    public:
        uint16_t val = 0;
        void set(uint16_t val);
        void reset();
    };
}
#endif //PARTS_H