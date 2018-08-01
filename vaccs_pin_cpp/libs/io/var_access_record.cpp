/*
 * var_access_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/var_access_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
var_access_record::var_access_record() :
		vaccs_record(VACCS_VAR_ACCESS) {
	event_num = -1;
	c_line_num = -1;
	c_file_name = NULL;
	scope = NULL;
	address = -1;
	name = NULL;
	points_to = -1;
	value = NULL;
	type = NULL;
}

/**
 * Constructor
 */
var_access_record::~var_access_record() {
	sfree(c_file_name);
	sfree(scope);
	sfree(name);
	sfree(value);
	sfree(type);
}

/**
 * Write an var_access record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void var_access_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_VAR_ACCESS;
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

	assert(fwrite(&address, sizeof(address), 1, fp) == 1);

	assert(
			(length = strnlen(type,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(type, length, 1, fp) == 1);

	assert(
			(length = strnlen(name,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(name, length, 1, fp) == 1);

	assert(fwrite(&points_to, sizeof(points_to), 1, fp) == 1);

	assert(
			(length = strnlen(value,VACCS_MAX_VALUE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(value, length, 1, fp) == 1);
}

/**
 * Read an var_access record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an var_access record
 */
vaccs_record *var_access_record::read(FILE *fp) {

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

	assert(fread(&address, sizeof(address), 1, fp) == 1);

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((type = (char *)malloc(length+1)) != NULL);
	assert(fread(type, length, 1, fp) == 1);
	type[length] = '\0';

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((name = (char *)malloc(length+1)) != NULL);
	assert(fread(name, length, 1, fp) == 1);
	name[length] = '\0';

	assert(fread(&points_to, sizeof(points_to), 1, fp) == 1);

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_VALUE_LENGTH);
	assert((value = (char *)malloc(length+1)) != NULL);
	assert(fread(value, length, 1, fp) == 1);
	value[length] = '\0';

	return this;
}

