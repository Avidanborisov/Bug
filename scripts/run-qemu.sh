#!/bin/bash
cd "$(dirname "$0")" # go to the current directory (scripts directory)
cd ../../build-*/    # go to the build directory
qemu-system-i386 -cdrom Bug.iso -monitor stdio