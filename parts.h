//
// Created by Zachary Mahan on 5/18/25.
//

#ifndef PARTS_H
#define PARTS_H
#include <cstdint>
#include <iostream>
#include <thread>

namespace parts {
    class Clock {
        uint64_t cycles = 0;
    public:
        [[nodiscard]] uint64_t getCycles() const {
            return cycles;
        }
        void resetCycles() {
            cycles = 0;
        }
        bool frozen = false;
        Clock() = default;
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
        uint8_t  immFlags; //top 2 bits of Opcode
        uint16_t opCode14; //bottom 2 bits of Opcode
        uint64_t val; //64 bit val

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
        void setOverflow(bool v);
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
        explicit GenRegister(int startingVal) {
            val = startingVal;
        }
        GenRegister() = default;
    };
}
#endif //PARTS_H