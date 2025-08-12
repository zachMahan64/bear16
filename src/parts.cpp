// Created by Zachary Mahan on 5/18/25.

#include "parts.h"
#include <chrono>
#include <cstdint>
#include <thread>

using namespace parts;

// clock
void Clock::initMemMappedTime() {
    using namespace std::chrono;

    // get system clock time (local time)
    const std::time_t now = std::time(nullptr);
    const std::tm* localTime = std::localtime(&now);

    // Set clock internal-state
    seconds = static_cast<uint8_t>(localTime->tm_sec);
    minutes = static_cast<uint8_t>(localTime->tm_min);
    hours = static_cast<uint8_t>(localTime->tm_hour);
    days = static_cast<uint8_t>(localTime->tm_mday);
    months = static_cast<uint8_t>(localTime->tm_mon);         // tm_mon: 0 = Jan
    years = static_cast<uint16_t>(localTime->tm_year + 1900); // tm_year: years since 1900

    // set mem-mapped values
    sram[isa::SECONDS_PTR_MEM_LOC] = seconds;
    sram[isa::MINUTES_PTR_MEM_LOC] = minutes;
    sram[isa::HOURS_PTR_MEM_LOC] = hours;
    sram[isa::DAYS_PTR_MEM_LOC] = days;
    sram[isa::MONTHS_PTR_MEM_LOC] = months;

    // for 16-bit year, store as little-endian
    sram[isa::YEARS_PTR_MEM_LOC] = static_cast<uint8_t>(years & 0xFF);
    sram[isa::YEARS_PTR_MEM_LOC + 1] = static_cast<uint8_t>((years >> 8) & 0xFF);
}
void Clock::incMemMappedTime() {
    ++frames;
    sram[isa::FRAMES_MEM_LOC] = frames;
    ++continuous_frames;
    sram[isa::CONT_FRAMES_MEM_LOC_HI] = (continuous_frames >> 8);
    sram[isa::CONT_FRAMES_MEM_LOC_LO] = continuous_frames;

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
                    if (daysInMonths[months] == days) {
                        ++months;
                        days = 1;
                        sram[isa::DAYS_PTR_MEM_LOC] = days;
                        sram[isa::MONTHS_PTR_MEM_LOC] = months;
                        if (months == 12) {
                            ++years;
                            months = 0;
                            sram[isa::MONTHS_PTR_MEM_LOC] = months;
                            sram[isa::YEARS_PTR_MEM_LOC] = years;
                        }
                    }
                }
            }
        }
    }
}
Clock::Clock(std::array<uint8_t, isa::SRAM_SIZE>& sram) : sram(sram) {}
void Clock::freeze() { frozen = true; }
void Clock::unfreeze() { frozen = false; }
void Clock::tick() {
    if (frozen) {
        return;
    }
    ++cycles;
}
void Clock::tick(int delayMillis) {
    if (frozen) {
        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMillis));
    ++cycles;
}

// instr
Instruction::Instruction(uint64_t raw) {
    opcode = (raw >> 48) & 0xFFFFU;
    dest = (raw >> 32) & 0xFFFFU;
    src1 = (raw >> 16) & 0xFFFFU;
    src2 = raw & 0xFFFFU;
    immFlags = opcode >> 14;
    opCode14 = opcode & 0x3FFF;
    val = raw;
}
uint64_t Instruction::toRaw() const {
    return (static_cast<uint64_t>(opcode) << 48) | (static_cast<uint64_t>(dest) << 32) |
           (static_cast<uint64_t>(src1) << 16) | static_cast<uint64_t>(src2);
}

// flagRegister definitions
void FlagRegister::reset() { carry = zero = negative = overflow = false; }
void FlagRegister::setCarry(const bool carry) { this->carry = carry; }
void FlagRegister::setZero(const bool zero) { this->zero = zero; }
void FlagRegister::setNegative(const bool neg) { negative = neg; }
void FlagRegister::setOverflow(const bool v) { overflow = v; }

// genRegister definitions
void GenRegister::set(uint16_t value) { val = value; }

void GenRegister::operator-=(uint16_t decAmount) { val -= decAmount; }

void GenRegister::operator+=(uint16_t incAmount) { val += incAmount; }

void GenRegister::reset() { val = 0; }
