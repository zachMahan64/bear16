#SIMPLE QUADRATIC SOLVER, SAT 20250614
.data
#struct: val {2-byte}, sqrt(val) {2-byte}
sqrt_table:
    .word 0 0
    .word 1 1
    .word 4 2
    .word 9 3
    .word 16 4
    .word 25 5
    .word 36 6
    .word 49 7
    .word 64 8
    .word 81 9
    .word 100 10
    .word 121 11
    .word 144 12
    .word 169 13
    .word 196 14
    .word 225 15
    .word 256 16
    .word 289 17
    .word 324 18
    .word 361 19
    .word 400 20
    .word 441 21
    .word 484 22
    .word 529 23
    .word 576 24
    .word 625 25
    .word 676 26
    .word 729 27
    .word 784 28
    .word 841 29
    .word 900 30
    .word 961 31
    .word 1024 32
    .word 1089 33
    .word 1156 34
    .word 1225 35
    .word 1296 36
    .word 1369 37
    .word 1444 38
    .word 1521 39
    .word 1600 40
    .word 1681 41
    .word 1764 42
    .word 1849 43
    .word 1936 44
    .word 2025 45
    .word 2116 46
    .word 2209 47
    .word 2304 48
    .word 2401 49
    .word 2500 50
    .word 2601 51
    .word 2704 52
    .word 2809 53
    .word 2916 54
    .word 3025 55
    .word 3136 56
    .word 3249 57
    .word 3364 58
    .word 3481 59
    .word 3600 60
    .word 3721 61
    .word 3844 62
    .word 3969 63
    .word 4096 64
    .word 4225 65
    .word 4356 66
    .word 4489 67
    .word 4624 68
    .word 4761 69
    .word 4900 70
    .word 5041 71
    .word 5184 72
    .word 5329 73
    .word 5476 74
    .word 5625 75
    .word 5776 76
    .word 5929 77
    .word 6084 78
    .word 6241 79
    .word 6400 80
    .word 6561 81
    .word 6724 82
    .word 6889 83
    .word 7056 84
    .word 7225 85
    .word 7396 86
    .word 7569 87
    .word 7744 88
    .word 7921 89
    .word 8100 90
    .word 8281 91
    .word 8464 92
    .word 8649 93
    .word 8836 94
    .word 9025 95
    .word 9216 96
    .word 9409 97
    .word 9604 98
    .word 9801 99
    .word 10000 100
    .word 10201 101
    .word 10404 102
    .word 10609 103
    .word 10816 104
    .word 11025 105
    .word 11236 106
    .word 11449 107
    .word 11664 108
    .word 11881 109
    .word 12100 110
    .word 12321 111
    .word 12544 112
    .word 12769 113
    .word 12996 114
    .word 13225 115
    .word 13456 116
    .word 13689 117
    .word 13924 118
    .word 14161 119
    .word 14400 120
    .word 14641 121
    .word 14884 122
    .word 15129 123
    .word 15376 124
    .word 15625 125
    .word 15876 126
    .word 16129 127
    .word 16384 128
    .word 16641 129
    .word 16900 130
    .word 17161 131
    .word 17424 132
    .word 17689 133
    .word 17956 134
    .word 18225 135
    .word 18496 136
    .word 18769 137
    .word 19044 138
    .word 19321 139
    .word 19600 140
    .word 19881 141
    .word 20164 142
    .word 20449 143
    .word 20736 144
    .word 21025 145
    .word 21316 146
    .word 21609 147
    .word 21904 148
    .word 22201 149
    .word 22500 150
    .word 22801 151
    .word 23104 152
    .word 23409 153
    .word 23716 154
    .word 24025 155
    .word 24336 156
    .word 24649 157
    .word 24964 158
    .word 25281 159
    .word 25600 160
    .word 25921 161
    .word 26244 162
    .word 26569 163
    .word 26896 164
    .word 27225 165
    .word 27556 166
    .word 27889 167
    .word 28224 168
    .word 28561 169
    .word 28900 170
    .word 29241 171
    .word 29584 172
    .word 29929 173
    .word 30276 174
    .word 30625 175
    .word 30976 176
    .word 31329 177
    .word 31684 178
    .word 32041 179
    .word 32400 180
    .word 32761 181
    .word 33124 182
    .word 33489 183
    .word 33856 184
    .word 34225 185
    .word 34596 186
    .word 34969 187
    .word 35344 188
    .word 35721 189
    .word 36100 190
    .word 36481 191
    .word 36864 192
    .word 37249 193
    .word 37636 194
    .word 38025 195
    .word 38416 196
    .word 38809 197
    .word 39204 198
    .word 39601 199
    .word 40000 200
    .word 40401 201
    .word 40804 202
    .word 41209 203
    .word 41616 204
    .word 42025 205
    .word 42436 206
    .word 42849 207
    .word 43264 208
    .word 43681 209
    .word 44100 210
    .word 44521 211
    .word 44944 212
    .word 45369 213
    .word 45796 214
    .word 46225 215
    .word 46656 216
    .word 47089 217
    .word 47524 218
    .word 47961 219
    .word 48400 220
    .word 48841 221
    .word 49284 222
    .word 49729 223
    .word 50176 224
    .word 50625 225
    .word 51076 226
    .word 51529 227
    .word 51984 228
    .word 52441 229
    .word 52900 230
    .word 53361 231
    .word 53824 232
    .word 54289 233
    .word 54756 234
    .word 55225 235
    .word 55696 236
    .word 56169 237
    .word 56644 238
    .word 57121 239
    .word 57600 240
    .word 58081 241
    .word 58564 242
    .word 59049 243
    .word 59536 244
    .word 60025 245
    .word 60516 246
    .word 61009 247
    .word 61504 248
    .word 62001 249
    .word 62500 250
    .word 63001 251
    .word 63504 252
    .word 64009 253
    .word 64516 254
    .word 65025 255
sqrt_table_size:
    .word 256
quadratic_params:
    _a:
    .word 1
    _b:
    .word -82
    _c:
    .word 1645
str_a:
    .string "a = "
str_b:
    .string "b = "
str_c:
    .string "c = "
str_ans:
    .string "ans: "
str_and:
    .string " and "
.text
.const STO_LOC = 4096
.const PRNT_STRT_LOC = 0
start:
    call pr_params
    call solve_quad
    call pr_ans
    hlt

pr_ans:
    #not done yet
    sb s6, ' '
    inc s6
    add s6, s6, 5
    lea t4, 26
    romcpy t4, str_ans, 5 #prnt "ans: "
    lw a0, STO_LOC
    call pr_s_num_four_dig
    romcpy s6, str_and, 5 #prnt " and "
    add s6, s6, 5
    add t0, STO_LOC, 2
    lw a0, t0
    call pr_s_num_four_dig
    sb s6, ' '
    ret

pr_params: #print into memory, not the console (we not there yet)
    clr s6 # this will be our print address ptr
    add s6, s6, 4
    lea t4, 0
    romcpy t4, str_a, 4 #prnt "a="
    lwrom a0, _a
    call pr_s_num_four_dig
    sb s6, ' '
    add s6, s6, 4
    lea t4, 9
    romcpy t4, str_b, 4 #prnt "b = "
    lwrom a0, _b
    call pr_s_num_four_dig
    sb s6, ' '
    add s6, s6, 4
    lea t4, 18
    romcpy t4, str_c, 4 #prnt "c = "
    lwrom a0, _c
    call pr_s_num_four_dig
    sb s6, ' '
    ret

pr_s_num_four_dig: #fn(a0, &s6) = fn(val_of_num, print_loc_ptr)
    comp a0, 0
    jcond_neg _neg
fd_main:
    divs t0, a0, 1000
    add t0, t0, 48
    sb s6, t0 # #000
    inc s6
    divs t0, a0, 100
    mods t0, t0, 10
    add t0, t0, 48
    sb s6, t0 # 0#00
    inc s6
    divs t0, a0, 10
    mods t0, t0, 10
    add t0, t0, 48
    sb s6, t0 # 00#0
    inc s6
    mods t0, a0, 10
    add t0, t0, 48
    sb s6, t0 # 000#
    inc s6
    ret
_neg:
    sb s6, '-'
    neg a0, a0
    inc s6
    jmp fd_main



solve_quad:
    lwrom s0, _a
    lwrom s1, _b
    lwrom s2, _c
    call pos_sol
    lea t3, STO_LOC
    sw t3, rv
    call neg_sol
    sw t3, 2, rv
    ret
pos_sol:
    neg t0, s1 # -b
    push t0 # save -b

    mults t1, s1, s1 # b^2
    mults t2, s0, s2 # a*c
    mults t2, t2, 4 # 4ac
    sub a0, t1, t2 # b^2 - 4ac
    lt b_ret a0, 0 # terminate if sqrt is of a neg number

    call sqrt  # sqrt(a0) = sqrt(b^2 - 4ac)
    mov t1, rv #save sqrt(b^2 - 4ac) into t1
    mults t2, s0, 2 # 2a
    pop t0 #get our beloved -b back
    add t0, t0, t1 # -b + sqrt(b^2 - 4ac)

    divs t0, t0, t2 # (-b + sqrt(b^2 - 4ac)) / 2a
    mov rv, t0 #return answer
    ret #"
    b_ret:
        mov rv, -1
        ret

neg_sol:
        neg t0, s1 # -b
        push t0 # save -b

        mults t1, s1, s1 # b^2
        mults t2, s0, s2 # a*c
        mults t2, t2, 4 # 4ac
        sub a0, t1, t2 # b^2 - 4ac
        lt bn_ret a0, 0 # terminate if sqrt is of a neg number

        call sqrt  # sqrt(a0) = sqrt(b^2 - 4ac)
        mov t1, rv #save sqrt(b^2 - 4ac) into t1
        mults t2, s0, 2 # 2a
        pop t0 #get our beloved -b back
        sub t0, t0, t1 # -b - sqrt(b^2 - 4ac)

        divs t0, t0, t2 # (-b - sqrt(b^2 - 4ac)) / 2a
        mov rv, t0 #return answer
        ret #"
        bn_ret:
            mov rv, -1
            ret

sqrt:
    clr t1 # our lil counter/offsetter
    s_loop:
        lwrom t2, sqrt_table, t1 # <load from rom> t2, *sqrt_table, offset
        eq s_hit, a0, t2
        ult overshot, a0, t2
        add t1, t1, 4 # inc by 2 word
        jmp s_loop
    s_hit:
        mov s7, 7 # debug, lucky # 7, this is just a little marker to indicate a hit!
        add t1, t1, 2
        mov s3, t1 # debug
        lwrom rv, sqrt_table, t1 # <load from rom> t2, *sqrt_table, new_offset (*sqrt(val))
        ret
    overshot:
        sub t3, t1, 2
        lwrom rv, sqrt_table, t3 # <load from rom> t2, *sqrt_table, new_offset (*est_down_sqrt(val))
        ret