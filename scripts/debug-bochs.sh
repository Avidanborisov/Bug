#!/bin/bash
cd "$(dirname "$0")" # go to the current directory (scripts directory)
bochsrc=`readlink -f bochsrc.txt`
cd ../../build-*/    # go to the build directory
bochs -qf $bochsrc 'display_library: sdl, options="gui_debug"' 'magic_break: enabled=1'