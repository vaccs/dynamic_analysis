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
void var_access_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_VAR_ACCESS;
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

	size =  sizeof(address); assert(OS_WriteFD(fd,&address,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert(
			(length = strnlen(type,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,type,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert(
			(length = strnlen(name,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(points_to); assert(OS_WriteFD(fd,&points_to,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert(
			(length = strnlen(value,VACCS_MAX_VALUE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,value,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read an var_access record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an var_access record
 */
vaccs_record *var_access_record::read(NATIVE_FD fd) {

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

	size =  sizeof(address); assert(OS_ReadFD(fd,&size,&address).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((type = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,type).generic_err == OS_RETURN_CODE_NO_ERROR);
	type[length] = '\0';

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,name).generic_err == OS_RETURN_CODE_NO_ERROR);
	name[length] = '\0';

	size =  sizeof(points_to); assert(OS_ReadFD(fd,&size,&points_to).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_VALUE_LENGTH);
	assert((value = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,value).generic_err == OS_RETURN_CODE_NO_ERROR);
	value[length] = '\0';

	return this;
}

