	jmp .L0000
	
.L0001:
	mov ebx, 15 ; primary(size = 4)
	mov ebx, 3 ; primary(size = 4)
	mov ebx, 4 ; primary(size = 4)
	jmp .L0002
	
.L0003:
	mov ebx, 5 ; primary(size = 4)
	jmp .L0002
	
.L0000:
	mov ebx, 7 ; primary(size = 4)
	test bl, bl
	jne .L0001
	jmp .L0003
	
.L0002:
	
.L0004:
	mov ebx, 1 ; primary(size = 4)
	test bl, bl
	je .L0005
	mov ebx, 6 ; primary(size = 4)
	jmp .L0004
	
.L0005:
	mov ebx, 9 ; primary(size = 4)
