/******************************************************************************
 *
 * File: var_table.cpp
 *
 * Summary: This file contains the code for building tables for mapping
 *          dwarf variables to their attributes
 *
 *
 ******************************************************************************/

#include <util/general.h>
#include <tables/var_table.h>
#include <tables/symbol_table_factory.h>
#include <strings.h>
#include <string>
#include <sstream>
#include <iostream>

const std::string void_type = "void";
const std::pair<std::string,var_record*> default_var_pair("",NULL);
/**
 * The constructor. We use a builder pattern, so there are no parameters
 */
var_record::var_record() :
		symbol_table_record(VAR_RECORD) {
	is_subprog = false;
	local_var_table = NULL;
	member_table = NULL;
	is_local = false;
	is_param = false;
	decl_line = -1;
	low_pc = -1;
	high_pc = -1;
	type = void_type;
	location = 0;
	first_access = true;
}

/**
 * A copy constructor
 *
 * @param vrec a var_record to copy
 */
var_record::var_record(var_record *vrec) :
	symbol_table_record(VAR_RECORD) {

	symbol_table_factory factory;

	DEBUGL(cerr << "Copying var_record\n");
	if (vrec->get_local_var_table() != NULL)
		local_var_table = (var_table *)factory.copy_symbol_table(VAR_TABLE,vrec->get_local_var_table());
	else
		local_var_table = NULL;

	if (vrec->get_member_table() != NULL)
		member_table = (var_table *)factory.copy_symbol_table(VAR_TABLE,vrec->get_member_table());
	else
		member_table = NULL;


	is_subprog = vrec->get_is_subprog();
	is_local = vrec->get_is_local();
	is_param = vrec->get_is_param();
	decl_line = vrec->get_decl_line();
	low_pc = vrec->get_low_pc();
	high_pc = vrec->get_high_pc();
	type = vrec->get_type();
	location = vrec->get_location();
	first_access = vrec->is_first_access();
}

var_record::~var_record() {}
/**
 * Write a var table record to a file
 *
 * @param fn the file name for the cu
 * @param fp a file pointer
 */
void var_record::write(std::string key,NATIVE_FD fd) {
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd,&id,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size_t length = key.length();
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,key.c_str(),&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(is_local); assert(OS_WriteFD(fd,&is_local,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(is_param); assert(OS_WriteFD(fd,&is_param,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	length = decl_file.length();
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,decl_file.c_str(),&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size = sizeof(decl_line); assert(OS_WriteFD(fd,&decl_line,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(low_pc); assert(OS_WriteFD(fd,&low_pc,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(high_pc); assert(OS_WriteFD(fd,&high_pc,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(location); assert(OS_WriteFD(fd,&location,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	length = type.length();
	size =  sizeof(length); assert(OS_WriteFD(fd,&length,&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd,type.c_str(),&size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size = sizeof(is_subprog); assert(OS_WriteFD(fd,&is_subprog,&size).generic_err == OS_RETURN_CODE_NO_ERROR);

	if (local_var_table != NULL) {
		local_var_table->write(fd);
	}
}

/**
 * Check if a pc is in this subprogram
 *
 * @param pc the pc to check
 * @return true if the pc is found in this subprogram, otherwise false.
 */
bool var_record::pc_in_range(Generic pc) {
	DEBUGL(LOG("In var_record::pc_in_range\n"));
	return (pc >= low_pc && pc < high_pc);
}

/**
 * Find the local variable declared in this procedure that has the given address.
 *
 * @param ctxt a pin process context
 * @param mem_addr the memory address for some variable
 * @param ttab the type table for this compilation unit
 * @return a name/var_record pair of a variable having the given address, default_var_pair if no such
 * 			variable
 */
std::pair<std::string,var_record*> var_record::find_address_in_subprog(const CONTEXT *ctxt,Generic mem_addr,type_table *ttab) {
	std::pair<std::string,var_record*> vpair = default_var_pair;
	DEBUGL(LOG("In var_record::find_address_in_subprog\n"));

	if (is_subprog)
		for (std::map<std::string,symbol_table_record*>::iterator it = local_var_table->begin();
				it != local_var_table->end();
				it++) {
			var_record* tvrec = (var_record*)it->second;
			type_record *trec = ttab->get(tvrec->get_type());
			DEBUGL(LOG("Checking variable "+ it->first +"\n"));
			if (!tvrec->get_is_subprog() && tvrec->is_at_address(ctxt,mem_addr,trec)) {
				vpair.first = it->first;
				vpair.second = tvrec;
				break;
			}
		}

	DEBUGL(LOG("Leave var_record::find_address_in_subprog\n"));
	return vpair;

}

/**
 * Determine if this variable is at the specified address
 *
 * @param ctxt a pin process context
 * @param mem_addr the prospective memory address
 * @param trec the type record for this variable
 * @return true if the variable is at the prospective address, otherwise false
 */
bool var_record::is_at_address(const CONTEXT *ctxt,Generic mem_addr, type_record *trec) {
	DEBUGL(LOG("Enter is_at_address\n"));
	DEBUGL(LOG("trec = " + hexstr(trec) + "\ntype name = "+*trec->get_name()+ "\n"));
	Generic var_addr;

	var_addr = get_base_address(ctxt);

	DEBUGL(LOG("var_addr: " +hexstr(var_addr) + " mem_addr: " + hexstr(mem_addr) +  " size: " + decstr(trec->get_size())+ "\n"));
	if (trec->get_is_array() || trec->get_is_struct()) { // is this an access to some element of an array or struct
		return (mem_addr >= var_addr) && (mem_addr < var_addr + trec->get_size());
	} else
		return var_addr == mem_addr;
}

/**
 * Get the base address of this variable in a given execution context
 * @param ctxt a pin process context
 * @return the base address of this variable in a given execution context
 */
Generic var_record::get_base_address(const CONTEXT *ctxt) {
	Generic base_address;

	if (is_local || is_param) {
		DEBUGL(LOG("Getting base address for a local or param\n"));
		base_address = PIN_GetContextReg( ctxt, REG_GBP) + location; // location is the offset from the frame pointer
	} else {
		DEBUGL(LOG("Getting base address for a global\n"));
		base_address = location; // location is the actual address
	}

	return base_address;
}

/**
 * Get the scope name of a var_record for a local variable 
 *
 * @param vrec a variable record
 * @return a true if this variable is in this subprogram's scope, otherwise false
 */
bool var_record::get_scope(var_record *vrec) {
	bool is_in_scope = false;

	if (is_subprog)
		for (std::map<std::string,symbol_table_record*>::iterator it = local_var_table->begin();
				!is_in_scope && it != local_var_table->end();
				it++) {
			var_record* tvrec = (var_record*)it->second;
			is_in_scope = (vrec == tvrec);
		}

	return is_in_scope;
}

/**
 * Get the value at an address based on a particular type
 *
 * @param trec a type record indicating the type of the value stored at the address
 * @param addr a memory address
 * @return a string containg the value stored at the address
 */
std::string var_record::read_value(type_record *trec, Generic addr) {
	std::string value;
	std::string type_name = *(trec->get_name());

	DEBUGL(LOG("In var_recod::read_value\n"));
	DEBUGL(LOG("Type is " + type_name + "\n"));
	DEBUGL(LOG("Reading " + decstr(trec->get_size()) + " bytes at address: " + hexstr(addr) + "\n"));

	ostringstream convert;

	if (type_name.find("*") != std::string::npos) {
		if (type_name.find("char") != std::string::npos) {
			char *ptr = (char *)addr;
			value = "";
			for (int i = 0; *ptr != '\0' && i < 10; i++)
				value += *ptr;

			if (*ptr != '\0')
				value += "...";

		} else {
			Generic *ptr = (Generic*)addr;
			convert << *ptr;
			value = convert.str();
		}
	} else {

		if (type_name.find("long long") != std::string::npos) {
			// interpret data as long long

			if (type_name.find("unsigned") != std::string::npos) {
				unsigned long long *ptr =(unsigned long long *)addr;
				convert << *ptr;
			}else {
				long long *ptr = (long long *)addr;
				convert << *ptr;
			}

			value = convert.str();

		} else if (type_name.find("long") != std::string::npos) {
			// interpret data as long

			if (type_name.find("unsigned") != std::string::npos) {
				unsigned long *ptr = (unsigned long *)addr;
				convert << *ptr;
			}
			else {
				long *ptr = (long *)addr;
				convert << *ptr;
			}

			value = convert.str();

		} else if (type_name.find("short") != std::string::npos) {
			// interpret data as short

			if (type_name.find("unsigned") != std::string::npos) {
				unsigned short *ptr = (unsigned short *)addr;
				convert << *ptr;
			}
			else {
				short *ptr = (short *)addr;
				convert << *ptr;
			}

			value = convert.str();

		} else if (type_name.find("char") != std::string::npos) {
			// interpret data as a char

			if (type_name.find("unsigned") != std::string::npos) {
				unsigned char *ptr = (unsigned char *)addr;
				convert << *ptr;
			}
			else {
				char *ptr = (char *)addr;
				convert << *ptr;
			}

			value = convert.str();

		} else if (type_name.find("int") != std::string::npos) {
			// interpret data as an int

			if (type_name.find("unsigned") != std::string::npos) {
				unsigned int *ptr = (unsigned int *)addr;
				convert << *ptr;
			}
			else {
				int *ptr = (int *)addr;
				convert << *ptr;
			}

			value = convert.str();

		} else {
			LOG("Type not supported: " + type_name + "\n");
			PIN_ExitProcess(-1);
		}
	}

	return value;
}

/**
 * Propagate local information about variables to the structure members
 *
 * @param ttab a type table
 * @param is_local is the variable declared locally
 * @param is_param is the variable a formal parameter
 */
void var_record::propagate_local_info(type_table *ttab,Generic location, bool is_local, bool is_param) {
	this->is_local = is_local;
	this->is_param = is_param;
	this->location += location;

	DEBUGL(cerr << "New updated address = " + hexstr(this->location) + "\n");
	type_record *trec = ttab->get(type);
	if (trec->get_is_struct()) {
		symbol_table_factory factory;
		member_table = (var_table *)factory.copy_symbol_table(VAR_TABLE,trec->get_member_table());
		member_table->propagate_local_info(ttab,this->location,this->is_local,this->is_param);
	}
}

/**
 * Create member tables for structure variables
 *
 * @param type_table a type table
 */
void var_record::create_member_tables(type_table *ttab) {
	DEBUGL(cerr << "var_table::create_member_tables\n");
	if (is_subprog)
		local_var_table->create_member_tables(ttab);
	else {
		type_record *trec = ttab->get(type);
		if (trec->get_is_struct()) {
			symbol_table_factory factory;
			DEBUGL(cerr << "Propagate local info from address " + hexstr(location) + "\n");
			member_table = (var_table *)factory.copy_symbol_table(VAR_TABLE,trec->get_member_table());
			member_table->propagate_local_info(ttab,location,is_local,is_param);
		}
	}
}

/**
 * Copy constructor (deep copy)
 *
 * @param vtab a variable table to copy
 */
var_table::var_table(var_table *vtab) :
	symbol_table(VAR_TABLE) {

	DEBUGL(cerr << "Copying var_table\n");
	symbol_table_record_factory factory;
	for (std::map<std::string,symbol_table_record*>::iterator it = vtab->begin();
			it != vtab->end();
			it++ ) {
		DEBUGL(cerr << "Calling factory to copy var_record for " + it->first + "\n");
		var_record *vrec = (var_record*)factory.copy_symbol_table_record(VAR_RECORD,(var_record*)it->second);
		DEBUGL(cerr << "Adding var_record for " + it->first + "\n");

		put(it->first,vrec);
	}

}

var_table::~var_table() {}

/**
 * Propagate local information about variables to the structure members
 *
 * @param ttab a type table
 * @param is_local is the variable declared locally
 * @param is_param is the variable a formal parameter
 */
void var_table::propagate_local_info(type_table *ttab,Generic location,bool is_local, bool is_param) {
	for (std::map<std::string,symbol_table_record*>::iterator it = begin();
			it != end();
			it++ ) {
		var_record *vrec = (var_record*)it->second;
		DEBUGL(cerr << "Propagate local info for variable " + it->first + "\n");
		vrec->propagate_local_info(ttab,location,is_local,is_param);
	}
}

/**
 * Create member tables for structure variables
 *
 * @param type_table a type table
 */
void var_table::create_member_tables(type_table *ttab) {
	DEBUGL(cerr << "var_table::create_member_tables\n");
	for (std::map<std::string,symbol_table_record*>::iterator it = begin();
			it != end();
			it++ ) {
		var_record *vrec = (var_record*)it->second;
		vrec->create_member_tables(ttab);
	}

}

/**
 * Write the var table to a file
 *
 * @param fp a file pointer
 */
void var_table::write(NATIVE_FD fd) {
	USIZE dsize =  sizeof(id); assert(OS_WriteFD(fd,&id,&dsize).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t num = size();
	dsize = sizeof(num); assert(OS_WriteFD(fd,&num,&dsize).generic_err == OS_RETURN_CODE_NO_ERROR);

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		var_record* vrec = (var_record*)it->second;
		vrec->write(it->first,fd);
	}
}

