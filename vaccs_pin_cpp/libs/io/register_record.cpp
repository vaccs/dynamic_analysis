/*
 * register_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include "register_record.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <util/c_string_utils.h>

/**
 * Constructor
 */
register_record::register_record() :
		vaccs_record(VACCS_REGISTER) {
	event_num = -1; /* The runtime event number for this invocation */
	c_line_num = -1; /* The C source line number for this event */
	c_file_name = NULL; /* The C source file for this event */
	register_name = NULL; /* the name of the register */
	value = -1; /* The new register value */
}

/**
 * Destructor
 */
register_record::~register_record() {
	sfree(c_file_name);
	sfree(register_name);
}

/**
 * Write a register record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void register_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_REGISTER;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	assert(fwrite(&event_num, sizeof(event_num), 1, fp) == 1);
	assert(fwrite(&c_line_num, sizeof(c_line_num), 1, fp) == 1);

	size_t length;
	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(c_file_name, length, 1, fp) == 1);

	assert(
			(length = strnlen(register_name,VACCS_MAX_REGISTER_NAME_LENGTH+1)) <= VACCS_MAX_REGISTER_NAME_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(register_name, length, 1, fp) == 1);

	assert(fwrite(&value, sizeof(value), 1, fp) == 1);
}

/**
 * Read a register record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a register record
 */
vaccs_record *register_record::read(FILE *fp) {
	assert(fread(&event_num, sizeof(event_num), 1, fp) == 1);
	assert(fread(&c_line_num, sizeof(c_line_num), 1, fp) == 1);

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	assert(fread(c_file_name, length, 1, fp) == 1);
	c_file_name[length] = '\0';

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_REGISTER_NAME_LENGTH);
	assert((register_name = (char *)malloc(length+1)) != NULL);
	assert(fread(register_name, length, 1, fp) == 1);
	register_name[length] = '\0';

	assert(fread(&value, sizeof(value), 1, fp) == 1);

	return this;
}

