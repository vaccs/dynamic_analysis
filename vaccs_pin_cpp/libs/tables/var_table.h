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
#include <tables/type_table.h>

#include <pin.H>

class var_record;

extern const std::string void_type;
extern const std::pair<std::string,var_record*> default_var_pair;

class var_table;
/**
 * Class: var_record
 *
 * The information found in the debug information of an executable for
 * the variables in a compilation unit.
 *
 */
class var_record: public symbol_table_record {

private:

	std::string type; /* the dwarf index for this type */
	bool is_subprog; /* is this variable a subprogram? */
	var_table *local_var_table; /* the local variable declared inside a subprogram */
	var_table *member_table; /* the member of a structure */
	bool is_local; /* is this variable local? */
	bool is_param; /* is this variable a formal parameter */
	std::string decl_file; /* the name of the file in which this is defined */
	Generic decl_line; /* the line of the file on which this is defined */
	Generic low_pc; /* the lowest pc of a subroutine variable */
	Generic high_pc; /* the highest pc of a subroutine variable */
	Generic location; /* an expression storing how to compute the address of the variable*/
	bool first_access; /* is this the first access of this variable in the program? */

public:

	/**
	 * The constructor. We use a builder pattern, so there are no parameters
	 */
	var_record();

	/**
	 * A copy constructor
	 *
	 * @param vrec a var_record to copy
	 */
	var_record(var_record *vrec);

	/**
	 * The destructor
	 */
	virtual ~var_record();

	/**
	 * Add the type to the object using a builder pattern
	 *
	 * @param type the type of this variable
	 * @return the object
	 */
	var_record* add_type(std::string& type) {
		this->type = type;
		return this;
	}

	/**
	 * Add the is_subprog filed to the object using a builder pattern
	 *
	 * @return the object
	 */
	var_record* add_is_subprog() {
		this->is_subprog = true;
		return this;
	}

	/**
	 * Add the local variable table to the object using a builder pattern
	 *
	 * @param local_var_table the table of variables local to a subprogram
	 * @return the object
	 */
	var_record* add_local_var_table(var_table *local_var_table) {
		this->local_var_table = local_var_table;
		return this;
	}

	/**
	 * Add the member table to the object using a builder pattern
	 *
	 * @param member_table the table of structure members
	 * @return the object
	 */
	var_record* add_member_table(var_table *member_table) {
		this->member_table = member_table;
		return this;
	}

	/**
	 * Add the is_local filed to the object using a builder pattern
	 *
	 * @return the object
	 */
	var_record* add_is_local() {
		this->is_local = true;
		return this;
	}

	/**
	 * Add the is_param field to the object using a builder pattern
	 *
	 * @return the object
	 */
	var_record* add_is_param() {
		this->is_param = true;
		return this;
	}

	/**
	 * Add the declartion file to the object using a builder pattern
	 *
	 * @param decl_file the declaration file of this variable
	 * @return the object
	 */
	var_record* add_decl_file(std::string& decl_file) {
		this->decl_file = decl_file;
		return this;
	}

	/**
	 * Add the declartion line to the object using a builder pattern
	 *
	 * @param decl_line the declartion line of this variable
	 * @return the object
	 */
	var_record* add_decl_line(Generic decl_line) {
		this->decl_line = decl_line;
		return this;
	}

	/**
	 * Add the lowest pc to the object using a builder pattern
	 *
	 * @param low_pc the lowest pc of this subprogram
	 * @return the object
	 */
	var_record* add_low_pc(Generic low_pc) {
		this->low_pc = low_pc;
		return this;
	}

	/**
	 * Add the highest pc to the object using a builder pattern
	 *
	 * @param high_pc the highest pc of this subprogram
	 * @return the object
	 */
	var_record* add_high_pc(Generic high_pc) {
		this->high_pc = high_pc;
		return this;
	}

	/**
	 * Add the location  to the object using a builder pattern
	 *
	 * @param location an expression for computing the address of a variable
	 * @return the object
	 */
	var_record* add_location(Generic location) {
		this->location = location;
		return this;
	}

	/**
	 * Set the first_access field to false
	 *
	 * @return the object
	 */
	void clear_first_access() {
		this->first_access = false;
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
	var_table *get_local_var_table() {
		return local_var_table;
	}

	/**
	 * Get the member table for a subprogram
	 *
	 * @return the member table for structure members
	 */
	var_table *get_member_table() {
		return member_table;
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

	/* Get the first access field
	 *
	 * @return true if this is the first access to this variable
	 */
	bool is_first_access() {
		return first_access;
	}

	/**
	 * Check if a pc is in this compilation unit
	 *
	 * @param pc the pc to check
	 * @return true if the pc is found in this compilation unit, otherwise false.
	 */
	bool pc_in_range(Generic pc);

	/**
	 * Find the variable declared in this procedure that has the given address.
	 *
	 * @param ctxt a pin process context
	 * @param mem_addr the memory address for some variable
	 * @param ttab the type table for this compilation unit
	 * @return a name/var_record pair of a variable having the given address, default_var_pair if no such
	 * 			variable
	 */
	std::pair<std::string,var_record*> find_address_in_subprog(const CONTEXT *ctxt, Generic mem_addr,type_table *ttab);

	/**
	 * Determine if this variable is at the specified address
	 *
	 * @param ctxt a pin process context
	 * @param mem_addr the prospective memory address
	 * @param trec the type record for this variable
	 * @return true if the variable is at the prospective address, otherwise false
	 */
	bool is_at_address(const CONTEXT *ctxt, Generic mem_addr, type_record *trec);

	/**
	 * Get the base address of this variable in a given execution context
	 * @param ctxt a pin process context
	 * @return the base address of this variable in a given execution context
	 */
	Generic get_base_address(const CONTEXT *ctxt);

	/**
	 * Get the scope name of a var_record for a local variable 
 	 *
 	 * @param vrec a variable record
 	 * @return a true if this variable is in this subprogram's scope, otherwise false
 	 */
	 bool get_scope(var_record *vrec);

	/**
 	 * Get the value at an address based on a particular type
 	 *
 	 * @param trec a type record indicating the type of the value stored at the address
 	 * @param addr a memory address
 	 * @return a string containg the value stored at the address
  	 */
	std::string read_value(type_record *trec, Generic addr);

	/**
	 * Propagate local information about variables to the structure members
	 *
	 * @param ttab a type table
	 * @param is_local is the variable declared locally
	 * @param is_param is the variable a formal parameter
	 */
	void propagate_local_info(type_table *ttab,Generic location,bool is_local, bool is_param);

	/**
	 * Create member tables for structure variables
	 *
	 * @param type_table a type table
	 */
	void create_member_tables(type_table *ttab);

   /**
	* Write a var table record to a file
	*
	* @param fn the file name for the cu
	* @param fp a file pointer
	*/
   virtual void write(std::string fn,NATIVE_FD fd);

};

class var_table: public symbol_table {

public:
	var_table() :
			symbol_table(VAR_TABLE) {
	}

	/**
	 * Copy constructor (deep copy)
	 *
	 * @param vtab a variable table to copy
	 */
	var_table(var_table *vtab);

	virtual ~var_table();

	var_record* get(std::string str) { return (var_record*) symbol_table::get(str); }

	/**
	 * Propagate local information about variables to the structure members
	 *
	 * @param ttab a type table
	 * @param is_local is the variable declared locally
	 * @param is_param is the variable a formal parameter
	 */
	void propagate_local_info(type_table *ttab,Generic location,bool is_local, bool is_param);

	/**
	 * Create member tables for structure variables
	 *
	 * @param type_table a type table
	 */
	void create_member_tables(type_table *ttab);

   /**
	* Write the var table to a file
	*
	* @param fp a file pointer
	*/
   virtual void write(NATIVE_FD fd);

};

#endif
