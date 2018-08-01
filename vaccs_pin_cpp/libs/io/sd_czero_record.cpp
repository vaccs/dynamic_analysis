/*
 * sd_czero_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/sd_czero_record.h>

#include <util/c_string_utils.h>

/**
 * Constructor
 */
sd_czero_record::sd_czero_record() :
		vaccs_record(VACCS_SD_CZERO) {
	event_num = -1;
	c_line_num = -1;
	c_file_name = NULL;

}

/**
 * Destructor
 */
sd_czero_record::~sd_czero_record() {
	sfree(c_file_name);
}

/**
 * Write an czero record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void sd_czero_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_SD_CZERO;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	assert(fwrite(&event_num, sizeof(event_num), 1, fp) == 1);
	assert(fwrite(&c_line_num, sizeof(c_line_num), 1, fp) == 1);

	size_t length;
	assert((length = strnlen(c_file_name,PATH_MAX+1)) <= PATH_MAX);
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(c_file_name, length, 1, fp) == 1);
}

/**
 * Read an czero record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return an czero record
 */
vaccs_record *sd_czero_record::read(FILE *fp) {
	assert(fread(&event_num, sizeof(event_num), 1, fp) == 1);
	assert(fread(&c_line_num, sizeof(c_line_num), 1, fp) == 1);

	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1);
	assert(length <= PATH_MAX);
	assert((c_file_name = (char *)malloc(length+1)) != NULL);
	assert(fread(c_file_name, length, 1, fp) == 1);
	c_file_name[length] = '\0';

	return this;
}

