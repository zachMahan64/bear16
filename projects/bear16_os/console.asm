#CONSOLE.ASM (WIP)
# REG CONV: Overload s10 to a3 & s9 to a4, {s0 = index ptr, s1 = line ptr} -> for cursor
@include "os_core.asm"
@include "text_editor_app.asm"    #WIP, for booting up later
@include "console_dispatch.asm"   #WIP, not yet implemented

.data
.const CON_STRT_LINE = 3
.const CON_BUFFER_STRT = 18433
.const CON_BUFFER_SIZE = 64 # ~ thisNum/32 = approx num lines not that safe -> increase later, currently overflow works fine just overwrites
                            #   anything that goes over 64 after the next line is malloc'd
con_name:
    .string "B16->"
.text
console_main:
    #LOCAL REG CONV: s3 = PTR TO CHAR IN CURRENT BUFFER (virtual cursor)
    .const FIRST_BUF_PTR_OFFS = -2     # (1st push)
    .const NUM_BUFFERS_ALLOC_OFFS = -4 # (2nd push)
    call con_init
    push rv # save FIRST_BUF_PTR
    push 0  # init NUM_BUFFERS_ALLOC
    con_main_loop:
        call os_update
        call con_get_line
        lw t0, fp, NUM_BUFFERS_ALLOC_OFFS
        inc t0
        sw fp, NUM_BUFFERS_ALLOC_OFFS, t0
        mov a0, rv # rv -> a0 = ptr to start of line buffer
        call con_process_line #just echoes right now, no command resolution
        jmp con_main_loop
    ret
con_init:
    # rv = ptr to first buffer
    #init starting line
    mov s1, CON_STRT_LINE
    mov a0, 0
    call util_malloc #blank malloc call to get the ptr to first buffer
    # reuse rv from the malloc call
    ret
con_print_cname:
    call check_to_scroll
    mov a0, s1 # line
    mov a1, 0             # index
    mov a2, con_name      # char*
    mov s10, TRUE         # bool updateCursor
    call blit_strl_rom
    ret
con_get_line:
    call check_to_scroll

    # ALLOCS a new BUFFER, save ptr as a local var and initialize s3 (virtual cursor) to it
    mov a0, CON_BUFFER_SIZE # malloc num bytes
    call util_malloc # reserve buffer memory
    mov s3, rv #get the ptr to the buffer from good ol malloc
    .const BUFFER_START_PTR_OFFS = -2
    push rv # push BUFFER_START_PTR_OFFS -> offset = -2

    call con_print_cname # WIP, later print username
    con_get_line_loop:
    call os_update
    call check_to_scroll
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
        #BUFFER WRITE----------------#
            sb s3, '\0' # ensure null termination
            inc s3
        #----------------------------#
            #clear @ current spot
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space to delete cursor
            call blit_cl
            # rv should still point to the start of buffer from the malloc call
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            ret
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
        subfunc_clamp_cursor:
            #clamp to preserve cname and <2 lines
            # a0 = buffer start
            ret
#SCROLLING
.const LINE_REACHED_TO_TRIGGER_SCROLL = 20
#SCROLLING CHECKS
check_to_scroll:
    uge check_to_scroll_do_scroll, s1, LINE_REACHED_TO_TRIGGER_SCROLL #check current cursor line
    ret
    check_to_scroll_do_scroll:
        call con_scroll_once_purely_visual
        dec s1
        ret
check_to_scroll_using_strlen:
    # a0 = char*
    uge check_to_scroll_using_strlen_do_scroll, s1, LINE_REACHED_TO_TRIGGER_SCROLL #check current cursor line
    ret
    check_to_scroll_using_strlen_do_scroll:
        #reuse a0
        call con_scroll_purely_visual_using_strlen
        dec s1
        ret
#DIRECT SCROLLING FUNCTIONS
.const START_FIRST_LINE_IDX = 256
.const NUM_BYTES_FROM_FIRST_LINE_TO_END_OF_21st = 5376 # LINE WIDTH * NUM LINES = 256 * 21
con_scroll_once_purely_visual:
    lea t0, FB_LOC
    memcpy FB_LOC, START_FIRST_LINE_IDX, NUM_BYTES_FROM_FIRST_LINE_TO_END_OF_21st
    ret
con_scroll_purely_visual_using_strlen:
    #a0 = char*
    # reuse a0
    call util_strlen_ram
    mov t1, rv #get length of string!
    div t1, t1, LINE_WIDTH_B     # LINE_WIDTH_B = 32
    mult t1, t1, LINE_SIZE   # LINE_SIZE = 256
    sub t1, NUM_BYTES_FROM_FIRST_LINE_TO_END_OF_21st, t1
    lea t0, FB_LOC
    memcpy FB_LOC, START_FIRST_LINE_IDX, t1
    ret

.data
con_succ_str:
    .string "CONSOLE SUCCESS"
con_err_str:
    .string "CONSOLE ERROR"
.text
con_process_line:
    # a0 = ptr to start of line buffer
    push a0 # save
    #call con_success     # reuse a0 here
    inc s1
    pop a0 # use saved
    call con_echo
    ret
con_success:
    call check_to_scroll
    inc s1 # increment line
    call con_print_cname
    mov a0, s1 # line
    mov a1, s0 # index
    mov a2, con_succ_str
    mov s10, TRUE #update cursor
    call blit_strl_rom #blitting a str
    call check_to_scroll
    ret
con_error:
    ret #WIP
con_echo:
    # a0 = ptr to start of line buffer
    push a0 # save that ptr
    push a0
    # reuse a0
    call check_to_scroll_using_strlen
    call con_print_cname
    mov a0, s1 # line
    mov a1, s0 # index
    pop a2 # retrieve ptr
    mov s10, TRUE #update cursor
    call blit_strl_ram #blitting a str
    ret