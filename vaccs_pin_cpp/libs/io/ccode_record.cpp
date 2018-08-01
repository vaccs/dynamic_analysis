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
void ccode_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_CCODE;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	assert(fwrite(&c_line_num, sizeof(c_line_num), 1, fp) == 1);
	assert(fwrite(&c_start_pos, sizeof(c_start_pos), 1, fp) == 1);

	size_t length;
	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(c_file_name, length, 1, fp) == 1);

	assert((length = strnlen(c_src_line,VACCS_MAX_SRC_LINE_LENGTH+1)) <= VACCS_MAX_SRC_LINE_LENGTH);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);

	if (length == 0)
		assert(fputc('\n',fp) != EOF);
	else
		assert(fwrite(c_src_line, length, 1, fp) == 1);
}

/**
 * Read a C code record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @param p a pointer to an analysis record
 */
vaccs_record *ccode_record::read(FILE *fp) {
	assert(fread(&c_line_num, sizeof(c_line_num), 1, fp) == 1);
	assert(fread(&c_start_pos, sizeof(c_start_pos), 1, fp) == 1);

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	assert(fread(c_file_name, length, 1, fp) == 1);
	c_file_name[length] = '\0';

	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= VACCS_MAX_SRC_LINE_LENGTH);
	assert((c_src_line = (char *)malloc(length+1)) != NULL);
	assert(fread(c_src_line, length, 1, fp) == 1);
	c_src_line[length] = '\0';

	return this;
}

