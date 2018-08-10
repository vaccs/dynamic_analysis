/*
 * asm_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/asm_record.h>

#include <util/c_string_utils.h>

#include <pin.H>

/**
 * Constructor
 */
asm_record::asm_record() :
		vaccs_record(VACCS_ASM) {
	asm_line_num = -1; /* The assembly line number */
	c_line_num = -1; /* The C line number for this assembly line */
	asm_file_name = NULL; /* ASM file name */
	c_file_name = NULL; /* C file name */
	asm_inst = NULL; /* The assembly instruction */
}

/**
 * Destructor
 */
asm_record::~asm_record() {
	sfree(asm_file_name);
	sfree(c_file_name);
	sfree(asm_inst);
}
/**
 * Write an assembly record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @param asm_ptr a pointer to an assembly record
 */
void asm_record::write(NATIVE_FD fp) {
	vaccs_id_t id = VACCS_ASM;
	USIZE size =  sizeof(id); assert(OS_WriteFD(fp,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(vaccs_line_num_t); assert(OS_WriteFD(fp,&asm_line_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(vaccs_line_num_t); assert(OS_WriteFD(fp,&c_line_num,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	assert((length = strnlen(asm_file_name,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fp,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fp,asm_file_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	size =  sizeof(length); assert(OS_WriteFD(fp,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fp,c_file_name,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	assert(
			(length = strnlen(asm_inst,VACCS_MAX_SRC_LINE_LENGTH+1)) <= VACCS_MAX_SRC_LINE_LENGTH);
	size =  sizeof(length); assert(OS_WriteFD(fp,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fp,asm_inst,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
}

/**
 * Read an assembly record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @param p a pointer to an analysis record
 */
vaccs_record *asm_record::read(NATIVE_FD fp) {

	USIZE size =  sizeof(vaccs_line_num_t); assert(OS_ReadFD(fp,&size,&asm_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(vaccs_line_num_t); assert(OS_ReadFD(fp,&size,&c_line_num).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t length;
	size =  sizeof(length); assert(OS_ReadFD(fp,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((asm_file_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fp,&size,asm_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	asm_file_name[length] = '\0';

	size =  sizeof(length); assert(OS_ReadFD(fp,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fp,&size,c_file_name).generic_err == OS_RETURN_CODE_NO_ERROR);
	c_file_name[length] = '\0';

	size =  sizeof(length); assert(OS_ReadFD(fp,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);
	assert(length <= VACCS_MAX_SRC_LINE_LENGTH);
	assert((asm_inst = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fp,&size,asm_inst).generic_err == OS_RETURN_CODE_NO_ERROR);
	asm_inst[length] = '\0';

	return this;
}

