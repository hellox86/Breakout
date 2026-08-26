all:
	nasm pingpong.asm -o pingpong
	qemu-system-i386 -monitor stdio pingpong
