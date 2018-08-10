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
void func_inv_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_FUNCTION_INV;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(event_num); assert(OS_WriteFD(fd,&event_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(func_line_num); assert(OS_WriteFD(fd,&func_line_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(inv_line_num); assert(OS_WriteFD(fd,&inv_line_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert((length = strnlen(c_func_file,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,c_func_file,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert((length = strnlen(c_inv_file,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,c_inv_file,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert(
			(length = strnlen(func_name,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,func_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(address); assert(OS_WriteFD(fd,&address,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read an function invocation record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
vaccs_record *func_inv_record::read(NATIVE_FD fd) {
	USIZE size =  sizeof(event_num); assert(OS_ReadFD(fd,&size,&event_num).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(func_line_num); assert(OS_ReadFD(fd,&size,&func_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(inv_line_num); assert(OS_ReadFD(fd,&size,&inv_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((c_func_file = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,c_func_file).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_func_file[length] = '\0';

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((c_inv_file = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,c_inv_file).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_inv_file[length] = '\0';

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((func_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,func_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	func_name[length] = '\0';

	size =  sizeof(address); assert(OS_ReadFD(fd,&size,&address).generic_err == OS_RETURN_CODE_NO_ERROR);

	return this;
}

