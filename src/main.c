#include <multiboot.h>
#include <vga.h>
#include <log.h>
#include <cvt.h>

// TODO: Document our stuff and have a proper FAQ
// TODO: See how to switch freely between Text mode and any VBE mode
// TODO: Set up the IDT
// TODO: Be able to format hex (and any base really)
// TODO: Implement malloc();
// TODO: Implement multi-threading;
// TODO: Have more control over our graphics device (and screen)
// TODO: Audio ?
// TODO: Keyboard
// TODO: Mouse
// TODO: Move on to x86_64 !

static void do_textmode_stuff(const MultibootInfo *mbi) {

    (void)mbi;

    VgaCursor cursor = {0};
    vga_clear();
    vga_set_cursor(cursor);

    vga_puts_logd(cursor, "Looks like we only got a text mode. Welp...", VgaWhite, VgaBlack);
    cursor.y += 2;
    vga_puts_logd(cursor, "Hello, kernel world! Have a rainbow.", VgaLightGreen, VgaBlack);
    cursor.y += 1;

    const VgaColor rainbow[] = { VgaRed, VgaBrown, VgaGreen, VgaCyan, VgaBlue, VgaMagenta };
    for(usize i=0 ; i<countof(rainbow) ; ++i) {
        vga_puts(cursor, " ", VgaWhite, rainbow[i]);
        cursor.x += 1;
    }

    cursor.x = 0;
    cursor.y += 1;

    vga_set_cursor(cursor);
}

static void do_rgbmode_stuff(const MultibootInfo *mbi) {
    let fb = &mbi->framebuffer;
    let w = fb->width;
    let h = fb->height;
    let bpp = fb->bpp;
    let fbmem = (u8*) (uptr) fb->addr;

    logd(
        "Entered RGB mode (", w, "x", h, "x", bpp, "); "
        "addr = ", fb->addr, ", " "pitch = ", fb->pitch, "."
    );
    logd("r_bits_offset = ", fb->r_bits_offset, ";");
    logd("r_num_bits    = ", fb->r_num_bits   , ";");
    logd("g_bits_offset = ", fb->g_bits_offset, ";");
    logd("g_num_bits    = ", fb->g_num_bits   , ";");
    logd("b_bits_offset = ", fb->b_bits_offset, ";");
    logd("b_num_bits    = ", fb->b_num_bits   , ";");

    // TODO Possible bpps to handle: 8 15 16 24 32
    // Today it's 32.

    for(usize frame_i=0 ; ; ++frame_i) {
        usize off = frame_i*4;
        let t_start = _rdtsc();
        for(usize y=0 ; y<h ; ++y) {
            for(usize x=0 ; x<w ; ++x) {
                fbmem[y*fb->pitch + x*(bpp/8) + fb->r_bits_offset/8] = 0xff*(((x+off)%w)/(float)w);
                fbmem[y*fb->pitch + x*(bpp/8) + fb->g_bits_offset/8] = 0xff*(((y+off)%h)/(float)h);
                fbmem[y*fb->pitch + x*(bpp/8) + fb->b_bits_offset/8] = 0xff/**((x+y)/(float)(w+h))*/;
            }
        }
        let t_end = _rdtsc();
        logd("Frame ", frame_i, " filled within ", (t_end - t_start), " cycles.");
    }
}

void main(const MultibootInfo *mbi) {
    logd("Multiboot info flags = ", mbi->flags);

    if(mbi->flags & MB_INFO_MEMORY) {
        logd("Mem: lower = ", mbi->mem.lower, "KB"
                ", upper = ", mbi->mem.upper, "KB");
    }

    if(mbi->flags & MB_INFO_BOOTDEV) {
        logd("Boot device = ", mbi->boot_device);
    }

    if(mbi->flags & MB_INFO_CMDLINE) {
        logd("cmdline = ", (char*) mbi->cmdline);
    }

    if(mbi->flags & MB_INFO_MODS) {
        logd("Mods: count = ", mbi->mods.count, ", addr = ", mbi->mods.addr);
        auto mod = (MultibootModList*) mbi->mods.addr;
        for(usize i = 0 ; i < mbi->mods.count ; i++, mod++) {
            logd(
                "Mod[", i, "]: start = ", mod->start, 
                ", end = ", mod->end, ", cmdline = ", (char*)mod->cmdline
            );
        }
    }

    if((mbi->flags & MB_INFO_AOUT_SYMS) && (mbi->flags & MB_INFO_ELF_SHDR)) {
        logd(
            "Both bits 4 and 5 are set, which is WRONG OMG. "
            "We can't be both AOUT and ELF! "
            "We are always expected to be an ELF image anyway."
        );
        // We could abort here
    }

    if(mbi->flags & MB_INFO_ELF_SHDR) {
        let es = &(mbi->elf_sec);
        logd(
            "Multiboot ELF section header table: "
            "num = ", es->num, ", size = ", es->size, ", "
            "addr = ", es->addr, ", shndx = ", es->shndx
        );
    }

    if(mbi->flags & MB_INFO_MEM_MAP) {
        logd("Mmap: addr = ", mbi->mmap.addr, ", length = ", mbi->mmap.length);

        auto mmap = (MultibootMmapEntry*) mbi->mmap.addr;
        for(;;) {
            if((uptr) mmap >= mbi->mmap.addr + mbi->mmap.length) {
                break;
            }
            logd_(
                "size = ", mmap->size, ", "
                "base_addr = ", mmap->addr, ", "
                "length = ", mmap->len, ", "
                "type = "
            );
            switch(mmap->type) {
            case MB_MEMORY_AVAILABLE       : logd("Available"); break;
            case MB_MEMORY_RESERVED        : logd("Reserved"); break;
            case MB_MEMORY_ACPI_RECLAIMABLE: logd("ACPI reclaimable"); break;
            case MB_MEMORY_NVS             : logd("NVS"); break;
            case MB_MEMORY_BADRAM          : logd("Bad RAM"); break;
            }
            mmap = (MultibootMmapEntry*) (((uptr)mmap) + mmap->size + sizeof mmap->size);
        }
    }
    if(mbi->flags & MB_INFO_BOOT_LOADER_NAME) {
        logd("The bootloader is \"", (const char*) (uptr) mbi->boot_loader_name, "\"");
    }

    if(mbi->flags & MB_INFO_VBE_INFO) {
        logd("VBE mode info:");
        logd("- control_info  = ", mbi->vbe.control_info );
        logd("- mode_info     = ", mbi->vbe.mode_info    );
        logd("- mode          = ", mbi->vbe.mode         );
        logd("- interface.seg = ", mbi->vbe.interface.seg);
        logd("- interface.off = ", mbi->vbe.interface.off);
        logd("- interface.len = ", mbi->vbe.interface.len);
    }

    if(mbi->flags & MB_INFO_FRAMEBUFFER_INFO) {
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
}
