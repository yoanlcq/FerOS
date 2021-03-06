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
	-m 1G \
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
	-std=gnu11 -Wall -Wextra -Werror -Wshadow -Iinclude \
	-ffreestanding -O3 -nostdlib -g -ggdb \
	-fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions \
	-fno-stack-protector -static -fno-pic \
	-fms-extensions \
	-masm=intel \
	-mno-red-zone \
	-mfxsr -mmmx -msse -msse2 -mfpmath=sse \
	-Wl,-melf_i386 -Wl,--fatal-warnings \
	-DIS_QEMU_GUEST \
)
gcc_c_only_flags := -include __prelude.h
asflags := $(strip \
	-g --gstabs+ -L --fatal-warnings -Iinclude \
	-msyntax=intel -mmnemonic=intel -mnaked-reg \
)
# Store it into a variable to "escape" the comma at the end for use in patsubst
wa_prefix := -Wa,
gcc_asflags := $(patsubst %,$(wa_prefix)%,$(asflags))
ldlibs  := -lgcc


.PHONY: all
all: $(iso)


$(kernel_dbg): src/elf.ld $(ofiles)
	@mkdir -p $(@D)
	@echo Link $@ using $<
	@$(gcc) $(gccflags) -T $< -o $@ $(ofiles) $(ldlibs)
#$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump

$(kernel_sym): $(kernel_dbg)
	@mkdir -p $(@D)
	@echo Extract debug symbols from $< into $@
	@$(objcopy) --only-keep-debug $< $@

$(kernel_elf): $(kernel_dbg) $(kernel_sym)
	@mkdir -p $(@D)
	@echo Strip $(kernel_dbg) into $@
	@$(objcopy) --strip-all $< $@
	@echo Assert that $@ is multiboot-compliant
	@grub-file --is-x86-multiboot $@ 
	@du -sh $@
#$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump

$(grubcfg): 
	@mkdir -p $(@D)
	@echo Generate $@
	@echo "menuentry \"$(os_name)\" {" > $@
	@echo "    multiboot /boot/$(notdir $(kernel_elf))" >> $@
	@echo "}" >> $@

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
	@echo Assemble $^ "(skipping pre-processing)"
	@$(as) $(asflags) $< -o $@
#$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump

build/%.S.o: src/%.S
	@mkdir -p $(@D)
	@echo Assemble $^
	@$(gcc) $(gccflags) $(gcc_asflags) -c $< -o $@
#$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump

build/%.c.o: src/%.c
	@mkdir -p $(@D)
	@echo Compile $^
	@$(gcc) $(gccflags) $(gcc_c_only_flags) -c $< -o $@
#$(gcc) $(gccflags) $(gcc_c_only_flags) -S $< -o $@.S
#$(objdump) --disassemble-all --prefix-addresses $@ > $@.dump


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
