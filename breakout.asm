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
    cmp al, 'd'
    jz .right
    jmp .loop

.left:
	pusha
	mov ax, OFFSET
	mov es, ax
	
	xor dl, dl
	call draw_paddle
	popa
	
	push ax
	mov ax, [game + Game.padx]
	sub ax, 10
	mov [game + Game.padx], ax
	pop ax
	
	jmp .loop
.right:
	pusha
	mov ax, OFFSET
	mov es, ax
	
	xor dl, dl
	call draw_paddle
	popa

	push ax
	mov ax, [game + Game.padx]
	add ax, 10
	mov [game + Game.padx], ax
	pop ax
	
	jmp .loop

draw_paddle:
	mov ax, [game + Game.pady]
	mov bx, [game + Game.padx]
	sub bx, PADDLE_WIDTH/2
	mov cx, PADDLE_HEIGHT
	call rowloop
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
	call draw_paddle
	popa
	iret

exit_mode:
	mov ax, 0x0003
	int 0x10	
	hlt

game:	
istruc Game
	at .padx, dw 160
	at .pady, dw 180
	at .ballx, dw 160
	at .bally, dw 170 
	at .dx, dw 0
	at .dy, dw 0

iend

times 510 - ($ - $$) db 0
;; Fills all free space from the current position to the 510th byte with zeros. for example my code file is 126b, then asm puts 510-126=384 zero bytes
dw 0xAA55
%if $ - $$ != 512
        %fatal Resulting size is not 512
%endif
	
;; This magic number (signature) of the boot sector is a standard one, hardcoded into the BIOS of all x86-compatible computers:
;; The BIOS reads the first sector of the disk (512 bytes) into memory at address 0x7C00 and checks the last 2 bytes of that sector (0xAA55).
