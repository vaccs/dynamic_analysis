/*
 * vaccs_record_factory.h
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#ifndef vaccs_record_factory_h
#define vaccs_record_factory_h

#include <cstdio>

#include <util/general.h>
#include <io/vaccs_record.h>

/**
 * Class: vaccs_record_factory
 *
 * A factory pattern for making a vaccs_record
 */
class vaccs_record_factory {

public:
	vaccs_record_factory() {
	}
	~vaccs_record_factory() {
	}

	/**
	 * Create a vaccs_record object
	 *
	 * @param id the type of analysis record
	 * @return a pointer to vaccs_record object
	 */
	vaccs_record *make_vaccs_record(vaccs_id_t id);
};

#endif /* vaccs_record_factory_h */
