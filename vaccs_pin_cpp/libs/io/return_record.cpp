/*
 * return_record.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <io/vaccs_record.h>
#include <io/return_record.h>

/**
 * Constructor
 */
return_record::return_record() :
		vaccs_record(VACCS_RETURN) {
	event_num = -1;
}
/**
 * Write a return record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
void return_record::write(FILE *fp) {
	vaccs_id_t id = VACCS_RETURN;
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	assert(fwrite(&event_num, sizeof(event_num), 1, fp) == 1);
}

/**
 * Read a return record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a return record
 */
vaccs_record *return_record::read(FILE *fp) {
	assert(fread(&event_num, sizeof(event_num), 1, fp) == 1);

	return this;
}

