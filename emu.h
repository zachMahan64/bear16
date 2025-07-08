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
        [[nodiscard]] int assembleAndRun(const std::string &projectPath, const std::string &entry) const;
        void binFileIntoDisk(const std::string& binPath);
    };

}

#endif //EMU_H
