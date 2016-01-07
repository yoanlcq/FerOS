all: FerOS.iso
CCFLAGS = -Wall -Iinclude -ffreestanding -O0 -nostdlib

FerOS.iso: build/ isodir/boot/FerOS.kernel isodir/boot/grub/grub.cfg
	grub-mkrescue /usr/lib/grub/i386-pc -o FerOS.iso isodir/
build/:
	mkdir build

isodir/boot/FerOS.kernel: build/boot.o build/kernel.o src/arch/i686/linker.ld
	i686-elf-gcc -T src/arch/i686/linker.ld -o isodir/boot/FerOS.kernel \
		$(CCFLAGS) \
		build/boot.o \
		build/kernel.o \
		-lgcc

build/boot.o: src/arch/i686/boot.s
	i686-elf-as src/arch/i686/boot.s -o build/boot.o
build/kernel.o: src/kernel/kernel.c
	i686-elf-gcc $(CCFLAGS) -c src/kernel/kernel.c -o build/kernel.o 

#build/vga.o: src/arch/i686/vga.c
#	i686-elf-gcc $(CCFLAGS) -c src/vga.c -o build/vga.o
#build/kio_vga.o: src/arch/i686/kio_vga.c
#	i686-elf-gcc $(CCFLAGS) -c src/kio_vga.c -o build/kio_vga.o
#build/kio.o: src/kio.c
#	i686-elf-gcc $(CCFLAGS) -c src/kio.c -o build/kio.o
#build/utils.o: src/utils.c
#	i686-elf-gcc $(CCFLAGS) -c src/utils.c -o build/utils.o


clean:
	rm -f FerOS.iso build/* isodir/boot/FerOS.kernel
re: clean all
mrproper: clean all

.PHONY: clean re mrproper
