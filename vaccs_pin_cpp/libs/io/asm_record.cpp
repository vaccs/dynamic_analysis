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
void asm_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_ASM;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	assert(fwrite(&asm_line_num, sizeof(vaccs_line_num_t), 1, fp) == 1);
	assert(fwrite(&c_line_num, sizeof(vaccs_line_num_t), 1, fp) == 1);

	size_t length;
	assert((length = strnlen(asm_file_name,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(asm_file_name, length, 1, fp) == 1);

	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(c_file_name, length, 1, fp) == 1);

	assert(
			(length = strnlen(asm_inst,VACCS_MAX_SRC_LINE_LENGTH+1)) <= VACCS_MAX_SRC_LINE_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(asm_inst, length, 1, fp) == 1);
}

/**
 * Read an assembly record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @param p a pointer to an analysis record
 */
vaccs_record *asm_record::read(FILE *fp) {

	assert(fread(&asm_line_num, sizeof(vaccs_line_num_t), 1, fp) == 1);
	assert(fread(&c_line_num, sizeof(vaccs_line_num_t), 1, fp) == 1);

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((asm_file_name = (char *)malloc(length+1)) != NULL);
	assert(fread(asm_file_name, length, 1, fp) == 1);
	asm_file_name[length] = '\0';

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	assert(fread(c_file_name, length, 1, fp) == 1);
	c_file_name[length] = '\0';

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_SRC_LINE_LENGTH);
	assert((asm_inst = (char *)malloc(length+1)) != NULL);
	assert(fread(asm_inst, length, 1, fp) == 1);
	asm_inst[length] = '\0';

	return this;
}

