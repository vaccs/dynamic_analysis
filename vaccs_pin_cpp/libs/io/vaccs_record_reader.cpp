/*
 * vaccs_record_reader.c
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdlib>

#include <cassert>

#include <io/vaccs_record_reader.h>
#include <io/vaccs_record.h>

/**
 * Constructor
 */
vaccs_record_reader::vaccs_record_reader() {
	fd = -1;
	factory = NULL;
}

vaccs_record_reader::~vaccs_record_reader() {
	delete factory;
}

/**
 * Read a vaccs_record object
 *
 * @return the object read
 */
vaccs_record *vaccs_record_reader::read_next_vaccs_record() {

	vaccs_record *rec;
	vaccs_id_t id;
	USIZE size = sizeof(id);
	if (OS_ReadFD(fd,&size,&id).generic_err == OS_RETURN_CODE_NO_ERROR) {

		assert((rec = factory->make_vaccs_record(id)) != NULL);
		return rec->read(fd);

	} else {
		return NULL;
	}
}

