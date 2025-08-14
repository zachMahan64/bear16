#GENERAL TEST, 20250612
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
.byte 'c' 'c' 'c' 'c'
.byte '\s'
greeting_str:
    .string "hello"
.byte 's'
.text
data_sec_test:
    lea t0, my_val
    lbrom t3, t0
    ret

.const INIT_OFFSET_FROM_STO_LOC = 1
.const STR_LEN = 6 # for "hello" (with \0)
greeting:
    clr t0
    mov t0, INIT_OFFSET_FROM_STO_LOC
    lea s0, greeting_str
    loop:
        inc t0
        lbrom s1, s0
        sb STO_LOC, t0, s1
        inc s0
        lt loop t0, STR_LEN
    ret