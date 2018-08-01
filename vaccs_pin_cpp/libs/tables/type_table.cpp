/******************************************************************************
 *
 * File: type_table.cpp
 *
 * Summary: This file contains the code for building tables for mapping
 *          dwarf types to their attributes
 *
 *
 ******************************************************************************/

#include <util/general.h>
#include <tables/type_table.h>

#include <cassert>
#include <cstring>

#include <sstream>


/**
 * The constructor. We use a builder pattern, so there are no parameters
 */
type_record::type_record() :
		symbol_table_record(TYPE_RECORD) {
	size = -1;
	name = NULL;
	is_array = false;
	upper_bound = -1;
	base_type = NULL;
}

type_record::~type_record() {}

/**
 * Add the upper bound to the object using a builder pattern
 *
 * @param upper_bound the upper bound of this array type
 * @return the object
 */
type_record* type_record::add_upper_bound(Generic upper_bound, bool compute_name_and_size) {
	this->upper_bound = upper_bound;
	if (compute_name_and_size) {
		this->name->append("[");

		std::ostringstream convert;
		convert << upper_bound;

		this->name->append(convert.str());

		this->name->append("]");
		this->size *= upper_bound;
	}

	return this;
}

/**
 * Write a type table record to a file
 *
 * @param fn the file name for the cu
 * @param fp a file pointer
 */
void type_record::write(std::string key,FILE *fp) {
	printf("Begin type_record::write()\n");
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

	if (base_type != NULL) {
		length = base_type->length();
		assert(fwrite(&length, sizeof(length), 1, fp) == 1);
		assert(fwrite(base_type->c_str(), length, 1, fp) == 1);
	} else {
		length = 0;
		assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	}
	printf("End type_record::write()\n");
}

type_table::~type_table() {}
/**
 * Write the type table to a file
 *
 * @param fp a file pointer
 */
void type_table::write(FILE *fp) {
	printf("Begin type_table::write()\n");
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	size_t num = size();
	assert(fwrite(&num,sizeof(size_t), 1, fp) == 1);

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		type_record* trec = (type_record*)it->second;
		trec->write(it->first,fp);
	}
	printf("End type_table::write()\n");

}
