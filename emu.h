//
// Created by Zachary Mahan on 6/4/25.
//
#ifndef EMU_H
#define EMU_H
#include <string>
namespace emu {
    constexpr std::string version = "0.0.1";
    class Emulator {
    public:
        int run();
    };

}

#endif //EMU_H
