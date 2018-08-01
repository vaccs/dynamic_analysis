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

/**
 * Class: type_record
 *
 * The information found in the debug information of an executable for
 * the types in a compilation unit.
 *
 */
class type_record: public symbol_table_record {

private:
	Generic size; /* the # of bytes for a type */
	std::string *name; /* the name of the type */
	bool is_array; /* is the type an array? */
	std::string *base_type; /* the dwarf index of the base type for an array */
	Generic upper_bound; /* the upper bound for the array type */

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
	 * @param size the size of this type
	 * @return the object
	 */
	type_record* add_size(Generic size) {
		this->size = size;
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
	 * Add the is_array filed to the object using a builder pattern
	 *
	 * @return the object
	 */
	type_record* add_is_array() {
		this->is_array = true;
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
	 * Get the size from the object
	 *
	 * @return the size
	 */
	Generic get_size() {
		return size;
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
	 * Write a type table record to a file
	 *
	 * @param fn the file name for the cu
	 * @param fp a file pointer
	 */
	virtual void write(std::string fn,FILE *fp);
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
	virtual void write(FILE *fp);
};

#endif
