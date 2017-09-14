rglob = $(wildcard \
	$(1)/$(2) \
	$(1)/*/$(2) \
	$(1)/*/*/$(2) \
	$(1)/*/*/*/$(2) \
	$(1)/*/*/*/*/$(2) \
)

iso      := FerOS.iso
grubcfg  := isodir/boot/grub/grub.cfg
kern     := isodir/boot/FerOS.elf
kern_sym := build/FerOS.sym
kern_dbg := build/FerOS.dbg.elf
asflags  := -msyntax=intel -mmnemonic=intel -mnaked-reg
cflags   := $(strip \
	-std=c11 -Wall -pedantic -Iinclude \
	-ffreestanding -O0 -nostdlib -g \
	-masm=intel \
)
ldlibs   := -lgcc
cfiles   := $(call rglob,src,*.c)
sfiles   := $(call rglob,src,*.s)
ofiles   := $(patsubst src/%.c,build/%.c.o,$(cfiles))
ofiles   += $(patsubst src/%.s,build/%.s.o,$(sfiles))

gcc := i686-elf-gcc
as  := i686-elf-as

.PHONY: all
all: $(iso)


$(grubcfg): 
	@mkdir -p $(@D)
	echo "menuentry \"FerOS\" {" > $@
	echo "    multiboot $(subst isodir,,$(kern))" >> $@
	echo "}" >> $@

$(iso): $(kern) $(kern_sym) $(grubcfg)
	@mkdir -p $(@D)
	grub-mkrescue /usr/lib/grub/i386-pc -o $@ isodir/

$(kern): $(kern_dbg) $(kern_sym)
	@mkdir -p $(@D)
	cp $< $@
	objcopy --strip-debug $@
$(kern_dbg): src/arch/i686/kern.ld $(ofiles)
	@mkdir -p $(@D)
	$(gcc) -T $< -o $@ $(cflags) $(ofiles) $(ldlibs)
$(kern_sym): $(kern_dbg)
	@mkdir -p $(@D)
	objcopy --only-keep-debug $< $@

build/%.s.o: src/%.s
	@mkdir -p $(@D)
	$(as) $(asflags) $< -o $@

build/%.c.o: src/%.c
	@mkdir -p $(@D)
	$(gcc) $(cflags) -c $< -o $@


.PHONY: clean re mrproper
clean:
	rm -rf build
re: clean all
mrproper: clean all


.PHONY: run run-release dbg
run-release: all
	qemu-system-i386 -cdrom $(iso)
run: all
	qemu-system-i386 -s -cdrom $(iso)
# NOTE: We don't start QEMU in background in `dbg` because it doesn't work
# until we've entered the kernel.
# Instead, do:
# 	make run &
# Then wait, boot in QEMU, then
# 	make dbg
dbg: all
	gdb -q \
		-ex "set disassembly-flavor intel" \
		-ex "symbol-file $(kern_sym)" \
		-ex "target remote localhost:1234"
