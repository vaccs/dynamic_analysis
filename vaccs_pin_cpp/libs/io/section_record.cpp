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
void section_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_SECTION;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size_t length;
	assert(
			(length = strnlen(info,VACCS_MAX_OUTPUT_LENGTH+1)) <= VACCS_MAX_OUTPUT_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,info,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read an section record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an section record
 */
vaccs_record *section_record::read(NATIVE_FD fd) {

	size_t length;
	USIZE size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_OUTPUT_LENGTH);
	assert((info = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,info).generic_err == OS_RETURN_CODE_NO_ERROR);
	info[length] = '\0';

	return this;
}

