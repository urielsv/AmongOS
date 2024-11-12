global loader
extern main
extern initialize_kernel_binary

loader:
	call initialize_kernel_binary	; set up the kernel binary, and get thet stack address
	mov rsp, rax				; set up the stack with the returned address
	call main
hang:
	cli
	hlt	; halt machine should kernel return
	jmp hang
