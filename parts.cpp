// Created by Zachary Mahan on 5/18/25.

#include "parts.h"
#include <cstdint>
#include <thread>
#include <chrono>

using namespace parts;

// ─── Clock definitions ────────────────────────────────
void Clock::freeze()  { frozen = true;  }
void Clock::unfreeze(){ frozen = false; }
void Clock::tick() {
    if (frozen) return;
    ++cycles;
    lastBit = bit;
    bit     = 1 - bit;
}
void Clock::tick(auto delayMillis) {
    if (frozen) return;
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMillis));
    lastBit = bit;
    bit = 1 - bit;
}

// (tick<> in header)

// ─── Instruction ctor ─────────────────────────────────
Instruction::Instruction(uint64_t raw) {
    opcode   = (raw >> 48) & 0xFFFFu;
    dest     = (raw >> 32) & 0xFFFFu;
    src1     = (raw >> 16) & 0xFFFFu;
    src2     =  raw        & 0xFFFFu;
    immFlags =  opcode >> 14;
    opCode14 =  opcode & 0x3FFF;
    val = raw;
}
uint64_t Instruction::toRaw() const {
    return (static_cast<uint64_t>(opcode) << 48) |
           (static_cast<uint64_t>(dest)   << 32) |
           (static_cast<uint64_t>(src1)   << 16) |
           static_cast<uint64_t>(src2);
}



// ─── FlagRegister definitions ────────────────────────
void FlagRegister::reset() { carry = zero = negative = overflow = false; }
void FlagRegister::setCarry(const bool carry)    { this->carry    = carry; }
void FlagRegister::setZero(const bool zero)     { this->zero     = zero; }
void FlagRegister::setNegative(const bool neg) { negative = neg; }
void FlagRegister::setOverflow(const bool v) { overflow = v; }

// ─── GenRegister definitions ─────────────────────────
void GenRegister::set(uint16_t v) { val = v; }
void GenRegister::reset()         { val = 0; }
