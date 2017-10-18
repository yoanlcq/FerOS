#include <ksections.h>
#include <string.h>
#include <multiboot.h>
#include <ega.h>
#include <vbe.h>
#include <log.h>
#include <cvt.h>
#include <gfx.h>
#include <elf.h>

// TODO: Add a high-precision timer to measure perf and wall clock
// TODO: Finish the implementation of fonts and improve blitting API
// TODO: Add the SIL open font license (maybe ??)
// TODO: Allow setting handlers from anywhere
// TODO: Be able to format hex (and any base really)
// TODO: Be able to format floats and pointers
// TODO: Have a proper, readable, exhaustive, memory map
// TODO: Implement memory allocation;
// TODO: Support mouse hot-plugging;
// TODO: Implement the basics of the rasterizer (needs vectors, matrices, and sin cos)
// TODO: Document our stuff and have a proper FAQ
// TODO: See how to switch freely between Text mode and any VBE mode
// TODO: Implement multi-threading;
// TODO: Have more control over our graphics device (and screen)
// TODO: Audio ?
// TODO: Keyboard
// TODO: Mouse
// TODO: Move on to x86_64 !

static void do_textmode_stuff(const MultibootInfo *mbi) {

    (void)mbi;

    EgaCursor cursor = {0};
    ega_clear();
    ega_set_cursor(cursor);

    ega_puts_logd(cursor, "Looks like we only got a text mode. Welp...", EgaWhite, EgaBlack);
    cursor.y += 2;
    ega_puts_logd(cursor, "Hello, kernel world! Have a rainbow.", EgaLightGreen, EgaBlack);
    cursor.y += 1;

    const EgaColor rainbow[] = { EgaRed, EgaBrown, EgaGreen, EgaCyan, EgaBlue, EgaMagenta };
    for(usize i=0 ; i<countof(rainbow) ; ++i) {
        ega_puts(cursor, " ", EgaWhite, rainbow[i]);
        cursor.x += 1;
    }

    cursor.x = 0;
    cursor.y += 1;

    ega_set_cursor(cursor);
}


static void render_frame(VbeRgbFb *fb, u32 frame_i) {

    (void)frame_i;

    const XbmMonoFont *f = &noto_mono;
    const char* txtlines[] = { "Hello!", "Goodbye!" };

    for(u32 y=0 ; y<fb->h ; ++y) for(u32 x=0 ; x<fb->w ; ++x) {
        let px8 = (u8*) fb->pixels;
        let idx = y*fb->pitch + x*(fb->bits_per_pixel/8);
        let color = (Rgba) {
            .r = y/(float)fb->h,
            .g = 1.0f,
            .b = x/(float)fb->w,
            .a = 1.0f,
        };
        px8[idx + fb->r.bits_offset/8] = color.r*0xff;
        px8[idx + fb->g.bits_offset/8] = color.g*0xff;
        px8[idx + fb->b.bits_offset/8] = color.b*0xff;
    }

    for(usize i=0 ; i<countof(txtlines) ; ++i) {

        const char *txt = txtlines[i];
        const usize txtlen = strlen(txt);
        Rgba txtpixels[f->char_w * f->h * txtlen];
        RgbaFb txtfb = {
            .w = txtlen * f->char_w,
            .h = f->h,
            .pixels = txtpixels,
            .depth = NULL
        };
        XbmMonoFont_rasterize(f, &txtfb, txt, txtlen, (Rgba){ .g = 1.0f, .a = 1.0f });
        
        VbeRgbFb_copy_RgbaFb(fb, (Vec2u){.y=i*f->h}, &txtfb, Vec2u_zero, txtfb.extent);
    }
}

static void do_rgbmode_stuff(const MultibootInfo *mbi) {
    let mbi_fb = &mbi->framebuffer;
    VbeRgbFb fb = {
        .pixels = (void*) (uptr) mbi_fb->addr,
        .pitch = mbi_fb->pitch,
        .w = mbi_fb->width,
        .h = mbi_fb->height,
        .bits_per_pixel = mbi_fb->bpp,
        .r = { .num_bits = mbi_fb->r_num_bits, .bits_offset = mbi_fb->r_bits_offset },
        .g = { .num_bits = mbi_fb->g_num_bits, .bits_offset = mbi_fb->g_bits_offset },
        .b = { .num_bits = mbi_fb->b_num_bits, .bits_offset = mbi_fb->b_bits_offset }
    };

    for(usize frame_i=0 ; ; ++frame_i) {
        render_frame(&fb, frame_i);
        hang_preserving_interrupts();
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
        abort();
    }
    if(!((mbi->flags & MB_INFO_AOUT_SYMS) || (mbi->flags & MB_INFO_ELF_SHDR))) {
        logd(
            "Bits 4 and 5 are not set! This is most likely because the "
            "claimed Multiboot header disagrees with the kernel image's "
            "format."
        );
        abort();
    }

    if(mbi->flags & MB_INFO_AOUT_SYMS) {
        let as = &(mbi->aout_sym);
        logd(
            "Multiboot A.OUT symbol table: "
            "tabsize = ", as->tabsize, ", "
            "strsize = ", as->strsize, ", "
            "addr = ", as->addr
        );
    }

    u32 max_addr = 0, max_addr_size = 0;
    (void)max_addr;
    (void)max_addr_size;

    if(mbi->flags & MB_INFO_ELF_SHDR) {
        let es = &(mbi->elf_sec);
        logd(
            "Multiboot ELF section header table: "
            "num = ", es->num, ", size = ", es->size, ", "
            "addr = ", es->addr, ", shndx = ", es->shndx
        );
        const Elf32_Shdr *shdr = (Elf32_Shdr*) es->addr;
        const Elf32_Shdr *shndx = &shdr[es->shndx];
        for(usize i=0 ; i<es->num ; ++i) {
            const Elf32_Shdr *cur = &shdr[i];
            logd(
                "> name = \"", &(((char*)shndx->addr)[cur->name]), "\""
                ", type = ", cur->type,
                ", addr = ", cur->addr,
                ", offset = ", cur->offset,
                ", size = ", cur->size
            );
            if(cur->addr > max_addr) {
                max_addr = cur->addr;
                max_addr_size = cur->size;
            }
        }
    }

    logd("&mbh_load_addr    = ", (uptr) &mbh_load_addr   );
    logd("&ks_phys_addr     = ", (uptr) &ks_phys_addr    );
    logd("&ks_text_start    = ", (uptr) &ks_text_start   );
    logd("&ks_text_end      = ", (uptr) &ks_text_end     );
    logd("&ks_rodata_start  = ", (uptr) &ks_rodata_start );
    logd("&ks_rodata_end    = ", (uptr) &ks_rodata_end   );
    logd("&ks_assets_start  = ", (uptr) &ks_assets_start );
    logd("&ks_assets_end    = ", (uptr) &ks_assets_end   );
    logd("&ks_data_start    = ", (uptr) &ks_data_start   );
    logd("&ks_data_end      = ", (uptr) &ks_data_end     );
    logd("&ks_bss_start     = ", (uptr) &ks_bss_start    );
    logd("&ks_stack_bottom  = ", (uptr) &ks_stack_bottom );
    logd("&ks_stack_top     = ", (uptr) &ks_stack_top    );
    logd("&ks_bss_end       = ", (uptr) &ks_bss_end      );
    logd("&ks_end           = ", (uptr) &ks_end          );
    logd("&mbh_bss_end_addr = ", (uptr) &mbh_bss_end_addr);

    logd("* Kernel image   : ", (uptr)&ks_phys_addr, " - ", (uptr) &ks_end);
    logd("* Multiboot info : ", (uptr)mbi, " - ", ((uptr)mbi) + sizeof *mbi);
    logd("* Bootloader name: ", (uptr)mbi->bootloader_name, " - ", ((uptr)mbi->bootloader_name) + strlen((char*)mbi->bootloader_name));
    logd("* Command line   : ", (uptr)mbi->cmdline, " - ", ((uptr)mbi->cmdline) + strlen((char*)mbi->cmdline));
    logd("* Mmap data      : ", (uptr)mbi->mmap.addr, " - ", ((uptr)mbi->mmap.addr)+mbi->mmap.length);
    logd("* VBE ctrl info  : ", (uptr)mbi->vbe.control_info, " - ", (uptr)mbi->vbe.control_info + sizeof (VbeInfoBlock));
    logd("* VBE mode info  : ", (uptr)mbi->vbe.mode_info, " - ", (uptr) mbi->vbe.mode_info + sizeof(VbeModeInfoBlock));
    logd("* EGA Text FB    : ", (uptr)EGA_FB, " - ", ((uptr)EGA_FB) + EGA_W*EGA_H*sizeof *EGA_FB);
    logd("* RGB framebuffer: ", (uptr)mbi->framebuffer.addr, " - ", ((uptr)mbi->framebuffer.addr) + mbi->framebuffer.height*mbi->framebuffer.pitch);


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

            if(mmap->type == MB_MEMORY_AVAILABLE && mmap->addr >= (uptr)&ks_phys_addr) {
                u8 *mem = (u8*) (uptr) mmap->addr;
                usize len = mmap->len;
                // Pay attention not to overwrite the physical memory at which
                // our kernel is loaded.
                // TODO: We should protect its memory, BUT not the stack.
                // TODO: We should also protect the stack's bounds so we know
                // when we get a stack overflow.
                // FIXME: This is NOT an accurate way of claiming memory on
                // real hardware!
                if(mem == (void*) &ks_phys_addr) {
                    mem = (void*) (max_addr + max_addr_size);
                    len -= ((uptr) mem) - ((uptr) &ks_phys_addr);
                }
                logd("Claiming ", len, " bytes at ", (uptr) mem, "...");
                for(usize i=0 ; i<len ; ++i) {
                    mem[i] = 0xde;
                }
                logd("Done");
            }

            mmap = (MultibootMmapEntry*) (((uptr)mmap) + mmap->size + sizeof mmap->size);
        }
    }

    if(mbi->flags & MB_INFO_BOOT_LOADER_NAME) {
        logd("The bootloader is \"", (const char*) (uptr) mbi->bootloader_name, "\"");
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
            {
                let fb = &mbi->framebuffer;
                logd(
                    "Entered RGB mode (", 
                    fb->width, "x", fb->height, "x", fb->bpp, 
                    "); "
                    "addr = ", fb->addr, ", " "pitch = ", fb->pitch, "."
                );
                logd("r_bits_offset = ", fb->r_bits_offset, ";");
                logd("r_num_bits    = ", fb->r_num_bits   , ";");
                logd("g_bits_offset = ", fb->g_bits_offset, ";");
                logd("g_num_bits    = ", fb->g_num_bits   , ";");
                logd("b_bits_offset = ", fb->b_bits_offset, ";");
                logd("b_num_bits    = ", fb->b_num_bits   , ";");
            }
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
