.const balls = 2
.const doub_balls = 4
start:
    mov t0 balls
    call func
    mov t3 balls
    hlt
func:
    mov t1 doub_balls
#    call funcy
    ret
#funcy:
#   mov t2 doub_balls
#   ret