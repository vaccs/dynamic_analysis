// Copyright (c) 2013 Austin T. Clements. All rights reserved.
// Use of this source code is governed by an MIT license
// that can be found in the LICENSE file.

#include "dwarf++.hh"

#include <cstring>

using namespace std;

DWARFPP_BEGIN_NAMESPACE

struct Section
{
        const char *name;
        section_type type;
};

Section sections[] = {
        {".debug_abbrev",   section_type_ns::abbrev},
        {".debug_aranges",  section_type_ns::aranges},
        {".debug_frame",    section_type_ns::frame},
        {".debug_info",     section_type_ns::info},
        {".debug_line",     section_type_ns::line},
        {".debug_loc",      section_type_ns::loc},
        {".debug_macinfo",  section_type_ns::macinfo},
        {".debug_pubnames", section_type_ns::pubnames},
        {".debug_pubtypes", section_type_ns::pubtypes},
        {".debug_ranges",   section_type_ns::ranges},
        {".debug_str",      section_type_ns::str},
        {".debug_types",    section_type_ns::types},
};

bool
elf::section_name_to_type(const char *name, section_type *out)
{
        for (size_t i = 0; i < sizeof(sections); i++) {
                const Section &sec = sections[i];
                if (strcmp(sec.name, name) == 0) {
                        *out = sec.type;
                        return true;
                }
        }
        return false;
}

const char *
elf::section_type_to_name(section_type type)
{
        for (size_t i = 0; i < sizeof(sections); i++) {
                const Section &sec = sections[i];
                if (sec.type == type)
                        return sec.name;
        }
        return NULL;
}

DWARFPP_END_NAMESPACE
