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

#include <pin.H>

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
void register_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_REGISTER;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(event_num); assert(OS_WriteFD(fd,&event_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_line_num); assert(OS_WriteFD(fd,&c_line_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,c_file_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert(
			(length = strnlen(register_name,VACCS_MAX_REGISTER_NAME_LENGTH+1)) <= VACCS_MAX_REGISTER_NAME_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,register_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(value); assert(OS_WriteFD(fd,&value,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read a register record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a register record
 */
vaccs_record *register_record::read(NATIVE_FD fd) {
	USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd,&size,&event_num).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_line_num); assert(OS_ReadFD(fd,&size,&c_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,c_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_file_name[length] = '\0';

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_REGISTER_NAME_LENGTH);
	assert((register_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,register_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	register_name[length] = '\0';

	size =  sizeof(value); assert(OS_ReadFD(fd,&size,&value).generic_err == OS_RETURN_CODE_NO_ERROR);

	return this;
}

