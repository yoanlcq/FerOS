rglob = $(wildcard \
	$(1)/$(2) \
	$(1)/*/$(2) \
	$(1)/*/*/$(2) \
	$(1)/*/*/*/$(2) \
	$(1)/*/*/*/*/$(2) \
)

KERNEL     := kernel.bin
KERNEL_SYM := kernel.sym
ASFLAGS    := -msyntax=intel -mmnemonic=intel -mnaked-reg
CFLAGS     := $(strip \
	-std=c11 -Wall -pedantic -Iinclude \
	-ffreestanding -O0 -nostdlib -g \
	-masm=intel \
)
LDLIBS   := 
CFILES   := $(wildcard *.c)
SFILES   := $(wildcard *.s)
C_OFILES := $(patsubst %.c,%.c.o,$(CFILES))
S_OFILES := $(patsubst %.s,%.s.o,$(SFILES))
OFILES   := $(C_OFILES) $(S_OFILES)


.PHONY: all
all: $(KERNEL)


$(KERNEL): linker.ld $(OFILES)
	@mkdir -p $(@D)
	i686-elf-ld -T $< -o $@ $(OFILES) $(LDLIBS)

%.s.o: %.s
	@mkdir -p $(@D)
	i686-elf-as $(ASFLAGS) $< -o $@

%.c.o: %.c
	@mkdir -p $(@D)
	i686-elf-gcc -m16 $(CFLAGS) -c $< -o $@


.PHONY: clean re mrproper
clean:
	rm -rf build
re: clean all
mrproper: clean all


.PHONY: run run-release debug
disas:
	i686-elf-objdump \
		--target binary -mi386 \
		-Mintel,intel-mnemonic,addr16,data16 \
		-D $(KERNEL)
run-release: all
	qemu-system-i386 -drive format=raw,file=$(KERNEL)
run: all
	qemu-system-i386 -s -drive format=raw,file=$(KERNEL)
dbg: all
	gdb -q \
		-ex "set disassembly-flavor intel" \
		-ex "symbol-file $(KERNEL_SYM)" \
		-ex "target remote localhost:1234"
