rglob = $(wildcard \
	$(1)/$(2) \
	$(1)/*/$(2) \
	$(1)/*/*/$(2) \
	$(1)/*/*/*/$(2) \
	$(1)/*/*/*/*/$(2) \
)

BOOT     := boot.bin
BOOT_SYM := boot.sym
ASFLAGS    := -march=i686 -msyntax=intel -mmnemonic=intel -mnaked-reg
LDFLAGS    := -melf_i386 -static -nostdlib --nmagic
CFLAGS     := $(strip \
	-std=gnu11 -Wall -Wextra -Wshadow -pedantic -Iinclude -I. \
	-ffreestanding -nostdlib \
    -march=i686 -m16 -masm=intel \
	-fomit-frame-pointer -fno-asynchronous-unwind-tables -Os \
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
	ld $(LDFLAGS) -T$< $(OFILES) -o $@ $(LDLIBS)

%.s.o: %.s
	@mkdir -p $(@D)
	as $(ASFLAGS) $< -o $@

%.c.o: %.c
	@mkdir -p $(@D)
	gcc $(CFLAGS) -c $< -o $@


.PHONY: clean re mrproper
clean:
	rm -rf *.o
re: clean all
mrproper: clean all


.PHONY: run run-release debug disas
disas: all
	objdump -mi386 --target binary \
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
