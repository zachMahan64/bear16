
#HELLO WORLD IN ASM, 6/2/2025
.const dos = 2
.const MAGIC_WORD = 69
.const MY_PAGE = 4096 #our reference point in memory, this idea can be expanded to support virtual memory
.const RESPONSE_START = 4108
start:
   mov t0, 69
   mov t1, dos
   ne end, t0, MAGIC_WORD
   mov s0, MAGIC_WORD # shows that magic word was guessed
   jal hello_world # think uses the stack (stack pointer and frame pointer)
   jal respond
end:
   hlt


hello_world:
   sb MY_PAGE, 0, 'H' # this does -> dest, offset (in mem), value_to_store
   sb MY_PAGE, 1, 'E'
   sb MY_PAGE, 2, 'L'
   sb MY_PAGE, 3, 'L'
   sb MY_PAGE, 4, 'O'
   sb MY_PAGE, 5, ' '
   sb MY_PAGE, 6, 'W'
   sb MY_PAGE, 7, 'O'
   sb MY_PAGE, 8, 'R'
   sb MY_PAGE, 9, 'L'
   sb MY_PAGE, 10, 'D'
   retl


respond:
   memcpy RESPONSE_START, MY_PAGE, 5
   push '!'
   pop t2
   sb RESPONSE_START, 5, t2
   retl
