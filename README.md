# FerOS
My toy OS, mostly following the tutorials on [wiki.osdev.org](http://wiki.osdev.org).  
  
I'll probably go back to it when I have the time. No expecting to make a feature-complete OS, but it'd be fun to turn it into a game, just like [this project](https://github.com/Overv/MineAssemble). I like the idea that the computer would be dedicated to run that single program, and it'd remind of old consoles. Good times. c:

## What does it run on ? What does it do ?
It is an i686 kernel (aka 32-bit x86).  
It does, um... This.  
![Hello](http://yoanlecoq.com/dl/dev/feros/hello.png)  
Exciting, isn't it ? c: Didn't go that far beyond VGA text mode.  
  
Boot time...    
![Boot](http://yoanlecoq.com/dl/dev/feros/boot.png) 
  
Debugging with GDB (shortened into `make run` and `make dbg`).
![Debug](http://yoanlecoq.com/dl/dev/feros/dbg.png) 


## Requirements
Development happens on Linux. Haven't given a thought about Windows, OS X and stuff.
- GNU Make;
- i686-elf-gcc (`setup-cross-ubuntu.sh` can set it up, or [follow the Bare Bones tutorial on osdev.org](http://wiki.osdev.org/Bare_Bones));
- grub-mkrescue (`sudo apt-get install -y xorriso grub-pc-bin`);

And probably some other stuff I left out, because it's been too long since last time.

`make` will build everything.  
To test, you might want to install `qemu-system-i386` (`sudo apt-get install -y qemu`)  
Then run it with : `qemu-system-i386 -cdrom FerOS.iso` (`make run` happens to do this).
