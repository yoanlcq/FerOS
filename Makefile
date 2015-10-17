all: FerOS.iso

FerOS.iso: isodir/boot/FerOS.bin isodir/boot/grub/grub.cfg
	grub-mkrescue -o FerOS.iso isodir/
isodir/boot/FerOS.bin: build/boot.o build/kernel.o build/vga.o build/kio_vga.o build/kio.o build/utils.o src/linker.ld
	i686-elf-gcc -Iinclude -T src/linker.ld -o isodir/boot/FerOS.bin \
		-ffreestanding -fbuiltin -O0 -nostdlib \
		build/boot.o \
		build/kernel.o \
		build/utils.o \
		build/vga.o \
		build/kio_vga.o \
		build/kio.o \
		-lgcc
	#^ Including the kio.o causes major trouble.

build/boot.o: src/boot.s
	i686-elf-as src/boot.s -o build/boot.o
build/kernel.o: src/kernel.c
	i686-elf-gcc -Iinclude -c src/kernel.c -o build/kernel.o \
		-ffreestanding -fbuiltin -O0 
build/vga.o: src/vga.c
	i686-elf-gcc -Iinclude -c src/vga.c -o build/vga.o \
		-ffreestanding -fbuiltin -O0 
build/kio_vga.o: src/kio_vga.c
	i686-elf-gcc -Iinclude -c src/kio_vga.c -o build/kio_vga.o \
		-ffreestanding -fbuiltin -O0 
build/kio.o: src/kio.c
	i686-elf-gcc -Iinclude -c src/kio.c -o build/kio.o \
		-ffreestanding -fbuiltin -O0 
build/utils.o: src/utils.c
	i686-elf-gcc -Iinclude -c src/utils.c -o build/utils.o \
		-ffreestanding -fbuiltin -O0 


clean:
	rm -f FerOS.iso build/* isodir/boot/FerOS.bin
re: clean all
mrproper: clean all

.PHONY: clean re mrproper
