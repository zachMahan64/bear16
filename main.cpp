#include <iostream>
#include <cstdint>
#include <map>
#include <thread>
#include <vector>
#include "isa.h"
#include "parts.h"
#include "core.h"

using namespace std;



int runEmu() {
    constexpr std::size_t ROM_SIZE  = isa::ROM_SIZE;
    constexpr std::size_t SRAM_SIZE = isa::SRAM_SIZE;

    Board board(ROM_SIZE, SRAM_SIZE);

    while (true) {

    }
    return 0;
}

int main() {
    return runEmu();
}