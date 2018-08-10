/*
 * sd_clear_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef sd_clear_record_h
#define sd_clear_record_h

#include <util/general.h>
#include <util/c_string_utils.h>

#include <io/vaccs_record.h>

#include <pin.H>

/**
 * Class: sd_clear_record
 *
 * A record for sensitive data clear information
 */
class sd_clear_record: public vaccs_record {
private:
	vaccs_event_num_t event_num; /* the event number */
	vaccs_line_num_t c_line_num; /* the C line number */
	char *c_file_name; /* The C source file for this event */
	char *scope; /* the function name for the scope of the variable */
	char *variable; /* the sensitive data variable name */

public:
	sd_clear_record();
	~sd_clear_record();

	/**
	 * Add the event number using a builder pattern
	 *
	 * @return the object
	 */
	sd_clear_record *add_event_num(vaccs_event_num_t event_num) {
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
	sd_clear_record *add_c_line_num(vaccs_line_num_t c_line_num) {
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
	sd_clear_record *add_c_file_name(const char* c_file_name) {
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
	 * Add the scope using a builder pattern
	 *
	 * @return the object
	 */
	sd_clear_record *add_scope(const char* scope) {
		this->scope = ssave(scope);
		return this;
	}

	/**
	 * Get the scope
	 *
	 * @return the scope
	 */
	char* get_scope() {
		return scope;
	}

	/**
	 * Add the variable using a builder pattern
	 *
	 * @return the object
	 */
	sd_clear_record *add_variable(const char* variable) {
		this->variable = ssave(variable);
		return this;
	}

	/**
	 * Get the variable
	 *
	 * @return the variable
	 */
	char* get_variable() {
		return variable;
	}

	/**
	 * Write an analysis record to a file
	 *
	 * @param fp the file pointer for the output file
	 * @param rec the analysis record to be written to the file
	 */
	virtual void write(NATIVE_FD fd);

	/**
	 * Read an analysis record to a file
	 *
	 * @param fp the file pointer for the output file
	 */
	virtual vaccs_record *read(NATIVE_FD fd);
};

#endif /* sd_clear_record_h */
