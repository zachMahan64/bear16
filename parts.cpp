// Created by Zachary Mahan on 5/18/25.

#include "parts.h"
#include <cstdint>
#include <thread>
#include <chrono>

using namespace parts;

void Clock::incMemMappedTime() {
    ++frames;
    sram[isa::FRAMES_MEM_LOC] = frames;
    if (frames == 60) {
        ++seconds;
        frames = 0;
        sram[isa::FRAMES_MEM_LOC] = frames;
        sram[isa::SECONDS_PTR_MEM_LOC] = seconds;
        if (seconds == 60) {
            ++minutes;
            seconds = 0;
            sram[isa::SECONDS_PTR_MEM_LOC] = seconds;
            sram[isa::MINUTES_PTR_MEM_LOC] = minutes;
            if (minutes == 60) {
                ++hours;
                minutes = 0;
                sram[isa::MINUTES_PTR_MEM_LOC] = minutes;
                sram[isa::HOURS_PTR_MEM_LOC] = hours;
                if (hours == 24) {
                    ++days;
                    hours = 0;
                    sram[isa::HOURS_PTR_MEM_LOC] = hours;
                    sram[isa::DAYS_PTR_MEM_LOC] = days;
                }
            }
        }
    }
}

Clock::Clock(std::array<uint8_t, isa::SRAM_SIZE> &sram) : sram(sram){}

//clock
void Clock::freeze()  { frozen = true;  }
void Clock::unfreeze(){ frozen = false; }
void Clock::tick() {
    if (frozen) return;
    ++cycles;
}
void Clock::tick(int delayMillis) {
    if (frozen) return;
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMillis));
    ++cycles;
}

//instr
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



//flagRegister definitions
void FlagRegister::reset() { carry = zero = negative = overflow = false; }
void FlagRegister::setCarry(const bool carry)    { this->carry    = carry; }
void FlagRegister::setZero(const bool zero)     { this->zero     = zero; }
void FlagRegister::setNegative(const bool neg) { negative = neg; }
void FlagRegister::setOverflow(const bool v) { overflow = v; }

//genRegister definitions
void GenRegister::set(uint16_t v) { val = v; }

void GenRegister::operator-=(uint16_t decAmount) {
    val -= decAmount;
}

void GenRegister::operator+=(uint16_t incAmount) {
    val += incAmount;
}

void GenRegister::reset()         { val = 0; }
