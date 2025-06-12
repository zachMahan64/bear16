#PROPER RECURSIVE FIBONACCI CALCULATOR, WORKING MON 20250609
.text
.const FIB_N = 8
.const STO_LOC = 4096
start:
    mov a0, FIB_N
    call fibonacci
    sw STO_LOC, rv
    call data_sec_test
    call greeting
    hlt

fibonacci:
    # a0 = n
    le base_case, a0, 2   # if a0 <= 1 goto base_case

    push a0            # save n

    sub a0, a0, 1         # a0 = n - 1
    call fibonacci        # fib(n - 1)
    pop t0 # n -> t0
    push rv            # save result onto stack

    sub a0, t0, 2         # a0 = n - 2
    call fibonacci        # fib(n - 2)
    push rv           # save result onto stack
    pop t1 # get fib(n - 2) from stack
    pop t2 # get fib(n - 2) from stack
    add rv, t1, t2        # rv = fib(n-1) + fib(n-2)
    ret

base_case:
    mov rv, 1
    ret

.data
my_val:
.byte 'a'
.octbyte 'b' 'b' 'b' 'b' 'b' 'b' 'b' 'b'
.qword 'c' 'c' 'c' 'c'
.word '\s'
greeting_str:
    .string "hello"

.text
data_sec_test:
    lea t0, my_val
    lbrom t3, t0
    ret

.const INIT_OFFSET_FROM_STO_LOC = 2
.const STR_LEN = 6 # for "hello" (with \0)
greeting:
    clr t0
    mov t1, INIT_OFFSET_FROM_STO_LOC
    loop:
        add t0, t0, 1
        add t1, t1, 1
        lbrom s1, greeting_str #not working rn
        sb STO_LOC, t1, s1
        ge loop t0, STR_LEN
    ret