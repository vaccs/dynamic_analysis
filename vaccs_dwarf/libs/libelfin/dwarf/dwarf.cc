// Copyright (c) 2013 Austin T. Clements. All rights reserved.
// Use of this source code is governed by an MIT license
// that can be found in the LICENSE file.

#include "internal.hh"

using namespace std;

DWARFPP_BEGIN_NAMESPACE

//////////////////////////////////////////////////////////////////
// class dwarf
//

struct dwarf::impl
{
        impl(const loader *l)
                : l(l), have_type_units(false) { }

        loader * l;

        section * sec_info;
        section * sec_abbrev;

        std::vector<compilation_unit> compilation_units;

        std::map<uint64_t, type_unit> type_units;
        bool have_type_units;

        std::map<section_type, section>  * sections;
};

dwarf::dwarf(const loader *l)
        : m(new impl(l))
{
        const void *data;
        size_t size;

        // Get required sections
        data = l->load(section_type::info, &size);
        if (!data)
                throw format_error("required .debug_info section missing");
        m->sec_info = new section(section_type::info, data, size, byte_order::lsb);

        // Sniff the endianness from the version field of the first
        // CU. This is always a small but non-zero integer.
        cursor endcur(m->sec_info);
        // Skip length.
        section_length length = endcur.fixed<uword>();
        if (length == 0xffffffff)
                endcur.fixed<uint64_t>();
        // Get version in both little and big endian.
        uhalf version = endcur.fixed<uhalf>();
        uhalf versionbe = (version >> 8) | ((version & 0xFF) << 8);
        if (versionbe < version) {
                m->sec_info = new section(section_type::info, data, size, byte_order::msb);
        }

        data = l->load(section_type::abbrev, &size);
        if (!data)
                throw format_error("required .debug_abbrev section missing");
        m->sec_abbrev = new section(section_type::abbrev, data, size, m->sec_info->ord);

        // Get compilation units.  Everything derives from these, so
        // there's no point in doing it lazily.
        cursor infocur(m->sec_info);
        while (!infocur.end()) {
                // XXX Circular reference.  Given that we now require
                // the dwarf object to stick around for DIEs, maybe we
                // might as well require that for units, too.
                m->compilation_units.emplace_back(
                        *this, infocur.get_section_offset());
                infocur.subsection();
        }
}

dwarf::~dwarf()
{
}

const std::vector<compilation_unit> &
dwarf::compilation_units() const
{
        static std::vector<compilation_unit> empty;
        if (!m)
                return empty;
        return m->compilation_units;
}

const type_unit &
dwarf::get_type_unit(uint64_t type_signature) const
{
        if (!m->have_type_units) {
                cursor tucur(get_section(section_type::types));
                while (!tucur.end()) {
                        // XXX Circular reference
                        type_unit tu(*this, tucur.get_section_offset());
                        m->type_units[tu.get_type_signature()] = tu;
                        tucur.subsection();
                }
                m->have_type_units = true;
        }
        if (!m->type_units.count(type_signature))
                throw out_of_range("type signature 0x" + to_hex(type_signature));
        return m->type_units[type_signature];
}

section *
dwarf::get_section(section_type type) const
{
        if (type == section_type::info)
                return m->sec_info;
        if (type == section_type::abbrev)
                return m->sec_abbrev;

        std::map<section_type, section>::iterator it = m->sections.find(type);
        if (it != m->sections.end())
                return it->second;

        size_t size;
        const void *data = m->l->load(type, &size);
        if (!data)
                throw format_error(std::string(elf::section_type_to_name(type))
                                   + " section missing");
        m->sections[type] = new section(section_type::str, data, size, m->sec_info->ord);
        return m->sections[type];
}

//////////////////////////////////////////////////////////////////
// class unit
//

/**
 * Implementation of a unit.
 */
struct unit::impl
{
        const dwarf file;
        const section_offset offset;
        const section * subsec;
        const section_offset debug_abbrev_offset;
        const section_offset root_offset;

        // Type unit-only values
        const uint64_t type_signature;
        const section_offset type_offset;

        // Lazily constructed root and type DIEs
        die root, type;

        // Lazily constructed line table
        line_table lt;

        // Map from abbrev code to abbrev.  If the map is dense, it
        // will be stored in the vector; otherwise it will be stored
        // in the map.
        bool have_abbrevs;
        std::vector<abbrev_entry> abbrevs_vec;
        std::map<abbrev_code, abbrev_entry> abbrevs_map;

        impl(const dwarf &file, section_offset offset,
             const section *subsec,
             section_offset debug_abbrev_offset, section_offset root_offset,
             uint64_t type_signature = 0, section_offset type_offset = 0)
                : file(file), offset(offset), subsec(subsec),
                  debug_abbrev_offset(debug_abbrev_offset),
                  root_offset(root_offset), type_signature(type_signature),
                  type_offset(type_offset), have_abbrevs(false) { }

        void force_abbrevs();
};

unit::~unit()
{
}

const dwarf &
unit::get_dwarf() const
{
        return m->file;
}

section_offset
unit::get_section_offset() const
{
        return m->offset;
}

const die&
unit::root() const
{
        if (!m->root.valid()) {
                m->force_abbrevs();
                m->root = die(this);
                m->root.read(m->root_offset);
        }
        return m->root;
}

const section *
unit::data() const
{
        return m->subsec;
}

const abbrev_entry &
unit::get_abbrev(abbrev_code acode) const
{
        if (!m->have_abbrevs)
                m->force_abbrevs();

        if (!m->abbrevs_vec.empty()) {
                if (acode >= m->abbrevs_vec.size())
                        goto unknown;
                const abbrev_entry &entry = m->abbrevs_vec[acode];
                if (entry.code == 0)
                        goto unknown;
                return entry;
        } else {
                std::map<abbrev_code, abbrev_entry>::iterator it = m->abbrevs_map.find(acode);
                if (it == m->abbrevs_map.end())
                        goto unknown;
                return it->second;
        }

unknown:
        throw format_error("unknown abbrev code 0x" + to_hex(acode));
}

void
unit::impl::force_abbrevs()
{
        // XXX Compilation units can share abbrevs.  Parse each table
        // at most once.
        if (have_abbrevs)
                return;

        // Section 7.5.3
        cursor c(file.get_section(section_type::abbrev),
                 debug_abbrev_offset);
        abbrev_entry entry;
        abbrev_code highest = 0;
        while (entry.read(&c)) {
                abbrevs_map[entry.code] = entry;
                if (entry.code > highest)
                        highest = entry.code;
        }

        // Typically, abbrev codes are assigned linearly, so it's more
        // space efficient and time efficient to store the table in a
        // vector.  Convert to a vector if it's dense enough, by some
        // rough estimate of "enough".
        if (highest * 10 < abbrevs_map.size() * 15) {
                // Move the map into the vector
                abbrevs_vec.resize(highest + 1);
                for (std::map<abbrev_code, abbrev_entry>::iterator it = abbrevs_map.begin(); it != abbrevs_map.end(); it++)
                        abbrevs_vec[it->first] = move(it->second);
                abbrevs_map.clear();
        }

        have_abbrevs = true;
}

//////////////////////////////////////////////////////////////////
// class compilation_unit
//

compilation_unit::compilation_unit(const dwarf &file, section_offset offset)
{
        // Read the CU header (DWARF4 section 7.5.1.1)
        cursor cur(file.get_section(section_type::info), offset);
        section * subsec = cur.subsection();
        cursor sub(subsec);
        sub.skip_initial_length();
        uhalf version = sub.fixed<uhalf>();
        if (version < 2 || version > 4)
                throw format_error("unknown compilation unit version " + std::to_string(version));
        // .debug_abbrev-relative offset of this unit's abbrevs
        section_offset debug_abbrev_offset = sub.offset();
        ubyte address_size = sub.fixed<ubyte>();
        subsec->addr_size = address_size;

        m = new impl(file, offset, subsec, debug_abbrev_offset,
                              sub.get_section_offset());
}

const line_table &
compilation_unit::get_line_table() const
{
        if (!m->lt.valid()) {
                const die &d = root();
                if (!d.has(DW_AT::stmt_list) || !d.has(DW_AT::name))
                        goto done;

                shared_ptr<section> sec;
                try {
                        sec = m->file.get_section(section_type::line);
                } catch (format_error &e) {
                        goto done;
                }

                std::string comp_dir = d.has(DW_AT::comp_dir) ? at_comp_dir(d) : "";
                
                m->lt = line_table(sec, d[DW_AT::stmt_list].as_sec_offset(),
                                   m->subsec->addr_size, comp_dir,
                                   at_name(d));
        }
done:
        return m->lt;
}

//////////////////////////////////////////////////////////////////
// class type_unit
//

type_unit::type_unit(const dwarf &file, section_offset offset)
{
        // Read the type unit header (DWARF4 section 7.5.1.2)
        cursor cur(file.get_section(section_type::types), offset);
        section * subsec = cur.subsection();
        cursor sub(subsec);
        sub.skip_initial_length();
        uhalf version = sub.fixed<uhalf>();
        if (version != 4)
                throw format_error("unknown type unit version " + std::to_string(version));
        // .debug_abbrev-relative offset of this unit's abbrevs
        section_offset debug_abbrev_offset = sub.offset();
        ubyte address_size = sub.fixed<ubyte>();
        subsec->addr_size = address_size;
        uint64_t type_signature = sub.fixed<uint64_t>();
        section_offset type_offset = sub.offset();

        m = new impl(file, offset, subsec, debug_abbrev_offset,
                              sub.get_section_offset(), type_signature,
                              type_offset);
}

uint64_t
type_unit::get_type_signature() const
{
        return m->type_signature;
}

const die &
type_unit::type() const
{
        if (!m->type.valid()) {
                m->force_abbrevs();
                m->type = die(this);
                m->type.read(m->type_offset);
        }
        return m->type;
}

DWARFPP_END_NAMESPACE
