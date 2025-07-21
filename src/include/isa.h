// Created by Zachary Mahan on 5/18/25.

#ifndef ISA_H
#define ISA_H

#include <cstdint>
#include <cstddef>

namespace isa {
    static constexpr size_t SRAM_SIZE = 64 * 1024; //bytes
    static constexpr size_t ROM_SIZE = 64 * 1024; //bytes
    static constexpr size_t DISK_SIZE = 256 * 65536;
    static constexpr size_t GEN_REG_COUNT = 27;
    static constexpr size_t IO_COUNT = 2;
    static constexpr size_t MAX_UINT16_T = 65535;
    static constexpr size_t RA_REGISTER_INDEX = 0x000F;
    static constexpr size_t TOP_OF_HEAP_PTR = 6656;
    static constexpr size_t STARTING_HEAP_PTR_VALUE = 16384;
    //RESERVED MEMORY REGIONS FOR MEMORY ALLOC/MANAGEMENT
    static constexpr size_t FREE_LIST_HEAD_LOC = 6558;
    static constexpr size_t DISK_ADDR_LO = 6559;
    static constexpr size_t DISK_ADDR_MID = 6560;
    static constexpr size_t DISK_ADDR_HI = 6561;
    static constexpr size_t DISK_DATA = 6562;
    static constexpr size_t DISK_OP = 6563;
    static constexpr size_t DISK_STATUS = 6564;
    //RESERVED MEMORY FOR I/O
    static constexpr uint16_t MIO_STRT = 6144;
    static constexpr uint16_t KEY_IO_MEM_LOC = MIO_STRT;
    static constexpr uint16_t SHIFT_KEY_IO_MEM_LOC = MIO_STRT + 1;
    static constexpr uint16_t ARROW_KEY_STATE = MIO_STRT + 2;
    static constexpr uint16_t FRAMES_MEM_LOC = MIO_STRT + 3;
    static constexpr uint16_t SECONDS_PTR_MEM_LOC = MIO_STRT + 4;
    static constexpr uint16_t MINUTES_PTR_MEM_LOC = MIO_STRT + 5;
    static constexpr uint16_t HOURS_PTR_MEM_LOC = MIO_STRT + 6;
    static constexpr uint16_t DAYS_PTR_MEM_LOC = MIO_STRT + 7;
    static constexpr uint16_t MONTHS_PTR_MEM_LOC = MIO_STRT + 8;
    static constexpr uint16_t YEARS_PTR_MEM_LOC = MIO_STRT + 9;
    static constexpr uint16_t CONT_FRAMES_MEM_LOC_LO = MIO_STRT + 11;
    static constexpr uint16_t CONT_FRAMES_MEM_LOC_HI = MIO_STRT + 12;



enum class Opcode_E : uint16_t {
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
        LW     = 0x0021,
        LB     = 0x0022,
        COMP   = 0x0023,
        LEA    = 0x0024,
        PUSH   = 0x0025,
        POP    = 0x0026,
        CLR    = 0x0027,
        INC    = 0x0028,
        DEC    = 0x0029,
        MEMCPY = 0x002A, //multicycle, simulate with a simCycDelayCnt that is set and decrements until zero when needed and prevents the PC from stepping
        SW     = 0x002B, //store word (16 bit)
        SB     = 0x002C, //store byte (8 bit)
        LBROM  = 0x002D, //load byte from ROM -> not implemented/ WIP
        LWROM  = 0x002E, //load word from ROM -> not implemented/ WIP
        ROMCPY = 0x002F,
        MULTS  = 0x0030,
        DIVS   = 0x0031,
        MODS   = 0x0032,
        MULT_FPT  = 0x0033,
        DIV_FPT   = 0x0034,
        MOD_FPT   = 0x0035,
        //CTRL Flow
        CALL      = 0x0040,   // call sub-routine  (dest = target PC)
        RET       = 0x0041,   // return from CALL
        JMP       = 0x0042,   // unconditional jump        (dest = target PC)
        JCOND_Z   = 0x0043,   // jump if     Z flag == 1
        JCOND_NZ  = 0x0044,   // jump if     Z flag == 0
        JCOND_NEG = 0x0045,   // jump if     N flag == 1
        JCOND_NNEG= 0x0046,   // jump if     N flag == 0
        JCOND_POS = 0x0047,   // jump if NOT N && Z == 0
        JCOND_NPOS= 0x0048,   // jump if     N flag == 1 || Z == 0
        NOP       = 0x0049,   // no-operation
        HLT       = 0x004A,   // halt
        JAL       = 0x004B,   // Jump and link
        RETL      = 0x004C,   // Ret from link (in ra)
        RETT      = 0x004D,   // Ret from trap
    };
};

#endif //ISA_H