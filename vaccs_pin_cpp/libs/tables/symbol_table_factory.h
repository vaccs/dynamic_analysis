/*
 * symbol_table_factory.h
 *
 *  Created on: Jul 9, 2018
 *      Author: carr
 */

#ifndef LIBS_TABLES_SYMBOL_TABLE_FACTORY_H_
#define LIBS_TABLES_SYMBOL_TABLE_FACTORY_H_

#include <tables/symbol_table.h>


class symbol_table_factory {
public:
	symbol_table_factory(){}
	virtual ~symbol_table_factory(){}

	symbol_table *make_symbol_table(int type);
};

class symbol_table_record_factory {
public:
	symbol_table_record_factory() {}
	virtual ~symbol_table_record_factory() {}

	symbol_table_record *make_symbol_table_record(int type);
};

#endif /* LIBS_TABLES_SYMBOL_TABLE_FACTORY_H_ */
