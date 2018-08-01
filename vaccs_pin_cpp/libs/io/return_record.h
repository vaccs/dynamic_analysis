/*
 * return_record.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef return_record_h
#define return_record_h

#include <util/general.h>
#include <io/vaccs_record.h>

/**
 * Class: return_record
 *
 * A record function return data
 */
class return_record: public vaccs_record {
private:
	vaccs_event_num_t event_num; /* the id of the analysis event */

public:
	return_record();
	~return_record() {
	}

	/**
	 * Add the event number using a builder pattern
	 *
	 * @return the object
	 */
	return_record *add_event_num(vaccs_event_num_t event_num) {
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

#endif /* return_record_h */
