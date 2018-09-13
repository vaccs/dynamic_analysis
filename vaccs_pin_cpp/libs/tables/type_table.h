/**
 * type_table.H
 *
 * This file contains the code for a type table and its associated
 * records 
 *
 */
#ifndef type_table_h
#define type_table_h

#include <util/general.h>
#include <map>
#include <string>

#include <tables/symbol_table.h>
#include <tables/var_table.h>

#include <pin.H>

class var_table;

/**
 * Class: type_record
 *
 * The information found in the debug information of an executable for
 * the types in a compilation unit.
 *
 */
class type_record: public symbol_table_record {

private:
	Generic type_size; /* the # of bytes for a type */
	std::string *name; /* the name of the type */
	bool is_array; /* is the type an array? */
	bool is_pointer; /* is the type a pointer? */
	bool is_struct; /* is the type a struct? */
	bool is_typedef; /* is this type a typedef? */
	bool is_const; /* is this type const? */
	std::string *base_type; /* the dwarf index of the base type for an array */
	Generic upper_bound; /* the upper bound for the array type */
	var_table *member_tab; /* the members of a c struct */

public:
	/**
	 * The constructor. We use a builder pattern, so there are no parameters
	 */
	type_record();

	/**
	 * The destructor
	 */
	virtual ~type_record();

	/**
	 * Add the type size to the object using a builder pattern
	 *
	 * @param type_size the size of this type
	 * @return the object
	 */
	type_record* add_size(Generic type_size) {
		this->type_size = type_size;
		return this;
	}

	/**
	 * Add the type name to the object using a builder pattern
	 *
	 * @param name the name of this type
	 * @return the object
	 */
	type_record* add_name(std::string name) {
		this->name = new std::string(name);
		return this;
	}

	/**
	 * Add the is_pointer filed to the object using a builder pattern
	 *
	 * @return the object
	 */
	type_record* add_is_pointer() {
		this->is_pointer = true;
		return this;
	}

	/**
	 * Add the is_array filed to the object using a builder pattern
	 *
	 * @return the object
	 */
	type_record* add_is_array() {
		this->is_array = true;
		return this;
	}

	/**
	 * Add the is_struct field to the object using a builder pattern
	 *
	 * @return the object
	 */
	type_record* add_is_struct() {
		this->is_struct = true;
		return this;
	}

	/**
	 * Add the is_typedef field to the object using a builder pattern
	 *
	 * @return the object
	 */
	type_record* add_is_typedef() {
		this->is_typedef = true;
		return this;
	}

	/**
	 * Add the is_const field to the object using a builder pattern
	 *
	 * @return the object
	 */
	type_record* add_is_const() {
		this->is_const = true;
		return this;
	}

	/**
	 * Add the base type to the object using a builder pattern
	 *
	 * @param base_type the dwarf index for the base_type of this array type
	 * @return the object
	 */
	type_record* add_base_type(std::string base_type) {
		this->base_type = new std::string(base_type);
		return this;
	}

	/**
	 * Add the upper bound to the object using a builder pattern
	 *
	 * @param upper_bound the upper bound of this array type
	 * @return the object
	 */
	type_record* add_upper_bound(Generic upper_bound,bool compute_name_and_size);

	/**
	 * Add a table for structure members
	 *
	 * @param memtab a table of structure members
	 * @return the object
	 */
	type_record *add_member_table(var_table *memtab) {
		this->member_tab = memtab;
		return this;
	}

	/**
	 * Get the size from the object
	 *
	 * @return the size
	 */
	Generic get_size() {
		return type_size;
	}

	/**
	 * Get the name from the object
	 *
	 * @return the name
	 */
	std::string* get_name() {
		return name;
	}

	/**
	 * Is this an array type?
	 *
	 * @return true if this is an array type; otherwise, false
	 */
	bool get_is_array() {
		return is_array;
	}

	/**
	 * Is this a pointer type?
	 *
	 * @return true if this is a pointer type; otherwise, false
	 */
	bool get_is_pointer() {
		return is_pointer;
	}

	/**
	 * Is this a struct type?
	 *
	 * @return true if this is a struct type; otherwise, false
	 */
	bool get_is_struct() {
		return is_struct;
	}

	/**
	 * Is this a typedef type?
	 *
	 * @return true if this is a typedef type; otherwise, false
	 */
	bool get_is_typedef() {
		return is_typedef;
	}

	/**
	 * Is this a const type?
	 *
	 * @return true if this is a const type; otherwise, false
	 */
	bool get_is_const() {
		return is_const;
	}

	/**
	 * Get the base type from the object
	 *
	 * @return the base type
	 */
	std::string* get_base_type() {
		return base_type;
	}

	/**
	 * Get the upper bound from the object
	 *
	 * @return the upper bound
	 */
	Generic get_upper_bound() {
		return upper_bound;
	}

	/**
	 * Get the structure member table
	 */
	var_table *get_member_table() {
		return member_tab;
	}

	/**
	 * Write a type table record to a file
	 *
	 * @param fn the file name for the cu
	 * @param fp a file pointer
	 */
	virtual void write(std::string fn,NATIVE_FD fd);
};

/**
 * Class: type_table
 *
 * A table mapping dwarf index for types to records containing the debug
 * information for that type.
 */
class type_table: public symbol_table {

public:
	type_table() :
			symbol_table(TYPE_TABLE) {
	}
	virtual ~type_table();

	type_record* get(std::string str) { return (type_record*) symbol_table::get(str); }

	/**
	 * Write the type table to a file
	 *
	 * @param fp a file pointer
	 */
	virtual void write(NATIVE_FD fd);
};

#endif
