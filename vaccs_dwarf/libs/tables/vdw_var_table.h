/**
 * var_table.H
 *
 * This file contains the code for a variable table and its associated
 * records
 *
 */
#ifndef var_table_h
#define var_table_h

#include <util/general.h>
#include <map>
#include <string>

#include <tables/symbol_table.h>


const std::string void_type = "void";

class vdw_var_table;
/**
 * Class: var_record
 *
 * The information found in the debug information of an executable for
 * the variables in a compilation unit.
 *
 */
class vdw_var_record: public symbol_table_record {

private:

	std::string type; /* the dwarf index for this type */
	bool is_subprog; /* is this variable a subprogram? */
	vdw_var_table *local_var_table; /* the local variable declared inside a subprogram */
	bool is_local; /* is this variable local? */
	bool is_param; /* is this variable a formal parameter */
	std::string decl_file; /* the name of the file in which this is defined */
	Generic decl_line; /* the line of the file on which this is defined */
	Generic low_pc; /* the lowest pc of a subroutine variable */
	Generic high_pc; /* the highest pc of a subroutine variable */
	Generic location; /* either an offset from the frame pointer (is_local || is_param) or a global address */
	Generic start_pc; /* the pc of the first executable statement in a subprogram */

public:

	/**
	 * The constructor. We use a builder pattern, so there are no parameters
	 */
	vdw_var_record();

	/**
	 * The destructor
	 */
	virtual ~vdw_var_record();

	/**
	 * Add the type to the object using a builder pattern
	 *
	 * @param type the type of this variable
	 * @return the object
	 */
	vdw_var_record* add_type(std::string& type) {
		this->type = type;
		return this;
	}

	/**
	 * Add the is_subprog filed to the object using a builder pattern
	 *
	 * @return the object
	 */
	vdw_var_record* add_is_subprog() {
		this->is_subprog = true;
		return this;
	}

	/**
	 * Add the local variable table to the object using a builder pattern
	 *
	 * @param local_var_table the table of variables local to a subprogram
	 * @return the object
	 */
	vdw_var_record* add_local_var_table(vdw_var_table *local_var_table) {
		this->local_var_table = local_var_table;
		return this;
	}

	/**
	 * Add the is_local filed to the object using a builder pattern
	 *
	 * @return the object
	 */
	vdw_var_record* add_is_local() {
		this->is_local = true;
		return this;
	}

	/**
	 * Add the is_param field to the object using a builder pattern
	 *
	 * @return the object
	 */
	vdw_var_record* add_is_param() {
		this->is_param = true;
		return this;
	}

	/**
	 * Add the declartion file to the object using a builder pattern
	 *
	 * @param decl_file the declaration file of this variable
	 * @return the object
	 */
	vdw_var_record* add_decl_file(std::string& decl_file) {
		this->decl_file = decl_file;
		return this;
	}

	/**
	 * Add the declartion line to the object using a builder pattern
	 *
	 * @param decl_line the declaration line of this variable
	 * @return the object
	 */
	vdw_var_record* add_decl_line(Generic decl_line) {
		this->decl_line = decl_line;
		return this;
	}

	/**
	 * Add the lowest pc to the object using a builder pattern
	 *
	 * @param low_pc the lowest pc of this subprogram
	 * @return the object
	 */
	vdw_var_record* add_low_pc(Generic low_pc) {
		this->low_pc = low_pc;
		return this;
	}

	/**
	 * Add the highest pc to the object using a builder pattern
	 *
	 * @param high_pc the highest pc of this subprogram
	 * @return the object
	 */
	vdw_var_record* add_high_pc(Generic high_pc) {
		this->high_pc = high_pc;
		return this;
	}

	/**
	 * Add the location  to the object using a builder pattern
	 *
	 * @param location an expression for computing the address of a variable
	 * @return the object
	 */
	vdw_var_record* add_location(Generic location) {
		this->location = location;
		return this;
	}

	/**
	 * Add the start_pc to the object using a builder pattern
	 *
	 * @param start_pc the pc of the first executable statement in a subprogram
	 * @return the object
	 */
	vdw_var_record* add_start_pc(Generic start_pc) {
		this->start_pc = start_pc;
		return this;
	}

	/**
	 * Get the variable type die offset
	 *
	 * @return a pointer to a string containing the die offset for the variable type
	 */
	std::string& get_type() {
		return type;
	}

	/**
	 * Check if this variable is a subprogram
	 *
	 * @return true if this variable is a subprogram, else false
	 */
	bool get_is_subprog() {
		return is_subprog;
	}

	/**
	 * Get the local variable table for a subprogram
	 *
	 * @return the local variable table for variables declared in this subprogram
	 */
	vdw_var_table *get_local_var_table() {
		return local_var_table;
	}

	/**
	 * Check if this is a local variable
	 *
	 * @return true if this is a local variable, otherwise false
	 */
	bool get_is_local() {
		return is_local;
	}

	/**
	 * Check if this is a formal parameter variable
	 *
	 * @return true if this is a formal parameter variable, otherwise false
	 */
	bool get_is_param() {
		return is_local;
	}

	/**
	 * Get the compilation unit record for this variable
	 *
	 * @return a record in the cu_table for the compilation unit containing this variable
	 */
	std::string& get_decl_file() {
		return this->decl_file;
	}

	/**
	 * Get the line number on which this variable is declared
	 *
	 * @return the line number
	 */
	Generic get_decl_line() {
		return decl_line;
	}

	/**
	 * Get the low pc address for a subprogram
	 *
	 * @return the lowest pc address in the subprogram
	 */
	Generic get_low_pc() {
		return low_pc;
	}

	/**
	 * Get the offset from the lowest pc of the highest pc in a subprogram
	 *
	 * @return the high pc offset
	 */
	Generic get_high_pc() {
		return high_pc;
	}

	/**
	 * Get the location of a variable in memory
	 *
	 * @return the high pc offset
	 */
	Generic get_location() {
		return location;
	}

	/**
	 * Get the start_pc of a subprogram in memory
	 *
	 * @return the start pc
	 */
	Generic get_start_pc() {
		return start_pc;
	}

	/**
	 * Check if a pc is in this compilation unit
	 *
	 * @param pc the pc to check
	 * @return true if the pc is found in this compilation unit, otherwise false.
	 */
	bool pc_in_range(Generic pc);

	/**
	 * Find the variable declared in this procedure that has the given address. If there is no
	 * local variable, then check the global symbol table for a global variable
	 *
	 * @param mem_addr the memory address for some variable
	 * @return a pointer to the var_record of a variable having the given address, nullptr if no such
	 * 			variable
	 */
	vdw_var_record *find_address(Generic mem_addr);

	/**
	 * Write a var table record to a file
	 *
	 * @param fn the file name for the cu
	 * @param fp a file pointer
	 */
	virtual void write(std::string fn,FILE *fp);

};

class vdw_var_table: public symbol_table {

public:
	vdw_var_table() :
			symbol_table(VAR_TABLE) {
	}
	virtual ~vdw_var_table();

	vdw_var_record* get(std::string str) { return (vdw_var_record*) symbol_table::get(str); }

	/**
	 * Write the var table to a file
	 *
	 * @param fp a file pointer
	 */
	virtual void write(FILE *fp);
};

#endif
