## Links
- [...](https://www.reinterpretcast.com/creating-a-bare-bones-bootloader)
- [...](http://www.codeproject.com/Articles/664165/Writing-a-boot-loader-in-Assembly-and-C-Part)

## Bugs
#### Why does the OS hang when calling a C function ?
The actual cause was that the global `const char hello3[]` was placed in
the `rodata` section, which appeared after the boot signature.
The answer was that the `rodata` section, for some reason, needs to be
placed before the boot signature. Need to figure out why.


### linker.ld
This is the linker script : what is does is specify how our final
executable's data is to be layed out.  
More info: `info ld`.  
- `OUTPUT_FORMAT(binary)` : We only want raw data
- `ENTRY(_start)` : Set the `_start` label as our entry point (doesn't seem to mean much though)
- `kernel.s.o` because otherwise, other `.text` section contents coming from other files would be placed before the code at `_start`.
- `bootsig 510...` 
   This really only creates a fake section, named `bootsig`, which 
   sole purpose is to place the `0x55` and `0xAA` bytes at offset 
   `510` (`510+2 = 512`, mind-blowing). 
   Some BIOSes (in my case, SeaBIOS) require this "signature" to ensure 
   that the code can be trusted, something like that.
   See "Boot sector" on Wikipedia.
