//
//  malloc_record.h
//  libs
//
//  Created by Steven M. Carr on 12/10/2019.
//
//

#ifndef malloc_record_h
#define malloc_record_h

#include <stdio.h>
#include <stdlib.h>

#include <io/vaccs_record.h>
#include <util/general.h>
#include <util/c_string_utils.h>

class malloc_record : public vaccs_record {
private:
	vaccs_event_num_t event_num;
	size_t num_bytes;
	vaccs_address_t start_address;
	vaccs_line_num_t c_line_num;
	char *c_file_name;

public:
	malloc_record();
	virtual ~malloc_record();

	/**
	 * Add the event number using a builder pattern
	 *
	 * @return the object
	 */
	malloc_record *add_event_num(vaccs_event_num_t event_num)
	{
		this->event_num = event_num;
		return this;
	}

	/**
	 * Get the event number
	 *
	 * @return the event number
	 */
	vaccs_event_num_t get_event_num()
	{
		return event_num;
	}

	/**
	 * Add the c line number using a builder pattern
	 *
	 * @return the object
	 */
	malloc_record *add_c_line_num(vaccs_line_num_t c_line_num)
	{
		this->c_line_num = c_line_num;
		return this;
	}

	/**
	 * Get the c line number
	 *
	 * @return the c line number
	 */
	vaccs_line_num_t get_c_line_num()
	{
		return c_line_num;
	}

	/**
	 * Add the c file name using a builder pattern
	 *
	 * @return the object
	 */
	malloc_record *add_c_file_name(const char *c_file_name)
	{
		this->c_file_name = ssave(c_file_name);
		return this;
	}

	/**
	 * Get the c file name
	 *
	 * @return the c file name
	 */
	char *get_c_file_name()
	{
		return c_file_name;
	}

	/**
	 * Add the address using a builder pattern
	 *
	 * @return the object
	 */
	malloc_record *add_start_address(vaccs_address_t address)
	{
		this->start_address = address;
		return this;
	}

	/**
	 * Get the address
	 *
	 * @return the address
	 */
	vaccs_address_t get_start_address()
	{
		return start_address;
	}

	/**
	 * Add the number of bytes using a builder pattern
	 *
	 * @return the object
	 */
	malloc_record *add_num_bytes(size_t num_bytes)
	{
		this->num_bytes = num_bytes;
		return this;
	}

	/**
	 * Get the num_bytes
	 *
	 * @return the address
	 */
	size_t get_num_bytes()
	{
		return num_bytes;
	}

/**
 * Write a malloc record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
	void write(NATIVE_FD fd);

/**
 * Read a malloc record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a malloc record
 */
		vaccs_record *read(NATIVE_FD fd);

};

#endif /* malloc_record_h */
