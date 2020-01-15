//
//  file_open_record.h
//  libs
//
//  Created by Steven M. Carr on 12/10/2019.
//
//

#ifndef file_open_record_h
#define file_open_record_h

#include <stdio.h>
#include <stdlib.h>

#include <io/vaccs_record.h>
#include <util/general.h>
#include <util/c_string_utils.h>

class file_open_record : public vaccs_record {
private:
	vaccs_event_num_t event_num;
	vaccs_line_num_t c_line_num;
	char *c_file_name;
	char *path;
	long fd;
	int flags;
	mode_t mode;
	char is_sym_link;

public:
	file_open_record();
	virtual ~file_open_record();

	/**
	 * Add the event number using a builder pattern
	 *
	 * @return the object
	 */
	file_open_record *add_event_num(vaccs_event_num_t event_num)
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
	file_open_record *add_c_line_num(vaccs_line_num_t c_line_num)
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
	file_open_record *add_c_file_name(const char *c_file_name)
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
	 * Add the file descriptor using a builder pattern
	 *
	 * @return the object
	 */
	file_open_record *add_file_descriptor(long fd)
	{
		this->fd = fd;
		return this;
	}

	/**
	 * Get the file descriptor
	 *
	 * @return the address
	 */
	long get_file_descriptor()
	{
		return fd;
	}

/**
	 * Add the path using a builder pattern
	 *
	 * @return the object
	 */
	file_open_record *add_path(const char *path)
	{
		this->path = ssave(path);
		return this;
	}

	/**
	 * Get the c file name
	 *
	 * @return the c file name
	 */
	char *get_path()
	{
		return path;
	}

	/**
	 * Add the flags using a builder pattern
	 *
	 * @return the object
	 */
	file_open_record *add_flags(int flags)
	{
		this->flags = flags;
		return this;
	}

	/**
	 * Get the flags
	 *
	 * @return the flags
	 */
	size_t get_flags()
	{
		return flags;
	}

	/**
	 * Add the mode using a builder pattern
	 *
	 * @return the object
	 */
	file_open_record *add_mode(mode_t mode)
	{
		this->mode = mode;
		return this;
	}

	/**
	 * Get the mode
	 *
	 * @return the mode
	 */
	mode_t get_mode()
	{
		return mode;
	}

	/**
	 * Add is_sym_link using a builder pattern
	 *
	 * @return the object
	 */
	file_open_record *add_is_sym_link()
	{
		this->is_sym_link = 1;
		return this;
	}

	/**
	 * Get the is_sym_link
	 *
	 * @return the is_sym_link
	 */
	mode_t get_is_sym_link()
	{
		return is_sym_link;
	}

/**
 * Write a file_open record to the analysis file
 *
 * @param fp a file pointer for the analysis file
 */
	void write(NATIVE_FD fd);

/**
 * Read a file_open record from the analysis file
 *
 * @param fp a file pointer for the analysis file
 * @return a file_open record
 */
		vaccs_record *read(NATIVE_FD fd);

};

#endif /* file_open_record_h */
