#WIP OS
# REG CONV: Overload s10 to a3 & s9 to a4, {s0 = index ptr, s1 = line ptr} -> for cursor
@include "text_processing.asm"


.data
month_str_array:
    .string "JAN"
    .string "FEB"
    .string "MAR"
    .string "APR"
    .string "MAY"
    .string "JUN"
    .string "JUL"
    .string "AUG"
    .string "SEP"
    .string "OCT"
    .string "NOV"
    .string "DEC"
welcome_msg:
    .string "Welcome to the Bear16 Console! \n    VERSION 0.0.1, 20250619"

.text
#WELCOME
print_welcome_msg:
    mov a0, 1 # line
    mov a1, 0 # index
    mov a2, welcome_msg
    mov s10, TRUE #update cursor
    call blit_strl_rom #blitting a str
    ret

#UTILS
.const TRUE = 1
.const FALSE = 0
util_inf_loop:
    jmp util_inf_loop
    ret

.text
# -> MEMORY MANAGEMENT
.const TOP_OF_HEAP_PTR = 6656
.const STARTING_HEAP_PTR_VALUE = 16384
util_malloc:
    # a0 = num bytes
    lw t0, TOP_OF_HEAP_PTR
    mov rv, t0 # this will be the base of the allocated memory -> return this!
    add t1, t0, a0 # end loop val / new top of heap value
    util_malloc_loop:
    sb t0, 0 # write zero/clear memory
    inc t0 # inc store location
    ult util_malloc_loop, t0, t1
    lea t2, TOP_OF_HEAP_PTR
    sw t2, t1 # store new top of heap value
    ret

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


#OS CORE METHODS
#CLOCK MEM_LOC CONSTANTS (ALL SUBJ TO CHANGE)
.const FRAMES_MEM_LOC = 6147
.const SECONDS_PTR_MEM_LOC = 6148
.const MINUTES_PTR_MEM_LOC = 6149
.const HOURS_PTR_MEM_LOC = 6150
.const DAYS_PTR_MEM_LOC = 6151
.const MONTHS_PTR_MEM_LOC = 6152
.const YEARS_PTR_MEM_LOC = 6153
    init_os:
        call subr_init_heap
        call subr_init_os_draw_bottom_line # perhaps inline
        call subr_init_day_month_year      # initialize dates (static/non updated until restart) -> note: may cause inaccuracies
        ret
        subr_init_heap:
            lea t0, TOP_OF_HEAP_PTR
            sw t0, STARTING_HEAP_PTR_VALUE
            ret
        subr_init_os_draw_bottom_line:
            clr s2 # cnt & index
            subr_init_os_draw_bottom_line_loop:
                mov a0, 22
                mov a1, s2
                mov a2, '_'
                call blit_cl
                inc s2
                ult subr_init_os_draw_bottom_line_loop, s2, 32
            ret
        subr_init_day_month_year:
            #DAYS
            mov a0, 23 # line
            mov a1, 21 # index
            lb a2, DAYS_PTR_MEM_LOC
            call blit_2dig_pint
            #MON
            mov a0, 23 # line
            mov a1, 17 # index
            lb a2, MONTHS_PTR_MEM_LOC
            mult a2, a2, MONTH_STR_ARRAY_ENTRY_SIZE
            add a2, a2, month_str_array
            call blit_strl_rom
            #YEAR
            mov a0, 23 # line
            mov a1, 11 # index
            lw a2, YEARS_PTR_MEM_LOC
            call blit_4dig_pint
            ret
    os_update:
        call os_update_time_display
        ret

    os_update_time_display:
        # SECONDS
        mov a0, 23 # line
        mov a1, 30 # index
        lb a2, SECONDS_PTR_MEM_LOC
        call blit_2dig_pint
        mov a0, 23 # line
        mov a1, 29 # index
        mov a2, ':'
        call blit_cl
        #MINUTES
        mov a0, 23 # line
        mov a1, 27 # index
        lb a2, MINUTES_PTR_MEM_LOC
        call blit_2dig_pint
        mov a0, 23 # line
        mov a1, 26 # index
        mov a2, ':'
        call blit_cl
        #HOURS
        mov a0, 23 # line
        mov a1, 24 # index
        lb a2, HOURS_PTR_MEM_LOC
        call blit_2dig_pint
        mov a0, 23 # line
        mov a1, 23 # index
        mov a2, ','
        call blit_cl
        # DAYS, MONTHS, YEARS generated in init
        ret