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
void cmd_line_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_CMD_LINE;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	size_t length;
	assert((length = strnlen(cmd_line,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(cmd_line, length, 1, fp) == 1);
}

/**
 * Read a command line record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @param p a pointer to an analysis record
 */
vaccs_record *cmd_line_record::read(FILE *fp) {

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((cmd_line = (char *)malloc(length+1)) != NULL);
	assert(fread(cmd_line, length, 1, fp) == 1);
	cmd_line[length] = '\0';

	return this;
}
