/*
 * ccode_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef ccode_record_h
#define ccode_record_h

#include <util/general.h>
#include <util/c_string_utils.h>
#include <io/vaccs_record.h>

#include <pin.H>

/**
 * Class: ccode_record
 *
 * A record for a line of C code
 */
class ccode_record: public vaccs_record {
private:
	vaccs_line_num_t c_line_num; /* The line number of this source line in the C file */
	vaccs_line_pos_t c_start_pos; /* The character starting position of this source line */
	char *c_file_name; /* The name of the file in which this source line appears */
	char *c_src_line; /* The C source line itself */

public:
	ccode_record();
	~ccode_record();

	/**
	 * Add the c line number using a builder pattern
	 *
	 * @return the object
	 */
	ccode_record *add_c_line_num(vaccs_line_num_t c_line_num) {
		this->c_line_num = c_line_num;
		return this;
	}

	/**
	 * Get the c line number
	 *
	 * @return the c line number
	 */
	vaccs_line_num_t get_c_line_num() {
		return c_line_num;
	}

	/**
	 * Add the c start position using a builder pattern
	 *
	 * @return the object
	 */
	ccode_record *add_c_start_pos(vaccs_line_pos_t c_start_pos) {
		this->c_start_pos = c_start_pos;
		return this;
	}

	/**
	 * Get the c start position
	 *
	 * @return the c start position
	 */
	vaccs_line_pos_t get_c_start_pos() {
		return c_start_pos;
	}

	/**
	 * Add the c source file using a builder pattern
	 *
	 * @return the object
	 */
	ccode_record *add_c_file_name(const char *c_file_name) {
		this->c_file_name = ssave(c_file_name);
		return this;
	}

	/**
	 * Get the c file name
	 *
	 * @return the c file name
	 */
	char *get_c_file_name() {
		return c_file_name;
	}

	/**
	 * Add the c source line using a builder pattern
	 *
	 * @return the object
	 */
	ccode_record *add_c_src_line(const char *c_src_line) {
		this->c_src_line = ssave(c_src_line);
		return this;
	}

	/**
	 * Get the c source line
	 *
	 * @return the c source line
	 */
	char *get_c_src_line() {
		return c_src_line;
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

#endif /* ccode_record_h */
