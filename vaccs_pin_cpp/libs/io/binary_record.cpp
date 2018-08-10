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
void binary_record::write(NATIVE_FD fp) {
	vaccs_id_t id = VACCS_BINARY;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fp,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size_t length;
	assert((length = strnlen(bin_file_name,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fp,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fp,bin_file_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read a binary record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
vaccs_record *binary_record::read(NATIVE_FD fp) {

	size_t length;
	USIZE size =  sizeof(length); assert(OS_ReadFD(fp,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((bin_file_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fp,&size,bin_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	bin_file_name[length] = '\0';

	return this;
}

