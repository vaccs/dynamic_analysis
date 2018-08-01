/*
 * binary_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/binary_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
binary_record::binary_record() :
		vaccs_record(VACCS_BINARY) {
	bin_file_name = NULL;
}

/**
 * Destructor
 */
binary_record::~binary_record() {
	sfree(bin_file_name);
}
/**
 * Write a binary record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void binary_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_BINARY;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	size_t length;
	assert((length = strnlen(bin_file_name,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(bin_file_name, length, 1, fp) == 1);
}

/**
 * Read a binary record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
vaccs_record *binary_record::read(FILE *fp) {

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((bin_file_name = (char *)malloc(length+1)) != NULL);
	assert(fread(bin_file_name, length, 1, fp) == 1);
	bin_file_name[length] = '\0';

	return this;
}

