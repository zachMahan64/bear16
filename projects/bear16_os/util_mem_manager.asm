# UTIL_MEM_MANAGER.ASM (WIP)

# WIP, need to implement dealloc w/ basic free list & then update malloc to search accordingly
# ALSO PROBABLY ADD SMART STACK ALLOC FUNCTIONS TOO! -> add salloc and sallocz
.text
.const TOP_OF_HEAP_PTR = 6656
.const STARTING_HEAP_PTR_VALUE = 16384
.const FREE_LIST_HEAD_LOC = 6558

.const NULL = 0

util_init_free_list:
    lea t0, FREE_LIST_HEAD_LOC
    sw t0, NULL
    ret

# HEAP ALLOCATION FUNCTIONS
util_malloc:  #LINKED FREE LIST NOT YET IMPLEMENTED
    # reserves a0 + 2 bytes and then store size in the base & return the ptr just above the base
    # a0 = num bytes
    lw t0, TOP_OF_HEAP_PTR
    util_malloc_hit_in_free_list_exit:
    sw t0, a0 # save size to base of the allocated
    add t0, t0, 2 # move store ptr (t0) forward
    mov rv, t0 # base of the allocated memory + 2, where the allocated mem data starts -> return this!
    add t1, t0, a0 # end loop val / new top of heap value
    util_malloc_loop:
        sb t0, 0 # write zero/clear memory
        inc t0 # inc store location
        ult util_malloc_loop, t0, t1
    lea t2, TOP_OF_HEAP_PTR
    sw t2, t1 # store new top of heap value
    ret
    util_malloc_traverse_free_list:
        lea t0, FREE_LIST_HEAD_LOC

util_free: # WIP, BUILD W/ LINKED FREE LIST (NO COALESENCE YET)
    # a0 = pointer to memory data (from a malloc return)
    # block layout:
    #   [a0 - 2] = size
    #   [a0    ] = data
    # we insert [a0 - 2] into free list
    # LINK STRUCT: {WORD: SIZE, WORD: NEXT*}
    sub t0, a0, 2               # t0 = start of block (where header begins)
    lw t1, FREE_LIST_HEAD_LOC   # t1 = old head of free list
    sw t0, 2, t1                # store old head into new block’s NEXT*
    lea t2, FREE_LIST_HEAD_LOC
    sw t2, t0                   # set free list head to this block
    ret

util_ralloc:
    # !!! REGION ALLOC, does not adjust TOP_OF_HEAP_PTR -> this function is dangerous altough useful in priveledged...
    # ...memory space
    # a0 = ptr to start of region
    # a1 = size/num bytes
    # ~ no rv becuz it would be the same as a0 (ptr to start is already known)
    lw t0, a0
    add t1, t0, a1 # end loop val
    util_ralloc_loop:
        sb t0, 0 # write zero/clear memory
        inc t0 # inc store location
        ult util_ralloc_loop, t0, t1
    ret
#STACK ALLOCATION -> inline when possible, but these may be useful
util_salloc:
    # a0 = num bytes to allocate on stack
    sub sp, sp, a0
    ret
util_sallocz:
    # a0 = num bytes to allocate on stack
    # ~ don't return anything, use fp for referencing local vars
    mov t0, sp # t0 = end
    sub sp, sp, a0
    mov t1, sp # start, counter, & ptr
    util_sallocz_loop:
        sb t1, 0 # clear memory
        inc t1
        ult util_sallocz_loop, t1, t0
# HELPER/BASICS
util_get_top_of_heap_ptr:
    lw t0, TOP_OF_HEAP_PTR
    mov rv, t0
    ret


