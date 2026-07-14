all:
	nasm breakout.asm -o breakout
	qemu-system-i386 breakout
