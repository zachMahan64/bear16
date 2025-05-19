// Created by Zachary Mahan on 5/18/25.

#ifndef ISA_H
#define ISA_H

#include <cstdint>
#include <cstddef>

namespace isa {
    static constexpr size_t SRAM_SIZE = 64 * 1024;
    static constexpr size_t ROM_SIZE = 16 * 1024;
    static constexpr size_t GEN_REG_COUNT = 16;
    static constexpr size_t IO_COUNT = 2;
    static constexpr size_t MAX_UINT_16BIT = 65536;

enum class Op : uint16_t {
        //Arith & Bitwise
        ADD  = 0x0000,
        SUB  = 0x0001,
        MULT  = 0x0002,
        DIV  = 0x0003,
        MOD = 0x0004,
        AND = 0x0005,
        OR  = 0x0006,
        XOR = 0x0007,
        NOT = 0x0008,
        NAND = 0x0009,
        NOR = 0x000A,
        NEG = 0x000B,
        LSH = 0x000C,
        RSH = 0x000D,
        ROL = 0x000E,
        ROR = 0x000F,
        //Cond
        EQ  = 0x0010,
        NE  = 0x0011,
        LT  = 0x0012,
        LE  = 0x0013,
        GT  = 0x0014,
        GE  = 0x0015,
        ULT = 0x0016,
        ULE = 0x0017,
        UGT = 0x0018,
        UGE = 0x0019,
        Z   = 0x001A, //sets flag
        NZ  = 0x001B, //"
        C   = 0x001C, //"
        NC  = 0x001D, //"
        N   = 0x001E, //"
        NN  = 0x001F, //"
        //Data Trans (wip)
        MOV    = 0x0020,
        LD     = 0x0021,
        STO    = 0x0022, //add mult variants (half word, full word, double word) w/ offset support
        LEA    = 0x0023,
        PUSH   = 0x0024,
        POP    = 0x0025,
        CLR    = 0x0026,
        INC    = 0x0027,
        DEC    = 0x0028,
        MEMCPY = 0x0029, //multicycle, simulate with a simCycDelayCnt that is set and decrements until zero when needed and prevents the PC from stepping
        COMP   = 0x0030,
        //CTRL Flow
        CALL      = 0x0040,   // call sub-routine  (src1 = target PC)
        RET       = 0x0041,   // return from CALL
        JMP       = 0x0042,   // unconditional jump        (src1 = target PC)
        JCOND_Z   = 0x0043,   // jump if     Z flag == 1
        JCOND_NZ  = 0x0044,   // jump if     Z flag == 0
        JCOND_NEG = 0x0045,   // jump if     N flag == 1
        JCOND_NNEG= 0x0046,   // jump if     N flag == 0
        JCOND_POS = 0x0047,   // jump if NOT N && Z == 0
        JCOND_NPOS= 0x0048,   // jump if     N flag == 1 || Z == 0
        NOP       = 0x0049,   // no-operation
        HLT       = 0x004A,    // halt
        //Video
        CLRFB  = 0x0080,  // clear framebuffer
        SETPX  = 0x0081,  // set pixel at (X,Y), dest = 0/1
        BLIT   = 0x0082   // block image transfer: srcAddr, byteCount, destAddr
    };
};

#endif //ISA_H