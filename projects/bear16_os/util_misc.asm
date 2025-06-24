# UTIL_MISC.ASM
@include "text_processing.asm"
@include "util_mem_manager.asm"
.text
#CTRL FLOW UTILITIES
util_stall:
    jmp util_stall
    ret
# FRAMEBUFFER FUNCTIONS
util_clr_fb:
    mov t0, FB_LOC # cnt/ptr
    util_clr_fb_loop:
        sb t0, 0
        inc t0
        lt util_clr_fb_loop, t0, FB_SIZE
    ret
# STRING FUNCTIONS
util_strcomp_ram_rom:
    # a0 = char* in ram
    # a1 = char* in rom
    # -> rv = TRUE/FALSE
    clr t2 # cnt
    util_strcomp_ram_rom_loop:
        lb t0, a0, t2 # load *char w/ cnt as offset
        lbrom t1, a1, t2 # load *char w/ cnt as offset
        ne util_strcomp_ram_rom_ne, t0, t1
        util_strcomp_ram_rom_char_eq:
           ne util_strcomp_ram_rom_char_eq_exit, t0, '\0'
           ne util_strcomp_ram_rom_char_eq_exit, t1, '\0'
           mov rv, TRUE
           ret
        util_strcomp_ram_rom_char_eq_exit:
        inc t2
        jmp util_strcomp_ram_rom_loop
    util_strcomp_ram_rom_ne:
        mov rv, FALSE
        ret
util_strlen_ram:
    # a0 = char*
    # ~> rv = string length (not including '\0')
    clr t0 # counter
    util_strlen_ram_loop:
        lb t1, a0, t0 # load *[currentChar + t0/counter]
        eq util_strlen_ram_hit_null_term, t1, '\0'
        inc t0
        jmp util_strlen_ram_loop
    util_strlen_ram_hit_null_term:
        mov rv, t0
        ret
