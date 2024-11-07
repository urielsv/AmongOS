GLOBAL read, write, random, read_char, draw, sleep, time, sound, hlt, clear, writing_pos, screen_info,font_size,sys_registers,test_exc_zero,test_exc_invalid_opcode,mem_alloc,mem_free, exec, kill, block, unblock, set_priority, get_pid, yield, sem_open, sem_wait, sem_post, sem_close, waitpid, process_exists, process_snapshot



read:
    mov rax, 0
    int 0x80
    ret

write:
    mov rax, 1
    int 0x80
    ret

random:
    mov rax, 6
    int 0x80
    ret

read_char:
    mov rax, 7
    int 0x80
    ret

draw:
    mov rax, 8
    int 0x80
    ret


sleep:
    mov rax,9
    int 0x80
    ret

time:
    mov rax,10
    int 0x80
    ret

sound:
    mov rax,11
    int 0x80
    ret

hlt:
    mov rax,12
    int 0x80
    ret

clear:
    mov rax, 13
    int 0x80
    ret

writing_pos:
    mov rax, 14
    int 0x80
    ret

screen_info:
    mov rax, 15
    int 0x80
    ret

font_size:
    mov rax,16
    int 0x80
    ret

sys_registers:
    mov rax,17
    int 0x80
    ret

test_exc_zero:
    mov rax,18
    int 0x80
    ret

test_exc_invalid_opcode:
    mov rax,19
    int 0x80
    ret

mem_alloc:
    mov rax, 20
    int 0x80
    ret

mem_free:
    mov rax, 21
    int 0x80
    ret

exec:
    mov rax, 22
    int 0x80
    ret

kill:
    mov rax, 23
    int 0x80
    ret

block:
    mov rax, 24
    int 0x80
    ret

unblock:
    mov rax, 25
    int 0x80
    ret

set_priority:
    mov rax, 26
    int 0x80
    ret

get_pid:
    mov rax, 27
    int 0x80
    ret

yield:
    mov rax, 28
    int 0x80
    ret

sem_open:
    mov rax, 29
    int 0x80
    ret

sem_wait:
    mov rax, 30
    int 0x80
    ret

sem_post:
    mov rax, 31
    int 0x80
    ret

sem_close:
    mov rax, 32
    int 0x80
    ret

waitpid:
    mov rax, 2
    int 0x80
    ret
   
process_exists:
    mov rax, 3
    int 0x80
    ret
;proxima 33

process_snapshot:
    mov rax, 11
    int 0x80
    ret
