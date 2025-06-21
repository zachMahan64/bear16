//
// Created by Zachary Mahan on 6/4/25.
//
#ifndef EMU_H
#define EMU_H
#include <string>
namespace emu {
    class Emulator {
        static constexpr std::string version = "0.0.1";
    public:
        int assembleAndRun(const std::string &path = "../projects/asm_test.asm") const;
    };

}

#endif //EMU_H
