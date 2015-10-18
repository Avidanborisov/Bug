asm(".code16gcc");

#include "e820.hpp"
#include "bios.hpp"

E820::Entry E820::entries[E820::MAX_ENTRIES];
uint16_t E820::Entry::count = 0;

void E820::detectMemory() {
    uint32_t continuationID = 0;

    do {
        auto& entry = entries[Entry::count];
        BIOS::RegPack args;

        args.eax = 0xe820;
        args.ebx = continuationID;
        args.ecx = sizeof entry;
        args.edx = SMAP_SIGNATURE;
        args.edi = (uint32_t)&entry;

        BIOS::RegPack output = BIOS::interrupt(0x15, args);
        if (output.eax != SMAP_SIGNATURE)
            return;

        if (output.ecx <= sizeof entry || entry.acpi & 1)
            ++Entry::count;

        continuationID = output.ebx;
    } while (continuationID != 0 && Entry::count < MAX_ENTRIES);
}
