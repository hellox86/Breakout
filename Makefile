# Makefile

NASM         := nasm
DD           := dd
SECTOR       := 512
FLOPPY_SECS  := 2880    

ASM_BOOT     := boot.asm
ASM_STAGE2   := pingpong.asm

BIN_BOOT     := boot.bin
BIN_STAGE2   := pingpong.bin
IMG          := pong.img

.PHONY: all clean run

all: $(IMG)


$(BIN_BOOT): $(ASM_BOOT)
	$(NASM) -f bin $< -o $@

$(BIN_STAGE2): $(ASM_STAGE2)
	$(NASM) -f bin $< -o $@

$(IMG): $(BIN_BOOT) $(BIN_STAGE2)
	$(DD) if=/dev/zero of=$@ bs=$(SECTOR) count=$(FLOPPY_SECS)
	$(DD) if=$(BIN_BOOT)  of=$@ bs=$(SECTOR) seek=0 conv=notrunc
	$(DD) if=$(BIN_STAGE2) of=$@ bs=$(SECTOR) seek=1 conv=notrunc
run: $(IMG)
	qemu-system-i386 -drive if=floppy,format=raw,file=$(IMG)

clean:
	-rm -f $(BIN_BOOT) $(BIN_STAGE2) $(IMG)
