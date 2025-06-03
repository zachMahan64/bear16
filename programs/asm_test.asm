.const FIB_END = 20
.const RESULT_LOC = 4096
start:
    mov a0, FIB_END #pass desired fib num to calculate
    call fibonacci
end:
    lea t0, RESULT_LOC
    sw t0, s2 # storing result little-endian @ RESULT_LOC
    hlt

fibonacci:
    eq f_ret, s0, a0
    gt main_loop, s0, 2
    call set_1
main_loop:
    mov t0, s2
    add s2, s1, s2
    mov s1, t0
    add s0, s0, 1
    call fibonacci
f_ret:
    mov t2, 0
    ret
set_1:
    mov s2, 1
    ret

