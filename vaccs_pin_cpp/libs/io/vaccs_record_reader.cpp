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
	fp = NULL;
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

	if (fread(&id, sizeof(vaccs_id_t), 1, fp) == 1) {

		assert((rec = factory->make_vaccs_record(id)) != NULL);
		return rec->read(fp);

	} else {
		assert(feof(fp)); // fail if there is some kind of read error other than reaching the end of the file
		return NULL;
	}
}

