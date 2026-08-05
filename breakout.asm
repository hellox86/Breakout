org 0x7C00
bits 16

%define OFFSET 0xA000
%define WIDTH 320
%define HEIGHT 200
%define PADDLE_WIDTH 20
%define PADDLE_HEIGHT 7
	
struc Game
	.padx resw 1
	.pady resw 1
	.ballx resw 1
	.bally resw 1
	.dx resw 1
	.dy resw 1	
endstruc
	
start: 
	;; video mode (320x200 VGA, 256 colors)		
	mov ax, 0x13
	int 0x10	
	xor ax, ax
	mov es, ax
	mov ds, ax

	mov si, igame
	mov di, game
	mov cx, Game_size
	cld
	rep movsb
	mov dword [0x0070], render
	
.loop:
    mov ah, 0x01
    int 0x16

    jz .loop
    
    xor ah, ah
    int 0x16	
    
    cmp al, 0x1B
    jz exit_mode 

    cmp al, 'a'
    jz .left
    cmp ah, 0x4B
    jz .left

    cmp al, 'd'
    jz .right
    cmp ah, 0x4D
    jz .right
    cmp al, 'r'
    jz start
    jmp .loop


.left:
	sub word [game + Game.padx], 10
	jmp .loop
	
.right:
	add word [game + Game.padx], 10
	jmp .loop

draw_paddle:
	mov ax, [game + Game.pady]
	mov bx, [game + Game.padx]
	sub bx, PADDLE_WIDTH/2
	mov cx, PADDLE_HEIGHT
	call rowloop
	ret

clearbg:
	pusha
	mov ax, OFFSET
	mov es, ax
	mov di, ax
	mov cx, WIDTH*HEIGHT
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
	
render:
	pusha
	mov ax, OFFSET
	mov es, ax
	mov dl, 0x4F
	call clearbg
	call draw_paddle
	popa
	iret

exit_mode:
	mov ax, 0x0003
	int 0x10	
	hlt
igame:	
istruc Game
	at .padx, dw 160
	at .pady, dw 180
	at .ballx, dw 160
	at .bally, dw 170 
	at .dx, dw 0
	at .dy, dw 0

iend
game:	
	times 32 db 0
	times 510 - ($ - $$) db 0
	dw 0xAA55

%if $ - $$ != 512
        %fatal Resulting size is not 512
%endif
	
