//
// Created by Zachary Mahan on 5/18/25.
//

#ifndef CORE_H
#define CORE_H
#include "isa.h"
#include "parts.h"
#include <SDL2/SDL.h>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <span>
#include <string>
#include <vector>

class CPU16 {
    bool isEnableDebug = false;
    // ISA specs
    const size_t NUM_GEN_REGS = isa::GEN_REG_COUNT;
    const short NUM_IO = isa::IO_COUNT;
    // MEMORY
    std::array<uint8_t, isa::SRAM_SIZE> &sram;
    std::array<uint8_t, isa::ROM_SIZE> &userRom;
    std::array<uint8_t, isa::ROM_SIZE> &kernelRom;
    std::span<uint8_t, isa::ROM_SIZE> activeRom;
    // DISK
    std::span<uint8_t, isa::DISK_SIZE> disk;
    // CTRL FLOW (primitive registers)
    uint16_t tickWaitCnt = 0; // for multicycle operations
    uint16_t tickWaitStopPt = 0;
    bool pcIsFrozenThisCycle = false;
    bool isInMemcpyLoop = false; // rework at some point
    // IO
    std::array<uint16_t, isa::IO_COUNT> inps{};
    std::array<uint16_t, isa::IO_COUNT> outs{};
    // REG
    std::array<parts::GenRegister, isa::GEN_REG_COUNT> genRegs{};
    parts::FlagRegister flagReg = parts::FlagRegister();
    parts::GenRegister stackPtr = parts::GenRegister(
        isa::MAX_UINT16_T); // sp stacks at end of RAM for downward growth
    parts::GenRegister framePtr = parts::GenRegister(isa::MAX_UINT16_T);
    parts::GenRegister trapRetAddr = parts::GenRegister(isa::MAX_UINT16_T);
    [[nodiscard]] uint64_t fetchInstruction() const;
    void execute(parts::Instruction instr);
    void performOp(const parts::Instruction &instr, uint16_t src1Val,
                   uint16_t src2Val);
    void doArith(uint16_t op14, uint16_t src1Val, uint16_t src2Val,
                 uint16_t dest);
    void doCond(uint16_t op14, uint16_t src1Val, uint16_t src2Val,
                uint16_t dest);
    void doDataTrans(parts::Instruction instr, uint16_t src1Val,
                     uint16_t src2Val);
    void doCtrlFlow(parts::Instruction instr, uint16_t src1Val,
                    uint16_t src2Val);
    void writeback(uint16_t dest, uint16_t val);
    [[nodiscard]] uint16_t fetchByteAsWordFromRam(uint16_t addr) const;
    [[nodiscard]] uint8_t fetchByteFromRam(uint16_t addr) const;
    [[nodiscard]] uint16_t
    fetchWordFromRam(uint16_t addr) const; // little endian
    static std::array<uint8_t, 2> convertWordToBytePair(uint16_t val);
    void writeWordToRam(uint16_t addr, uint16_t val); // little endian
    void writeByteToRam(uint16_t addr, uint8_t val);
    [[nodiscard]] uint16_t fetchByteAsWordFromRom(uint16_t addr) const;
    [[nodiscard]] uint8_t fetchByteFromRom(uint16_t addr) const;
    [[nodiscard]] uint16_t fetchWordFromRom(uint16_t addr) const;
    void jumpTo(uint16_t destAddrInRom);

  public:
    // PUBLIC FLAG(S)
    bool isHalted = false;
    // PC
    uint16_t pc = 0;
    // CONSTRUCTOR
    CPU16(std::array<uint8_t, isa::SRAM_SIZE> &sram,
          std::array<uint8_t, isa::ROM_SIZE> &userRom,
          std::array<uint8_t, isa::ROM_SIZE> &kernelRom,
          std::unique_ptr<std::array<uint8_t, isa::DISK_SIZE>> &disk,
          bool enableDebug);
    // EXECUTION
    void step();
    // INTERRUPT COMMUNICATION INTERFACE
    void setActiveRomToUser();
    void setActiveRomToKernel();
    void setTrapReturnAddress(uint16_t trapRetAddr);
    // DIAGNOSTIC
    [[nodiscard]] uint16_t getValInReg(uint16_t reg) const;
    void printSectionOfRam(uint16_t &startingAddr, uint16_t &numBytes,
                           bool asChars) const;
};

// scr helper
constexpr uint32_t makeRGBA8888(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (static_cast<uint32_t>(r) << 24) | (static_cast<uint32_t>(g) << 16) |
           (static_cast<uint32_t>(b) << 8) | static_cast<uint32_t>(a);
}

class Screen {
    static constexpr int WIDTH = 256;
    static constexpr int HEIGHT = 192;
    static constexpr int SCALE = 4;

    static constexpr uint32_t FB_COLOR = makeRGBA8888(0, 255, 0, 255); // green
    static constexpr uint16_t FB_ADDR =
        0x0000; // framebuffer base address in SRAM

    std::array<uint32_t, WIDTH * HEIGHT> framebuffer{};

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window{
        nullptr, SDL_DestroyWindow};
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer{
        nullptr, SDL_DestroyRenderer};
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture{
        nullptr, SDL_DestroyTexture};

  public:
    Screen();
    ~Screen();
    void updateFB();
    void renderSramToFB(const std::array<uint8_t, isa::SRAM_SIZE> &sram,
                        uint16_t fbAddr = FB_ADDR);
};

class InputController {
    static constexpr uint8_t ARROW_UP_MASK = 0x01;
    static constexpr uint8_t ARROW_DOWN_MASK = 0x02;
    static constexpr uint8_t ARROW_LEFT_MASK = 0x04;
    static constexpr uint8_t ARROW_RIGHT_MASK = 0x08;
    std::array<uint8_t, isa::SRAM_SIZE> &sram;

  public:
    explicit InputController(std::array<uint8_t, isa::SRAM_SIZE> &sramRef);
    void handleKeyboardPress(const SDL_Event &e) const;
    void handleKeyboardRelease(const SDL_Event &e) const;
};

class InterruptController {
  public:
    void handleKeyboardInterrupt();
};

class DiskController {
    uint32_t addrPtr = 0;
    std::span<uint8_t, isa::DISK_SIZE> disk;
    std::array<uint8_t, isa::SRAM_SIZE> &sram;
    // disk operations
    static constexpr uint8_t NO_OP = 0x00;
    static constexpr uint8_t READ_BYTE_OP = 0x01;
    static constexpr uint8_t WRITE_BYTE_OP = 0x02;
    static constexpr uint8_t READ_WORD_OP = 0x03;
    static constexpr uint8_t WRITE_WORD_OP = 0x04;
    static constexpr uint8_t RESET_STATUS_OP = 0x05;
    // disk flags
    static constexpr uint8_t OVERFLOW_ERROR = 0x01;
    static constexpr uint8_t UNKNOWN_OP_ERROR = 0x02;
    static constexpr uint8_t READ_DONE = 0x04;
    static constexpr uint8_t WRITE_DONE = 0x08;

  public:
    explicit DiskController(
        std::array<uint8_t, isa::SRAM_SIZE> &sramRef,
        const std::unique_ptr<std::array<uint8_t, isa::DISK_SIZE>> &disk);
    void handleDiskOperation();
};

class Board {
    // flags
    bool power = true;
    bool isEnableDebug = false;
    // diagnostic
    double clockSpeedHz{};
    // memory
    std::array<uint8_t, isa::SRAM_SIZE> sram{};
    std::array<uint8_t, isa::ROM_SIZE> userRom{};
    std::array<uint8_t, isa::ROM_SIZE> kernelRom{};
    // disk
    std::unique_ptr<std::array<uint8_t, isa::DISK_SIZE>> disk =
        std::make_unique<std::array<uint8_t, isa::DISK_SIZE>>();
    // IO (WIP)
    uint64_t input = 0;
    uint64_t output = 0;
    // Core Components
    Screen screen{};
    CPU16 cpu;
    parts::Clock clock;
    InterruptController interruptController{};
    InputController inputController;
    DiskController diskController;
    void setKernelRom(std::vector<uint8_t> &rom);
    void setUserRom(std::vector<uint8_t> &rom);

  public:
    // CONSTRUCTOR
    explicit Board(bool enableDebug);
    // EXECUTING ROM
    int run();
    // ROM LOADING
    void loadUserRomFromBinInTxtFile(const std::string &path);
    void loadUserRomFromHexInTxtFile(const std::string &path);
    void loadUserRomFromByteVector(std::vector<uint8_t> &rom);
    void loadKernelRomFromByteVector(std::vector<uint8_t> &rom);
    // BINARY ROM IO
    void loadRomFromBinFile(const std::string &path);
    void saveRomToBinFile(const std::string &path) const;
    // DISK IO
    void loadDiskFromBinFile(const std::string &path);
    void saveDiskToBinFile(const std::string &path) const;
    // DIAGNOSTICS
    void calcClockSpeedHz(double elapsedMillis);
    void printDiagnostics(bool printMemAsChars) const;
    void printAllRegisterContents() const;
};
// helper
template <typename T>
void writeToFile(const std::string &filename, const T &data) {
    static_assert(
        requires {
            data.data();
            data.size();
        }, "T must have data() & size() methods and be a contiguous data "
           "container");
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing: " +
                                 filename);
    }
    outFile.write(reinterpret_cast<const char *>(data.data()),
                  static_cast<std::streamsize>(data.size()));
    outFile.close();
}
uint64_t currentTimeMillis();

#endif // CORE_H
