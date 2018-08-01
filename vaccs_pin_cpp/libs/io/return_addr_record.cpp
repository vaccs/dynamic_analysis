/*
 * return_addr_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/return_addr_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
return_addr_record::return_addr_record() :
		vaccs_record(VACCS_RETURN_ADDR) {
	dynamic_link = -1; /* the dynamic link */
	return_address = -1; /* the return address */
	c_func_name = NULL; /* function name */
}

/**
 * Destructor
 */
return_addr_record::~return_addr_record() {
	sfree(c_func_name);
}

/**
 * Write a return address record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void return_addr_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_RETURN_ADDR;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	assert(fwrite(&dynamic_link, sizeof(dynamic_link), 1, fp) == 1);
	assert(fwrite(&return_address, sizeof(return_address), 1, fp) == 1);

	size_t length;
	assert(
			(length = strnlen(c_func_name,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(c_func_name, length, 1, fp) == 1);
}

/**
 * Read a return address record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a return address record
 */
vaccs_record *return_addr_record::read(FILE *fp) {
	assert(fread(&dynamic_link, sizeof(dynamic_link), 1, fp) == 1);
	assert(fread(&return_address, sizeof(return_address), 1, fp) == 1);

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((c_func_name = (char *)malloc(length+1)) != NULL);
	assert(fread(c_func_name, length, 1, fp) == 1);
	c_func_name[length] = '\0';

	return this;
}

