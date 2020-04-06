/*
 * dwreader.h
 *
 *  Created on: Jul 2, 2018
 *      Author: carr
 */

#ifndef LIBS_READ_DW_READER_H_
#define LIBS_READ_DW_READER_H_

#include <string>
#include <stack>

#include <libelfin/dwarf/dwarf++.hh>
#include <tables/symbol_table_factory.h>

#include "../tables/vdw_cu_table.h"
#include "../tables/vdw_type_table.h"
#include "../tables/vdw_var_table.h"

class dw_reader {

private:
	dwarf::compilation_unit current_cu; /* the current compilation unit */
	std::string file_name; /* the name of the executable containing DWARF info */
	vdw_cu_table *cutab;		/* the compile unit DWARF info */
	vdw_cu_record *current_cu_rec; /* the DWARF information for the current compilation unit */
	vdw_type_record *last_array_rec; /* used to save array record so that upper bound can be added when subrange found */
	vdw_type_record *last_struct_rec; /* used to save struct record so that members can be added when found */
	std::stack<vdw_var_table*> var_table_stack; /* stack of variable tables to implement scope */

	symbol_table_factory table_factory;
	symbol_table_record_factory record_factory;


	/**
	 * Build a record in the type table for a base type
	 *
	 * @param node a DW_TAG_base_type DIE
	 */
	void process_base_type_die(const dwarf::die &node);

	/**
	 * Set the upper bound information from a die node for an array type
	 *
	 * @node a die for an subrange type
	 */
	void set_upper_bound(const dwarf::die& node);

	/**
	 * Build a record in the type table for an array type
	 *
	 * @param node a DW_TAG_array_type DIE
	 */
	void process_array_type_die(const dwarf::die &node);

	/**
	 * Build a record in the type table for a pointer type
	 *
	 * @param node a DW_TAG_pointer_type DIE
	 */
	void process_pointer_type_die(const dwarf::die &node);

	/**
	 * Add a member from a die node for a struct type
	 *
	 * @node a die for a member of a struct
	 */
	void add_struct_member(const dwarf::die& node);

	/**
	 * Build a record in the type table for a struct type
	 *
	 * @param node a DW_TAG_struct_type DIE
	 */
	void process_struct_type_die(const dwarf::die &node);

	/**
	 * Build a record in the type table for a const type
	 *
	 * @param node a DW_TAG_const_type DIE
	 */
	void process_const_type_die(const dwarf::die &node);

	/**
	 * Build a record in the type table for a typedef
	 *
	 * @param node a DW_TAG_typedef_ DIE
	 */
	void process_typedef_die(const dwarf::die &node);

	/**
	 * Find the start pc in a subprogram (first executable statement)
	 *
	 * @param node a DW_TAG_subprogram DIE
	 */
	Generic get_start_pc_from_line_table(std::string name, Generic low_pc, Generic high_pc);

	/**
	 * Build a record in the variable table for a subprogram
	 *
	 * @param node a DW_TAG_subprogram DIE
	 */
	void process_subprogram_die(const dwarf::die &node);

	/**
	 * Build a record in the variable table for a variable
	 *
	 * @param node a DW_TAG_variable DIE
	 */
	void process_variable_die(const dwarf::die &node, bool is_param);

	/**
	 * Walk the DIE tree to build the tables of information needed for program analysis
	 *
	 * @param node a DIE
	 */
	void process_die_tree(const dwarf::die &node);

	/**
	 * Process a top level DIE for a compilation unit
	 *
	 * @param node a DW_TAG_compilation_unit DIE
	 */
	void process_cu_die(const dwarf::die &node);

public:
	dw_reader();
	virtual ~dw_reader();

	/**
	 * Add a file name containing DWARF information to this object. Use a builder pattern
	 *
	 * @param file_name a name of a file with DWARF info
	 */
	dw_reader *add_file_name(std::string& file_name) {
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
	void read_dw_info();

	/**
	 * Get the compilation unit table
	 *
	 * @return a table of compilation units for a file
	 */
	vdw_cu_table* get_cutab() {
		return cutab;
	}

	/**
	 * Get the current compilation unit record
	 *
	 * @return a cu_record for the current compilation unit
	 */
	vdw_cu_record* get_current_cu_rec() {
		return current_cu_rec;
	}

};

#endif /* LIBS_READ_DW_READER_H_ */
