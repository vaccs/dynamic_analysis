/*
 * section_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/section_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
section_record::section_record() :
		vaccs_record(VACCS_SECTION) {
	info = NULL;
}

/**
 * Destructor
 */
section_record::~section_record() {
	sfree(info);
}

/**
 * Write an section record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void section_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_SECTION;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	size_t length;
	assert(
			(length = strnlen(info,VACCS_MAX_OUTPUT_LENGTH+1)) <= VACCS_MAX_OUTPUT_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(info, length, 1, fp) == 1);
}

/**
 * Read an section record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an section record
 */
vaccs_record *section_record::read(FILE *fp) {

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_OUTPUT_LENGTH);
	assert((info = (char *)malloc(length+1)) != NULL);
	assert(fread(info, length, 1, fp) == 1);
	info[length] = '\0';

	return this;
}

