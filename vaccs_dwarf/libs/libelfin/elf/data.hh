// Copyright (c) 2013 Austin T. Clements. All rights reserved.
// Use of this source code is governed by an MIT license
// that can be found in the LICENSE file.

#ifndef _ELFPP_DATA_HH_
#define _ELFPP_DATA_HH_

#include "common.hh"

#include <cstring>
#include <string>

ELFPP_BEGIN_NAMESPACE

// Object file classes (ELF64 table 3)
typedef unsigned char elfclass;
namespace elfclass_ns {
        typedef unsigned char elfclass;
        const elfclass _32 = 1;                // 32-bit objects
        const elfclass _64 = 2;                // 64-bit objects
};

std::string
to_string(elfclass v);

// Common basic data types
struct ElfTypes
{
        typedef uint16_t Half;
        typedef uint32_t Word;
        typedef int32_t  Sword;
};

struct Elf32 : public ElfTypes
{
        // ELF class
        static const elfclass cls = elfclass_ns::_32;

        // Basic data types (ELF32 figure 1-2)
        typedef uint32_t Addr;
        typedef uint32_t Off;

        // Predicated types
        typedef Word Word32_Xword64;

        template<typename t32, typename t64>
        struct pick
        {
                typedef t32 t;
        };
};

struct Elf64 : ElfTypes
{
        // ELF class
        static const elfclass cls = elfclass_ns::_64;

        // Basic data types (ELF64 table 1)
        typedef uint64_t Addr;
        typedef uint64_t Off;
        typedef uint64_t Xword;
        typedef int64_t  Sxword;

        // Predicated types
        typedef Xword Word32_Xword64;

        template<typename t32, typename t64>
        struct pick
        {
                typedef t64 t;
        };
};

// Data encodings (ELF64 table 4)
typedef unsigned char elfdata;
namespace elfdata_ns {
        typedef unsigned char elfdata;
        const elfdata lsb = 1;
        const elfdata msb = 2;
};

std::string
to_string(elfdata v);

// Operating system and ABI identifiers (ELF64 table 5)
typedef unsigned char elfosabi;
namespace elfosabi_ns {
        typedef unsigned char elfosabi;
        const elfosabi sysv = 0;
        const elfosabi hpux = 1;
        const elfosabi standalone = 255;
};

std::string
to_string(elfosabi v);

typedef ElfTypes::Half et;
namespace et_ns {
        typedef ElfTypes::Half et;
        const et none   = 0;             // No file type
        const et rel    = 1;             // Relocatable object file
        const et exec   = 2;             // Executable file
        const et dyn    = 3;             // Shared object file
        const et core   = 4;             // Core file
        const et loos   = 0xfe00;        // Environment-specific use
        const et hios   = 0xfeff;
        const et loproc = 0xff00;        // Processor-specific use
        const et hiproc = 0xffff;
};

std::string
to_string(et v);

// ELF header (ELF32 figure 1-3, ELF64 figure 2)
template<typename E = Elf64, byte_order Order = byte_order_ns::native>
struct Ehdr
{
        typedef E types;
        static const byte_order order = Order;

        // ELF identification
        unsigned char ei_magic[4];
        elfclass ei_class;
        elfdata ei_data;
        unsigned char ei_version;
        elfosabi ei_osabi;
        unsigned char ei_abiversion;
        unsigned char ei_pad[7];

        et               type;           // Object file type
        typename E::Half machine;        // Machine type
        typename E::Word version;        // Object file version
        typename E::Addr entry;          // Entry point address
        typename E::Off  phoff;          // Program header offset
        typename E::Off  shoff;          // Section header offset
        typename E::Word flags;          // Processor-specific flags
        typename E::Half ehsize;         // ELF header size
        typename E::Half phentsize;      // Size of program header entry
        typename E::Half phnum;          // Number of program header entries
        typename E::Half shentsize;      // Size of section header entry
        typename E::Half shnum;          // Number of section header entries
        typename E::Half shstrndx;       // Section name string table index

        template<typename E2>
        void from(const E2 &o)
        {
                std::memcpy(ei_magic, o.ei_magic, sizeof(ei_magic));
                ei_class = swizzle(o.ei_class, o.order, order);
                ei_data = swizzle(o.ei_data, o.order, order);
                ei_version = swizzle(o.ei_version, o.order, order);
                ei_osabi = swizzle(o.ei_osabi, o.order, order);
                ei_abiversion = swizzle(o.ei_abiversion, o.order, order);
                std::memcpy(ei_pad, o.ei_pad, sizeof(ei_pad));

                type      = swizzle(o.type, o.order, order);
                machine   = swizzle(o.machine, o.order, order);
                version   = swizzle(o.version, o.order, order);
                entry     = swizzle(o.entry, o.order, order);
                phoff     = swizzle(o.phoff, o.order, order);
                shoff     = swizzle(o.shoff, o.order, order);
                flags     = swizzle(o.flags, o.order, order);
                ehsize    = swizzle(o.ehsize, o.order, order);
                phentsize = swizzle(o.phentsize, o.order, order);
                phnum     = swizzle(o.phnum, o.order, order);
                shentsize = swizzle(o.shentsize, o.order, order);
                shnum     = swizzle(o.shnum, o.order, order);
                shstrndx  = swizzle(o.shstrndx, o.order, order);
        }
};

// Special section indices (ELF32 figure 1-7, ELF64 table 7)
//
// This is an integer with a few special values, so this is a regular
// enum, rather than a type-safe enum.  However, this is declared in a
// namespace and then used to avoid polluting the elf:: namespace.
namespace enums {
typedef ElfTypes::Half shn;
namespace shn_ns {      // This is a Word in Shdr and Half in Sym.
        typedef ElfTypes::Half shn;
        const shn undef = 0;              // Undefined or meaningless

        const shn loproc = 0xff00;        // Processor-specific use
        const shn hiproc = 0xff1f;
        const shn loos   = 0xff20;        // Environment-specific use
        const shn hios   = 0xff3f;

        const shn abs    = 0xfff1;        // Reference is an absolute value
        const shn common = 0xfff2;        // Symbol declared as a common block
};

std::string
to_string(shn v);
}

typedef ElfTypes::Word sht;
namespace sht_ns {
        typedef ElfTypes::Word sht;
        const sht null     = 0;           // Marks an unseen section header
        const sht progbits = 1;           // Contains information defined by the program
        const sht symtab   = 2;           // Contains a linker symbol table
        const sht strtab   = 3;           // Contains a string table
        const sht rela     = 4;           // Contains "Rela" type relocation entries
        const sht hash     = 5;           // Contains a symbol hash table
        const sht dynamic  = 6;           // Contains dynamic linking tables
        const sht note     = 7;           // Contains note information
        const sht nobits   = 8;           // Contains uninitialized space;
                                // does not occupy any space in the file
        const sht rel      = 9;           // Contains "Rel" type relocation entries
        const sht shlib    = 10;          // Reserved
        const sht dynsym   = 11;          // Contains a dynamic loader symbol table
        const sht loos     = 0x60000000;  // Environment-specific use
        const sht hios     = 0x6FFFFFFF;
        const sht loproc   = 0x70000000;  // Processor-specific use
        const sht hiproc   = 0x7FFFFFFF;
};

std::string
to_string(sht v);

// Section attributes (ELF64 table 9).  Note: This is an Elf32_Word in
// ELF32.  We use the larger ELF64 type for the canonical
// representation and switch it out for a plain Elf32_Word in the
// ELF32 format.
typedef Elf64::Xword shf;
namespace shf_ns {
        typedef Elf64::Xword shf;
        const shf write     = 0x1;        // Section contains writable data
        const shf alloc     = 0x2;        // Section is allocated in memory image of program
        const shf execinstr = 0x4;        // Section contains executable instructions
        const shf maskos    = 0x0F000000; // Environment-specific use
        const shf maskproc  = 0xF0000000; // Processor-specific use
}

std::string
to_string(shf v);

static inline shf shf_operator_and(shf a, shf b)
{
        return (shf)((uint64_t)a & (uint64_t)b);
}

static inline shf shf_operator_or(shf a, shf b)
{
        return (shf)((uint64_t)a | (uint64_t)b);
}

static inline shf shf_operator_xor(shf a, shf b)
{
        return (shf)((uint64_t)a ^ (uint64_t)b);
}

static inline shf shf_operator_not(shf a)
{
        return (shf)~((uint64_t)a);
}

static inline shf& shf_operator_andeq(shf &a, shf b)
{
        a = shf_operator_and(a, b);
        return a;
}

static inline shf& shf_operator_oreq(shf &a, shf b)
{
        a = shf_operator_or(a, b);
        return a;
}

static inline shf& shf_operator_xoreq(shf &a, shf b)
{
        a = shf_operator_xor(a, b);
        return a;
}

using enums::shn;
// Section header (ELF32 figure 1-8, ELF64 figure 3)
template<typename E = Elf64, byte_order Order = byte_order_ns::native>
struct Shdr
{
        typedef E types;
        static const byte_order order = Order;
        // Section numbers are half-words in some structures and full
        // words in others. Here we declare a local shn type that is
        // elf::shn for the native byte order, but the full word for
        // specific encoding byte orders.
        typedef typename internal::OrderPick<Order, elf::shn, typename E::Word, typename E::Word>::T shn;

        typename E::Word           name; // Section name
        sht                        type; // Section type
        typename E::template pick<typename E::Word, shf>::t flags; // Section attributes
        typename E::Addr           addr; // Virtual address in memory
        typename E::Off            offset; // Offset in file
        typename E::Word32_Xword64 size; // Size of section
        typename elf::shn          link; // Link to other section
        typename E::Word           info; // Miscellaneous information
        typename E::Word32_Xword64 addralign; // Address alignment boundary
        typename E::Word32_Xword64 entsize; // Size of entries, if section has table

        template<typename E2>
        void from(const E2 &o)
        {
                name      = swizzle(o.name, o.order, order);
                type      = swizzle(o.type, o.order, order);
                flags     = (typename E::template pick<typename E::Word, shf>::t)swizzle(o.flags, o.order, order);
                addr      = swizzle(o.addr, o.order, order);
                offset    = swizzle(o.offset, o.order, order);
                size      = swizzle(o.size, o.order, order);
                link      = (typename elf::shn)swizzle((typename E::Word)o.link, o.order, order);
                info      = swizzle(o.info, o.order, order);
                addralign = swizzle(o.addralign, o.order, order);
                entsize   = swizzle(o.entsize, o.order, order);
        }
};

// Segment types (ELF64 table 16)
typedef ElfTypes::Word pt;
namespace pt_ns {
        typedef ElfTypes::Word pt;
        const pt null    = 0;            // Unused entry
        const pt load    = 1;            // Loadable segment
        const pt dynamic = 2;            // Dynamic linking tables
        const pt interp  = 3;            // Program interpreter path name
        const pt note    = 4;            // Note sections
        const pt shlib   = 5;            // Reserved
        const pt phdr    = 6;            // Program header table
        const pt loos    = 0x60000000;   // Environment-specific use
        const pt hios    = 0x6FFFFFFF;
        const pt loproc  = 0x70000000;   // Processor-specific use
        const pt hiproc  = 0x7FFFFFFF;
};

std::string
to_string(pt v);

// Segment attributes
typedef ElfTypes::Word pf;
namespace pf_ns {
        typedef ElfTypes::Word pf;
        const pf x        = 0x1;         // Execute permission
        const pf w        = 0x2;         // Write permission
        const pf r        = 0x4;         // Read permission
        const pf maskos   = 0x00FF0000;  // Environment-specific use
        const pf maskproc = 0xFF000000;  // Processor-specific use
};

std::string
to_string(pf v);

static inline pf pf_operator_and(pf a, pf b)
{
        return (pf)((uint64_t)a & (uint64_t)b);
}

static inline pf pf_operator_or(pf a, pf b)
{
        return (pf)((uint64_t)a | (uint64_t)b);
}

static inline pf pf_operator_xor(pf a, pf b)
{
        return (pf)((uint64_t)a ^ (uint64_t)b);
}

static inline pf pf_operator_not(pf a)
{
        return (pf)~((uint64_t)a);
}

static inline pf& pf_operator_andeq(pf &a, pf b)
{
        a = pf_operator_and(a, b);
        return a;
}

static inline pf& pf_operator_oreq(pf &a, pf b)
{
        a = pf_operator_or(a, b);
        return a;
}

static inline pf& pf_operator_xoreq(pf &a, pf b)
{
        a = pf_operator_xor(a, b);
        return a;
}

// Program header (ELF32 figure 2-1, ELF64 figure 6)
template<typename E = Elf64, byte_order Order = byte_order_ns::native>
struct Phdr;

template<byte_order Order>
struct Phdr<Elf32, Order>
{
        typedef Elf32 types;
        static const byte_order order = Order;

        pt           type;      // Type of segment
        Elf32::Off   offset;    // Offset in file
        Elf32::Addr  vaddr;     // Virtual address in memory
        Elf32::Addr  paddr;     // Reserved
        Elf32::Word  filesz;    // Size of segment in file
        Elf32::Word  memsz;     // Size of segment in memory
        pf           flags;     // Segment attributes
        Elf32::Word  align;     // Alignment of segment

        template<typename E2>
        void from(const E2 &o)
        {
                type   = swizzle(o.type, o.order, order);
                offset = swizzle(o.offset, o.order, order);
                vaddr  = swizzle(o.vaddr, o.order, order);
                paddr  = swizzle(o.paddr, o.order, order);
                filesz = swizzle(o.filesz, o.order, order);
                memsz  = swizzle(o.memsz, o.order, order);
                flags  = swizzle(o.flags, o.order, order);
                align  = swizzle(o.align, o.order, order);
        }
};

template<byte_order Order>
struct Phdr<Elf64, Order>
{
        typedef Elf64 types;
        static const byte_order order = Order;

        pt           type;      // Type of segment
        pf           flags;     // Segment attributes
        Elf64::Off   offset;    // Offset in file
        Elf64::Addr  vaddr;     // Virtual address in memory
        Elf64::Addr  paddr;     // Reserved
        Elf64::Xword filesz;    // Size of segment in file
        Elf64::Xword memsz;     // Size of segment in memory
        Elf64::Xword align;     // Alignment of segment

        template<typename E2>
        void from(const E2 &o)
        {
                type   = swizzle(o.type, o.order, order);
                offset = swizzle(o.offset, o.order, order);
                vaddr  = swizzle(o.vaddr, o.order, order);
                paddr  = swizzle(o.paddr, o.order, order);
                filesz = swizzle(o.filesz, o.order, order);
                memsz  = swizzle(o.memsz, o.order, order);
                flags  = swizzle(o.flags, o.order, order);
                align  = swizzle(o.align, o.order, order);
        }
};

// Symbol bindings (ELF32 figure 1-16, ELF64 table 14)
typedef int stb;
namespace stb_ns {
        typedef int stb;
        const stb local  = 0;             // Not visible outside the object file
        const stb global = 1;             // Global symbol
        const stb weak   = 2;             // Global scope, but with lower
                                // precedence than global symbols
        const stb loos   = 10;            // Environment-specific use
        const stb hios   = 12;
        const stb loproc = 13;            // Processor-specific use
        const stb hiproc = 15;
};

std::string
to_string(stb v);

// Symbol types (ELF32 figure 1-17, ELF64 table 15)
typedef int stt;
namespace stt_ns {
        typedef int stt;
        const stt notype  = 0;            // No type (e.g., absolute symbol)
        const stt object  = 1;            // Data object
        const stt func    = 2;            // Function entry point
        const stt section = 3;            // Symbol is associated with a section
        const stt file    = 4;            // Source file associated with the
                                // object file
        const stt loos    = 10;           // Environment-specific use
        const stt hios    = 12;
        const stt loproc  = 13;           // Processor-specific use
        const stt hiproc  = 15;
};

std::string
to_string(stt v);

// Symbol table (ELF32 figure 1-15, ELF64 figure 4)
template<typename E = Elf64, byte_order Order = byte_order_ns::native>
struct Sym;

template<byte_order Order>
struct Sym<Elf32, Order>
{
        typedef Elf32 types;
        static const byte_order order = Order;

        Elf32::Word   name;  // Symbol name (strtab offset)
        Elf32::Addr   value; // Symbol value (address)
        Elf32::Word   size;  // Size of object
        unsigned char info;  // Type and binding attributes
        unsigned char other; // Reserved
        shn           shnxd; // Section table index

        template<typename E2>
        void from(const E2 &o)
        {
                name  = swizzle(o.name, o.order, order);
                value = swizzle(o.value, o.order, order);
                size  = swizzle(o.size, o.order, order);
                info  = o.info;
                other = o.other;
                shnxd = swizzle(o.shnxd, o.order, order);
        }

        stb binding() const
        {
                return (stb)(info >> 4);
        }

        void set_binding(stb v)
        {
                info = (info & 0x0F) | ((unsigned char)v << 4);
        }

        stt type() const
        {
                return (stt)(info & 0xF);
        }

        void set_type(stt v)
        {
                info = (info & 0xF0) | (unsigned char)v;
        }
};

template<byte_order Order>
struct Sym<Elf64, Order>
{
        typedef Elf64 types;
        static const byte_order order = Order;

        Elf64::Word   name;  // Symbol name (strtab offset)
        unsigned char info;  // Type and binding attributes
        unsigned char other; // Reserved
        shn           shnxd; // Section table index
        Elf64::Addr   value; // Symbol value (address)
        Elf64::Xword  size;  // Size of object

        template<typename E2>
        void from(const E2 &o)
        {
                name  = swizzle(o.name, o.order, order);
                value = swizzle(o.value, o.order, order);
                size  = swizzle(o.size, o.order, order);
                info  = o.info;
                other = o.other;
                shnxd = swizzle(o.shnxd, o.order, order);
        }

        stb binding() const
        {
                return (stb)(info >> 4);
        }

        void set_binding(stb v)
        {
                info = (info & 0xF) | ((unsigned char)v << 4);
        }

        stt type() const
        {
                return (stt)(info & 0xF);
        }

        void set_type(stt v)
        {
                info = (info & 0xF0) | (unsigned char)v;
        }
};

ELFPP_END_NAMESPACE

#endif
