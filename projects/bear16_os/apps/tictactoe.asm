# TICTACTOE.ASM

@include "text_processing.asm"
@include "os_core.asm"

.data
tictactoe_title_name_str:
    .string "TIC-TAC-TOE"
tictactoe_title_edition_str:
    .string "Ultimate Edition"

.text
tictactoe_start:
    call tictactoe_init
    call tictactoe_enter_title_page
    call util_stall_esc
    call tictactoe_exit
    ret

tictactoe_init:
    call util_clr_fb
    ret
tictactoe_enter_title_page:
    mov a0, 3
    mov a1, 10
    mov a2, tictactoe_title_name_str
    call blit_strl_rom
    mov a0, 4
    mov a1, 8
    mov a2, tictactoe_title_edition_str
    call blit_strl_rom
    ret
tictactoe_exit:
    call util_clr_fb
    call os_init_taskbar
    call con_clear
    ret