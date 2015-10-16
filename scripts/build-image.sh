#!/bin/bash

# Assumes the current directory is the build directory (not the scripts directory)

# Create FAT12 floppy with boot sector
dd if=/dev/zero of=Bug.img status=none bs=512 count=2880
dd if=bootloader/bootloader.bin of=Bug.img status=none conv=notrunc

# Copy second stage
mcopy -i Bug.img kernel/kernel.bin ::

# Old way of copying second stage (mcopy is better because it doesn't require root)
# 
# sudo mount Bug.img /mnt -t msdos -o loop,fat=12
# sudo cp kernel/kernel.bin /mnt/
# sudo umount /mnt

# Generate an ISO too with floppy emulation
genisoimage -quiet -input-charset utf-8 -A Bug -o Bug.iso -b Bug.img Bug.img
