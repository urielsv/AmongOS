GLOBAL read, write, random, read_char, draw, sleep, time, sound, hlt, clear, writing_pos, screen_info,font_size,sys_registers,test_exc_zero,test_exc_invalid_opcode,mem_alloc,mem_free, exec, kill, block, unblock



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