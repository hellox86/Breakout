org 0x7C00
bits 16

OFFSET EQU 0xA000
WIDTH EQU 320
HEIGHT EQU 200
PADDLE_WIDTH EQU 20
PADDLE_HEIGHT EQU 7

start: 
	mov ax, 0x13
	int 0x10	
	xor ax, ax
	mov es, ax
	mov ds, ax

	mov word [padx], 160
	mov word [pady], 180
	mov word [ballx], 160
	mov word [bally], 170

	mov dword [0x0070], draw
	
.loop:
    mov ah, 0x01
    int 0x16

    jz .no_key
	
    xor ah, ah
    int 0x16	
    
    cmp al, 0x1B
    jz exit

    cmp al, 'a'
    jz .left
    cmp ah, 0x4B
    jz .left

    cmp al, 'd'
    jz .right
    cmp ah, 0x4D
    jz .right
    cmp al, 'r'
    jz restart
    jmp .no_key


.left:
	mov word [pdx], -10
	jmp .no_key
	
.right:
	mov word [pdx], 10	
	jmp .no_key

.no_key:
	jmp .loop 

draw_paddle:
	mov ax, [pady]
	mov bx, [padx]
	sub bx, PADDLE_WIDTH>>1
	mov cx, PADDLE_HEIGHT
	call rowloop
	ret
clear_scr:
	pusha
	mov ax, OFFSET
	mov es, ax
	xor di, di
	mov cx, WIDTH * HEIGHT
	rep stosb
	popa
	ret
rowx:
	push ax
	push bx
	push di
	push cx
	push dx
	
	mov dx, WIDTH
	mul dx
	pop dx
	
	add ax, bx
	mov di, ax
	
	mov al, dl
	mov cx, PADDLE_WIDTH
	rep stosb
	pop cx
	pop di
	pop bx
	pop ax
	ret
	
rowloop:
	push cx
	call rowx
	pop cx
	inc ax
	loop rowloop
	ret
	

update:
	cmp word [padx], PADDLE_WIDTH>>1
	jle .neg_pad
	cmp word [padx], WIDTH - (PADDLE_WIDTH>>1)
	jge .neg_pad

.upad:
	mov ax, [padx]
	add ax, [pdx]
	mov [padx], ax
	ret
	
.neg_pad:
	neg word [pdx]
	jmp .upad 

draw:
	pusha
	mov ax, OFFSET
	mov es, ax
	mov dl, 0x4F
	call update
	call clear_scr
	call draw_paddle
	popa
	iret

restart:
	int 0x19
	hlt
exit:
	mov ax, 0x0003
	int 0x10	
	hlt

padx   dw 0
pady   dw 0
pdx    dw 0
ballx  dw 0
bally  dw 0 
bdx    dw 0
bdy    dw 0

times 510 - ($ - $$) db 0
dw 0xAA55
	
