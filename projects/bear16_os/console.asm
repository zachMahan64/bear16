#CONSOLE (WIP)
# REG CONV: Overload s10 to a3 & s9 to a4, {s0 = index ptr, s1 = line ptr} -> for cursor
@include "text_processing.asm"
@include "os_core.asm"
@include "text_editor_app.asm" #for booting up later

@include "console_maps.asm"

.data
.const CON_STRT_LINE = 3
.const CON_BUFFER_STRT = 18433
.const CON_BUFFER_END = 18944
.const CON_BUFFER_SIZE = 512
con_name:
    .string "B16->"
.text
console_main:
    #s3 = BUFFER PTR
    call con_init
    console_main_loop:
        call os_update
        call con_get_line
        #do somethin
        jmp console_main_loop
    ret
con_init:
    #INIT BUFFER
    mov s3, CON_BUFFER_STRT

    mov a0, s3
    mov a1, CON_BUFFER_SIZE
    call util_malloc # reserve buffer memory
    call con_print_cname
    ret
con_print_cname:
    mov a0, CON_STRT_LINE # line
    mov a1, 0             # index
    mov a2, con_name      # char*
    mov s10, TRUE         # bool updateCursor
    call blit_strl_rom
ret
con_get_line:
    con_get_line_loop:
    call os_update
    mov a0, s1  # line ptr
    mov a1, s0  # index ptr
    mov a2, '_' # underscore for our cursor
    call blit_cl
    lb s2, IO_LOC            # save inp from IO
    eq subr_con_backspace, s2, 8 # for backspace
    eq subr_con_newline, s2, 13  # for newline
    eq subr_con_tab, s2, 9       # for tab
    # ~~~~~~~~~~~~~~~~~~~~~~ # branch divide
    ugt subr_con_print_new_char, s2, 0
    jmp con_get_line_loop
    ret
    subr_con_print_new_char:
        # s0 = current char* to print
        mov a0, s1
        mov a1, s0     # a1 = index
        lb a2, IO_LOC # a2 = char
        lb t6, SHIFT_LOC
        ugt subr_con_shift, t6, 0 # if shift = true
        subr_con_shift_exit:
        #BUFFER WRITE----------------#
        sb s3, a2 # store char
        inc s3
        #----------------------------#
        call blit_cl # a0, a1, a2 used
        inc s0
        lea t0, IO_LOC # ->
        sb t0, 0       # clear IO memory location
        uge subr_con_go_on_newline, s0, LINE_WIDTH_B
        jmp con_get_line_loop
        subr_con_go_on_newline:
            inc s1 # next line!
            clr s0 # set index on line back to zero
            jmp con_get_line_loop
        subr_con_shift:
            ult ssubr_con_nonalpha_shift, a2, 97
            ugt ssubr_con_nonalpha_shift, a2, 122
            sub a2, a2, 32
            jmp subr_con_shift_exit
            ssubr_con_nonalpha_shift:
                    lea t7, nonalpha_shift_map
                ssubr_con_nonalpha_shift_loop:
                    lbrom t8, t7
                    mov s6, t7 # debug
                    eq ssubr_con_nonalpha_shift_hit, a2, t8
                    add t7, t7, 2
                    jmp ssubr_con_nonalpha_shift_loop
                    #error if t7 > size of nonalpha shift map
                    jmp subr_con_shift_exit
                ssubr_con_nonalpha_shift_hit:
                    lbrom a2, t7, 1
                    jmp subr_con_shift_exit
        subr_con_backspace:
            #BUFFER WRITE----------------#
            sb s3, '\0' # pop char
            dec s3
            #----------------------------#
            #clear @ current spot
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space for blank
            call blit_cl
            dec s0
            lt ssubr_con_backline, s0, 0
            ssubr_con_backline_exit:
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            jmp con_get_line_loop
                ssubr_con_backline:
                    dec s1               # go back a line
                    mov s0, 31 # set index ptr to end of last line
                    jmp ssubr_con_backline_exit
        subr_con_newline:
            #BUFFER WRITE----------------# --> make this call something like "process_line"
            sb s3, '\n' # store char
            inc s3
            #----------------------------#
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space for blank
            call blit_cl
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            inc s1
            clr s0
            jmp con_get_line_loop
        subr_con_tab:
            #BUFFER WRITE----------------#
            sb s3, '\t' # store char
            inc s3
            #----------------------------#
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space for blank
            call blit_cl
            add s0, s0, 3 # move forward 2 indices for tab + 1 for going to next char
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            jmp con_get_line_loop