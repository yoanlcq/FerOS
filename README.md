# FerOS
My toy operating system. Though the most accurate description right now would be "ring 0 application".  

The plan is rather to make something like a video game or graphics demo implemented on very low level.  
It's mostly just for fun and out of technical interest - it taught me a lot about x86 (the architecture and the assembly), and how to debug some very obscure and nasty problems.

It's also somehow written in its "own C dialect", thanks to the assumption I'll always use a modern GCC.  
There's type inference with `let` and `auto`, there are generics thanks to C11, a super-powerful `assert` (`assert_cmp`), etc.  
See `include/__prelude.h` and various calls to `logd()` for how it looks like.

## Screens
Pretty graphics!  
![Pretty graphics](http://yoanlecoq.com/dl/dev/feros/2017_gfx.png)  

Boot time.  
![Boot](http://yoanlecoq.com/dl/dev/feros/boot.png) 
  
Debugging with GDB (shortened into `make run` and `make dbg`).  
![Debug](http://yoanlecoq.com/dl/dev/feros/dbg.png) 


## What does/can it do ?
- Write color data to a VESA BIOS Extensions (VESA)-provided framebuffer.
- Rasterize an embedded XBM font.
- Log messages through a pre-defined "debug" serial port.
- Catch hardware exceptions and enable SSE and SSE2.

It doesn't enable paging for now. All addresses are physical.  

I had tested it on my laptop for real at some point and I'm unsure if it still
works. I might take a photo if I get it to work again.

## What's planned ?
The way is open for a lot of fun stuff !
- Dynamic memory allocation : we know about a large chunk of available memory but haven't used it yet.
- Basic software rasterization of triangle meshes.
- Better support for keyboard and mouse (the rough basics are there);

And in general a lot of stuff that is listed in `src/main.c`.

## What does it run on ?
It is an IA-32 kernel (that is, the 32-bit legacy mode of modern PCs, which are on x86-64).  
The kernel image is Multiboot-compliant and relies on GRUB for booting right now.  

I'd like to move to x86-64 in the future because it's standard today. Eventually, moving to UEFI would be cool too, I guess.

## Requirements
Development happens on Ubuntu. Haven't given a thought about Windows, OS X and stuff.
- GNU Make;
- `i686-elf-gcc` (`setup-cross-ubuntu.sh` can set it up, or [follow the Bare Bones tutorial on osdev.org](http://wiki.osdev.org/Bare_Bones));
- `grub-mkrescue` (`sudo apt-get install -y xorriso grub-pc-bin`);

And probably some other stuff I left out, because it's been too long since last time.

`make` will build everything.  
To test, you might want to install `qemu-system-i386` (`sudo apt-get install -y qemu`)  
Then run it with : `qemu-system-i386 -cdrom FerOS.iso` (`make run` happens to do this).
