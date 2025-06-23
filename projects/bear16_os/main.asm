#WIP OS
# REG CONV: Overload s10 to a3 & s9 to a4, {s0 = index ptr, s1 = line ptr} -> for cursor
@include "text_processing.asm"
@include "os_core.asm"
@include "console.asm"
@include "text_editor_app.asm"


.text
start:
    call init_os
    call print_welcome_msg
    call console_main
    call util_stall
    #go into straight text editor (WIP)
    #mov a0, s1 # move into s1 line ptr into line arg
    #call text_editor_main
    hlt
