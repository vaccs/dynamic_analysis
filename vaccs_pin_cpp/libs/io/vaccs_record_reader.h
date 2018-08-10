/*
 * vaccs_record_reader.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef vaccs_record_reader_h
#define vaccs_record_reader_h

#include <util/general.h>
#include <io/vaccs_record_reader.h>
#include <io/vaccs_record.h>
#include <io/vaccs_record_factory.h>
#include <pin.H>

/**
 * Class: vaccs_record_reader
 *
 * A reader for reading a vaccs_record from a FILE
 */
class vaccs_record_reader {

private:
	NATIVE_FD fd;
	vaccs_record_factory *factory;

public:
	vaccs_record_reader();
	~vaccs_record_reader();

	/**
	 * Create a vaccs_record object
	 *
	 * @return the object
	 */
	vaccs_record *read_next_vaccs_record();

	/**
	 * Add the file pointer using a builder pattern
	 *
	 * @return the object
	 */
	vaccs_record_reader *add_fp(NATIVE_FD fd) {
		this->fd = fd;
		return this;
	}

	/**
	 * Get the file pointer
	 *
	 * @return the architecture type
	 */
	NATIVE_FD get_fd() {
		return fd;
	}

	/**
	 * Add the factory using a builder pattern
	 *
	 * @return the object
	 */
	vaccs_record_reader *add_factory(vaccs_record_factory *factory) {
		this->factory = factory;
		return this;
	}

	/**
	 * Get the factory
	 *
	 * @return the factory
	 */
	vaccs_record_factory *get_factory() {
		return factory;
	}

};

#endif /* vaccs_record_reader_h */
