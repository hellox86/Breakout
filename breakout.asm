org 0x7C00
bits 16

OFFSET EQU 0xA000
WIDTH EQU 320
HEIGHT EQU 200
PADDLE_WIDTH EQU 16
PADDLE_HEIGHT EQU 7
BALL_RADIUS EQU 4
COLUMNS EQU 40
ROWS EQU 25

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
	mov word [state], 0
	mov dword [0x0070], draw
	
.loop:
    hlt
    in al, 0x60
    cmp al, 0x1E
    jz .left
    cmp al, 0x4B
    jz .left

    cmp al, 0x20
    jz .right
    cmp al, 0x4D
    jz .right

    cmp al, 0x13
    jz restart
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
	test ax, ax
	jae .div
	add ax, 3
.div:
	sar ax, 2
	add ax, [bdx]
	neg word [bdy]
	ret
	
update:
	cmp word [ballx], 0
	jbe .neg_ballx
	cmp word [ballx], WIDTH - (BALL_RADIUS)
	jae .neg_ballx	

	cmp word [bally], 0
	jbe .neg_bally
	cmp word [bally], HEIGHT - (BALL_RADIUS)
	jae .game_over
	call col
	
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
	mov byte [state], 1
	xor ax, ax
	mov es, ax
	mov ah, 0x13
	mov bx, 0x0F
	mov cx, game_over_sign_len
	;(COLS, ROWS)
	mov dx, 0x0C10
	mov bp, game_over_sign
	int 0x10

.end:
	ret

col:
	;; optimized collision 
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
	jmp .hand
	
.hand:
	call bounce
.end:
	ret

draw:
	mov ax, [state]
	test ax, ax
	jz .d
	jmp .e
	
.d:
	pusha
	mov ax, OFFSET
	mov es, ax
	call clear_scr
	call update
	mov dl, 0x4F
	call draw_paddle
	
	mov dl, 0xC
	call draw_ball
	
	popa
.e:
	iret

restart:
	int 0x19
	hlt
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
	
