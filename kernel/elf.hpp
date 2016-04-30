#ifndef ELF_HPP
#define ELF_HPP

#include "string.hpp"
#include "scheduler.hpp"

namespace ELF {

class ProgramHeader;

class Header {
public:
    bool isValid() const {
        return String::compare(reinterpret_cast<const char*>(e_ident), "\177ELF", 4) == 0;
    }

    constexpr uint32_t entry() {
        return e_entry;
    }

    constexpr size_t programHeaders() {
        return e_phnum;
    }

    ProgramHeader* programHeaderTable() {
        return reinterpret_cast<ProgramHeader*>(reinterpret_cast<uint8_t*>(this) + e_phoff);
    }

private:
    char e_ident[16];     /* Magic number and other info */
    uint16_t e_type;      /* Object file type */
    uint16_t e_machine;   /* Architecture */
    uint32_t e_version;   /* Object file version */
    uint32_t e_entry;     /* Entry point virtual address */
    uint32_t e_phoff;     /* Program header table file offset */
    uint32_t e_shoff;     /* Section header table file offset */
    uint32_t e_flags;     /* Processor-specific flags */
    uint16_t e_ehsize;    /* ELF header size in bytes */
    uint16_t e_phentsize; /* Program header table entry size */
    uint16_t e_phnum;     /* Program header table entry count */
    uint16_t e_shentsize; /* Section header table entry size */
    uint16_t e_shnum;     /* Section header table entry count */
    uint16_t e_shstrndx;  /* Section header string table index */
};

class ProgramHeader {
public:
    constexpr bool isLoadable() {
        return p_type == 1;
    }

    constexpr uint32_t virtualAddress() {
        return p_vaddr;
    }

    constexpr uint32_t memorySize() {
        return p_memsz;
    }

    constexpr uint32_t fileSize() {
        return p_filesz;
    }

    uint8_t* data(Header* header) {
        return reinterpret_cast<uint8_t*>(reinterpret_cast<uint8_t*>(header) + p_offset);
    }

    constexpr Paging::Flags flags() {
        return p_flags & 0x2 ? Paging::Flags::WRITE : Paging::Flags::NONE;
    }

private:
    uint32_t p_type;   /* Segment type */
    uint32_t p_offset; /* Segment file offset */
    uint32_t p_vaddr;  /* Segment virtual address */
    uint32_t p_paddr;  /* Segment physical address */
    uint32_t p_filesz; /* Segment size in file */
    uint32_t p_memsz;  /* Segment size in memory */
    uint32_t p_flags;  /* Segment flags */
    uint32_t p_align;  /* Segment alignment */
};

}

#endif // ELF_HPP
