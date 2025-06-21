#hi!
# this is a test
test_bite_of_87:
    mov a0, 7
    mov a1, 0
    mov a2, 8
    mov s10, BNUMS_LEFT
    call blit_bnum
    mov a2, 7
    mov s10, BNUMS_RIGHT
    call blit_bnum
    ret