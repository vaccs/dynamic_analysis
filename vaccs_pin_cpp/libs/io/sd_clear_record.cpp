/*
 * sd_clear_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/sd_clear_record.h>

#include <util/c_string_utils.h>
#include <pin.H>

/**
 * Constructor
 */
sd_clear_record::sd_clear_record() :
		vaccs_record(VACCS_SD_CLEAR) {
	event_num = -1; /* the event number */
	c_line_num = -1; /* the C line number */
	c_file_name = NULL; /* The C source file for this event */
	scope = NULL; /* the function name for the scope of the variable */
	variable = NULL; /* the sensitive data variable name */

}

/**
 * Destructor
 */
sd_clear_record::~sd_clear_record() {
	sfree(c_file_name);
	sfree(scope);
	sfree(variable);
}

/**
 * Write a sd clear record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void sd_clear_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_SD_CLEAR;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(event_num); assert(OS_WriteFD(fd,&event_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_line_num); assert(OS_WriteFD(fd,&c_line_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,c_file_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert(
			(length = strnlen(scope,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,scope,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert(
			(length = strnlen(variable,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,variable,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read a sd clear record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a sd clear record
 */
vaccs_record *sd_clear_record::read(NATIVE_FD fd) {
	USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd,&size,&event_num).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_line_num); assert(OS_ReadFD(fd,&size,&c_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,c_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_file_name[length] = '\0';

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((scope = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,scope).generic_err == OS_RETURN_CODE_NO_ERROR);
	scope[length] = '\0';

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((variable = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,variable).generic_err == OS_RETURN_CODE_NO_ERROR);
	variable[length] = '\0';

	return this;
}

