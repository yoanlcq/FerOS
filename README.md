# FerOS
My toy OS, mostly following the tutorials on [wiki.osdev.org](wiki.osdev.org).  
  
I'll probably go back to it when I have the time. No expecting to make a feature-complete OS, but it'd be fun to turn it into a game, just like [this project](https://github.com/Overv/MineAssemble). I like the idea that the computer would be dedicated to run that single program, and it'd remind of old consoles. Good times. c:

## What does it run on ? What does it do ?
It is an i386 kernel (aka 32-bit x86).  
It does, um... This.  
![Boot](https://dl.dropboxusercontent.com/u/76675545/feros/hello.png)  
Exciting, isn't it ? c: Didn't go that far beyond VGA text mode.  
  
Boot time...    
![Boot](https://dl.dropboxusercontent.com/u/76675545/feros/boot.png) 

## Requirements
Development happens on Linux. Haven't given a thought about Windows, OS X and stuff.
- GNU Make
- i686-elf-gcc (Follow the Bare Bones tutorial on [osdev.org](osdev.org) to set up the GCC cross-compiler.)
- grub-mkrescue (`sudo apt-get install -y m4 xorriso grub-pc-bin`)

And probably some other stuff I left out, because it's been too long since last time.

`make` will build everything.  
To test, you might want to install `qemu-system-i386` (`sudo apt-get install -y m4 qemu-system-i386`)  
Then run it with : `qemu-system-i386 -cdrom FerOS.iso`.
