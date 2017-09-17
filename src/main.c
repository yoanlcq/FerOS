#include <multiboot.h>
#include <vga.h>
#include <log.h>
#include <cvt.h>

// TODO: Improve `assert_cmp()` so that it's able to properly format operands and display them
// TODO: Log the multiboot info structure, especially the `vbe_` fields
// TODO: Use _rdtsc() from <x86intrin.h>
// TODO: Tell QEMU to use an ia32 CPU which supports SSE and SSE2 (-cpu, -cpu help)
// TODO: Move on to x86_64 !

static void setup_subsystems() {
    log_setup();
}
static void shutdown_subsystems() {
    log_shutdown();
}

static void do_textmode_stuff(const MultibootInfo *mbi) {

    (void)mbi;

    auto vgapos = Vec2_u8_new(0, 0);
    vga_clear();
    vga_set_cursor(vgapos);

    vga_puts_logd(vgapos, "Looks like we only got a text mode. Welp...", VgaWhite, VgaBlack);
    vgapos.y += 2;
    vga_puts_logd(vgapos, "Hello, kernel world! Have a rainbow.", VgaLightGreen, VgaBlack);
    vgapos.y += 1;

    const VgaColor rainbow[] = { VgaRed, VgaBrown, VgaGreen, VgaCyan, VgaBlue, VgaMagenta };
    for(usize i=0 ; i<countof(rainbow) ; ++i) {
        vga_puts(vgapos, " ", VgaWhite, rainbow[i]);
        vgapos.x += 1;
    }

    vgapos.x = 0;
    vgapos.y += 1;
    vga_set_cursor(vgapos);
}

static void do_rgbmode_stuff(const MultibootInfo *mbi) {
    // TODO Implement accelerated mouse cursor
    let fb = mbi->framebuffer;
    let w = fb.width;
    let h = fb.height;
    let bpp = fb.bpp;
    let fbmem = (u8*) (uptr) fb.addr;

    // NOTE: Well APPARENTLY we can't log `fb.addr` because it makes the
    // VM crash and I can't seem to diagnose it.
    // Is it because it's an u64 ?
    //
    // FIXME: So actually it looks like we're running out of space for
    // storing our code. Perhaps we need to pull more data from the drive ?
    // OR it's possible that our `.text` section got heavier than the 4K
    // claimed by `elf.ld`.
    logd(
        "Entered RGB mode (", w, "x", h, "x", bpp, "); "
        "addr = ", fb.addr, ", " "pitch = ", fb.pitch, "."
    );
    logd("r_bits_offset = ", fb.r_bits_offset, ";");
    logd("r_num_bits    = ", fb.r_num_bits   , ";");
    logd("g_bits_offset = ", fb.g_bits_offset, ";");
    logd("g_num_bits    = ", fb.g_num_bits   , ";");
    logd("b_bits_offset = ", fb.b_bits_offset, ";");
    logd("b_num_bits    = ", fb.b_num_bits   , ";");

    // u32 r_mask = (1<<fb.r_num_bits)-1;
    for(usize i=0 ; i<w*h ; ++i) {
        *(fbmem + (i*(bpp/8)+fb.g_bits_offset/8)) = 0xff;
        /*
        *(fbmem + (i*(bpp/8)+fb.r_bits_offset/8)) = 0xff;
        *(fbmem + (i*(bpp/8)+fb.b_bits_offset/8)) = 0xff;
        */
    }
}

void main(u32 mb_magic, MultibootInfo *mbi) {

    setup_subsystems();

    logd("Hello host!");

    if(mb_magic != MB_BOOTLOADER_MAGIC) {
        auto vgapos = Vec2_u8_new(0, 0);
        let msg = "Invalid multiboot bootloader magic value!";
        if(mbi->framebuffer.type == MB_FRAMEBUFFER_TYPE_EGA_TEXT) {
            vga_puts_logd(vgapos, msg, VgaLightRed, VgaBlack);
        } else {
            logd(msg);
        }
        return;
    }

    logd("Multiboot info flags = ", mbi->flags);

#if 0 // FIXME just sparing executable memory
// ^ Should look at examples linked by the multiboot spec ?

    if(mbi->flags & (1<<0)) {
        logd("Mem: lower = ", mbi->mem.lower, "KB"
                ", upper = ", mbi->mem.upper, "KB");
    }

    if(mbi->flags & (1<<1)) {
        logd("Boot device = ", mbi->boot_device);
    }

    if(mbi->flags & (1<<2)) {
        logd("cmdline = ", (char*) mbi->cmdline);
    }

    if(mbi->flags & (1<<3)) {
        logd("Mods: count = ", mbi->mods.count, ", addr = ", mbi->mods.addr);
        auto mod = (MultibootModList*) mbi->mods.addr;
        for(usize i = 0 ; i < mbi->mods.count ; i++, mod++) {
            logd(
                "Mod[", i, "]: start = ", mod->start, 
                ", end = ", mod->end, ", cmdline = ", (char*)mod->cmdline
            );
        }
    }

    if((mbi->flags & (1<<4)) && (mbi->flags & (1<<5))) {
        logd(
            "Both bits 4 and 5 are set, which is WRONG OMG. "
            "We can't be both AOUT and ELF! "
            "We are always expected to be ELF anyway."
        );
        // We could abort here
    }

    if(mbi->flags & (1<<5)) {
        let es = &(mbi->elf_sec);
        logd(
            "Multiboot ELF section header table: "
            "num = ", es->num, ", size = ", es->size, ", "
            "addr = ", es->addr, ", shndx = ", es->shndx
        );
    }

    /* Are mmap_* valid? */
    if(mbi->flags & (1<<6)) {
        logd("Mmap: addr = ", mbi->mmap.addr, ", length = ", mbi->mmap.length);

        auto mmap = (MultibootMmapEntry*) mbi->mmap.addr;
        for(;;) {
            if((uptr) mmap >= mbi->mmap.addr + mbi->mmap.length) {
                break;
            }
            logd(
                "size = ", mmap->size, ", "
                "base_addr = ", mmap->addr, ", "
                "length = ", mmap->len, ", "
                "type = ", mmap->type
            );
            mmap = (MultibootMmapEntry*) (((uptr)mmap) + mmap->size + sizeof mmap->size);
        }
    }
#endif
    if(mbi->flags & 1<<9) {
        logd("The bootloader is \"", (const char*) (uptr) mbi->boot_loader_name, "\"");
    }

    bool has_framebuffer = !!(mbi->flags & (1<<12));
    if(has_framebuffer) {
        switch(mbi->framebuffer.type) {
        case MB_FRAMEBUFFER_TYPE_INDEXED:
            logd("The framebuffer is in indexed format (we can't handle it yet!).");
            break;
        case MB_FRAMEBUFFER_TYPE_RGB:
            logd("The framebuffer is in RGB format.");
            do_rgbmode_stuff(mbi);
            break;
        case MB_FRAMEBUFFER_TYPE_EGA_TEXT:
            logd("We are in EGA text mode.");
            do_textmode_stuff(mbi);
            break;
        default:
            logd("Invalid framebuffer type : ", mbi->framebuffer.type);
            break;
        }
    } else {
        logd("We've haven't got a framebuffer!");
    }

    shutdown_subsystems();
}
