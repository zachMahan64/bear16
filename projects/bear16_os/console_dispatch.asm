#COMMAND DISPATCH TABLE
@include "os_core.asm"
@include "console.asm"
.data
cmd_table:
    # char*, [label to call]
    .word cmdt_echo, con_echo
    .word cmdt_test, con_test
    .word cmdt_help, con_success
    #add more
    .word NULL, con_error # throw an error if we read the table terminator
cmd_table_strings:
    cmdt_echo:
        .string "echo"
    cmdt_test:
        .string "test"
    cmdt_help:
        .string "help"
.text
# DISPATCHING FUNCTIONS
console_dispatch_main: # currently just echos
    # a0 = ptr to start of line buffer
    # reuse a0
    call cd_isolate_cmd
    mov a0, rv
    call con_echo
    mov a0, rv
    call util_free

    lea t0, cmd_table #ptr inside table
    console_dispatch_main_loop: # WIP
    ret
.const CMD_MAX_SIZE = 17 # including '/0'
.const CMD_MAX_SIZE_WO_NULL_TERM = 15
cd_isolate_cmd:
    # a0 = char* to orig buffer
    # ~ rv = char* to cmd
    push a0 # save char* to orig buffer to t0
    mov a0, CMD_MAX_SIZE # allocate
    call util_malloc
    pop t0 #get char* to orig buffer back
    clr t1 # init curr char to zero
    mov t2, rv #ptr to start of cmd from malloc
    clr t3 # cnt, use as offset!
    cd_isolate_cmd_loop:
        lb t1, t0, t3 # load char into t1 w/ offset of t3
        inc s7 # TODO
        eq cd_isolate_cmd_ret, t1, '\0' # break if we hit a null terminator
        uge cd_isolate_cmd_ret, t3, CMD_MAX_SIZE_WO_NULL_TERM
        sb t2, t3, t1 # save char in t1 into addr in t2 w/ offset of t3
        inc t3
        ne cd_isolate_cmd_loop, t1, ' ' #loop until we hit a space/' '
    cd_isolate_cmd_ret:
    sb t2, t3, 0   # null terminate
    #reuse rv (ptr to start of cmd from malloc)
    ret

cd_isolate_args:
    #WIP
    ret
