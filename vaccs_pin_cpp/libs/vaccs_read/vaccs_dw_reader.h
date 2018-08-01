/*
 * dwreader.h
 *
 *  Created on: Jul 2, 2018
 *      Author: carr
 */

#ifndef LIBS_READ_VACCS_DW_READER_H_
#define LIBS_READ_VACCS_DW_READER_H_

#include <string>
#include <stack>

#include <tables/cu_table.h>
#include <tables/type_table.h>
#include <tables/var_table.h>

class vaccs_dw_reader {

private:
	FILE *fp;
	std::string file_name; /* the name of the executable containing DWARF info */
	cu_table *cutab;		/* the compile unit DWARF info */
	cu_record *current_cu_rec; /* the DWARF information for the current compilation unit */
	std::stack<var_table*> var_table_stack; /* stack of variable tables to implement scope */

	symbol_table_factory table_factory;
	symbol_table_record_factory record_factory;



	/**
	 * Read a record in the type table for an array type
	 */
	void read_type_record();

	/**
	 * Read a record in the variable table for a variable
	 */
	void read_var_record();

	/**
	 * Read a var table
	 */
	void read_var_table();

	/**
	 * Read a type table
	 */
	void read_type_table();

	/**
	 * Process a top level DIE for a compilation unit
	 *
	 * @param node a DW_TAG_compilation_unit DIE
	 */
	void read_cu_record();

public:
	vaccs_dw_reader();
	virtual ~vaccs_dw_reader();

	/**
	 * Add a file name containing DWARF information to this object. Use a builder pattern
	 *
	 * @param file_name a name of a file with DWARF info
	 */
	vaccs_dw_reader *add_file_name(std::string& file_name) {
		this->file_name = file_name;
		return this;
	}

	/**
	 * Get the file name
	 *
	 * @return a name of a file name containing DWARF info
	 */
	std::string& get_file_name() {
		return file_name;
	}

	/**
	 * Read the DWARF information and store it in symbol tables
	 */
	void read_vaccs_dw_info();

	/**
	 * Get the compilation unit table
	 *
	 * @return a table of compilation units for a file
	 */
	cu_table* get_cutab() {
		return cutab;
	}

	/**
	 * Get the current compilation unit record
	 *
	 * @return a cu_record for the current compilation unit
	 */
	cu_record* get_current_cu_rec() {
		return current_cu_rec;
	}

};

#endif /* LIBS_READ_VACCS_DW_READER_H_ */
