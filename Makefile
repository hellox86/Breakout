all:
	nasm -f bin breakout.asm -o breakout.bin
	qemu-system-i386 -audiodev dsound,id=out -monitor stdio breakout.bin
