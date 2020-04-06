/**
 * cu_table.H
 *
 * This file contains the code for a compliation unit table and its associated
 * records 
 *
 */
#ifndef cu_table_h
#define cu_table_h

#include <util/general.h>
#include <string>

#include <tables/symbol_table.h>
#include <tables/symbol_table_factory.h>
#include "vdw_type_table.h"
#include "vdw_var_table.h"

/**
 * Class: cu_record
 *
 * The information found in the debug information of an executable for a
 * compilation unit.
 *
 */
class vdw_cu_record: public symbol_table_record {

private:

	vdw_var_table* vtab; /* a table of global variables and function in the executable */
	vdw_type_table* ttab; /* a table of the type declared in the executable */
	Generic low_pc; /* the lowest pc value of the instructions in the executable */
	Generic high_pc; /* the highest pc value of the instructions in the executable */

	symbol_table_factory table_factory;

public:

	/**
	 * The constructor. We use a builder pattern, so there are no parameters
	 */
	vdw_cu_record();

	/**
	 * The destructor
	 */
	virtual ~vdw_cu_record();

	/**
	 * Add the var_table to the object using a builder pattern
	 *
	 * @param vt a variable table for this compile unit
	 * @return the object
	 */
	vdw_cu_record* add_var_table(vdw_var_table* vt) {
		vtab = vt;
		return this;
	}

	/**
	 * Add the type table to the object using a builder pattern
	 *
	 * @param tt a type table for this compile unit
	 * @return the object
	 */
	vdw_cu_record* add_type_table(vdw_type_table* tt) {
		ttab = tt;
		return this;
	}

	/**
	 * Add the lowest pc value to the object using a builder pattern
	 *
	 * @param lpc the lowest pc value for this compile unit
	 * @return the object
	 */
	vdw_cu_record* add_low_pc(Generic lpc) {
		low_pc = lpc;
		return this;
	}

	/**
	 * Add the highest pc value to the object using a builder pattern
	 *
	 * @param hpc the highest pc value for this compile unit
	 * @return the object
	 */
	vdw_cu_record* add_high_pc(Generic hpc) {
		high_pc = hpc;
		return this;
	}

	/**
	 * Get the varable table from the object
	 *
	 * @return the variable table
	 */
	vdw_var_table* get_var_table() {
		return vtab;
	}

	/**
	 * Get the type table from the object
	 *
	 * @return the type table
	 */
	vdw_type_table* get_type_table() {
		return ttab;
	}

	/**
	 * Get the lowest pc value for this compile unit from the object
	 *
	 * @return the lowest pc value
	 */
	Generic get_low_pc() {
		return low_pc;
	}

	/**
	 * Get the highest pc value for this compile unit from the object
	 *
	 * @return the highest pc value
	 */
	Generic get_high_pc() {
		return high_pc;
	}

	/**
	 * Check if a pc is in this compilation unit
	 *
	 * @param pc the pc to check
	 * @return true if the pc is found in this compilation unit, otherwise false.
	 */
	bool pc_in_range(Generic pc);

	/**
	 * Translate a memory access by an instruction in this compilation unit to a variable name
	 *
	 * @param inst_addr the address of the instruction making the memory access
	 * @param mem_addr the memory address being accessed
	 * @return the variable record of the variable being accessed if possible to determine, otherwise
	 * 			nullptr
	 */
	vdw_var_record* translate_address_to_variable(Generic inst_addr, Generic mem_addr);

	/**
	 * Write a compilation unit record to a file
	 *
	 * @param fn the file name for the cu
	 * @param fp a file pointer
	 */
	virtual void write(std::string fn,FILE *fp);

};

/**
 * Class: cu_table
 *
 * A table mapping compile unit names to records containing the debug
 * information for that compile unit.
 */
class vdw_cu_table: public symbol_table {

public:
	vdw_cu_table() :
			symbol_table(CU_TABLE) {
	}
	virtual ~vdw_cu_table();

	vdw_cu_record* get(std::string str) { return (vdw_cu_record*) symbol_table::get(str);}

	/**
	 * Translate a memory access to a variable name
	 *
	 * @param inst_addr the address of the instruction making the memory access
	 * @param mem_addr the memory address being accessed
	 * @return the variable record of the variable being accessed if possible to determine, otherwise
	 * 			nullptr
	 */
	vdw_var_record* translate_address_to_variable(Generic inst_addr, Generic mem_addr);

	/**
	 * Write the compilation unit table to a file
	 *
	 * @param fp a file pointer
	 */
	virtual void write(FILE *fp);

	void resolve_base_types();
};

#endif
