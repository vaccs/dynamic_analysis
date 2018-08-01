/*
 * func_inv_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef func_inv_record_h
#define func_inv_record_h

#include <util/general.h>
#include <io/vaccs_record.h>

#include <util/c_string_utils.h>

/**
 * Class: func_inv_record
 *
 * A record containing function invocation data
 */
class func_inv_record: public vaccs_record {
private:
	vaccs_event_num_t event_num; /* The runtime event number for this invocation */
	vaccs_line_num_t func_line_num; /* The C source line number of the callee */
	vaccs_line_num_t inv_line_num; /* The C source line number of the invocation */
	char *c_func_file; /* The C source file of the callee */
	char *c_inv_file; /* The C source file of the invocation */
	char *func_name; /* The name of the callee */
	vaccs_address_t address; /* The address of the callee */

public:
	func_inv_record();
	~func_inv_record();

	/**
	 * Add the event number using a builder pattern
	 *
	 * @return the object
	 */
	func_inv_record *add_event_num(vaccs_event_num_t event_num) {
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
	 * Add the function line number using a builder pattern
	 *
	 * @return the object
	 */
	func_inv_record *add_func_line_num(vaccs_line_num_t func_line_num) {
		this->func_line_num = func_line_num;
		return this;
	}

	/**
	 * Get the function line number
	 *
	 * @return the function line number
	 */
	vaccs_line_num_t get_func_line_num() {
		return func_line_num;
	}

	/**
	 * Add the invocation line number using a builder pattern
	 *
	 * @return the object
	 */
	func_inv_record *add_inv_line_num(vaccs_line_num_t inv_line_num) {
		this->inv_line_num = inv_line_num;
		return this;
	}

	/**
	 * Get the invocation line number
	 *
	 * @return the invocation line number
	 */
	vaccs_line_num_t get_inv_line_num() {
		return inv_line_num;
	}

	/**
	 * Add the c function file using a builder pattern
	 *
	 * @return the object
	 */
	func_inv_record *add_c_func_file(const char *c_func_file) {
		this->c_func_file = ssave(c_func_file);
		return this;
	}

	/**
	 * Get the c function file
	 *
	 * @return the c function file
	 */
	char *get_c_func_file() {
		return c_func_file;
	}

	/**
	 * Add the c invocation file using a builder pattern
	 *
	 * @return the object
	 */
	func_inv_record *add_c_inv_file(const char *c_inv_file) {
		this->c_inv_file = ssave(c_inv_file);
		return this;
	}

	/**
	 * Get the c invocation file
	 *
	 * @return the c invocation file
	 */
	char *get_c_inv_file() {
		return c_inv_file;
	}

	/**
	 * Add the c function name using a builder pattern
	 *
	 * @return the object
	 */
	func_inv_record *add_func_name(const char *func_name) {
		this->func_name = ssave(func_name);
		return this;
	}

	/**
	 * Get the c function name
	 *
	 * @return the c function name
	 */
	char *get_func_name() {
		return func_name;
	}

	/**
	 * Add the callee address using a builder pattern
	 *
	 * @return the object
	 */
	func_inv_record *add_address(vaccs_address_t address) {
		this->address = address;
		return this;
	}

	/**
	 * Get the callee address
	 *
	 * @return the callee address
	 */
	vaccs_address_t get_address() {
		return address;
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

#endif /* func_inv_record_h */
