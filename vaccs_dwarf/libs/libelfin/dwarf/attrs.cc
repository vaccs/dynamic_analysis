// Copyright (c) 2013 Austin T. Clements. All rights reserved.
// Use of this source code is governed by an MIT license
// that can be found in the LICENSE file.

#include "dwarf++.hh"

using namespace std;

DWARFPP_BEGIN_NAMESPACE

#define AT_ANY(name)                            \
        value at_##name(const die &d)           \
        {                                       \
                return d[DW_AT_NS::name];          \
        }                                       

#define AT_ADDRESS(name)                                \
        taddr at_##name(const die &d)                   \
        {                                               \
                return d[DW_AT_NS::name].as_address();     \
        }                                               

#define AT_ENUM(name, type)                                \
        type at_##name(const die &d)                            \
        {                                                       \
                return (type)d[DW_AT_NS::name].as_uconstant();     \
        }                                                       

#define AT_FLAG(name)                                   \
        bool at_##name(const die &d)                    \
        {                                               \
                return d[DW_AT_NS::name].as_flag();        \
        }                                               

#define AT_FLAG_(name)                                  \
        bool at_##name(const die &d)                    \
        {                                               \
                return d[DW_AT_NS::name##_].as_flag();     \
        }                                               

#define AT_REFERENCE(name)                              \
        die at_##name(const die &d)                     \
        {                                               \
                return d[DW_AT_NS::name].as_reference();   \
        }                                               

#define AT_STRING(name)                                 \
        string at_##name(const die &d)                  \
        {                                               \
                return d[DW_AT_NS::name].as_string();      \
        }                                               

#define AT_UDYNAMIC(name)                                       \
        uint64_t at_##name(const die &d, expr_context *ctx)     \
        {                                                       \
                return _at_udynamic(DW_AT_NS::name, d, ctx);       \
        }                                                       

static uint64_t _at_udynamic(DW_AT attr, const die &d, expr_context *ctx, int depth = 0)
{
        // DWARF4 section 2.19
        if (depth > 16)
                throw format_error("reference depth exceeded for " + to_string(attr));

        value v(d[attr]);
        switch (v.get_type()) {
        case value::constant:
        case value::uconstant:
                return v.as_uconstant();
        case value::reference:
                return _at_udynamic(attr, v.as_reference(), ctx, depth + 1);
        case value::exprloc:
                return v.as_exprloc().evaluate(ctx).value;
        default:
                throw format_error(to_string(attr) + " has unexpected type " +
                                   to_string(v.get_type()));
        }
}

//////////////////////////////////////////////////////////////////
// 0x0X
//

AT_REFERENCE(sibling);
// XXX location
AT_STRING(name);
AT_ENUM(ordering, DW_ORD);
AT_UDYNAMIC(byte_size);
AT_UDYNAMIC(bit_offset);
AT_UDYNAMIC(bit_size);

//////////////////////////////////////////////////////////////////
// 0x1X
//

// XXX stmt_list
AT_ADDRESS(low_pc);
taddr
at_high_pc(const die &d)
{
        value v(d[DW_AT_NS::high_pc]);
        switch (v.get_type()) {
        case value::address:
                return v.as_address();
        case value::constant:
        case value::uconstant:
                return at_low_pc(d) + v.as_uconstant();
        default:
                throw format_error(to_string(DW_AT_NS::high_pc) + " has unexpected type " +
                                   to_string(v.get_type()));
        }
}
AT_ENUM(language, DW_LANG);
AT_REFERENCE(discr);
AT_ANY(discr_value);            // XXX Signed or unsigned
AT_ENUM(visibility, DW_VIS);
AT_REFERENCE(import);
// XXX string_length
AT_REFERENCE(common_reference);
AT_STRING(comp_dir);
AT_ANY(const_value);
AT_REFERENCE(containing_type);
// XXX default_value

//////////////////////////////////////////////////////////////////
// 0x2X
//

DW_INL at_inline(const die &d)
{
        // XXX Missing attribute is equivalent to DW_INL_not_inlined
        // (DWARF4 section 3.3.8)
        return (DW_INL)d[DW_AT_NS::inline_].as_uconstant();
}
AT_FLAG(is_optional);
AT_UDYNAMIC(lower_bound);       // XXX Language-based default?
AT_STRING(producer);
AT_FLAG(prototyped);
// XXX return_addr
// XXX start_scope
AT_UDYNAMIC(bit_stride);
AT_UDYNAMIC(upper_bound);

//////////////////////////////////////////////////////////////////
// 0x3X
//

AT_REFERENCE(abstract_origin);
AT_ENUM(accessibility, DW_ACCESS);
// XXX const address_class
AT_FLAG(artificial);
// XXX base_types
AT_ENUM(calling_convention, DW_CC);
AT_UDYNAMIC(count);
expr_result
at_data_member_location(const die &d, expr_context *ctx, taddr base, taddr pc)
{
        value v(d[DW_AT_NS::data_member_location]);
        switch (v.get_type()) {
        case value::constant:
        case value::uconstant: {
                expr_result r;
                r.location_type = expr_result::address;
                r.value = base + v.as_uconstant();
                return r;
        }
        case value::exprloc:
                return v.as_exprloc().evaluate(ctx, base);
        case value::loclist:
                // XXX
                throw std::runtime_error("not implemented");
        default:
                throw format_error("DW_AT_data_member_location has unexpected type " +
                                   to_string(v.get_type()));
        }
}
// XXX decl_column decl_file decl_line
AT_FLAG(declaration);
// XXX discr_list
AT_ENUM(encoding, DW_ATE);
AT_FLAG(external);

//////////////////////////////////////////////////////////////////
// 0x4X
//

// XXX frame_base
die at_friend(const die &d)
{
        return d[DW_AT_NS::friend_].as_reference();
}
AT_ENUM(identifier_case, DW_ID);
// XXX macro_info
AT_REFERENCE(namelist_item);
AT_REFERENCE(priority);         // XXX Computed might be useful
// XXX segment
AT_REFERENCE(specification);
// XXX static_link
AT_REFERENCE(type);
// XXX use_location
AT_FLAG(variable_parameter);
// XXX 7.11 The value DW_VIRTUALITY_none is equivalent to the absence
// of the DW_AT_virtuality attribute.
AT_ENUM(virtuality, DW_VIRTUALITY);
// XXX vtable_elem_location
AT_UDYNAMIC(allocated);
AT_UDYNAMIC(associated);

//////////////////////////////////////////////////////////////////
// 0x5X
//

// XXX data_location
AT_UDYNAMIC(byte_stride);
AT_ADDRESS(entry_pc);
AT_FLAG(use_UTF8);
AT_REFERENCE(extension);
rangelist
at_ranges(const die &d)
{
        return d[DW_AT_NS::ranges].as_rangelist();
}
// XXX trampoline
// XXX const call_column, call_file, call_line
AT_STRING(description);
// XXX const binary_scale
// XXX const decimal_scale
AT_REFERENCE(small);
// XXX const decimal_sign
// XXX const digit_count

//////////////////////////////////////////////////////////////////
// 0x6X
//

AT_STRING(picture_string);
AT_FLAG_(mutable);
AT_FLAG(threads_scaled);
AT_FLAG_(explicit);
AT_REFERENCE(object_pointer);
AT_ENUM(endianity, DW_END);
AT_FLAG(elemental);
AT_FLAG(pure);
AT_FLAG(recursive);
AT_REFERENCE(signature);        // XXX Computed might be useful
AT_FLAG(main_subprogram);
// XXX const data_bit_offset
AT_FLAG(const_expr);
AT_FLAG(enum_class);
AT_STRING(linkage_name);

rangelist
die_pc_range(const die &d)
{
        // DWARF4 section 2.17
        if (d.has(DW_AT_NS::ranges))
                return at_ranges(d);
        taddr low = at_low_pc(d);
        taddr high = d.has(DW_AT_NS::high_pc) ? at_high_pc(d) : (low + 1);
        std::vector<std::pair<taddr, taddr> > construct;
        std::pair<taddr, taddr> pair(low, high);
        construct.push_back(pair);
        return rangelist(construct);
}

DWARFPP_END_NAMESPACE
