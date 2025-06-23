#WIP, need to implement dealloc w/ basic free list & then update malloc to search accordingly
#ALSO PROBABLY ADD SMART STACK ALLOC FUNCTIONS TOO! -> add salloc and sallocz
.text
.const TOP_OF_HEAP_PTR = 6656
.const STARTING_HEAP_PTR_VALUE = 16384
util_malloc: #make this reserve a0 + 2 bytes and then store size in the base
    # a0 = num bytes
    lw t0, TOP_OF_HEAP_PTR
    mov rv, t0 # this will be the base of the allocated memory -> return this!
    add t1, t0, a0 # end loop val / new top of heap value
    util_malloc_loop:
    sb t0, 0 # write zero/clear memory
    inc t0 # inc store location
    ult util_malloc_loop, t0, t1
    lea t2, TOP_OF_HEAP_PTR
    sw t2, t1 # store new top of heap value
    ret
util_dealloc: #like free but can take a specified size
    # a0 = ptr, a1 = size (num bytes)

