org 0x7C00
bits 16

%define WIDTH 320
%define HEIGHT 200
	
start: 
	;; video mode (320x200 VGA, 256 colors)
	mov ax, 0x13
	int 0x10
	mov ax, 0xA000
	mov es, ax
	;; one pixel
	;; mov di, 320*100+160
	;; mov byte [es:di], 0x0F
	call draw_paddle
	jmp .loop
draw_paddle:
	pusha
	mov ax, 100
	mov bx, 100
	call draw_rect
	popa
	ret
draw_rect:
	pusha
	mov dx, WIDTH
	mul dx ;; result to ax
	add ax, bx
	mov di, bx
	popa
	ret
.loop:  
    hlt
    mov ah, 0x01
    int 0x16
    jz .loop
    xor al, al
    int 0x16

    cmp al, 0x1B
    jz exit_mode
	
    jmp .loop
	
	
exit_mode:
	mov ax, 0x0003
	int 0x10	

times 510 - ($ - $$) db 0
;; Fills all free space from the current position to the 510th byte with zeros. for example my code file is 126b, then asm puts 510-126=384 zero bytes
dw 0xAA55
;; This magic number (signature) of the boot sector is a standard one, hardcoded into the BIOS of all x86-compatible computers:
;; The BIOS reads the first sector of the disk (512 bytes) into memory at address 0x7C00 and checks the last 2 bytes of that sector (0xAA55).
