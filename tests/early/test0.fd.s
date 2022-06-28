	jmp .L0
	.L1:
	mov ebx, 15 ; primary(size = 4)
	mov ebx, 3 ; primary(size = 4)
	mov ebx, 4 ; primary(size = 4)
	jmp .L2
	.L3:
	mov ebx, 5 ; primary(size = 4)
	jmp .L2
	.L0:
	mov ebx, 7 ; primary(size = 4)
	test bl, bl
	jne .L1
	jmp .L3
	.L2:
	.L4:
	mov ebx, 1 ; primary(size = 4)
	test bl, bl
	je .L5
	mov ebx, 6 ; primary(size = 4)
	jmp .L4
	.L5:
	mov ebx, 9 ; primary(size = 4)
