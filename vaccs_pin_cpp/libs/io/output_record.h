/*
 * output_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef output_record_h
#define output_record_h

#include <util/general.h>
#include <util/c_string_utils.h>
#include <io/vaccs_record.h>

/**
 * Class: output_record
 *
 * A record of program output
 */
class output_record: public vaccs_record {
private:
	vaccs_event_num_t event_num;
	char *output; /* A line of output */

public:
	output_record();
	~output_record();

	/**
	 * Add the event number using a builder pattern
	 *
	 * @return the object
	 */
	output_record *add_event_num(vaccs_event_num_t event_num) {
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
	 * Add the line of output using a builder pattern
	 *
	 * @return the object
	 */
	output_record *add_output(const char *output) {
		this->output = ssave(output);
		return this;
	}

	/**
	 * Get the line of output
	 *
	 * @return the line of output
	 */
	char *get_output() {
		return output;
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

#endif /* output_record_h */
