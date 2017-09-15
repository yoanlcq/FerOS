#!/bin/bash

PREFIX="/usr/local"
TARGET=i686-elf
JOBS=3

sudo apt install -y \
    gcc-6-source binutils-source gdb-source \
    flex libgmp3-dev libmpfr-dev libmpc-dev \
    libncurses-dev

cd /tmp
mkdir cross
cd cross
cp /usr/src/gcc-6/gcc-6.3.0.tar.xz .
cp /usr/src/binutils/binutils-2.28.tar.xz .
cp /usr/src/gdb.tar.bz2 .
tar xvJf gcc-6.3.0.tar.xz
tar xvJf binutils-2.28.tar.xz
tar xvjf gdb.tar.bz2
rm *.tar.*

 
mkdir -p build/binutils
pushd build/binutils
../../binutils-2.28/configure \
	--target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls \
	--disable-werror
make -j$JOBS
sudo make install
popd

which -- $TARGET-as || echo $TARGET-as is not in the PATH
 
mkdir -p build/gcc
pushd build/gcc
../../gcc-6.3.0/configure \
	--target=$TARGET --prefix="$PREFIX" --disable-nls \
	--enable-languages=c --without-headers
make -j$JOBS all-gcc
make -j$JOBS all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc
popd

mkdir -p build/gdb
pushd build/gdb
../../gdb/configure \
    --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-tui
# NOTE: No multiple jobs here, on purpose
make
sudo make install
popd
