rglob = $(wildcard \
	$(1)/$(2) \
	$(1)/*/$(2) \
	$(1)/*/*/$(2) \
	$(1)/*/*/*/$(2) \
	$(1)/*/*/*/*/$(2) \
)

BOOT     := boot.bin
BOOT_SYM := boot.sym
ASFLAGS    := -msyntax=intel -mmnemonic=intel -mnaked-reg
CFLAGS     := $(strip \
	-std=c11 -Wall -pedantic -Iinclude \
	-ffreestanding -O3 -nostdlib \
	-masm=intel \
)
LDLIBS   := 
CFILES   := $(wildcard *.c)
SFILES   := $(wildcard *.s)
C_OFILES := $(patsubst %.c,%.c.o,$(CFILES))
S_OFILES := $(patsubst %.s,%.s.o,$(SFILES))
OFILES   := $(C_OFILES) $(S_OFILES)


.PHONY: all
all: $(BOOT)


$(BOOT): linker.ld $(OFILES)
	@mkdir -p $(@D)
	i686-elf-ld -static -T$< -nostdlib --nmagic $(OFILES) $(LDLIBS) -o $@

%.s.o: %.s
	@mkdir -p $(@D)
	i686-elf-as $(ASFLAGS) $< -o $@

%.c.o: %.c
	@mkdir -p $(@D)
	i686-elf-gcc -m16 $(CFLAGS) -c $< -o $@


.PHONY: clean re mrproper
clean:
	rm -rf *.o
re: clean all
mrproper: clean all


.PHONY: run run-release debug disas
disas: all
	i686-elf-objdump \
		--target binary -mi386 \
		-Mintel,intel-mnemonic,addr16,data16 \
		-D $(BOOT)
run-release: all
	qemu-system-i386 -drive format=raw,file=$(BOOT)
run: all
	qemu-system-i386 -s -drive format=raw,file=$(BOOT)
dbg: all
	gdb -q \
		-ex "set disassembly-flavor intel" \
		-ex "symbol-file $(BOOT_SYM)" \
		-ex "target remote localhost:1234"
