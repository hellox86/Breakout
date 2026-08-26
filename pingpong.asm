org 0x7C00
bits 16

OFFSET EQU 0xA000
WIDTH EQU 320
HEIGHT EQU 200
PADDLE_WIDTH EQU 4
PADDLE_HEIGHT EQU 16
BALL_RADIUS EQU 4
COLUMNS EQU 40
ROWS EQU 25

start:
	mov ax, 0x13
	int 0x10	
	xor ax, ax
	mov es, ax
	mov ds, ax
	
	mov word [padx], 10
	mov word [pady], 103
	mov word [ballx], 160
	mov word [bally], 174
	mov word [bdx], 6
	mov word [bdy], 6
	mov byte [state], 1
	mov dword [0x0070], draw
	
.loop:
    hlt
    in al, 0x60
    cmp al, 0x13
    jz start	
    cmp al, 0x1E
    jz .left
    cmp al, 0x4B
    jz .left

    cmp al, 0x20
    jz .right
    cmp al, 0x4D
    jz .right
    jmp .loop

.left:
	cmp word [padx], PADDLE_WIDTH
	jbe .false
	mov word [pdx], -10
	call upad
	jmp .loop
.right:
	cmp word [padx], WIDTH-PADDLE_WIDTH
	jae .false
	mov word [pdx], 10
	call upad	
	jmp .loop
.false:
	mov word [pdx], 0
	call upad
	jmp .loop

draw_paddle:
	mov ax, [pady]
	mov bx, [padx]
	mov cx, PADDLE_WIDTH
	call verlt
	ret
draw_ball:
	mov ax, [bally]
	mov bx, [ballx]
	sub bx, BALL_RADIUS>>1
	mov cx, BALL_RADIUS
	mov dh, BALL_RADIUS
	call rowloop
	ret
upad:
	mov ax, [padx]
	add ax, [pdx]
	mov [padx], ax
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
	
horl:
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
	
p:				
	push ax
	push bx
	push di
	push dx
	mov dx, WIDTH
	mul dx
	pop dx
	
	add ax, bx
	mov di, ax
	mov al, dl
        stosb
	pop di
	pop bx
	pop ax
	ret
verl:
	call p
	inc ax
	loop verl
	ret
verlt:
	push ax
	push cx
	mov cx, PADDLE_HEIGHT
	call verl
	pop cx
	pop ax
	inc bx
	loop verlt
	ret
rowloop:
	push cx
	call horl
	pop cx
	loop rowloop
	ret
	
bounce:
	mov ax, [ballx]
	sub ax, PADDLE_WIDTH>>1
	test ax, ax
	jae .div
	add ax, 3
.div:
	sar ax, 2
	add ax, [bdx]
	neg word [bdy]
	ret

update:
	cmp word [ballx], BALL_RADIUS
	jle .neg_ballx
	cmp word [ballx], WIDTH - (BALL_RADIUS)
	jge .neg_ballx	

	cmp word [bally], 0
	jle .neg_bally
	cmp word [bally], HEIGHT - (BALL_RADIUS)
	jge .neg_bally
	;; call col
	
.uball:
	mov ax, [ballx]
	add ax, [bdx]
	mov [ballx], ax
	xor ax, ax
	
	mov ax, [bally]
	add ax, [bdy]
	mov [bally], ax
	jmp .end
.neg_ballx:
	neg word [bdx]
	jmp .uball
.neg_bally:
	neg word [bdy]
	jmp .uball
.game_over:
	neg byte [state]
	xor ax, ax
	mov es, ax
	mov ax, 0x1301
	
	mov bx, 0x0F
	mov cx, game_over_sign_len
	;(COLS, ROWS)
	mov dx, 0x0C10
	mov bp, game_over_sign
	int 0x10
	
.end:
	ret

col:
	mov ax, [ballx]
	sub ax, [padx]
	cmp ax, PADDLE_WIDTH + BALL_RADIUS
	ja .end
	add ax, BALL_RADIUS
	jbe .end

	mov ax, [bally]
	sub ax, [pady]
	cmp ax, PADDLE_HEIGHT + BALL_RADIUS
	ja .end
	add ax, BALL_RADIUS
	jbe .end
	
	call bounce
.end:
	ret

draw:
	cmp byte [state], 1
	jz .d
	jmp .e	
.d:
	pusha
	mov ax, OFFSET
	mov es, ax
	call clear_scr
	call update
	mov dl, 0x0F
	call draw_paddle
	popa
.e:
	iret
padx   dw 0
pady   dw 0
pdx    dw 0
ballx  dw 0
bally  dw 0 
bdx    dw 0
bdy    dw 0
state db 0
game_over_sign: db "Game Over"
game_over_sign_len equ $ - game_over_sign
	
times 510 - ($ - $$) db 0
dw 0xAA55
	
