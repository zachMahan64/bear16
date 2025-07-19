#ARRAY COUNTER, THU 20250612
.data
my_arr:
    .byte 2, 2, 75, 3, 1, 4, 2, 8
my_arr_size:
    .byte 8
my_num:
    .byte 2

.text
.const STO_LOC = 4096
    lbrom s7, my_num
    call check_arr
    sw STO_LOC, rv
    hlt
check_arr:
    lbrom s1, my_arr_size
    loop:
        uge _ret, s0, s1
        call check_entry
        add s6, s6, rv
        clr rv
        add s0, s0, 1
        jmp loop
    _ret:
        mov rv, s6
        ret

check_entry:
    lbrom t0, my_arr, s0
    eq is_equal, s7, t0
    ret
    is_equal:
        mov rv, 1
        ret
