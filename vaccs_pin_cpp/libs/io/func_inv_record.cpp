/*
 * func_inv_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/func_inv_record.h>

#include <util/c_string_utils.h>
/**
 * Constructor
 */
func_inv_record::func_inv_record() :
		vaccs_record(VACCS_FUNCTION_INV) {
	event_num = -1; /* The runtime event number for this invocation */
	func_line_num = -1; /* The C source line number of the callee */
	inv_line_num = -1; /* The C source line number of the invocation */
	c_func_file = NULL; /* The C source file of the callee */
	c_inv_file = NULL; /* The C source file of the invocation */
	func_name = NULL; /* The name of the callee */
	address = -1; /* The address of the callee */
}

/**
 * Destructor
 */
func_inv_record::~func_inv_record() {
	sfree(c_func_file);
	sfree(c_inv_file);
	sfree(func_name);
}

/**
 * Write a function invocation record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void func_inv_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_FUNCTION_INV;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	assert(fwrite(&event_num, sizeof(event_num), 1, fp) == 1);
	assert(fwrite(&func_line_num, sizeof(func_line_num), 1, fp) == 1);
	assert(fwrite(&inv_line_num, sizeof(inv_line_num), 1, fp) == 1);

	size_t length;
	assert((length = strnlen(c_func_file,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(c_func_file, length, 1, fp) == 1);

	assert((length = strnlen(c_inv_file,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(c_inv_file, length, 1, fp) == 1);

	assert(
			(length = strnlen(func_name,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(func_name, length, 1, fp) == 1);

	assert(fwrite(&address, sizeof(address), 1, fp) == 1);
}

/**
 * Read an function invocation record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
vaccs_record *func_inv_record::read(FILE *fp) {
	assert(fread(&event_num, sizeof(event_num), 1, fp) == 1);
	assert(fread(&func_line_num, sizeof(func_line_num), 1, fp) == 1);
	assert(fread(&inv_line_num, sizeof(inv_line_num), 1, fp) == 1);

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((c_func_file = (char *)malloc(length+1)) != NULL);
	assert(fread(c_func_file, length, 1, fp) == 1);
	c_func_file[length] = '\0';

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((c_inv_file = (char *)malloc(length+1)) != NULL);
	assert(fread(c_inv_file, length, 1, fp) == 1);
	c_inv_file[length] = '\0';

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((func_name = (char *)malloc(length+1)) != NULL);
	assert(fread(func_name, length, 1, fp) == 1);
	func_name[length] = '\0';

	assert(fread(&address, sizeof(address), 1, fp) == 1);

	return this;
}

