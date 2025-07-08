# TEXT_EDITOR_APP.ASM

# REWORK this to have a dynamic cursor, and write purely to buffer & do page blits

@include "text_processing.asm"
@include "os_core.asm"
text_editor_main:
# buffer assumes this memory is reserved for now, in future malloc this
# actually, adjust this cuz bottom two lines reserved for OS display
.const TEXT_PAGE_SIZE = 768 # abs. max, also inexact
.const NUM_PAGES      = 8   # inexact

    # a0 = starting line
    #mov s1, a0 # s1 = line ptr
    #clr s0     # s0 = index ptr
    call text_editor_init
    text_editor_loop:
    call os_update

    mov a0, s1  # line ptr
    mov a1, s0  # index ptr
    mov a2, '_' # underscore for our cursor
    call blit_cl
    lb s2, IO_LOC            # save inp from IO
    eq subr_backspace, s2, 8 # for backspace
    eq subr_newline, s2, 13  # for newline
    eq subr_tab, s2, 9       # for tab
    eq subr_esc, s2, 27      # for escape
    # ~~~~~~~~~~~~~~~~~~~~~~ # branch divide
    ugt subr_print_new_char, s2, 0
    jmp text_editor_loop
    ret
    subr_print_new_char:
        # s0 = current char* to print
        mov a0, s1
        mov a1, s0     # a1 = index
        lb a2, IO_LOC # a2 = char
        lb t6, SHIFT_LOC
        ugt subr_shift, t6, 0 # if shift = true
        subr_shift_exit:
        call blit_cl # a0, a1, a2 used
        inc s0
        lea t0, IO_LOC # ->
        sb t0, 0       # clear IO memory location
        uge subr_go_on_newline, s0, LINE_WIDTH_B
        jmp text_editor_loop
        subr_go_on_newline:
            inc s1 # next line!
            clr s0 # set index on line back to zero
            jmp text_editor_loop
        subr_shift:
            ult ssubr_nonalpha_shift, a2, 97
            ugt ssubr_nonalpha_shift, a2, 122
            sub a2, a2, 32
            jmp subr_shift_exit
            ssubr_nonalpha_shift:
                    lea t7, nonalpha_shift_map
                ssubr_nonalpha_shift_loop:
                    lbrom t8, t7
                    mov s6, t7 # debug
                    eq ssubr_nonalpha_shift_hit, a2, t8
                    add t7, t7, 2
                    jmp ssubr_nonalpha_shift_loop
                    #error if t7 > size of nonalpha shift map
                    jmp subr_shift_exit
                ssubr_nonalpha_shift_hit:
                    lbrom a2, t7, 1
                    jmp subr_shift_exit
        subr_backspace:
            #clear @ current spot
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space for blank
            call blit_cl
            dec s0
            lt ssubr_backline, s0, 0
            ssubr_backline_exit:
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            jmp text_editor_loop
                ssubr_backline:
                    dec s1               # go back a line
                    mov s0, 31 # set index ptr to end of last line
                    jmp ssubr_backline_exit
        subr_newline:
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space for blank
            call blit_cl
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            inc s1
            clr s0
            jmp text_editor_loop
        subr_tab:
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space for blank
            call blit_cl
            add s0, s0, 3 # move forward 2 indices for tab + 1 for going to next char
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            jmp text_editor_loop
        subr_esc:
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            # back to console
            call con_clear
            ret

text_editor_init:
    mov a0, 22 # all but bottom two lines
    call util_clr_fb_by_line_idx # clear entire thing besides OS heads-up display at bottom
    mov s1, 0
    mov s0, 0
    ret