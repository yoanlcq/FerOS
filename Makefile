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
iso_kernel := isodir/boot/$(os_name).elf
kernel_bin := build/$(os_name).bin
kernel_elf := build/$(os_name).elf
kernel_sym := build/$(os_name).sym
kernel_dbg := build/$(os_name).dbg.elf

cfiles := $(call rglob,src,*.c)
sfiles := $(call rglob,src,*.s)
Sfiles := $(call rglob,src,*.S)
ofiles := $(patsubst src/%.c,build/%.c.o,$(cfiles))
ofiles += $(patsubst src/%.s,build/%.s.o,$(sfiles))
ofiles += $(patsubst src/%.S,build/%.S.o,$(Sfiles))

gdb := i686-elf-gdb
gcc := i686-elf-gcc
as  := i686-elf-as
ld  := i686-elf-ld
objcopy := i686-elf-objcopy
objdump := i686-elf-objdump -M intel
qemu-system := qemu-system-i386 $(strip \
	-enable-kvm \
	-name $(os_name) \
	-serial file:logs/serial.log \
	-serial stdio \
	-d guest_errors \
)
# ^ Redirect Guest COM1 to log file, and guest COM2 to stdio

gdbflags := $(strip \
	-q -tui \
	-ex "set disassembly-flavor intel" \
	-ex "file $(kernel_elf)" \
	-ex "symbol-file $(kernel_sym)" \
	-ex "target remote :1234" \
)
gccflags  := $(strip \
	-std=gnu11 -Wall -Wextra -Werror -Iinclude \
	-ffreestanding -O3 -nostdlib -g -ggdb \
	-fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions \
	-fno-stack-protector -static -fno-pic \
	-masm=intel \
	-mno-red-zone \
	-mfxsr -mmmx -msse -msse2 -mfpmath=sse \
	-Wl,-melf_i386 \
	-DIS_QEMU_GUEST \
	-DWANTS_VIDEOMODE \
	-DVIDEOMODE_WIDTH=320 \
	-DVIDEOMODE_HEIGHT=200 \
	-DVIDEOMODE_DEPTH=32 \
)
gcc_c_only_flags := -include __prelude.h
asflags := $(strip \
	-g --gstabs+ -L --fatal-warnings -Iinclude \
	-msyntax=intel -mmnemonic=intel -mnaked-reg \
)
wa_prefix := -Wa,
gcc_asflags := $(patsubst %,$(wa_prefix)%,$(asflags))
ldlibs  := -lgcc


.PHONY: all
all: $(iso) $(kernel_bin)


$(kernel_dbg): src/elf.ld $(ofiles)
	@mkdir -p $(@D)
	$(gcc) $(gccflags) -T $< -o $@ $(ofiles) $(ldlibs)
	$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump

$(kernel_sym): $(kernel_dbg)
	@mkdir -p $(@D)
	$(objcopy) --only-keep-debug $< $@

$(kernel_elf): $(kernel_dbg) $(kernel_sym)
	@mkdir -p $(@D)
	$(objcopy) --strip-all $< $@
	grub-file --is-x86-multiboot $@ # Check that our kernel is multiboot-compliant
	$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump

$(kernel_bin): src/bin.ld $(ofiles)
	@mkdir -p $(@D)
	$(gcc) $(gccflags) -T $< -o $@ $(ofiles) $(ldlibs)
	$(objdump) --disassemble-all --prefix-addresses -Mintel -bbinary -mi386 $@ > $@.dump

$(grubcfg): 
	@mkdir -p $(@D)
	echo "menuentry \"$(os_name)\" {" > $@
	echo "    multiboot /boot/$(notdir $(kernel_elf))" >> $@
	echo "}" >> $@

$(iso_kernel): $(kernel_elf)
	@mkdir -p $(@D)
	cp $< $@
# NOTE: We can't do `ln -sf` instead of `cp` because the file wouldn't get 
# picked up.

$(iso): $(iso_kernel) $(kernel_sym) $(grubcfg)
	@mkdir -p $(@D)
	grub-mkrescue /usr/lib/grub/i386-pc -o $@ isodir/


build/%.s.o: src/%.s
	@mkdir -p $(@D)
	$(as) $(asflags) $< -o $@
	$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump

build/%.S.o: src/%.S
	@mkdir -p $(@D)
	$(gcc) $(gccflags) $(gcc_asflags) -DASM_FILE=1 -c $< -o $@
	$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump

build/%.c.o: src/%.c
	@mkdir -p $(@D)
	$(gcc) $(gccflags) $(gcc_c_only_flags) -c $< -o $@
	$(gcc) $(gccflags) $(gcc_c_only_flags) -S $< -o $@.S
	$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump


.PHONY: clean re mrproper
clean:
	rm -rf build isodir
re: clean all
mrproper: clean all


.PHONY: run run-release dbg run-dbg run-bin
	
run-release: $(iso)
	@mkdir -p logs
	$(qemu-system) -cdrom $(iso)
run: $(iso)
	@mkdir -p logs
	$(qemu-system) -gdb tcp::1234 -cdrom $(iso)
dbg: all
	$(gdb) $(gdbflags)
run-dbg: $(iso)
	@mkdir -p logs
	$(qemu-system) -S -gdb tcp::1234 -cdrom $(iso) &
	$(gdb) $(gdbflags) -ex "continue"

run-bin: $(kernel_bin)
	@mkdir -p logs
	$(qemu-system) -gdb tcp::1234 -kernel $(kernel_bin)
