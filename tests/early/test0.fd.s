	jmp .L0
	
	mov ebx, 15 ; primary(size = 4)
	mov ebx, 3 ; primary(size = 4)
	mov ebx, 4 ; primary(size = 4)
	jmp .L2
	
	mov ebx, 5 ; primary(size = 4)
	jmp .L2
	
	mov ebx, 7 ; primary(size = 4)
	test ebx, ebx
	jne .L1
	jmp .L3
	