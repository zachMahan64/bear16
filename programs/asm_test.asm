#PROPER RECURSIVE FIBONACCI CALCULATOR, WORKING MON 20250609

.data
name_age_arr:
#struct (6): name (5), age(1) -> manually padding strings here
    .string "Bob"
    .byte 0, 42 #manual str padding
    .string "Ben"
    .byte 0, 27 #manual str padding
    .string "Karl"
    .byte 64
    .string "Mike"
    .byte 35
name_age_arr_end:
    .byte 0
desired_name:
    .string "Karl"

.text
.const STRUCT_SIZE = 0x6
.const STO_LOC = 4096
start:
    call cycle_thru_names
    sw STO_LOC, rv
    hlt

cycle_thru_names:
    mov s0, name_age_arr #struct to read, pass ptr to it

    c_loop:
        mov a0, s0
        call check_name

        eq found_name, rv, 1                 # found, go handle

        add s0, s0, STRUCT_SIZE              # increment before next loop
        ugt not_found, s0, name_age_arr_end  # ran past end
        jmp c_loop                             # continue looping
    not_found:
        mov rv, -1 #error/not found
        ret
    found_name:
        mov t0, STRUCT_SIZE
        sub t0, t0, 1
        lbrom rv, s0, t0
        ret

check_name: # check_name(ptr_to_str_start) <- passed by a0
    mov t0, a0 #pass argument into reg t0
    mov s7, desired_name #staring ptr to desired name
    clr t1 #counter/str offset for current char
    clr t2 #store current char
    clr t3 #store current desired char
    n_loop:
        lbrom t2, t0, t1
        lbrom t3, s7, t1
        inc t1
        eq found_ret, t2, 0
        ne bad_ret, t2, t3
        eq n_loop t2, t3
    found_ret:
        mov rv, 1
        ret
    bad_ret:
        mov rv, 0
        ret



