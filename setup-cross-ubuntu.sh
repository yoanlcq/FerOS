#!/bin/bash

PREFIX="/usr/local"
TARGET=i686-elf
JOBS=3

sudo apt install -y gcc-6-source binutils-source flex libgmp3-dev libmpfr-dev libmpc-dev

cd /tmp
mkdir cross
cd cross
cp /usr/src/gcc-6/gcc-6.3.0.tar.xz .
cp /usr/src/binutils/binutils-2.28.tar.xz .
tar xvJf gcc-6.3.0.tar.xz
tar xvJf binutils-2.28.tar.xz
rm *.tar.xz

mkdir -p build/binutils build/gcc
 
pushd build/binutils
../../binutils-2.28/configure \
	--target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls \
	--disable-werror
make -j$JOBS
sudo make install
popd

which -- $TARGET-as || echo $TARGET-as is not in the PATH
 
pushd build/gcc
../../gcc-6.3.0/configure \
	--target=$TARGET --prefix="$PREFIX" --disable-nls \
	--enable-languages=c --without-headers
make -j$JOBS all-gcc
make -j$JOBS all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc
popd
