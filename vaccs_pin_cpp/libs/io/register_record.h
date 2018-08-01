/*
 * register_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef register_record_h
#define register_record_h

#include <util/general.h>
#include <util/c_string_utils.h>
#include <io/vaccs_record.h>

/**
 * Class: register_record
 *
 * A record indicating on which architecture the analysis was gathered
 */
class register_record: public vaccs_record {
private:
	vaccs_event_num_t event_num; /* The runtime event number for this invocation */
	vaccs_line_num_t c_line_num; /* The C source line number for this event */
	char *c_file_name; /* The C source file for this event */
	char *register_name; /* the name of the register */
	vaccs_value_t value; /* The new register value */

public:
	register_record();
	~register_record();

	/**
	 * Add the event number using a builder pattern
	 *
	 * @return the object
	 */
	register_record *add_event_num(vaccs_event_num_t event_num) {
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
	register_record *add_c_line_num(vaccs_line_num_t c_line_num) {
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
	 * Add the file name using a builder pattern
	 *
	 * @return the object
	 */
	register_record *add_c_file_name(const char* c_file_name) {
		this->c_file_name = ssave(c_file_name);
		return this;
	}

	/**
	 * Get the file name
	 *
	 * @return the file name
	 */
	char* get_c_file_name() {
		return c_file_name;
	}

	/**
	 * Add the register name using a builder pattern
	 *
	 * @return the object
	 */
	register_record *add_register_name(const char* register_name) {
		this->register_name = ssave(register_name);
		return this;
	}

	/**
	 * Get the register name
	 *
	 * @return the register name
	 */
	char* get_register_name() {
		return register_name;
	}

	/**
	 * Add the value using a builder pattern
	 *
	 * @return the object
	 */
	register_record *add_value(vaccs_value_t value) {
		this->value = value;
		return this;
	}

	/**
	 * Get the value
	 *
	 * @return the value
	 */
	vaccs_value_t get_value() {
		return value;
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

#endif /* register_record_h */
