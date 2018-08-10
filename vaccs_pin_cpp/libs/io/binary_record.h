/*
 * binary_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef binary_record_h
#define binary_record_h

#include <util/general.h>
#include <util/c_string_utils.h>
#include <io/vaccs_record.h>

#include <pin.H>
/**
 * Class: binary_record
 *
 * A record containing the path to the executable
 */
class binary_record: public vaccs_record {
private:
	char *bin_file_name; /* The executable path */

public:
	binary_record();
	~binary_record();

	/**
	 * Add the binary file name using a builder pattern
	 *
	 * @return the object
	 */
	binary_record *add_bin_file_name(const char* bin_file_name) {
		this->bin_file_name = ssave(bin_file_name);
		return this;
	}

	/**
	 * Get the binary file name
	 *
	 * @return the file name
	 */
	char *get_bin_file_name() {
		return bin_file_name;
	}

	/**
	 * Write an analysis record to a file
	 *
	 * @param fp the file pointer for the output file
	 * @param rec the analysis record to be written to the file
	 */
	virtual void write(NATIVE_FD fp);

	/**
	 * Read an analysis record to a file
	 *
	 * @param fp the file pointer for the output file
	 */
	virtual vaccs_record *read(NATIVE_FD fp);
};

#endif /* binary_record_h */
