org 0x7C00
bits 16	

    mov [BootDrive], dl

    xor ax, ax
    mov ds, ax
    mov es, ax

    mov ah, 0x02        ; BIOS read sector
    mov al, 2           ; read 2 sectors
    mov ch, 0           ; cylinder 0
    mov cl, 2           ; starting at sector 2
    mov dh, 0           ; head 0
    mov dl, [BootDrive] ; original drive
    mov bx, 0x7E00      ; ES:BX → 0x0000:0x7E00
    int 0x13
    jc disk_error 

    jmp 0x0000:0x7E00

disk_error:
    mov si, DiskErrMsg
.print_disk_err:
    lodsb
    or al, al
    jz $
    mov ah, 0x0E
    int 0x10
    jmp .print_disk_err

BootDrive:      db 0
DiskErrMsg:     db 'Disk read error!',0

times 510-($-$$) db 0
dw 0xAA55
