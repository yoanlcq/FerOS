rglob = $(wildcard \
	$(1)/$(2) \
	$(1)/*/$(2) \
	$(1)/*/*/$(2) \
	$(1)/*/*/*/$(2) \
	$(1)/*/*/*/*/$(2) \
)

ISO        := FerOS.iso
GRUBCFG    := isodir/boot/grub/grub.cfg
KERNEL     := isodir/boot/FerOS.elf
KERNEL_SYM := build/FerOS.sym
KERNEL_DBG := build/FerOS.dbg.elf
CFLAGS     := $(strip \
	-std=c11 -Wall -pedantic -Iinclude \
	-ffreestanding -O0 -nostdlib -g \
	-masm=intel \
)
LDLIBS   := -lgcc
CFILES   := $(call rglob,src,*.c)
SFILES   := $(call rglob,src,*.s)
C_OFILES := $(patsubst src/%.c,build/%.c.o,$(CFILES))
S_OFILES := $(patsubst src/%.s,build/%.s.o,$(SFILES))
OFILES   := $(C_OFILES) $(S_OFILES)


.PHONY: all
all: $(ISO)


$(GRUBCFG): 
	@mkdir -p $(@D)
	echo "menuentry \"FerOS\" {" > $@
	echo "    multiboot $(subst isodir,,$(KERNEL))" >> $@
	echo "}" >> $@

$(ISO): $(KERNEL) $(KERNEL_SYM) $(GRUBCFG)
	@mkdir -p $(@D)
	grub-mkrescue /usr/lib/grub/i386-pc -o $@ isodir/

$(KERNEL): $(KERNEL_DBG) $(KERNEL_SYM)
	@mkdir -p $(@D)
	cp $< $@
	objcopy --strip-debug $@
$(KERNEL_DBG): src/arch/i686/linker.ld $(OFILES)
	@mkdir -p $(@D)
	i686-elf-gcc -T $< -o $@ $(CFLAGS) $(OFILES) $(LDLIBS)
$(KERNEL_SYM): $(KERNEL_DBG)
	@mkdir -p $(@D)
	objcopy --only-keep-debug $< $@

build/%.s.o: src/%.s
	@mkdir -p $(@D)
	i686-elf-as $< -o $@

build/%.c.o: src/%.c
	@mkdir -p $(@D)
	i686-elf-gcc $(CFLAGS) -c $< -o $@


.PHONY: clean re mrproper
clean:
	rm -rf build
re: clean all
mrproper: clean all


.PHONY: run run-release debug
run-release:
	qemu-system-i386 -cdrom $(ISO)
run:
	qemu-system-i386 -s -cdrom $(ISO)
dbg:
	gdb -q \
		-ex "set disassembly-flavor intel" \
		-ex "symbol-file $(KERNEL_SYM)" \
		-ex "target remote localhost:1234"
