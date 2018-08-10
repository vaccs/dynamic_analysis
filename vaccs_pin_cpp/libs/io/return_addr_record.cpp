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
void return_addr_record::write(NATIVE_FD fd) {
	vaccs_id_t id = VACCS_RETURN_ADDR;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(dynamic_link); assert(OS_WriteFD(fd,&dynamic_link,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(return_address); assert(OS_WriteFD(fd,&return_address,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert(
			(length = strnlen(c_func_name,VACCS_MAX_VARIABLE_LENGTH+1)) <= VACCS_MAX_VARIABLE_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,c_func_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read a return address record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a return address record
 */
vaccs_record *return_addr_record::read(NATIVE_FD fd) {
	USIZE size =  sizeof(dynamic_link); assert(OS_ReadFD(fd,&size,&dynamic_link).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(return_address); assert(OS_ReadFD(fd,&size,&return_address).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_VARIABLE_LENGTH);
	assert((c_func_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,c_func_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_func_name[length] = '\0';

	return this;
}

