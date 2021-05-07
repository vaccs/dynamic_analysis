// Copyright (c) 2013 Austin T. Clements. All rights reserved.
// Use of this source code is governed by an MIT license
// that can be found in the LICENSE file.

#ifndef _DWARFPP_DW_HH_
#define _DWARFPP_DW_HH_

#include <stdint.h>

#include <string>

DWARFPP_BEGIN_NAMESPACE

// Integer representations (Section 7.26)
typedef int8_t sbyte;
typedef uint8_t ubyte;
typedef uint16_t uhalf;
typedef uint32_t uword;

// Section offsets and lengths
typedef uint64_t section_offset;
typedef uint64_t section_length;

// A target machine address.  Targets may use smaller addresses; this
// represents the largest supported address type.
typedef uint64_t taddr;

// DIE tags (Section 7, figure 18).  typedef, friend, and namespace
// have a trailing underscore because they are reserved words.
typedef int DW_TAG;
namespace DW_TAG_NS {
        typedef int DW_TAG;

        const DW_TAG undefined                = 0x00;

        const DW_TAG array_type               = 0x01;
        const DW_TAG class_type               = 0x02;
        const DW_TAG entry_point              = 0x03;
        const DW_TAG enumeration_type         = 0x04;
        const DW_TAG formal_parameter         = 0x05;
        const DW_TAG imported_declaration     = 0x08;
        const DW_TAG label                    = 0x0a;
        const DW_TAG lexical_block            = 0x0b;
        const DW_TAG member                   = 0x0d;
        const DW_TAG pointer_type             = 0x0f;
        const DW_TAG reference_type           = 0x10;
        const DW_TAG compile_unit             = 0x11;
        const DW_TAG string_type              = 0x12;
        const DW_TAG structure_type           = 0x13;
        const DW_TAG subroutine_type          = 0x15;
        const DW_TAG typedef_                 = 0x16;

        const DW_TAG union_type               = 0x17;
        const DW_TAG unspecified_parameters   = 0x18;
        const DW_TAG variant                  = 0x19;
        const DW_TAG common_block             = 0x1a;
        const DW_TAG common_inclusion         = 0x1b;
        const DW_TAG inheritance              = 0x1c;
        const DW_TAG inlined_subroutine       = 0x1d;
        const DW_TAG module                   = 0x1e;
        const DW_TAG ptr_to_member_type       = 0x1f;
        const DW_TAG set_type                 = 0x20;
        const DW_TAG subrange_type            = 0x21;
        const DW_TAG with_stmt                = 0x22;
        const DW_TAG access_declaration       = 0x23;
        const DW_TAG base_type                = 0x24;
        const DW_TAG catch_block              = 0x25;
        const DW_TAG const_type               = 0x26;
        const DW_TAG constant                 = 0x27;
        const DW_TAG enumerator               = 0x28;
        const DW_TAG file_type                = 0x29;
        const DW_TAG friend_                  = 0x2a;

        const DW_TAG namelist                 = 0x2b;
        const DW_TAG namelist_item            = 0x2c;
        const DW_TAG packed_type              = 0x2d;
        const DW_TAG subprogram               = 0x2e;
        const DW_TAG template_type_parameter  = 0x2f;
        const DW_TAG template_value_parameter = 0x30;
        const DW_TAG thrown_type              = 0x31;
        const DW_TAG try_block                = 0x32;
        const DW_TAG variant_part             = 0x33;
        const DW_TAG variable                 = 0x34;
        const DW_TAG volatile_type            = 0x35;
        const DW_TAG dwarf_procedure          = 0x36;
        const DW_TAG restrict_type            = 0x37;
        const DW_TAG interface_type           = 0x38;
        const DW_TAG namespace_               = 0x39;
        const DW_TAG imported_module          = 0x3a;
        const DW_TAG unspecified_type         = 0x3b;
        const DW_TAG partial_unit             = 0x3c;
        const DW_TAG imported_unit            = 0x3d;
        const DW_TAG condition                = 0x3f;

        const DW_TAG shared_type              = 0x40;
        const DW_TAG type_unit                = 0x41;
        const DW_TAG rvalue_reference_type    = 0x42;
        const DW_TAG template_alias           = 0x43;
        const DW_TAG lo_user                  = 0x4080;
        const DW_TAG hi_user                  = 0xffff;
} // DW_TAG_NS

std::string
to_string(DW_TAG v);

// Child determination (Section 7, figure 19).
typedef ubyte DW_CHILDREN;
namespace DW_CHILDREN_NS {
typedef ubyte DW_CHILDREN;
const DW_CHILDREN no  = 0x00;
const DW_CHILDREN yes = 0x01;
}; // DW_CHILDREN_NS

std::string
to_string(DW_CHILDREN v);

// Attribute names (Section 7, figure 20).  inline, friend, mutable,
// and explicit have a trailing underscore because they are reserved
// words.
typedef int DW_AT;
namespace DW_AT_NS {
        typedef int DW_AT;
        const DW_AT sibling              = 0x01; // reference
        const DW_AT location             = 0x02; // exprloc, loclistptr
        const DW_AT name                 = 0x03; // string
        const DW_AT ordering             = 0x09; // constant
        const DW_AT byte_size            = 0x0b; // constant, exprloc, reference
        const DW_AT bit_offset           = 0x0c; // constant, exprloc, reference
        const DW_AT bit_size             = 0x0d; // constant, exprloc, reference
        const DW_AT stmt_list            = 0x10; // lineptr
        const DW_AT low_pc               = 0x11; // address
        const DW_AT high_pc              = 0x12; // address, constant
        const DW_AT language             = 0x13; // constant
        const DW_AT discr                = 0x15; // reference
        const DW_AT discr_value          = 0x16; // constant
        const DW_AT visibility           = 0x17; // constant
        const DW_AT import               = 0x18; // reference
        const DW_AT string_length        = 0x19; // exprloc, loclistptr
        const DW_AT common_reference     = 0x1a; // reference
        const DW_AT comp_dir             = 0x1b; // string
        const DW_AT const_value          = 0x1c; // block, constant, string

        const DW_AT containing_type      = 0x1d; // reference
        const DW_AT default_value        = 0x1e; // reference
        const DW_AT inline_              = 0x20; // constant
        const DW_AT is_optional          = 0x21; // flag
        const DW_AT lower_bound          = 0x22; // constant, exprloc, reference
        const DW_AT producer             = 0x25; // string
        const DW_AT prototyped           = 0x27; // flag
        const DW_AT return_addr          = 0x2a; // exprloc, loclistptr
        const DW_AT start_scope          = 0x2c; // constant, rangelistptr
        const DW_AT bit_stride           = 0x2e; // constant, exprloc, reference
        const DW_AT upper_bound          = 0x2f; // constant, exprloc, reference
        const DW_AT abstract_origin      = 0x31; // reference
        const DW_AT accessibility        = 0x32; // constant
        const DW_AT address_class        = 0x33; // constant
        const DW_AT artificial           = 0x34; // flag
        const DW_AT base_types           = 0x35; // reference
        const DW_AT calling_convention   = 0x36; // constant
        const DW_AT count                = 0x37; // constant, exprloc, reference
        const DW_AT data_member_location = 0x38; // constant, exprloc, loclistptr
        const DW_AT decl_column          = 0x39; // constant

        const DW_AT decl_file            = 0x3a; // constant
        const DW_AT decl_line            = 0x3b; // constant
        const DW_AT declaration          = 0x3c; // flag
        const DW_AT discr_list           = 0x3d; // block
        const DW_AT encoding             = 0x3e; // constant
        const DW_AT external             = 0x3f; // flag
        const DW_AT frame_base           = 0x40; // exprloc, loclistptr
        const DW_AT friend_              = 0x41; // reference
        const DW_AT identifier_case      = 0x42; // constant
        const DW_AT macro_info           = 0x43; // macptr
        const DW_AT namelist_item        = 0x44; // reference
        const DW_AT priority             = 0x45; // reference
        const DW_AT segment              = 0x46; // exprloc, loclistptr
        const DW_AT specification        = 0x47; // reference
        const DW_AT static_link          = 0x48; // exprloc, loclistptr
        const DW_AT type                 = 0x49; // reference
        const DW_AT use_location         = 0x4a; // exprloc, loclistptr
        const DW_AT variable_parameter   = 0x4b; // flag
        const DW_AT virtuality           = 0x4c; // constant
        const DW_AT vtable_elem_location = 0x4d; // exprloc, loclistptr

        // DWARF 3
        const DW_AT allocated            = 0x4e; // constant, exprloc, reference
        const DW_AT associated           = 0x4f; // constant, exprloc, reference
        const DW_AT data_location        = 0x50; // exprloc
        const DW_AT byte_stride          = 0x51; // constant, exprloc, reference
        const DW_AT entry_pc             = 0x52; // address
        const DW_AT use_UTF8             = 0x53; // flag
        const DW_AT extension            = 0x54; // reference
        const DW_AT ranges               = 0x55; // rangelistptr
        const DW_AT trampoline           = 0x56; // address, flag, reference, string
        const DW_AT call_column          = 0x57; // constant
        const DW_AT call_file            = 0x58; // constant
        const DW_AT call_line            = 0x59; // constant
        const DW_AT description          = 0x5a; // string
        const DW_AT binary_scale         = 0x5b; // constant
        const DW_AT decimal_scale        = 0x5c; // constant
        const DW_AT small                = 0x5d; // reference
        const DW_AT decimal_sign         = 0x5e; // constant
        const DW_AT digit_count          = 0x5f; // constant
        const DW_AT picture_string       = 0x60; // string
        const DW_AT mutable_             = 0x61; // flag

        const DW_AT threads_scaled       = 0x62; // flag
        const DW_AT explicit_            = 0x63; // flag
        const DW_AT object_pointer       = 0x64; // reference
        const DW_AT endianity            = 0x65; // constant
        const DW_AT elemental            = 0x66; // flag
        const DW_AT pure                 = 0x67; // flag
        const DW_AT recursive            = 0x68; // flag

        // DWARF 4
        const DW_AT signature            = 0x69; // reference
        const DW_AT main_subprogram      = 0x6a; // flag
        const DW_AT data_bit_offset      = 0x6b; // constant
        const DW_AT const_expr           = 0x6c; // flag
        const DW_AT enum_class           = 0x6d; // flag
        const DW_AT linkage_name         = 0x6e; // string

        const DW_AT lo_user              = 0x2000;
        const DW_AT hi_user              = 0x3fff;
}; // DW_AT_NS

std::string
to_string(DW_AT v);

// Attribute form encodings (Section 7, figure 21)
typedef int DW_FORM;
namespace DW_FORM_NS {
        typedef int DW_FORM;
        const DW_FORM addr         = 0x01;    // address
        const DW_FORM block2       = 0x03;    // block
        const DW_FORM block4       = 0x04;    // block
        const DW_FORM data2        = 0x05;    // constant
        const DW_FORM data4        = 0x06;    // constant
        const DW_FORM data8        = 0x07;    // constant
        const DW_FORM string       = 0x08;    // string
        const DW_FORM block        = 0x09;    // block
        const DW_FORM block1       = 0x0a;    // block
        const DW_FORM data1        = 0x0b;    // constant
        const DW_FORM flag         = 0x0c;    // flag
        const DW_FORM sdata        = 0x0d;    // constant
        const DW_FORM strp         = 0x0e;    // string
        const DW_FORM udata        = 0x0f;    // constant
        const DW_FORM ref_addr     = 0x10;    // reference
        const DW_FORM ref1         = 0x11;    // reference
        const DW_FORM ref2         = 0x12;    // reference
        const DW_FORM ref4         = 0x13;    // reference
        const DW_FORM ref8         = 0x14;    // reference

        const DW_FORM ref_udata    = 0x15;    // reference
        const DW_FORM indirect     = 0x16;    // (Section 7.5.3)

                // DWARF 4
        const DW_FORM sec_offset   = 0x17;    // lineptr, loclistptr, macptr, rangelistptr
        const DW_FORM exprloc      = 0x18;    // exprloc
        const DW_FORM flag_present = 0x19;    // flag
        const DW_FORM ref_sig8     = 0x20;    // reference
}; // DW_FORM_NS

std::string
to_string(DW_FORM v);

// DWARF operation encodings (Section 7.7.1 and figure 24)
typedef ubyte DW_OP;
namespace DW_OP_NS {
        typedef ubyte DW_OP;
        const DW_OP addr                = 0x03; // [constant address (size target specific)]
        const DW_OP deref               = 0x06;

        const DW_OP const1u             = 0x08; // [1-byte constant]
        const DW_OP const1s             = 0x09; // [1-byte constant]
        const DW_OP const2u             = 0x0a; // [2-byte constant]
        const DW_OP const2s             = 0x0b; // [2-byte constant]
        const DW_OP const4u             = 0x0c; // [4-byte constant]
        const DW_OP const4s             = 0x0d; // [4-byte constant]
        const DW_OP const8u             = 0x0e; // [8-byte constant]
        const DW_OP const8s             = 0x0f; // [8-byte constant]
        const DW_OP constu              = 0x10; // [ULEB128 constant]
        const DW_OP consts              = 0x11; // [SLEB128 constant]
        const DW_OP dup                 = 0x12;
        const DW_OP drop                = 0x13;
        const DW_OP over                = 0x14;
        const DW_OP pick                = 0x15; // [1-byte stack index]
        const DW_OP swap                = 0x16;
        const DW_OP rot                 = 0x17;
        const DW_OP xderef              = 0x18;
        const DW_OP abs                 = 0x19;
        const DW_OP and_                = 0x1a;
        const DW_OP div                 = 0x1b;

        const DW_OP minus               = 0x1c;
        const DW_OP mod                 = 0x1d;
        const DW_OP mul                 = 0x1e;
        const DW_OP neg                 = 0x1f;
        const DW_OP not_                = 0x20;
        const DW_OP or_                 = 0x21;
        const DW_OP plus                = 0x22;
        const DW_OP plus_uconst         = 0x23; // [ULEB128 addend]
        const DW_OP shl                 = 0x24;
        const DW_OP shr                 = 0x25;
        const DW_OP shra                = 0x26;
        const DW_OP xor_                = 0x27;
        const DW_OP skip                = 0x2f; // [signed 2-byte constant]
        const DW_OP bra                 = 0x28; // [signed 2-byte constant]
        const DW_OP eq                  = 0x29;
        const DW_OP ge                  = 0x2a;
        const DW_OP gt                  = 0x2b;
        const DW_OP le                  = 0x2c;
        const DW_OP lt                  = 0x2d;
        const DW_OP ne                  = 0x2e;

                // Literals 0..31 = (lit0 + literal)
        const DW_OP lit0                = 0x30;
        const DW_OP lit31               = 0x4f;

                // Registers 0..31 = (reg0 + regnum)
        const DW_OP reg0                = 0x50;
        const DW_OP reg31               = 0x6f;

                // Base register 0..31 = (breg0 + regnum)
        const DW_OP breg0               = 0x70; // [SLEB128 offset]
        const DW_OP breg31              = 0x8f; // [SLEB128 offset]

        const DW_OP regx                = 0x90; // [ULEB128 register]
        const DW_OP fbreg               = 0x91; // [SLEB128 offset]
        const DW_OP bregx               = 0x92; // [ULEB128 register, SLEB128 offset]
        const DW_OP piece               = 0x93; // [ULEB128 size of piece addressed]
        const DW_OP deref_size          = 0x94; // [1-byte size of data retrieved]
        const DW_OP xderef_size         = 0x95; // [1-byte size of data retrieved]
        const DW_OP nop                 = 0x96;

                // DWARF 3
        const DW_OP push_object_address = 0x97;
        const DW_OP call2               = 0x98; // [2-byte offset of DIE]
        const DW_OP call4               = 0x99; // [4-byte offset of DIE]
        const DW_OP call_ref            = 0x9a; // [4- or 8-byte offset of DIE]
        const DW_OP form_tls_address    = 0x9b;
        const DW_OP call_frame_cfa      = 0x9c;
        const DW_OP bit_piece           = 0x9d; // [ULEB128 size, ULEB128 offset]

                // DWARF 4
        const DW_OP implicit_value      = 0x9e; // [ULEB128 size, block of that size]
        const DW_OP stack_value         = 0x9f;

        const DW_OP lo_user             = 0xe0;
        const DW_OP hi_user             = 0xff;
};

std::string
to_string(DW_OP v);

// DW_AT_NS::encoding constants (DWARF4 section 7.8 figure 25)
typedef int DW_ATE;
namespace DW_ATE_NS {
        typedef int DW_ATE;
        const DW_ATE address         = 0x01;
        const DW_ATE boolean         = 0x02;
        const DW_ATE complex_float   = 0x03;
        const DW_ATE float_          = 0x04;
        const DW_ATE signed_         = 0x05;
        const DW_ATE signed_char     = 0x06;
        const DW_ATE unsigned_       = 0x07;
        const DW_ATE unsigned_char   = 0x08;
        const DW_ATE imaginary_float = 0x09;
        const DW_ATE packed_decimal  = 0x0a;
        const DW_ATE numeric_string  = 0x0b;
        const DW_ATE edited          = 0x0c;
        const DW_ATE signed_fixed    = 0x0d;
        const DW_ATE unsigned_fixed  = 0x0e;
        const DW_ATE decimal_float   = 0x0f;

        // DWARF 4
        const DW_ATE UTF             = 0x10;

        const DW_ATE lo_user         = 0x80;
        const DW_ATE hi_user         = 0xff;
}; // DW_ATE_NS

std::string
to_string(DW_ATE v);

// DW_AT_NS::decimal_sign constants (DWARF4 section 7.8 figure 26)
typedef int DW_DS;
namespace DW_DS_NS {
        typedef int DW_DS;
        const DW_DS unsigned_          = 0x01;
        const DW_DS leading_overpunch  = 0x02;
        const DW_DS trailing_overpunch = 0x03;
        const DW_DS leading_separate   = 0x04;
        const DW_DS trailing_separate  = 0x05;
}; // DW_DS_NS

std::string
to_string(DW_DS v);

// DW_AT_NS::endianity constants (DWARF4 section 7.8 figure 27)
typedef int DW_END;
namespace DW_END_NS {
        typedef int DW_END;
        const DW_END default_ = 0x00;
        const DW_END big      = 0x01;
        const DW_END little   = 0x02;
        const DW_END lo_user  = 0x40;
        const DW_END hi_user  = 0xff;
};

std::string
to_string(DW_END v);

// DW_AT_NS::accessibility constants (DWARF4 section 7.9 figure 28)
typedef int DW_ACCESS;
namespace DW_ACCESS_NS {
        typedef int DW_ACCESS;
        const DW_ACCESS public_    = 0x01;
        const DW_ACCESS protected_ = 0x02;
        const DW_ACCESS private_   = 0x03;
}; // DW_ACCESS_NS

std::string
to_string(DW_ACCESS v);

// DW_AT_NS::visibility constants (DWARF4 section 7.10 figure 29)
typedef int DW_VIS;
namespace DW_VIS_NS {
        typedef int DW_VIS;
        const DW_VIS local     = 0x01;
        const DW_VIS exported  = 0x02;
        const DW_VIS qualified = 0x03;
}; // DW_VIS_NS

std::string
to_string(DW_VIS v);

// DW_AT_NS::virtuality constants (DWARF4 section 7.11 figure 30)
typedef int DW_VIRTUALITY;
namespace DW_VIRTUALITY_NS {
        typedef int DW_VIRTUALITY;
        const DW_VIRTUALITY none         = 0x00;
        const DW_VIRTUALITY virtual_     = 0x01;
        const DW_VIRTUALITY pure_virtual = 0x02;
}; // DW_VIRTUALITY_NS

std::string
to_string(DW_VIRTUALITY v);

// DW_AT_NS::language constants (DWARF4 section 7.12 figure 31)
typedef int DW_LANG;
namespace DW_LANG_NS {
        typedef int DW_LANG;
        const DW_LANG C89            = 0x0001; // Lower bound 0
        const DW_LANG C              = 0x0002; // Lower bound 0
        const DW_LANG Ada83          = 0x0003; // Lower bound 1
        const DW_LANG C_plus_plus    = 0x0004; // Lower bound 0
        const DW_LANG Cobol74        = 0x0005; // Lower bound 1
        const DW_LANG Cobol85        = 0x0006; // Lower bound 1
        const DW_LANG Fortran77      = 0x0007; // Lower bound 1
        const DW_LANG Fortran90      = 0x0008; // Lower bound 1
        const DW_LANG Pascal83       = 0x0009; // Lower bound 1
        const DW_LANG Modula2        = 0x000a; // Lower bound 1
        const DW_LANG Java           = 0x000b; // Lower bound 0
        const DW_LANG C99            = 0x000c; // Lower bound 0
        const DW_LANG Ada95          = 0x000d; // Lower bound 1
        const DW_LANG Fortran95      = 0x000e; // Lower bound 1
        const DW_LANG PLI            = 0x000f; // Lower bound 1

        const DW_LANG ObjC           = 0x0010; // Lower bound 0
        const DW_LANG ObjC_plus_plus = 0x0011; // Lower bound 0
        const DW_LANG UPC            = 0x0012; // Lower bound 0
        const DW_LANG D              = 0x0013; // Lower bound 0
        const DW_LANG Python         = 0x0014; // Lower bound 0
        const DW_LANG lo_user        = 0x8000;
        const DW_LANG hi_user        = 0xffff;
}; // DW_LANG_NS

std::string
to_string(DW_LANG v);

// DW_AT_NS::identifier_case constants (DWARF4 section 7.14 figure 32)
typedef int DW_ID;
namespace DW_ID_NS {
        typedef int DW_ID;
        const DW_ID case_sensitive   = 0x00;
        const DW_ID up_case          = 0x01;
        const DW_ID down_case        = 0x02;
        const DW_ID case_insensitive = 0x03;
}; // DW_ID_NS

std::string
to_string(DW_ID v);

// DW_AT_NS::calling_convention constants (DWARF4 section 7.15 figure 33)
typedef int DW_CC;
namespace DW_CC_NS {
        typedef int DW_CC;
        const DW_CC normal = 0x01;
        const DW_CC program = 0x02;
        const DW_CC nocall = 0x03;
        const DW_CC lo_user = 0x40;
        const DW_CC hi_user = 0xff;
}; // DW_CC_NS

std::string
to_string(DW_CC v);

// DW_AT_NS::inline constants (DWARF4 section 7.16 figure 34)
typedef int DW_INL;
namespace DW_INL_NS {
        typedef int DW_INL;
        const DW_INL not_inlined = 0x00;
        const DW_INL inlined = 0x01;
        const DW_INL declared_not_inlined = 0x02;
        const DW_INL declared_inlined = 0x03;
}; // DW_INL_NS

std::string
to_string(DW_INL v);

// DW_AT_NS::ordering constants (DWARF4 section 7.17 figure 35)
typedef int DW_ORD;
namespace DW_ORD_NS {
        typedef int DW_ORD;
        const DW_ORD row_major = 0x00;
        const DW_ORD col_major = 0x01;
}; // DW_ORD_NS

std::string
to_string(DW_ORD v);

// DW_AT_NS::discr_list constants (DWARF4 section 7.18 figure 36)
typedef int DW_DSC;
namespace DW_DSC_NS {
        typedef int DW_DSC;
        const DW_DSC label = 0x00;
        const DW_DSC range = 0x01;
}; // DW_DSC_NS

std::string
to_string(DW_DSC v);

// Line number standard opcodes (DWARF4 section 7.21 figure 37)
typedef int DW_LNS;
namespace DW_LNS_NS {
        typedef int DW_LNS;
        const DW_LNS copy = 0x01;
        const DW_LNS advance_pc = 0x02;
        const DW_LNS advance_line = 0x03;
        const DW_LNS set_file = 0x04;
        const DW_LNS set_column = 0x05;
        const DW_LNS negate_stmt = 0x06;
        const DW_LNS set_basic_block = 0x07;
        const DW_LNS const_add_pc = 0x08;
        const DW_LNS fixed_advance_pc = 0x09;

        // DWARF 3
        const DW_LNS set_prologue_end = 0x0a;
        const DW_LNS set_epilogue_begin = 0x0b;
        const DW_LNS set_isa = 0x0c;
}; // DW_LNS_NS

std::string
to_string(DW_LNS v);

// Line number extended opcodes (DWARF4 section 7.21 figure 38)
typedef int DW_LNE;
namespace DW_LNE_NS {
        typedef int DW_LNE;
        const DW_LNE end_sequence = 0x01;
        const DW_LNE set_address = 0x02;
        const DW_LNE define_file = 0x03;

        // DWARF 4
        const DW_LNE set_discriminator = 0x04;

        // DWARF 3
        const DW_LNE lo_user = 0x80;
        const DW_LNE hi_user = 0xff;
}; // DW_LNE_NS

std::string
to_string(DW_LNE v);

DWARFPP_END_NAMESPACE

#endif
