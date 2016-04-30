TEMPLATE = subdirs

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SUBDIRS = \
    bootloader \
    kernel \
    stdlib \
    userlib \
    programs/shell \
    programs/snake

image.target   = Bug.img
image.depends  = bootloader/bootloader.bin kernel/kernel.bin
image.commands = $$PWD/scripts/build-image.sh

first.depends = image
QMAKE_EXTRA_TARGETS = image first

kernel.depends = stdlib \
    programs/shell \
    programs/snake

OTHER_FILES += \
    README.md \
    .gitignore \
    docs/specification.docx \
    scripts/install-build-tools.sh \
    scripts/build-image.sh \
    scripts/run-qemu.sh \
    scripts/debug-qemu.sh \
    scripts/run-bochs.sh \
    scripts/debug-bochs.sh \
    scripts/create-usb.sh \
    scripts/bochsrc.txt
