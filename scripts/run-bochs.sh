#!/bin/bash
cd "$(dirname "$0")" # go to the current directory (scripts directory)
bochsrc=`readlink -f bochsrc.txt`
cd ../../build-*/    # go to the build directory
bochs -qf $bochsrc