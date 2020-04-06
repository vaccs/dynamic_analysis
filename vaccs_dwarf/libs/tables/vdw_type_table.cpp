/******************************************************************************
 *
 * File: type_table.cpp
 *
 * Summary: This file contains the code for building tables for mapping
 *          dwarf types to their attributes
 *
 *
 ******************************************************************************/

#include "vdw_type_table.h"

#include <util/general.h>
#include <cassert>
#include <cstring>

#include <sstream>


/**
 * The constructor. We use a builder pattern, so there are no parameters
 */
vdw_type_record::vdw_type_record() :
		symbol_table_record(TYPE_RECORD) {
	size = -1;
	name = NULL;
	is_array = false;
	is_pointer = false;
	is_struct = false;
	is_typedef = false;
	is_const = false;
	upper_bound = -1;
	base_type = NULL;
	member_tab = NULL;
}

vdw_type_record::~vdw_type_record() {}

/**
 * Add the upper bound to the object using a builder pattern
 *
 * @param upper_bound the upper bound of this array type
 * @return the object
 */
vdw_type_record* vdw_type_record::add_upper_bound(Generic upper_bound, bool compute_name_and_size) {
	this->upper_bound = upper_bound;
	if (compute_name_and_size) {
		this->name->append("[");

		std::ostringstream convert;
		convert << upper_bound;

		this->name->append(convert.str());

		this->name->append("]");
		this->size *= (upper_bound+1);
	}

	return this;
}

/**
 * Write a type table record to a file
 *
 * @param fn the file name for the cu
 * @param fp a file pointer
 */
void vdw_type_record::write(std::string key,FILE *fp) {
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	size_t length = key.length();
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(key.c_str(), length, 1, fp) == 1);
	assert(fwrite(&size,sizeof(size),1,fp) == 1);
	length = strlen(name->c_str());
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(name->c_str(), length, 1, fp) == 1);
	assert(fwrite(&is_array, sizeof(is_array), 1, fp) == 1);
	assert(fwrite(&upper_bound, sizeof(upper_bound), 1, fp) == 1);
	assert(fwrite(&is_pointer, sizeof(is_pointer), 1, fp) == 1);
	assert(fwrite(&is_struct, sizeof(is_struct), 1, fp) == 1);
	assert(fwrite(&is_const,sizeof(is_const),1,fp) == 1);
	assert(fwrite(&is_typedef,sizeof(is_typedef),1,fp) == 1);

	if (base_type != NULL) {
		length = base_type->length();
		assert(fwrite(&length, sizeof(length), 1, fp) == 1);
		assert(fwrite(base_type->c_str(), length, 1, fp) == 1);
	} else {
		length = 0;
		assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	}

	if (member_tab != NULL)
		member_tab->write(fp);
}

vdw_type_table::~vdw_type_table() {}
/**
 * Write the type table to a file
 *
 * @param fp a file pointer
 */
void vdw_type_table::write(FILE *fp) {
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	size_t num = size();
	assert(fwrite(&num,sizeof(size_t), 1, fp) == 1);

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		vdw_type_record* trec = (vdw_type_record*)it->second;
		trec->write(it->first,fp);
	}
}

std::string vdw_type_table::get_type_offset(std::string str) {
	std::string type = "";

	for (std::map<std::string,symbol_table_record*>::iterator it = begin();
			type.empty() && it != end();
			it++) {
		vdw_type_record *trec = (vdw_type_record *)it->second;
		if (*trec->get_name() == str)
			type = *trec->get_name();
	}

	return type;
}

void vdw_type_table::resolve_base_types() {
	for (std::map<std::string,symbol_table_record*>::iterator it = begin();
			it != end();
			it++) {
		vdw_type_record *trec = (vdw_type_record *)it->second;
		if (trec->get_is_pointer()) {
			vdw_type_record *btrec = get(*trec->get_base_type());
			trec->add_name(*btrec->get_name() + "*");
		} else if (trec->get_is_array()) {
			vdw_type_record *btrec = get(*trec->get_base_type());
			trec->add_size(btrec->get_size()*(trec->get_upper_bound()+1));
			trec->add_name(*btrec->get_name()+"[]");
		} else if (trec->get_is_const()) {
			vdw_type_record *btrec = get(*trec->get_base_type());
			trec->add_name("const " + *btrec->get_name());
		} else if (trec->get_is_typedef()) {
			trec->add_name("typedef " + *trec->get_name());
		}
	}

}
