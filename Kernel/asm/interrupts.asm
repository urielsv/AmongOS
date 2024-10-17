global asm_cli, asm_sti, asm_hlt

global pic_master_mask, pic_slave_mask

global asm_syscall80_handler

global asm_irq00_handler, asm_irq01_handler

global asm_exception00_handler, asm_exception06_handler

global create_process_stack_frame

extern irq_dispatcher, syscall_dispatcher, exception_dispatcher, save_registers, scheduler

SECTION .text

REGISTER_CAPTURE equ '0'


%macro push_state_no_rax 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro pop_state_no_rax 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro push_state 0
	push rax
	push_state_no_rax
%endmacro

%macro pop_state 0
	pop_state_no_rax
	pop rax
%endmacro

;handler de las interrupcion Hardware
%macro irq_handler 1
	push rax
    push_state

    mov rdi,%1
    call irq_dispatcher

	push rax
    ;signal pic EOI (End of Interrupt)
    mov al,0x20
    out 0x20, al
	pop rax

	cmp rax,REGISTER_CAPTURE 
	jne .end
	mov rdi,rsp
	call save_registers

.end:
	pop_state
	pop rax
    iretq
%endmacro

%macro exception_handler 1
	push_state

	mov rdi,%1
	mov rsi,rsp
	call exception_dispatcher
	
	pop_state
	iretq
%endmacro

; sys_id is stored in rax (hardcoded)
%macro syscall_handler 1
    ;push_state_no_rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

    mov r9, rax
    call syscall_dispatcher

    ;pop_state_no_rax
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
    iretq
%endmacro

asm_cli:
    cli
    ret

asm_sti:
    sti
    ret


asm_hlt:
	sti
	hlt
	ret

pic_master_mask:
    push rbp
    mov rbp, rsp

    mov ax, di
    out 0x21, al

    pop rbp
    retn

pic_slave_mask:
    push rbp
    mov rbp, rsp

    mov ax, di
    out 0x0A1, al

    pop rbp
    retn


;Timer interrupt(timer tick)
asm_irq00_handler:
    ;irq_handler 0
	;caso especial para el scheduler

    push_state

	mov rdi, 0 ; pasaje de parametro
	call irq_dispatcher

	mov rdi, rsp
	call scheduler
	mov rsp, rax

	mov al, 0x20
	out 0x20, al

	pop_state
	iretq

;Keyboard interrupt
asm_irq01_handler:
    irq_handler 1

; Zero Division Exception
asm_exception00_handler:
	exception_handler 0

; Invalid Opcode Exception
asm_exception06_handler:
	exception_handler 6

; Syscall (id) interrupt
asm_syscall80_handler:
    syscall_handler 80

create_process_stack_frame:

	mov r8, rsp 	; Preservar rsp
	mov r9, rbp		; Preservar rbp
	mov rsp, rsi 	; sp del proceso
	mov rbp, rsi	; bp del proceso

	push 0x0		; ss
	push rsi		; rsp
	push 0x202		; rflags
	push 0x8		; cs
	push rdi		; rip
	
	push_state
	mov rax, rsp
	mov rsp, r8
	mov rbp, r9

	ret
