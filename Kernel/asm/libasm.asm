global cpu_vendor
global rtc_time
global asm_get_key
global scanf
global asm_getsp
global asm_getbp

global test_write

global outb, inb

section .text
	
cpu_vendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

rtc_time:
	push rbp
	mov rbp, rsp

	mov rax, rdi
	out 0x70, al
	in al, 0x71

	leave
	ret

; get the key pressed (or wait until its pressed)
asm_get_key:

	xor rax, rax

	.loop:
	in al, 0x64
	and al, 0x01
	cmp al, 0
	je .loop

	in al, 0x60

	ret



test_write:

    mov rax, 1
    int 0x80
    ret



inb:				; funciones para el correcto funcionamiento del sound_driver
	push rbp
	mov rbp, rsp

    mov rdx,rdi
    in al,dx		; pasaje en 8 bits

	mov rsp, rbp
	pop rbp
	ret

outb:
	push rbp
	mov rbp, rsp

    mov rax, rsi    
    mov rdx, rdi
	out dx, al		; pasaje en 8 bits

	mov rsp, rbp
	pop rbp
	ret

; devuelve el valor del stack-pointer
asm_getsp:
    mov rax,rsp
    ret

; devuelve el valor del base-pointer
asm_getbp:
    mov rax,rbp
    ret

