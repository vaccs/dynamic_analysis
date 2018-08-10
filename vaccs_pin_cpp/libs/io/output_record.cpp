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
void output_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_OUTPUT;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(event_num); assert(OS_WriteFD(fd,&event_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert(
			(length = strnlen(output,VACCS_MAX_OUTPUT_LENGTH+1)) <= VACCS_MAX_OUTPUT_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,output,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read an output record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an output record
 */
vaccs_record *output_record::read(NATIVE_FD fd) {
	USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd,&size,&event_num).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_OUTPUT_LENGTH);
	assert((output = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,output).generic_err == OS_RETURN_CODE_NO_ERROR);
	output[length] = '\0';

	return this;
}

