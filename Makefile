all:
	nasm -f bin breakout.asm -o breakout.bin
	qemu-system-i386 breakout.bin
