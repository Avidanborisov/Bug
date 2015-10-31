TEMPLATE = subdirs

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SUBDIRS = \
    bootloader \
    kernel

image.target   = Bug.img
image.depends  = bootloader/bootloader.bin kernel/kernel.bin
image.commands = $$PWD/scripts/build-image.sh

first.depends = image
QMAKE_EXTRA_TARGETS = image first

OTHER_FILES += \
    README.md \
    .gitignore \
    scripts/install-build-tools.sh \
    scripts/build-image.sh \
    scripts/run-qemu.sh \
    scripts/debug-qemu.sh \
    scripts/run-bochs.sh \
    scripts/debug-bochs.sh \
    scripts/create-usb.sh \
    scripts/bochsrc.txt
