.const FIB_N = 5
.const STO_LOC = 4096
start:
    mov t0, FIB_N
    call fibonacci
    sw STO_LOC, rv
    hlt
fibonacci:
    le base_case, a0, 1 # go to base case if fib num <= 1

    mov t0, a0 # save OG arg into t0

    sub a0, a0, 1 # a0 = a0 - 1
    call fibonacci # fib(n-1)
    mov t1, rv # save result into t1

    sub a0, a0, 2 # a0 = a0 - 2
    call fibonacci # fib(n-2)
    mov t2, rv # save result into t2

    add rv, t1, t2
    ret

base_case:
    mov rv, 1
    ret
