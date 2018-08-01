/*
 * output_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include "output_record.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
output_record::output_record() :
		vaccs_record(VACCS_OUTPUT) {
	event_num = -1;
	output = NULL;
}

/**
 * Destructor
 */
output_record::~output_record() {
	sfree(output);
}

/**
 * Write an output record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void output_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_OUTPUT;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	assert(fwrite(&event_num, sizeof(event_num), 1, fp) == 1);

	size_t length;
	assert(
			(length = strnlen(output,VACCS_MAX_OUTPUT_LENGTH+1)) <= VACCS_MAX_OUTPUT_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(output, length, 1, fp) == 1);
}

/**
 * Read an output record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an output record
 */
vaccs_record *output_record::read(FILE *fp) {
	assert(fread(&event_num, sizeof(event_num), 1, fp) == 1);

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_OUTPUT_LENGTH);
	assert((output = (char *)malloc(length+1)) != NULL);
	assert(fread(output, length, 1, fp) == 1);
	output[length] = '\0';

	return this;
}

