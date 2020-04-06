/*
 * symbol_table_factory.cpp
 *
 *  Created on: Jul 9, 2018
 *      Author: carr
 */

#include <tables/symbol_table_factory.h>
#include <tables/symbol_table.h>
#include "vdw_cu_table.h"
#include "vdw_type_table.h"
#include "vdw_var_table.h"


symbol_table *symbol_table_factory::make_symbol_table(int type){

	symbol_table *table;

	switch(type) {
	case CU_TABLE:
		table = new vdw_cu_table();
		break;
	case TYPE_TABLE:
		table = new vdw_type_table();
		break;
	case VAR_TABLE:
		table = new vdw_var_table();
		break;
	default:
		table = NULL;
		break;
	}

	return table;
}

symbol_table_record *symbol_table_record_factory::make_symbol_table_record(int type) {

	symbol_table_record *rec;

	switch (type) {
	case CU_RECORD:
		rec = new vdw_cu_record();
		break;
	case TYPE_RECORD:
		rec = new vdw_type_record();
		break;
	case VAR_RECORD:
		rec = new vdw_var_record();
		break;
	default:
		rec = NULL;
	}

	return rec;
}
