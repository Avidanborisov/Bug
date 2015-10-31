#!/bin/bash

# NOTE: replace /dev/sdb with the correct USB /dev file
USB_BLOCK_FILE="/dev/sdb"

cd "$(dirname "$0")" # go to the current directory (scripts directory)
cd ../../build-*/    # go to the build directory
../Bug/scripts/build-image.sh

dd if=Bug.img of=$USB_BLOCK_FILE