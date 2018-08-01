/*
 * sd_lock_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/sd_lock_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
sd_lock_record::sd_lock_record() :
		vaccs_record(VACCS_SD_LOCK) {
	event_num = -1;
	c_line_num = -1;
	c_file_name = NULL;
	scope = NULL;
	variable = NULL;
}

/**
 * Destructor
 */
sd_lock_record::~sd_lock_record() {
	sfree(c_file_name);
	sfree(scope);
	sfree(variable);
}
/**
 * Write an lock record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void sd_lock_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_SD_LOCK;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	assert(fwrite(&event_num, sizeof(event_num), 1, fp) == 1);
	assert(fwrite(&c_line_num, sizeof(c_line_num), 1, fp) == 1);

	size_t length;
	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(c_file_name, length, 1, fp) == 1);

	assert(
			(length = strnlen(scope,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(scope, length, 1, fp) == 1);

	assert(
			(length = strnlen(variable,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(variable, length, 1, fp) == 1);
}

/**
 * Read an lock record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an lock record
 */
vaccs_record *sd_lock_record::read(FILE *fp) {
	assert(fread(&event_num, sizeof(event_num), 1, fp) == 1);
	assert(fread(&c_line_num, sizeof(c_line_num), 1, fp) == 1);

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	assert(fread(c_file_name, length, 1, fp) == 1);
	c_file_name[length] = '\0';

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((scope = (char *)malloc(length+1)) != NULL);
	assert(fread(scope, length, 1, fp) == 1);
	scope[length] = '\0';

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((variable = (char *)malloc(length+1)) != NULL);
	assert(fread(variable, length, 1, fp) == 1);
	variable[length] = '\0';

	return this;
}

