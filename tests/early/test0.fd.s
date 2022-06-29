	jmp .L0000
	.L0001:
	mov ebx, 15 ; primary(size = 4)
	mov ebx, 3 ; primary(size = 4)
	mov ebx, 4 ; primary(size = 4)
	.L0003:
	xor ebx, ebx ; zero(size = 4)
	test bl, bl
	je .L0004
	mov ebx, 33 ; primary(size = 4)
	mov ebx, 11 ; primary(size = 4)
	jmp .L0003
	.L0004:
	jmp .L0002
	.L0005:
	mov ebx, 5 ; primary(size = 4)
	jmp .L0002
	.L0000:
	mov ebx, 7 ; primary(size = 4)
	test bl, bl
	jne .L0001
	jmp .L0005
	.L0002:
	.L0006:
	mov ebx, 1 ; primary(size = 4)
	test bl, bl
	je .L0007
	mov ebx, 6 ; primary(size = 4)
	.L0008:
	mov ebx, 7 ; primary(size = 4)
	mov ebx, 30 ; primary(size = 4)
	xor ebx, ebx ; zero(size = 4)
	jmp .L0009
	.L000A:
	mov ebx, 4 ; primary(size = 4)
	jmp .L000B
	.L000C:
	mov ebx, 7 ; primary(size = 4)
	jmp .L000B
	.L0009:
	mov ebx, 3 ; primary(size = 4)
	test bl, bl
	jne .L000A
	jmp .L000C
	.L000B:
	mov ebx, 2160 ; primary(size = 4)
	test bl, bl
	jne .L0008
	mov ecx, 4 ; primary(size = 4)
	jmp .L0006
	.L0007:
	mov ecx, 9 ; primary(size = 4)
	mov ecx, 234 ; primary(size = 4)
