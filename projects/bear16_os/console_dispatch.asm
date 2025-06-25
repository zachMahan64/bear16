#COMMAND DISPATCH TABLE
@include "os_core.asm"
@include "console.asm"
.data
cmd_table:
    # char*, [label to call]
    .word cmdt_echo, con_echo
    .word cmdt_test, con_test
    #add more
    .word NULL, con_error # throw an error if we read the table terminator
cmd_table_strings:
    cmdt_echo:
        .string "echo"
    cmdt_test:
        .string "test"
.text
# DISPATCHING FUNCTIONS
console_dispatch_main:
    push a0 #save char* to orignal buffer
    call cd_isolate_cmd # reuse a0
    lea t0, cmd_table
    ret
.const CMD_MAX_SIZE = 9 # including '/0'
cd_isolate_cmd:
    # a0 = char* to orig buffer
    # ~ rv = char* to cmd
    mov t0, a0 # save char* to orig buffer to t0
    mov a0, CMD_MAX_SIZE # allocate
    call util_malloc
    mov t2, rv #ptr to start of cmd
    clr t3 # cnt, use as offset!
    cd_isolate_cmd_loop:
        lb t1, t0, t3 # load char into t1 w/ offset of t3
        sw t2, t3, t1 # save char in t1 into addr in t2 w/ offset of t3
        inc t3
        eq cd_isolate_cmd_hit_null, t1, '\0' # break if we hit a null terminator
        ne cd_isolate_cmd_loop, t1, ' ' #loop until we hit a space/' '
    #reuse rv (ptr to start of cmd)
    sw t2, t3, 0   # null terminate
    cd_isolate_cmd_hit_null:
    ret
cd_isolate_args:
