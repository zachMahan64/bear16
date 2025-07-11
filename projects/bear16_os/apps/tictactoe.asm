# TICTACTOE.ASM

@include "text_processing.asm"
@include "os_core.asm"

.data
tictactoe_title_name_str:
    .string "  TIC-TAC-TOE"
tictactoe_title_edition_str:
    .string "Ultimate Edition"
tictactoe_title_play_prompt_str:
    .string " ENTER to play"
tictactoe_title_esc_prompt_str:
    .string "  ESC to exit"

.text
tictactoe_start:
    call tictactoe_init
    call tictactoe_enter_title_page
    call tictactoe_exit
    ret

tictactoe_init:
    call util_clr_fb
    ret
tictactoe_enter_title_page:
    # a0 = line idx, a1 = idx on line
    mov a0, 3
    mov a1, 8
    mov a2, tictactoe_title_name_str
    call blit_strl_rom
    mov a0, 4
    mov a1, 8
    mov a2, tictactoe_title_edition_str
    call blit_strl_rom
    mov a0, 7
    mov a1, 8
    mov a2, tictactoe_title_play_prompt_str
    call blit_strl_rom
    mov a0, 8
    mov a1, 8
    mov a2, tictactoe_title_esc_prompt_str
    call blit_strl_rom
    tictactoe_enter_title_page_loop:
        lb t0, IO_LOC
        eq tictactoe_goto_ret, t0, K_ESC
        eq tictactoe_goto_play, t0, K_ENTER
        jmp tictactoe_enter_title_page_loop
    tictactoe_goto_play:
        lea t1, IO_LOC
        sb t1, 0 # clear
        call tictactoe_play
        jmp tictactoe_enter_title_page # re-enter title page w/o call routine (non-recursive)
    tictactoe_goto_ret:
        lea t1, IO_LOC
        sb t1, 0 # clear
    ret
tictactoe_play:
    call util_clr_fb
    call util_stall_esc
    ret
tictactoe_exit:
    call util_clr_fb
    call os_init_taskbar
    call con_clear
    ret