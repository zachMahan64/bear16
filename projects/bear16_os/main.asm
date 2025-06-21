#FULL WORKING ASCII extremely basic text processor (no input handling yet, though) TUE 20250617
# REG CONV: Overload s10 to a3 & s9 to a4, {s0 = index ptr, s1 = line ptr} -> for cursor,

@include "test.asm"

.data
ctset_start: #Char Tile Set (ASCII)
ctset_0:
.byte 0b00000000 0b00010000 0b01010000 0b00101000 0b00010000 0b11000010 0b01100000 0b00010000 0b00000110 0b11000000 0b01010100 0b00000000 0b00000000 0b00000000 0b00000000 0b00000010 0b00111000 0b00010000 0b00111000 0b00111000 0b01000100 0b01111100 0b01111100 0b01111100 0b00111000 0b00111000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00111000
.byte 0b00000000 0b00010000 0b01010000 0b00101000 0b01111100 0b11000100 0b10000010 0b00010000 0b00001000 0b00100000 0b00111000 0b00010000 0b00000000 0b00000000 0b00000000 0b00000100 0b01000100 0b00110000 0b01000100 0b01000100 0b01000100 0b01000000 0b10000000 0b00000100 0b01000100 0b01000100 0b01100000 0b01100000 0b00000110 0b00000000 0b11000000 0b01000100
.byte 0b00000000 0b00010000 0b00000000 0b11111110 0b10010000 0b00001000 0b10001100 0b00000000 0b00001000 0b00100000 0b01111100 0b00010000 0b00000000 0b00000000 0b00000000 0b00001000 0b01000100 0b00010000 0b01000100 0b00000100 0b01000100 0b01000000 0b10000000 0b00001000 0b01000100 0b01000100 0b01100000 0b01100000 0b00011000 0b01111100 0b00110000 0b00000100
.byte 0b00000000 0b00010000 0b00000000 0b00101000 0b01111100 0b00010000 0b01111000 0b00000000 0b00001000 0b00100000 0b00111000 0b01111100 0b00000000 0b00111000 0b00000000 0b00010000 0b01000100 0b00010000 0b00011000 0b00011000 0b01111110 0b01111000 0b11111000 0b00010000 0b00111000 0b00111100 0b00000000 0b00000000 0b01100000 0b00000000 0b00001100 0b00011000
.byte 0b00000000 0b00010000 0b00000000 0b11111110 0b00010010 0b00100000 0b10001100 0b00000000 0b00001000 0b00100000 0b01010100 0b00010000 0b01000000 0b00000000 0b00000000 0b00100000 0b01000100 0b00010000 0b00100000 0b00000100 0b00000100 0b00000100 0b10000100 0b00010000 0b01000100 0b00000100 0b01100000 0b01100000 0b00011000 0b01111100 0b00110000 0b00010000
.byte 0b00000000 0b00000000 0b00000000 0b00101000 0b01111100 0b01000110 0b10001010 0b00000000 0b00001000 0b00100000 0b00000000 0b00010000 0b01000000 0b00000000 0b01100000 0b01000000 0b01000100 0b00010000 0b01000000 0b01000100 0b00000100 0b00000100 0b10000100 0b00010000 0b01000100 0b00000100 0b01100000 0b01100000 0b00000110 0b00000000 0b11000000 0b00000000
.byte 0b00000000 0b00010000 0b00000000 0b00101000 0b00010000 0b10000110 0b01110000 0b00000000 0b00000110 0b11000000 0b00000000 0b00000000 0b10000000 0b00000000 0b01100000 0b10000000 0b00111000 0b00111000 0b01111100 0b00111000 0b00000100 0b01111000 0b01111000 0b00010000 0b00111000 0b00011000 0b00000000 0b00100000 0b00000000 0b00000000 0b00000000 0b00010000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
ctset_1:
.byte 0b01111100 0b00010000 0b10111100 0b01111100 0b10111100 0b11111110 0b11111110 0b01111110 0b10000010 0b11111110 0b01111110 0b10000010 0b01000000 0b10000010 0b10000010 0b01111100 0b10111100 0b01111100 0b10111100 0b01111100 0b11111110 0b10000010 0b10000010 0b10000010 0b10000010 0b10000010 0b11111110 0b00001110 0b10000000 0b11100000 0b00010000 0b00000000
.byte 0b00000010 0b00101000 0b11000010 0b10000010 0b11000010 0b10000000 0b10000000 0b10000000 0b10000010 0b00010000 0b00001000 0b10000010 0b01000000 0b11000110 0b11000010 0b10000010 0b11000010 0b10000010 0b11000010 0b10000010 0b00010000 0b10000010 0b10000010 0b10000010 0b01000100 0b10000010 0b00000100 0b00001000 0b01000000 0b00100000 0b00101000 0b00000000
.byte 0b01110010 0b01000100 0b10000010 0b10000000 0b10000010 0b10000000 0b10000000 0b10000000 0b10000010 0b00010000 0b00001000 0b10000100 0b01000000 0b10101010 0b10100010 0b10000010 0b10000010 0b10000010 0b10000010 0b10000000 0b00010000 0b10000010 0b10000010 0b10000010 0b00101000 0b01000100 0b00001000 0b00001000 0b00100000 0b00100000 0b01000100 0b00000000
.byte 0b10001010 0b10000010 0b11111100 0b10000000 0b10000010 0b11111110 0b11111110 0b10001110 0b11111110 0b00010000 0b00001000 0b11111100 0b01000000 0b10010010 0b10010010 0b10000010 0b11000010 0b10000010 0b11000010 0b01111100 0b00010000 0b10000010 0b10000010 0b10010010 0b00010000 0b00101000 0b00010000 0b00001000 0b00010000 0b00100000 0b00000000 0b00000000
.byte 0b10001010 0b11111110 0b10000010 0b10000000 0b10000010 0b10000000 0b10000000 0b10000010 0b10000010 0b00010000 0b00001000 0b10000010 0b01000000 0b10000010 0b10001010 0b10000010 0b10111100 0b10001010 0b10111100 0b00000010 0b00010000 0b10000010 0b01000100 0b10010010 0b00101000 0b00010000 0b00100000 0b00001000 0b00001000 0b00100000 0b00000000 0b00000000
.byte 0b10010010 0b10000010 0b11000010 0b10000010 0b11000010 0b10000000 0b10000000 0b11000010 0b10000010 0b00010000 0b10001000 0b10000010 0b01000000 0b10000010 0b10000110 0b10000010 0b10000000 0b10000100 0b10000100 0b10000010 0b00010000 0b10000010 0b00101000 0b10101010 0b01000100 0b00010000 0b01000000 0b00001000 0b00000100 0b00100000 0b00000000 0b00000000
.byte 0b01111100 0b10000010 0b10111100 0b01111100 0b10111100 0b11111110 0b10000000 0b10111110 0b10000010 0b11111110 0b01110000 0b10000010 0b01111100 0b10000010 0b10000010 0b01111100 0b10000000 0b01111010 0b10000010 0b01111100 0b00010000 0b01111100 0b00010000 0b01000100 0b10000010 0b00010000 0b11111110 0b00001110 0b00000010 0b11100000 0b00000000 0b11111111
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
ctset_2:
.byte 0b00000100 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000110 0b00010000 0b11000000 0b00000000 0b00000000
.byte 0b00000010 0b00000000 0b01000000 0b00000000 0b00000100 0b00000000 0b00111000 0b00000000 0b01000000 0b00010000 0b00010000 0b01000000 0b00010000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00010000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00001000 0b00010000 0b00100000 0b00000000 0b00000000
.byte 0b00000000 0b00111000 0b01000000 0b00111100 0b00000100 0b00111000 0b01000000 0b00111100 0b01000000 0b00000000 0b00000000 0b01001000 0b00010000 0b01101000 0b01111000 0b00111000 0b01111000 0b00111100 0b00010100 0b00111100 0b00010000 0b01000100 0b01000100 0b01000100 0b01000100 0b01000100 0b01111100 0b00001000 0b00010000 0b00100000 0b00000010 0b00000000
.byte 0b00000000 0b00000100 0b01111000 0b01000000 0b00111100 0b01000100 0b01000000 0b01000100 0b01111000 0b00010000 0b00010000 0b01001000 0b00010000 0b01010100 0b01000100 0b01000100 0b01000100 0b01000100 0b00011000 0b01000000 0b00111000 0b01000100 0b01000100 0b01000100 0b00101000 0b00111100 0b00001000 0b00110000 0b00010000 0b00011000 0b01111100 0b00000000
.byte 0b00000000 0b00111100 0b01000100 0b01000000 0b01000100 0b01111000 0b01111000 0b00111100 0b01000100 0b00010000 0b00010000 0b01110000 0b00010000 0b01010100 0b01000100 0b01000100 0b01000100 0b01000100 0b00010000 0b00111100 0b00010000 0b01000100 0b00101000 0b01010100 0b00010000 0b00000100 0b00010000 0b00001000 0b00010000 0b00100000 0b10000000 0b00000000
.byte 0b00000000 0b01000100 0b01000100 0b01000000 0b01000100 0b01000000 0b01000000 0b00000100 0b01000100 0b00010000 0b00010000 0b01001000 0b00010000 0b01000100 0b01000100 0b01000100 0b01111000 0b00111100 0b00010000 0b00000010 0b00010000 0b01000100 0b00101000 0b01010100 0b00101000 0b00000100 0b00100000 0b00001000 0b00010000 0b00100000 0b00000000 0b00000000
.byte 0b00000000 0b00111100 0b01111000 0b00111100 0b00111100 0b00111000 0b10000000 0b01111000 0b01000100 0b00010000 0b01100000 0b01001000 0b00001000 0b01000100 0b01000100 0b00111000 0b01000000 0b00000100 0b00010000 0b00111100 0b00010000 0b00111100 0b00010000 0b00101000 0b01000100 0b00111000 0b01111100 0b00000110 0b00010000 0b11000000 0b00000000 0b00000000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b01000000 0b00000110 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
ctset_3: # we'll use this for an extended standard char set
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000
ctset_end:

cursor:
.byte 0b11111111 #not yet implemented

baby_nums_start:
baby_nums_l:
.byte 0b11100000 0b01000000 0b11100000 0b11100000 0b10100000 0b11100000 0b11100000 0b11100000 0b11100000 0b11100000
.byte 0b10100000 0b11000000 0b00100000 0b00100000 0b10100000 0b10000000 0b10000000 0b00100000 0b10100000 0b10100000
.byte 0b10100000 0b01000000 0b11100000 0b11100000 0b11100000 0b11100000 0b11100000 0b00100000 0b11100000 0b11100000
.byte 0b10100000 0b01000000 0b10000000 0b00100000 0b00100000 0b00100000 0b10100000 0b00100000 0b10100000 0b00100000
.byte 0b11100000 0b11100000 0b11100000 0b11100000 0b00100000 0b11100000 0b11100000 0b00100000 0b11100000 0b00100000
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000  
baby_nums_r:
.byte 0b00001110 0b00000100 0b00001110 0b00001110 0b00001010 0b00001110 0b00001110 0b00001110 0b00001110 0b00001110
.byte 0b00001010 0b00001100 0b00000010 0b00000010 0b00001010 0b00001000 0b00001000 0b00000010 0b00001010 0b00001010
.byte 0b00001010 0b00000100 0b00001110 0b00001110 0b00001110 0b00001110 0b00001110 0b00000010 0b00001110 0b00001110
.byte 0b00001010 0b00000100 0b00001000 0b00000010 0b00000010 0b00000010 0b00001010 0b00000010 0b00001010 0b00000010
.byte 0b00001110 0b00001110 0b00001110 0b00001110 0b00000010 0b00001110 0b00001110 0b00000010 0b00001110 0b00000010
.byte 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 0b00000000 
baby_nums_end:
.text
.const BNUMS_LINE_WIDTH_B = 10
.const BNUMS_LEFT    = 0
.const BNUMS_RIGHT   = 1
.const BNUMS_CLOBBER = 1
.data

nonalpha_shift_map: # we use this for linear look ups
    .byte '`', '~'
    .byte '1', '!'
    .byte '2', '@'
    .byte '3', '\#'
    .byte '4', '$'
    .byte '5', '%'
    .byte '6', '^'
    .byte '7', '&'
    .byte '8', '*'
    .byte '9', '('
    .byte '0', ')'
    .byte '-', '_'
    .byte '=', '+'
    .byte '[', '{'
    .byte ']', '}'
    .byte '\\', '|'
    .byte ';', ':'
    .byte '\'', '\"'
    .byte ',', '<'
    .byte '.', '>'
    .byte '/', '?'
    .byte ' ', ' '

month_str_array:
    .string "JAN"
    .string "FEB"
    .string "MAR"
    .string "APR"
    .string "MAY"
    .string "JUN"
    .string "JUL"
    .string "AUG"
    .string "SEP"
    .string "OCT"
    .string "NOV"
    .string "DEC"

.text
# nonalpha shift map
.const NONALPHA_SHIFT_MAP_ENTRY_SIZE = 2
.const NONALPHA_SHIFT_MAP_ENTRY_COUNT = 22
.const NONALPHA_SHIFT_MAP_SIZE = 44
# month str array
.const MONTH_STR_ARRAY_ENTRY_SIZE = 4
.const MONTH_STR_ARRAY_SIZE = 48
.data

my_str_0:
    .string "Welcome to the Bear16 Console! \n    VERSION 0.0.1, 20250619"

.text
.const FB_LOC = 0
.const LINE_SIZE = 256
.const TILE_MAP_SIZE = 256
.const LINE_WIDTH_B = 32
.const IO_LOC = 6144 # SUBJ TO CHANGE
.const SHIFT_LOC = 6145 # SUBJ TO CHANGE

start:
    call init_os
    mov a0, 1 # line
    mov a1, 0 # index
    mov a2, my_str_0
    mov s10, 1 # true
    call blit_strl_rom #blitting a str

    call test_bite_of_87 # from test.asm

    # go into text editor (WIP)
    mov a0, s1 # move into s1 line ptr into line arg
    call text_editor_main
    hlt

text_editor_main:
# buffer assumes this memory is reserved for now, in future malloc this
# actually, adjust this cuz bottom two lines reserved for OS display
.const TEXT_PAGE_SIZE = 768 # abs. max, also inexact
.const NUM_PAGES      = 8   # inexact
.const TEXT_EDITOR_BUFFER_STRT = 12288
.const TEXT_EDITOR_BUFFER_END  = 18432 # (8 pages difference)
    # a0 = starting line
    mov s1, a0 # s1 = line ptr
    clr s0     # s0 = index ptr
    text_editor_loop:
    call os_update
    mov a0, s1  # line ptr
    mov a1, s0  # index ptr
    mov a2, '_' # underscore for our cursor
    call blit_cl
    lb s2, IO_LOC            # save inp from IO
    eq subr_backspace, s2, 8 # for backspace
    eq subr_newline, s2, 13  # for newline
    eq subr_tab, s2, 9       # for tab
    # ~~~~~~~~~~~~~~~~~~~~~~ # branch divide
    ugt subr_print_new_char, s2, 0
    jmp text_editor_loop
    ret
    subr_print_new_char:
        # s0 = current char* to print
        mov a0, s1
        mov a1, s0     # a1 = index
        lb a2, IO_LOC # a2 = char
        lb t6, SHIFT_LOC
        ugt subr_shift, t6, 0 # if shift = true
        subr_shift_exit:
        call blit_cl # a0, a1, a2 used
        inc s0
        lea t0, IO_LOC # ->
        sb t0, 0       # clear IO memory location
        uge subr_go_on_newline, s0, LINE_WIDTH_B
        jmp text_editor_loop
        subr_go_on_newline:
            inc s1 # next line!
            clr s0 # set index on line back to zero
            jmp text_editor_loop
        subr_shift:
            ult ssubr_nonalpha_shift, a2, 97
            ugt ssubr_nonalpha_shift, a2, 122
            sub a2, a2, 32
            jmp subr_shift_exit
            ssubr_nonalpha_shift:
                    lea t7, nonalpha_shift_map
                ssubr_nonalpha_shift_loop:
                    lbrom t8, t7
                    mov s6, t7 # debug
                    eq ssubr_nonalpha_shift_hit, a2, t8
                    add t7, t7, 2
                    jmp ssubr_nonalpha_shift_loop
                    #error if t7 > size of nonalpha shift map
                    jmp subr_shift_exit
                ssubr_nonalpha_shift_hit:
                    lbrom a2, t7, 1
                    jmp subr_shift_exit
        subr_backspace:
            #clear @ current spot
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space for blank
            call blit_cl
            dec s0
            lt ssubr_backline, s0, 0
            ssubr_backline_exit:
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            jmp text_editor_loop
                ssubr_backline:
                    dec s1               # go back a line
                    mov s0, 31 # set index ptr to end of last line
                    jmp ssubr_backline_exit
        subr_newline:
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space for blank
            call blit_cl
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            inc s1
            clr s0
            jmp text_editor_loop
        subr_tab:
            mov a0, s1  # line ptr
            mov a1, s0  # index ptr
            mov a2, ' ' # space for blank
            call blit_cl
            add s0, s0, 3 # move forward 2 indices for tab + 1 for going to next char
            lea t0, IO_LOC # ->
            sb t0, 0       # clear IO memory location
            jmp text_editor_loop



utility_inf_loop:
    jmp utility_inf_loop
    ret
    
blit_cl:
    #a0 = line, a1 = index, a2 = desired char
    mult t0, a0, LINE_SIZE # set line
    add t0, t0, a1 # set index
    add t0, t0, FB_LOC #adjust for FB location start in SRAM
    sub t1, a2, 32 # get char index in rom
    div t7, t1, 32
    mov t8, t7 #save this
    mult t7, t7, LINE_SIZE
    add t1, t1, t7
    mult t8, t8, 32
    sub t1, t1, t8
    add t1, t1, ctset_start
    clr t2 # cnt
    bcl_loop:
        lbrom t3, t1       #load byte from rom in t3
        sb t0, t3          #store byte in t3 into addr @ t0
        add t0, t0, LINE_WIDTH_B     # t0 += 32
        add t1, t1, LINE_WIDTH_B     # t1 += 32
        inc t2             # t2++
        ult bcl_loop, t2, 8 # check cnt
    ret
blit_strl_rom:
    #a0 = line, a1 = index, a2 = char* (rom addr), s10 = bool updateCursorPtr
    mov t5, a2 # current char ptr
    bstrl_rom_loop:
        lbrom a2, t5 # a2 <- *char
        eq bstrl_rom_ret, a2, 0
        eq bstrl_rom_tab, a2, 9
        eq bstrl_rom_newline, a2, 10
        call blit_cl #reuse a0 & a1
        inc a1
        uge bstrl_rom_newline, a1, 32 # if index >= 32 do \n
        bstrl_rom_subr_exit:
        inc t5
        jmp bstrl_rom_loop
    bstrl_rom_ret:
        ugt bstrl_rom_do_update_cursor, s10, 0 # if s10 == true
        bstrl_rom_do_update_cursor_exit:
        clr s10       # reset this val (do not reuse, defaults to do not update)
        ret
    bstrl_rom_newline:
        inc a0
        clr a1
        jmp bstrl_rom_subr_exit
    bstrl_rom_tab:
        add a1, a1, 2
        jmp bstrl_rom_subr_exit
    bstrl_rom_do_update_cursor:
        add s1, a0, 1 # set line ptr to end of strl + 1
        clr s0        # set index ptr 0 (start of line)
        jmp bstrl_rom_do_update_cursor_exit

#OS Stuff (WIP)
#CLOCK MEM_LOC CONSTANTS (ALL SUBJ TO CHANGE)
.const FRAMES_MEM_LOC = 6146
.const SECONDS_PTR_MEM_LOC = 6147
.const MINUTES_PTR_MEM_LOC = 6148
.const HOURS_PTR_MEM_LOC = 6149
.const DAYS_PTR_MEM_LOC = 6150
.const MONTHS_PTR_MEM_LOC = 6151
.const YEARS_PTR_MEM_LOC = 6152
    init_os:
        call subr_init_os_draw_bottom_line # perhaps inline
        call subr_init_day_month_year      # initialize dates (static/non updated until restart) -> note: may cause inaccuracies
        ret
        subr_init_os_draw_bottom_line:
            clr s2 # cnt & index
            subr_init_os_draw_bottom_line_loop:
                mov a0, 22
                mov a1, s2
                mov a2, '_'
                call blit_cl
                inc s2
                ult subr_init_os_draw_bottom_line_loop, s2, 32
            ret
        subr_init_day_month_year:
            #DAYS
            mov a0, 23 # line
            mov a1, 21 # index
            lb a2, DAYS_PTR_MEM_LOC
            call blit_2dig_pint
            #MON
            mov a0, 23 # line
            mov a1, 17 # index
            lb a2, MONTHS_PTR_MEM_LOC
            mult a2, a2, MONTH_STR_ARRAY_ENTRY_SIZE
            add a2, a2, month_str_array
            call blit_strl_rom
            #YEAR
            mov a0, 23 # line
            mov a1, 11 # index
            lw a2, YEARS_PTR_MEM_LOC
            call blit_4dig_pint
            ret
    os_update:
        call os_update_time_display
        ret
        
    os_update_time_display:
        # SECONDS
        mov a0, 23 # line
        mov a1, 30 # index
        lb a2, SECONDS_PTR_MEM_LOC
        call blit_2dig_pint
        mov a0, 23 # line
        mov a1, 29 # index
        mov a2, ':'
        call blit_cl
        #MINUTES
        mov a0, 23 # line
        mov a1, 27 # index
        lb a2, MINUTES_PTR_MEM_LOC
        call blit_2dig_pint
        mov a0, 23 # line
        mov a1, 26 # index
        mov a2, ':'
        call blit_cl
        #HOURS
        mov a0, 23 # line
        mov a1, 24 # index
        lb a2, HOURS_PTR_MEM_LOC
        call blit_2dig_pint
        mov a0, 23 # line
        mov a1, 23 # index
        mov a2, ','
        call blit_cl
        # DAYS, MONTHS, YEARS generated in init
        ret

    blit_2dig_pint:
        # a0 = line, a1 = index, a2 = num
        div t0, a2, 10 # tens digit
        mod t1, a2, 10 # ones digit
        push t1
        # reuse a0 and a1 for blitting char tiles
        add a2, t0, 48 # recover tens digit into a2 & add 48 to get char val from num val
        call blit_cl
        inc a1         # increment index
        pop t1         # recover ones digit
        add a2, t1, 48 # t1 (ones digit) -> a2 & add 48 to get char val from num val
        call blit_cl
        ret
    blit_4dig_pint:
        # a0 = line, a1 = index, a2 = num
        mod t0, a2, 10 # ones place
        push t0
        div t0, a2, 10  # tens place
        mod t0, t0, 10  # "
        push t0
        div t0, a2, 100 # hundreds place
        mod t0, t0, 10  # "
        push t0
        div t0, a2, 1000 # thousands place
        push t0
        clr s2 # cnt, enter loop ->
        blit_4dig_pint_loop:
            # reusing a0 always (same line)
            add a1, a1, 1 # 1 -> 2 -> 3 -> 4
            pop a2 # thou -> hund -> tens -> ones
            add a2, a2, 48 # to get char representation of our digit
            call blit_cl
            inc s2
            uge blit_4dig_pint_ret, s2, 4
            jmp blit_4dig_pint_loop
        blit_4dig_pint_ret:
            ret
    blit_bnum:
        #a0 = line, a1 = index, a2 = num, s10 = l/r, s9 = shift down pixels (1:1 row of pix), s8 = clobber (do not or)
        mult t1, s9, LINE_WIDTH_B # this t1 will later get safely overwritten
        mult t0, a0, LINE_SIZE    # set line
        add t0, t0, a1            # set index
        add t0, t0, FB_LOC        # adjust for FB location start in SRAM
        add t0, t0, t1            # add shift down pixels
        eq bbnum_left, s10, BNUMS_LEFT
        eq bbnum_right, s10, BNUMS_RIGHT
        # t1 set by ^
        bbnum_dir_exit:
        clr t2 # cnt
        bbnum_loop:
            lbrom t3, t1       # load byte from rom in t3
            lb t4, t0          # load byte at target address
            eq bbnum_clobber, s8, BNUMS_CLOBBER
            or t3, t3, t4      # bitwise or so we can have split bytes (the entire point of these baby nums)
            bbnum_clobber:
            sb t0, t3          # store byte in t3 into FB addr @ t0
            add t0, t0, LINE_WIDTH_B       # t0 += 32
            add t1, t1, BNUMS_LINE_WIDTH_B # t1 += 10
            inc t2                # t2++
            ult bbnum_loop, t2, 6 # check cnt
        ret
        bbnum_left:
            add t1, a2, baby_nums_l
            jmp bbnum_dir_exit
        bbnum_right:
            add t1, a2, baby_nums_r
            jmp bbnum_dir_exit
