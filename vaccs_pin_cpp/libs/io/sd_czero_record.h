/*
 * sd_czero_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef sd_czero_record_h
#define sd_czero_record_h

#include <util/general.h>
#include <util/c_string_utils.h>
#include <io/vaccs_record.h>

/**
 * Class: sd_czero_record
 *
 * A record for sensitive data core zero data
 */
class sd_czero_record: public vaccs_record {
private:
	vaccs_event_num_t event_num; /* the event number */
	vaccs_line_num_t c_line_num; /* the C line number */
	char *c_file_name; /* The C source file for this event */

public:
	sd_czero_record();
	~sd_czero_record();

	/**
	 * Add the event number using a builder pattern
	 *
	 * @return the object
	 */
	sd_czero_record *add_event_num(vaccs_event_num_t event_num) {
		this->event_num = event_num;
		return this;
	}

	/**
	 * Get the event number
	 *
	 * @return the event number
	 */
	vaccs_event_num_t get_event_num() {
		return event_num;
	}

	/**
	 * Add the line number using a builder pattern
	 *
	 * @return the object
	 */
	sd_czero_record *add_c_line_num(vaccs_line_num_t c_line_num) {
		this->c_line_num = c_line_num;
		return this;
	}

	/**
	 * Get the line number
	 *
	 * @return the line number
	 */
	vaccs_line_num_t get_c_line_num() {
		return c_line_num;
	}

	/**
	 * Add the c file name using a builder pattern
	 *
	 * @return the object
	 */
	sd_czero_record *add_c_file_name(const char* c_file_name) {
		this->c_file_name = ssave(c_file_name);
		return this;
	}

	/**
	 * Get the c file name
	 *
	 * @return the c file name
	 */
	char* get_c_file_name() {
		return c_file_name;
	}

	/**
	 * Write an analysis record to a file
	 *
	 * @param fp the file pointer for the output file
	 * @param rec the analysis record to be written to the file
	 */
	virtual void write(FILE *fp);

	/**
	 * Read an analysis record to a file
	 *
	 * @param fp the file pointer for the output file
	 */
	virtual vaccs_record *read(FILE *fp);
};

#endif /* sd_czero_record_h */
