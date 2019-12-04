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
#include <tables/var_table.h>
#include <tables/type_table.h>


#include <string>
#include <list>

#include <tables/symbol_table.h>
#include <tables/symbol_table_factory.h>

#include <pin.H>

using namespace std;

/**
 * Class: cu_record
 *
 * The information found in the debug information of an executable for a
 * compilation unit.
 *
 */
class cu_record: public symbol_table_record {

private:

	var_table* vtab; /* a table of global variables and function in the executable */
	type_table* ttab; /* a table of the type declared in the executable */
	Generic low_pc; /* the lowest pc value of the instructions in the executable */
	Generic high_pc; /* the highest pc value of the instructions in the executable */

	symbol_table_factory table_factory;

public:

	/**
	 * The constructor. We use a builder pattern, so there are no parameters
	 */
	cu_record();

	/**
	 * The destructor
	 */
	virtual ~cu_record();

	/**
	 * Add the var_table to the object using a builder pattern
	 *
	 * @param vt a variable table for this compile unit
	 * @return the object
	 */
	cu_record* add_var_table(var_table* vt) {
		vtab = vt;
		return this;
	}

	/**
	 * Add the type table to the object using a builder pattern
	 *
	 * @param tt a type table for this compile unit
	 * @return the object
	 */
	cu_record* add_type_table(type_table* tt) {
		ttab = tt;
		return this;
	}

	/**
	 * Add the lowest pc value to the object using a builder pattern
	 *
	 * @param lpc the lowest pc value for this compile unit
	 * @return the object
	 */
	cu_record* add_low_pc(Generic lpc) {
		low_pc = lpc;
		return this;
	}

	/**
	 * Add the highest pc value to the object using a builder pattern
	 *
	 * @param hpc the highest pc value for this compile unit
	 * @return the object
	 */
	cu_record* add_high_pc(Generic hpc) {
		high_pc = hpc;
		return this;
	}

	/**
	 * Get the varable table from the object
	 *
	 * @return the variable table
	 */
	var_table* get_var_table() {
		return vtab;
	}

	/**
	 * Get the type table from the object
	 *
	 * @return the type table
	 */
	type_table* get_type_table() {
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
	 * Translate a memory access to an list of variables that point to it
	 *
	 * @param ctxt a pin process context
	 * @param inst_addr the address of the instruction making the memory access
	 * @param mem_addr the memory address being accessed
	 * @return a pointer to a list of variables that point to an address
	 */
	list<pair<string,var_record*>> *
	translate_address_to_pointer_list(const CONTEXT *ctxt, Generic mem_addr);

	/**
	 * Translate a memory access to a variable name
	 *
	 * @param ctxt a pin process context
	 * @param inst_addr the address of the instruction making the memory access
	 * @param mem_addr the memory address being accessed
	 * @return a key,var_record* pair that matches the given address, or default_var_pai
	 */
	pair<string,var_record*> translate_address_to_variable(const CONTEXT *ctxt,Generic inst_addr, Generic mem_addr);

	/**
	 * Translate an instruction address to a function containing the address
	 *
	 * @param inst_addr the address of the instruction making the memory access
	 * @return a key,var_record* pair that matches the given address, or default_var_pai
	 */
	pair<string,var_record*> translate_address_to_function(Generic inst_addr);

	/**
	 * Get the scope name of a var_record
	 *
	 * @param vrec a variable record
	 * @return a string containing the name of the scope (function name or *G*)
	 */
	string get_scope(var_record *vrec);

	/**
	 * Create the member tables from the type declarations for local variables that are structures.
	 *
	 */
	void create_member_tables();

	/**
	 * Write a compilation unit record to a file
	 *
	 * @param fn the file name for the cu
	 * @param fp a file pointer
	 */
	virtual void write(string fn,NATIVE_FD fd);

};

/**
 * Class: cu_table
 *
 * A table mapping compile unit names to records containing the debug
 * information for that compile unit.
 */
class cu_table: public symbol_table {

public:
	cu_table() :
			symbol_table(CU_TABLE) {
	}
	virtual ~cu_table();

	/**
	 * Get the cu record for a particular compilation unit
	 *
	 * @param str the path to a compilation unit
	 * @return a cu_record for the cu
	 */
	cu_record* get(string str) { return (cu_record*) symbol_table::get(str);}

	/**
	 * Get the cu record for a assembly instruction
	 *
	 * @param ip a instruction pointer value
	 * @return a cu_record for the cu in which the ip is located
	 */
	cu_record* get(Generic ip);

	/**
	 * Translate a memory access to an list of variables that point to it
	 *
	 * @param ctxt a pin process context
	 * @param inst_addr the address of the instruction making the memory access
	 * @param mem_addr the memory address being accessed
	 * @return a key,var_record* pair that matches the given address, or default_var_pai
	 */
	list<pair<string,var_record*>> *
	translate_address_to_pointer_list(const CONTEXT *ctxt, Generic mem_addr);

	/**
	 * Translate a memory access to a variable name
	 *
	 * @param ctxt a pin process context
	 * @param inst_addr the address of the instruction making the memory access
	 * @param mem_addr the memory address being accessed
	 * @return a key,var_record* pair that matches the given address, or default_var_pai
	 */
	pair<string,var_record*> translate_address_to_variable(const CONTEXT *ctxt,Generic inst_addr, Generic mem_addr);

	/**
	 * Translate an instruction address to a function containing the address
	 *
	 * @param inst_addr the address of the instruction making the memory access
	 * @return a key,var_record* pair that matches the given address, or default_var_pai
	 */
	pair<string,var_record*> translate_address_to_function(Generic inst_addr);

	/**
	 * Get the scope name of a var_record
	 *
	 * @param vrec a variable record
	 * @return a string containing the name of the scope (function name or *G*)
	 */
	string get_scope(var_record *vrec);

	/**
	* Get the var table for a particular function
	*
	* @param ip an instruction address in the function
	* @return the variable table for the function
	*/
	var_table* get_function_var_table(Generic ip);

	/**
	* Get the var record for a particular function
	*
	* @param ip an instruction address in the function
	* @return the variable table record for the function
	*/
	var_record* get_function_var_record(Generic ip);

	/**
 	 * Get the type record of a type given the dwarf index
 	 *
 	 * @param dw_index a dwarf index for a type (string)
 	 * @return a pointer to a type_record for the give dwarf index
 	 */
	type_record *get_type_record(string dw_index);

	/**
 	 * Get the type table containing a type given the dwarf index
 	 *
 	 * @param dw_index a dwarf index for a type (string)
 	 * @return a pointer to a type table containing the give dwarf index
 	 */
	type_table *get_type_table(string dw_index);

	/**
	 * Create the member tables from the type declarations for structures.
	 *
	 */
	void create_member_tables();
	/**
	 * Write the compilation unit table to a file
	 *
	 * @param fp a file pointer
	 */
	virtual void write(NATIVE_FD fd);
};

#endif
