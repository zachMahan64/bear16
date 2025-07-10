# TICTACTOE.ASM

@include "text_processing.asm"
@include "os_core.asm"

.data
tictactoe_title_name_str:
    .string "TIC-TAC-TOE"
tictactoe_title_edition_str:
    .string "Ultimate Edition"
tictactoe_title_play_prompt_str:
    .string "ESC to exit"
tictactoe_title_esc_prompt_str:
    .string "ENTER to play"

ttt_tset_start:
ttt_tset_0:
.byte  0b00011000 0b00000000 0b00011000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte  0b00011000 0b00000000 0b00011000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte  0b00011000 0b00000000 0b00011000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte  0b00011000 0b11111111 0b11111111 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte  0b00011000 0b11111111 0b11111111 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte  0b00011000 0b00000000 0b00011000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte  0b00011000 0b00000000 0b00011000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte  0b00011000 0b00000000 0b00011000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
ttt_tset_end:

ttt_x_map_start:
.byte  0b11000000 0b00000011
.byte  0b11100000 0b00000111
.byte  0b01110000 0b00001110
.byte  0b00111000 0b00011100
.byte  0b00011100 0b00111000
.byte  0b00001110 0b01110000
.byte  0b00000111 0b11100000
.byte  0b00000011 0b11000000
ttt_x_map_end:

ttt_o_map_start:
ttt_o_map_0:
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
ttt_o_map_1:
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
ttt_o_map_2:
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
.byte  0b00000000 0b00000000 0b00000000
ttt_o_map_end:

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
    mov a0, 3
    mov a1, 10
    mov a2, tictactoe_title_name_str
    call blit_strl_rom
    mov a0, 4
    mov a1, 8
    mov a2, tictactoe_title_edition_str
    call blit_strl_rom
    tictactoe_enter_title_page_loop:
        lb t0, IO_LOC
        eq tictactoe_goto_ret, t0, K_ESC
        eq tictactoe_play, t0, K_ENTER
        lea t1, IO_LOC
        sb t1, 0 # clear
        jmp tictactoe_enter_title_page_loop
    tictactoe_goto_ret:
    ret
tictactoe_play:
    ret
tictactoe_exit:
    call util_clr_fb
    call os_init_taskbar
    call con_clear
    ret