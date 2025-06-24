# UTIL_MEM_MANAGER.ASM (WIP)

# WIP, need to implement dealloc w/ basic free list & then update malloc to search accordingly
# ALSO PROBABLY ADD SMART STACK ALLOC FUNCTIONS TOO! -> add salloc and sallocz
.text
.const TOP_OF_HEAP_PTR = 6656
.const STARTING_HEAP_PTR_VALUE = 16384
.const FREE_LIST_START_ADDRESS = 6558
.const FREE_LIST_END_ADDRESS   = 7582   # = FREE_LIST_START_ADDRESS + 1024
.const FREE_LIST_SIZE = 1024
util_init_free_list:
    mov a0, FREE_LIST_START_ADDRESS
    mov a1, FREE_LIST_SIZE
    call util_ralloc
    ret

# HEAP ALLOCATION FUNCTIONS
util_malloc:  #NOTE: VERY UNOPTIMIZED CURRENTLY, replace free list w/ linked list at some point
    # reserves a0 + 2 bytes and then store size in the base & return the ptr just above the base
    # a0 = num bytes
    lw t0, TOP_OF_HEAP_PTR
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
    subfunc_util_malloc_search_free_list:


util_free: #NOTE: VERY UNOPTIMIZED CURRENTLY, replace free list w/ linked list at some point
    # a0 = ptr to memory chunk
    lw t0, a0 #load memory size
    mov t1, FREE_LIST_START_ADDRESS # cnt/ptr to addr in FREE LIST
    util_free_loop:
        lw t2, t1 # deference value in list
        eq util_free_found_room_in_list, t2, 0 # jump if found an empty entry
        add t1, t1, 2 #increment t0 by a word to cycle through the list cleanly & faster
        lt util_free_loop, t1, FREE_LIST_END_ADDRESS
        # MEMORY ERROR -> NO ROOM IN FREE LIST
        ret
        util_free_found_room_in_list:
            sw a0, t0
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


