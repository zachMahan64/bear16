#SUM OF THE FIRST <N> NATURAL NUMBERS, TUE 20250610
.text
.const N_ = 10
.const RESULT_ADDR = 4096

    mov t0, 1          # counter = 1
    mov t1, 0          # sum = 0

loop:
    add t1, t1, t0     # sum += counter
    add t0, t0, 1      # counter++
    comp t0, N_
    jcond_neg loop     # if counter <= N, repeat loop

    sw RESULT_ADDR, t1

end:
    hlt
