/*
 * ccode_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/ccode_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
ccode_record::ccode_record() :
		vaccs_record(VACCS_CCODE) {
	c_line_num = -1; /* The line number of this source line in the C file */
	c_start_pos = -1; /* The character starting position of this source line */
	c_file_name = NULL; /* The name of the file in which this source line appears */
	c_src_line = NULL; /* The C source line itself */
}

/**
 * Destructor
 */
ccode_record::~ccode_record() {
	sfree(c_file_name);
	sfree(c_src_line);
}

/**
 * Write a C code record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void ccode_record::write(NATIVE_FD fp) {
	vaccs_id_t id = VACCS_CCODE;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fp,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_line_num); assert(OS_WriteFD(fp,&c_line_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_start_pos); assert(OS_WriteFD(fp,&c_start_pos,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fp,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fp,c_file_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert((length = strnlen(c_src_line,VACCS_MAX_SRC_LINE_LENGTH+1)) <= VACCS_MAX_SRC_LINE_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fp,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	if (length != 0) {
		size =  length; assert(OS_WriteFD(fp,c_src_line,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	}
}

/**
 * Read a C code record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @param p a pointer to an analysis record
 */
vaccs_record *ccode_record::read(NATIVE_FD fp) {
	USIZE size =  sizeof(c_line_num); assert(OS_ReadFD(fp,&size,&c_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(c_start_pos); assert(OS_ReadFD(fp,&size,&c_start_pos).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	size =  sizeof(length); assert(OS_ReadFD(fp,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fp,&size,c_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_file_name[length] = '\0';

	size =  sizeof(length); assert(OS_ReadFD(fp,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_SRC_LINE_LENGTH);
	assert((c_src_line = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fp,&size,c_src_line).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_src_line[length] = '\0';

	return this;
}

