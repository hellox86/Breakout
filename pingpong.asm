org 0x7E00
bits 16
	
OFFSET EQU 0xA000
WIDTH EQU 320
HEIGHT EQU 200
PADDLE_WIDTH EQU 4
PADDLE_HEIGHT EQU 32
BALL_RADIUS EQU 4
COLUMNS EQU 40
ROWS EQU 25

start:
	mov ax, 0x13
	int 0x10	
	xor ax, ax
	mov es, ax
	mov ds, ax
	
	mov word [p1x], 10
	mov word [p1y], 103

	mov word [p2x], 310
	mov word [p2y], 103
	
	mov word [ballx], 160
	mov word [bally], 100
	mov word [bdx], 6
	mov word [bdy], 6
	mov dword [0x0070], draw
	
.loop:
    hlt
    in al, 0x60	
    cmp al, 0x13

    jz start
    cmp al, 0x48
    jz .up
    cmp al, 0x11
    jz .up

    cmp al, 0x50
    jz .down
    cmp al, 0x1F
    jz .down
    jmp .loop

.down:
	cmp word [p1y], HEIGHT-PADDLE_HEIGHT
	jge .false
	
	mov word [pdy], 10
	call upad
	jmp .loop
.up:
	cmp word [p1y], 4
	jle .false

	mov word [pdy], -10
	call upad	
	jmp .loop
.false:
	mov word [pdy], 0
	call upad
	jmp .loop

draw_p1:
	mov ax, [p1y]
	mov bx, [p1x]
	mov cx, PADDLE_WIDTH
	call verlt
	ret
draw_p2:
	mov ax, [p2y]
	mov bx, [p2x]
	mov cx, PADDLE_WIDTH
	call verlt
	ret

draw_ball:
	mov ax, [bally]
	mov bx, [ballx]
	mov cx, BALL_RADIUS
	mov dh, BALL_RADIUS
	call rowloop
	ret
draw_sep:
	xor ax, ax
	mov bx, 160
	mov cx, 50
	call verl2
	ret
upad:
	mov ax, [p1y]
	add ax, [pdy]
	mov [p1y], ax
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
verl2:
	call p
	add ax, 4
	loop verl2
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
	inc ax
	loop rowloop
	ret
	
bounce:
	
	ret
update:
	cmp word [ballx], BALL_RADIUS
	jle .goal
	cmp word [ballx], WIDTH - (BALL_RADIUS)
	jge .goal	

	cmp word [bally], 0
	jle .neg_bally
	cmp word [bally], HEIGHT - (BALL_RADIUS)
	jge .neg_bally
	call colp1	
	call colp2
	
.uball:
	mov ax, [ballx]
	add ax, [bdx]
	mov [ballx], ax
	xor ax, ax
	
	mov ax, [bally]
	add ax, [bdy]
	mov [bally], ax
	jmp .end
.neg_bally:
	neg word [bdy]
	jmp .uball
.goal:
	neg word [bdx]
	jmp .uball
.end:
	ret

colp1:
	mov ax, [p1x]
	add ax, PADDLE_WIDTH
	cmp word [ballx], ax
	jg .end
	mov ax, [ballx]
	add ax, BALL_RADIUS
	cmp ax, [p1x]
	jl .end

	mov ax, [p1y]
	add ax, PADDLE_HEIGHT
	cmp word [bally], ax
	jg .end

	mov ax, [bally]
	add ax, BALL_RADIUS
	cmp ax, [p1y]
	jl .end
	call bounce
.end:
	ret

colp2:
	mov ax, [p2x]
	add ax, PADDLE_WIDTH
	cmp word [ballx], ax
	jg .end
	mov ax, [ballx]
	add ax, BALL_RADIUS
	cmp ax, [p2x]
	jl .end

	mov ax, [p2y]
	add ax, PADDLE_HEIGHT
	cmp word [bally], ax
	jg .end

	mov ax, [bally]
	add ax, BALL_RADIUS
	cmp ax, [p2y]
	jl .end
	;; call bounce2
.end:
	ret

ai:
	;; https://github.com/kfigiela/asm-pong/blob/master/pong.asm thanks
	pusha
	cmp word[bdx], 0
	jl .e
	mov ax, [bally]            
	mov bx, [p2y]           
	sub ax, 16          
	sub ax, bx             
	mov bl, 10
	mov dx, ax
	sar dx, 5              
	sar ax, 3              
	sub ax, dx             
	add word [p2y], ax   
.e:
	popa
	ret
draw:
	pusha
	mov ax, OFFSET
	mov es, ax
	call clear_scr
	call update
	mov dl, 0x0F
	call ai
	call draw_p1
	call draw_p2
	call draw_ball
	call draw_sep
	popa
	iret
p1x   dw 0
p1y   dw 0
p2x   dw 0
p2y   dw 0
pdy    dw 0
p2dy dw 0	
ballx  dw 0
bally  dw 0 
bdx    dw 0
bdy    dw 0
lastkey db 0
game_over_sign: db "Game Over"
game_over_sign_len equ $ - game_over_sign

