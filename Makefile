rglob = $(wildcard \
	$(1)/$(2) \
	$(1)/*/$(2) \
	$(1)/*/*/$(2) \
	$(1)/*/*/*/$(2) \
	$(1)/*/*/*/*/$(2) \
)

os_name    := FerOS

iso        := $(os_name).iso
grubcfg    := isodir/boot/grub/grub.cfg
kernel     := isodir/boot/$(os_name).elf
kernel_sym := build/$(os_name).sym
kernel_dbg := build/$(os_name).dbg.elf

cfiles := $(call rglob,src,*.c)
sfiles := $(call rglob,src,*.s)
ofiles := $(patsubst src/%.c,build/%.c.o,$(cfiles))
ofiles += $(patsubst src/%.s,build/%.s.o,$(sfiles))

gdb := i686-elf-gdb
gcc := i686-elf-gcc
as  := i686-elf-as
ld  := i686-elf-ld
objcopy := i686-elf-objcopy
qemu-system := qemu-system-i386

gdbflags := $(strip \
	-q -tui \
	-ex "set disassembly-flavor intel" \
	-ex "file $(kernel)" \
	-ex "symbol-file $(kernel_sym)" \
	-ex "target remote :1234" \
)
gccflags  := $(strip \
	-std=c11 -Wall -Wextra -Werror -Iinclude \
	-ffreestanding -O0 -nostdlib -nostdinc -g -ggdb \
	-fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions \
	-fno-stack-protector -static -fno-pic \
	-masm=intel \
)
asflags := $(strip \
	-g --gstabs+ -L --fatal-warnings \
	-msyntax=intel -mmnemonic=intel -mnaked-reg \
)
gcc_asflags := $(patsubst %,-Wa\,%,$(asflags))
ldlibs  := -lgcc


.PHONY: all
all: $(iso)


$(kernel_dbg): src/elf.ld $(ofiles)
	@mkdir -p $(@D)
	$(gcc) $(gccflags) -T $< -o $@ $(ofiles) $(ldlibs)

$(kernel_sym): $(kernel_dbg)
	@mkdir -p $(@D)
	$(objcopy) --only-keep-debug $< $@

$(kernel): $(kernel_dbg) $(kernel_sym)
	@mkdir -p $(@D)
	$(objcopy) --strip-all $< $@
	grub-file --is-x86-multiboot $@ # Check that our kernel is multiboot-compliant

$(grubcfg): 
	@mkdir -p $(@D)
	echo "menuentry \"$(os_name)\" {" > $@
	echo "    multiboot $(subst isodir,,$(kernel))" >> $@
	echo "}" >> $@

$(iso): $(kernel) $(kernel_sym) $(grubcfg)
	@mkdir -p $(@D)
	grub-mkrescue /usr/lib/grub/i386-pc -o $@ isodir/


build/%.s.o: src/%.s
	@mkdir -p $(@D)
	$(as) $(asflags) $< -o $@

build/%.c.o: src/%.c
	@mkdir -p $(@D)
	$(gcc) $(gccflags) -c $< -o $@


.PHONY: clean re mrproper
clean:
	rm -rf build
re: clean all
mrproper: clean all


.PHONY: run run-release run-suspended
run-release: all
	$(qemu-system) -cdrom $(iso)
run: all
	$(qemu-system) -gdb tcp::1234 -cdrom $(iso)
dbg: all
	$(gdb) $(gdbflags)
run-dbg: all
	$(qemu-system) -S -gdb tcp::1234 -cdrom $(iso) &
	$(gdb) $(gdbflags) -ex "continue"

