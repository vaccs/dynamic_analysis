/*
 * var_access_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef var_access_record_h
#define var_access_record_h

#include <util/general.h>
#include <util/c_string_utils.h>

#include <io/vaccs_record.h>

/**
 * Class: var_access_record
 *
 * A record of variable access
 */
class var_access_record: public vaccs_record {
private:
	vaccs_event_num_t event_num; /* the event number */
	vaccs_line_num_t c_line_num; /* the C line number */
	char *c_file_name; /* The C source file for this event */
	char *scope; /* the function name for the scope of the variable */
	vaccs_address_t address; /* the variable address */
	char *type; /* the variable type */
	char *name; /* the name of the variable*/
	vaccs_address_t points_to; /* the address pointed to if a pointer */
	char *value; /* the variable value */

public:
	var_access_record();
	virtual ~var_access_record();

	/**
	 * Add the event number using a builder pattern
	 *
	 * @return the object
	 */
	var_access_record *add_event_num(vaccs_event_num_t event_num) {
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
	 * Add the c line number using a builder pattern
	 *
	 * @return the object
	 */
	var_access_record *add_c_line_num(vaccs_line_num_t c_line_num) {
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
	 * Add the c file name using a builder pattern
	 *
	 * @return the object
	 */
	var_access_record *add_c_file_name(const char *c_file_name) {
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
	 * Add the scope using a builder pattern
	 *
	 * @return the object
	 */
	var_access_record *add_scope(const char *scope) {
		this->scope = ssave(scope);
		return this;
	}

	/**
	 * Get the scope
	 *
	 * @return the scope
	 */
	char *get_scope() {
		return scope;
	}

	/**
	 * Add the address using a builder pattern
	 *
	 * @return the object
	 */
	var_access_record *add_address(vaccs_address_t address) {
		this->address = address;
		return this;
	}

	/**
	 * Get the address
	 *
	 * @return the address
	 */
	vaccs_address_t get_address() {
		return address;
	}

	/**
	 * Add the type using a builder pattern
	 *
	 * @return the object
	 */
	var_access_record *add_type(const char *type) {
		this->type = ssave(type);
		return this;
	}

	/**
	 * Get the type
	 *
	 * @return the type
	 */
	char *get_type() {
		return type;
	}

	/**
	 * Add the name using a builder pattern
	 *
	 * @return the object
	 */
	var_access_record *add_name(const char *name) {
		this->name = ssave(name);
		return this;
	}

	/**
	 * Get the name
	 *
	 * @return the name
	 */
	char *get_name() {
		return name;
	}

	/**
	 * Add the points to using a builder pattern
	 *
	 * @return the object
	 */
	var_access_record *add_points_to(vaccs_address_t points_to) {
		this->points_to = points_to;
		return this;
	}

	/**
	 * Get the points to
	 *
	 * @return the points to
	 */
	vaccs_address_t get_points_to() {
		return points_to;
	}

	/**
	 * Add the value using a builder pattern
	 *
	 * @return the object
	 */
	var_access_record *add_value(const char *value) {
		this->value = ssave(value);
		return this;
	}

	/**
	 * Get the value
	 *
	 * @return the value
	 */
	char *get_value() {
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

#endif /* var_access_record_h */
