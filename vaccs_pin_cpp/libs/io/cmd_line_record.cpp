/*
 * cmd_line_record.cpp
 *
 *  Created on: Jul 2, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>

#include <io/cmd_line_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
cmd_line_record::cmd_line_record() :
		vaccs_record(VACCS_CMD_LINE) {
	cmd_line = NULL;
}

/**
 * Destructor
 */
cmd_line_record::~cmd_line_record() {
	sfree(cmd_line);
}

/**
 * Write a command line record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void cmd_line_record::write(NATIVE_FD fp) {
	vaccs_id_t id = VACCS_CMD_LINE;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fp,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert((length = strnlen(cmd_line,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fp,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fp,cmd_line,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read a command line record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @param p a pointer to an analysis record
 */
vaccs_record *cmd_line_record::read(NATIVE_FD fp) {

	size_t length;
	USIZE size =  sizeof(length); assert(OS_ReadFD(fp,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((cmd_line = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fp,&size,cmd_line).generic_err == OS_RETURN_CODE_NO_ERROR);
	cmd_line[length] = '\0';

	return this;
}
