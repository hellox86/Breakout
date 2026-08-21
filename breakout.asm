org 0x7C00
bits 16

OFFSET EQU 0xA000
WIDTH EQU 320
HEIGHT EQU 200
PADDLE_WIDTH EQU 16
PADDLE_HEIGHT EQU 7
BALL_RADIUS EQU 4
	
start: 
	mov ax, 0x13
	int 0x10	
	xor ax, ax
	mov es, ax
	mov ds, ax

	mov word [padx], 160
	mov word [pady], 180
	mov word [ballx], 160
	mov word [bally], 174
	mov word [bdx], 4
	mov word [bdy], 4
	mov dword [0x0070], draw
	
.loop:
    hlt
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
	mov word [pdx], -5
	jmp .no_key
	
.right:
	mov word [pdx], 5	
	jmp .no_key

.no_key:
	jmp .loop 

draw_paddle:
	mov ax, [pady]
	mov bx, [padx]
	sub bx, PADDLE_WIDTH>>1
	mov cx, PADDLE_HEIGHT
	mov dh, PADDLE_WIDTH
	call rowloop
	ret
draw_ball:
	mov ax, [bally]
	mov bx, [ballx]
	sub bx, BALL_RADIUS>>1
	mov cx, BALL_RADIUS
	mov dh, BALL_RADIUS
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
	pop cx
	mov cl, dh
	xor ch, ch
	
	rep stosb
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

;; old hack
bounce:
	mov ax, [ballx]
	sub ax, PADDLE_WIDTH>>1
	cmp ax, 0
	jge .div
	add ax, 3
.div:
	sar ax, 2
	add ax, [bdx]
	neg word [bdy]
	ret


update:
	cmp word [padx], PADDLE_WIDTH
	jle .neg_pad
	cmp word [padx], WIDTH - PADDLE_WIDTH
	jge .neg_pad

	cmp word [ballx], 0
	jle .neg_ballx
	cmp word [ballx], WIDTH - (BALL_RADIUS)
	jge .neg_ballx

	cmp word [bally], 0
	jle .neg_bally
	cmp word [bally], HEIGHT - (BALL_RADIUS)
	jge .neg_bally
	call col
	
.uball:
	mov ax, [ballx]
	add ax, [bdx]
	mov [ballx], ax
	xor ax, ax
	
	mov ax, [bally]
	add ax, [bdy]
	mov [bally], ax

.upad:
	mov ax, [padx]
	add ax, [pdx]
	mov [padx], ax
	ret	
.neg_pad:
	neg word [pdx]
	jmp .upad 
.neg_ballx:
	neg word [bdx]
	jmp .uball
.neg_bally:
	neg word [bdy]
	jmp .uball

col:
	;; optimized collision 
	mov ax, [ballx]
	sub ax, [padx]
	cmp ax, PADDLE_WIDTH + BALL_RADIUS
	jg .end
	add ax, BALL_RADIUS
	jle .end

	mov ax, [bally]
	sub ax, [pady]
	cmp ax, PADDLE_HEIGHT + BALL_RADIUS
	jg .end
	add ax, BALL_RADIUS
	jle .end
	jmp .hand
	
.hand:
	call bounce
	call sound
.end:
	ret
sound:
	pusha
	mov dx, 0x61
	mov al, 0x02
	out dx, al

	mov cx, 1000
	.delay:
		dec cx
		jnz .delay

	xor al, al
	out dx, al
	popa
	ret
draw:
	pusha
	mov ax, OFFSET
	mov es, ax
	call update
	call clear_scr
	mov dl, 0x4F
	call draw_paddle
	
	mov dl, 0xC
	call draw_ball
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
	
