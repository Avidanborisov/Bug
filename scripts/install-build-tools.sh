#!/bin/sh

# Install the cross compiler required to build the kernel and some additional tools to
# build the final OS image.

export GCC_VER="5.2.0"
export BINUTILS_VER="2.25"

export PREFIX="/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir build
cd build

sudo apt-get install libmpfr-dev libmpc-dev texinfo mtools genisoimage qtcreator qemu
wget ftp://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VER.tar.gz
wget ftp://ftp.gnu.org/gnu/gcc/gcc-$GCC_VER/gcc-$GCC_VER.tar.gz

tar -xvf binutils-$BINUTILS_VER.tar.gz
mkdir build-binutils
cd build-binutils
../binutils-$BINUTILS_VER/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
echo "MAKEINFO = :" >> Makefile
make
sudo make install
cd ..

tar -xvf gcc-$GCC_VER.tar.gz
mkdir build-gcc
cd build-gcc
../gcc-$GCC_VER/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
sudo make install-gcc 
sudo make install-target-libgcc
cd ..

cd ..
rm -rf build

# Update PATH
cat <<EOF >> ~/.profile

# cross compiler
if [ -d "$PREFIX/bin" ] ; then
    export PATH="$PREFIX/bin:\$PATH"
fi
EOF

echo "Re-login to apply PATH modification.."
