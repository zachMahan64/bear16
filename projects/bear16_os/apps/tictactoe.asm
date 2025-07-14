# TICTACTOE.ASM

@include "text_processing.asm"
@include "os_core.asm"

@include "apps/tictactoe_assets.asm"

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
    call tictactoe_blit_board
    call util_stall_esc
    call util_clr_fb
    ret
tictactoe_blit_board:
    # blitting intersections
    .const TTT_BOARD_TILE_THICKNESS = 5
    .const TTT_ILINE_DIST = (TTT_BOARD_TILE_THICKNESS + 1)
    .const TTT_BOARD_LINE_LEN = (TTT_BOARD_TILE_THICKNESS * 3 + 2)

    .const TTT_BOARD_LINE_START = 5
    .const TTT_BOARD_IDX_START = 7
    tictactoe_blit_board_hlines:
        add a0, TTT_BOARD_LINE_START, TTT_BOARD_TILE_THICKNESS
        mov a1, TTT_BOARD_IDX_START
        mov a2, TTT_HLINE
        push 0 # init cnt
        tictactoe_blit_board_hlines_loop:
            call blit_ttt_tile
            add a0, a0, TTT_ILINE_DIST
            call blit_ttt_tile
            sub a0, a0, TTT_ILINE_DIST
            inc a1
            # cnt manip
            pop t0
            inc t0
            push t0
            ult tictactoe_blit_board_hlines_loop, t0, TTT_BOARD_LINE_LEN
    pop t0
    push 0
    tictactoe_blit_board_vlines:
        mov a0, TTT_BOARD_LINE_START
        add a1, TTT_BOARD_IDX_START, TTT_BOARD_TILE_THICKNESS
        mov a2, TTT_VLINE
        push 0 # init cnt
        tictactoe_blit_board_vlines_loop:
            call blit_ttt_tile
            add a1, a1, TTT_ILINE_DIST
            call blit_ttt_tile
            sub a1, a1, TTT_ILINE_DIST
            inc a0
            # cnt manip
            pop t0
            inc t0
            push t0
            ult tictactoe_blit_board_vlines_loop, t0, TTT_BOARD_LINE_LEN
    ret
tictactoe_exit:
    call util_clr_fb
    call os_init_taskbar
    call con_clear
    ret

# helpers
blit_ttt_tile:
    #a0 = line, a1 = index, a2 = desired tile, s10 = clobber (TRUE/FALSE)
    mult t0, a0, LINE_SIZE # set line
    add t0, t0, a1 # set index
    add t0, t0, FB_LOC #adjust for FB location start in SRAM

    mov t1, ttt_tset_map_start
    mult t2, a2, 8 # idx * tile width in ROM
    add t1, t1, t2

    clr t2 # cnt
    blit_ttt_tile_loop:
        lbrom t3, t1       #load byte from rom in t3
        eq blit_ttt_tile_clobber_false, s10, FALSE
        blit_ttt_tile_clobber_false_exit:
        sb t0, t3          #store byte in t3 into addr @ t0
        add t0, t0, LINE_WIDTH_B     # t0 += 32
        inc t1             # next byte in rom
        inc t2             # t2++
        ult blit_ttt_tile_loop, t2, 8 # check cnt
    ret
    blit_ttt_tile_clobber_false:
        lb t4, t0
        or t3, t3, t4 # bitwise or rom and ram byte
        jmp blit_ttt_tile_clobber_false_exit
blit_ttt_x:
    # a0 = place (0-8)
    .const BLIT_TTT_X_NUM_LOCAL_VARS = 2
    sub sp, sp, (BLIT_TTT_X_NUM_LOCAL_VARS * 2) # reserve for local vars
    .const BLIT_TTT_X_LINE_OFFS = -2
    .const BLIT_TTT_X_IDX_OFFS = -4
    mod t0, a0, 3
    sb fp, BLIT_TTT_X_IDX_OFFS, t0
    div t0, a0, 3
    sb fp, BLIT_TTT_X_LINE_OFFS, t0
    ret