#PROPER RECURSIVE FIBONACCI CALCULATOR, WORKING MON 20250609
.text
.const FIB_N = 1
.const STO_LOC = 4096
start:
    mov a0, FIB_N
    call fibonacci
    sw STO_LOC, rv
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
    .string "sybau"
