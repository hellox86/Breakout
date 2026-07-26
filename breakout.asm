org 0x7C00
bits 16

%define OFFSET 0xA000
%define WIDTH 320
%define HEIGHT 200
%define PADDLE_WIDTH 20
%define PADDLE_HEIGHT 7
%define PADDLE_COLOR 0x4F
	
start: 
	;; video mode (320x200 VGA, 256 colors)
	mov ax, 0x13
	int 0x10
	mov ax, OFFSET
	mov es, ax

	call draw_paddle
	jmp game_loop
draw_paddle:
	push ax
	push bx
	mov ax, 180
	mov bx, 160
	sub bx, PADDLE_WIDTH/2
	mov cx, PADDLE_HEIGHT
	call rowy
	pop ax
	pop bx
	ret

rowx:
	;; ax - y, bx - x
	pusha
	mov dx, WIDTH
	mul dx
	add ax, bx
	mov di, ax
	
	mov al, PADDLE_COLOR
	mov cx, PADDLE_WIDTH
	rep stosb
	popa
	ret
rowy:
	call rowx
	inc ax
	loop rowy
game_loop:  
    hlt
    mov ah, 0x01
    int 0x16
    jz game_loop
    xor al, al
    int 0x16

    cmp al, 0x1B
    jz exit_mode
    cmp al, 	
    jmp game_loop
	
exit_mode:
	mov ax, 0x0003

	int 0x10	
	cli
	hlt

em_msg:	db "Hello, from VGA text mode"

times 510 - ($ - $$) db 0
;; Fills all free space from the current position to the 510th byte with zeros. for example my code file is 126b, then asm puts 510-126=384 zero bytes
dw 0xAA55
;; This magic number (signature) of the boot sector is a standard one, hardcoded into the BIOS of all x86-compatible computers:
;; The BIOS reads the first sector of the disk (512 bytes) into memory at address 0x7C00 and checks the last 2 bytes of that sector (0xAA55).
