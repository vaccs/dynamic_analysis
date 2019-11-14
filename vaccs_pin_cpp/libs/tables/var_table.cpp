/******************************************************************************
*
* File: var_table.cpp
*
* Summary: This file contains the code for building tables for mapping
*          dwarf variables to their attributes
*
*
******************************************************************************/

#include <signal.h>
#include <setjmp.h>
#include <util/general.h>
#include <tables/var_table.h>
#include <tables/symbol_table_factory.h>
#include <tables/deref.h>
#include <strings.h>
#include <string>
#include <sstream>
#include <iostream>

const string void_type = "void";
const pair<string, var_record*> default_var_pair("", NULL);

/**
 * The constructor. We use a builder pattern, so there are no parameters
 */
var_record::var_record() : symbol_table_record(VAR_RECORD)
{

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
var_record::var_record(var_record *vrec) : symbol_table_record(VAR_RECORD)
{

	symbol_table_factory factory;

	DEBUGL(LOG("Copying var_record\n"));
	if (vrec->get_local_var_table() != NULL)
		local_var_table = (var_table*)factory.copy_symbol_table(VAR_TABLE, vrec->get_local_var_table());
	else
		local_var_table = NULL;

	if (vrec->get_member_table() != NULL)
		member_table = (var_table*)factory.copy_symbol_table(VAR_TABLE, vrec->get_member_table());
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

var_record::~var_record()
{
}

/**
 * Write a var table record to a file
 *
 * @param fn the file name for the cu
 * @param fp a file pointer
 */
void var_record::write(string key, NATIVE_FD fd)
{
	USIZE size =  sizeof(id); assert(OS_WriteFD(fd, &id, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size_t length = key.length();

	size =  sizeof(length); assert(OS_WriteFD(fd, &length, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd, key.c_str(), &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(is_local); assert(OS_WriteFD(fd, &is_local, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(is_param); assert(OS_WriteFD(fd, &is_param, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	length = decl_file.length();
	size =  sizeof(length); assert(OS_WriteFD(fd, &length, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd, decl_file.c_str(), &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size = sizeof(decl_line); assert(OS_WriteFD(fd, &decl_line, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(low_pc); assert(OS_WriteFD(fd, &low_pc, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(high_pc); assert(OS_WriteFD(fd, &high_pc, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  sizeof(location); assert(OS_WriteFD(fd, &location, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	length = type.length();
	size =  sizeof(length); assert(OS_WriteFD(fd, &length, &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size =  length; assert(OS_WriteFD(fd, type.c_str(), &size).generic_err == OS_RETURN_CODE_NO_ERROR);
	size = sizeof(is_subprog); assert(OS_WriteFD(fd, &is_subprog, &size).generic_err == OS_RETURN_CODE_NO_ERROR);

	if (local_var_table != NULL)
		local_var_table->write(fd);
}

/**
 * Check if a pc is in this subprogram
 *
 * @param pc the pc to check
 * @return true if the pc is found in this subprogram, otherwise false.
 */
bool var_record::pc_in_range(Generic pc)
{
	DEBUGL(LOG("In var_record::pc_in_range: check if pc = " + hexstr(pc) + " is between " + hexstr(low_pc + text_base_address) + " and " + hexstr(high_pc + text_base_address) + "\n"));
	return pc >= (low_pc + text_base_address) && pc < (high_pc + text_base_address);
}

/**
 * Find the local variable declared in this procedure that has the given address.
 *
 * @param ctxt a pin process context
 * @param mem_addr the memory address for some variable
 * @param ttab the type table for this compilation unit
 * @return a name/var_record pair of a variable having the given address, default_var_pair if no such
 *          variable
 */
pair<string, var_record*> var_record::find_address_in_subprog(const CONTEXT *ctxt, Generic mem_addr, type_table *ttab)
{
	pair<string, var_record*> vpair = default_var_pair;
	DEBUGL(LOG("In var_record::find_address_in_subprog\n"));

	if (is_subprog)
		for (map<string, symbol_table_record*>::iterator it = local_var_table->begin();
		     it != local_var_table->end();
		     it++) {

			var_record* tvrec = (var_record*)it->second;
			type_record *trec = ttab->get(tvrec->get_type());

			DEBUGL(LOG("Checking variable " + it->first + "\n"));

			if (!tvrec->get_is_subprog() && tvrec->is_at_address(ctxt, mem_addr, trec)) {
				vpair.first = it->first;
				vpair.second = tvrec;
				break;
			}
		}

	DEBUGL(LOG("Leave var_record::find_address_in_subprog\n"));

	return vpair;

}

/**
 * Find local variables that point to the address.
 *
 * @param ctxt a pin process context
 * @param mem_addr the memory address for some variable
 * @param ttab the type table for this compilation unit
 * @return a name/var_record pair of a variable having the given address, default_var_pair if no such
 *          variable
 */
list<pair<string, var_record*> > *
var_record::find_pointers_to_address_in_subprog(const CONTEXT *ctxt, Generic mem_addr, type_table *ttab)
{

	list<pair<string, var_record*> > *pointer_list = new list<pair<string, var_record*> >();

	DEBUGL(LOG("In var_record::find_pointers_to_address_in_subprog\n"));

	if (is_subprog)
		for (map<string, symbol_table_record*>::iterator it = local_var_table->begin();
		     it != local_var_table->end();
		     it++) {
			pair<string, var_record*> *vpair = new pair<string, var_record*>(it->first, (var_record*)it->second);
			type_record *trec = ttab->get(vpair->second->get_type());

			DEBUGL(LOG("Checking variable " + it->first + "\n"));

			if (trec->get_is_pointer() && vpair->second->points_to_address(ctxt, mem_addr, trec)) {
				DEBUGL(LOG(it->first + " points to address " + hexstr(mem_addr)));
				pointer_list->push_back(*vpair);
			}
		}

	DEBUGL(LOG("Leave var_record::find_pointers_to_address_in_subprog\n"));

	return pointer_list;

}

Generic var_record::deref_if_by_reference(type_record *trec, Generic var_addr)
{

	if (is_param && (trec->get_is_array() || trec->get_is_pointer())) {
		DEBUGL(LOG("Dereferencing a parameter to get the address"));
		bool is_segv;
		Generic addr = dereference_memory((Generic*)var_addr, &is_segv);
		return addr;
	} else
		return var_addr;
}

/**
 * Determine if this variable is at the specified address
 *
 * @param ctxt a pin process context
 * @param mem_addr the prospective memory address
 * @param trec the type record for this variable
 * @return true if the variable is at the prospective address, otherwise false
 */
bool var_record::is_at_address(const CONTEXT *ctxt, Generic mem_addr, type_record *trec)
{

	DEBUGL(LOG("Enter is_at_address\n"));
	DEBUGL(LOG("trec = " + hexstr(trec) + " type name = " + *trec->get_name() + "\n"));
	Generic var_addr = get_var_address(ctxt, trec);

	DEBUGL(LOG("var_addr: " + hexstr(var_addr) + " mem_addr: " + hexstr(mem_addr) +  " size: " +
	           decstr(trec->get_size()) + "\n"));

	//if (trec->get_is_array() || trec->get_is_struct()) {

	// is this an access to some element of an array or struct

	return (mem_addr >= var_addr) && (mem_addr < var_addr + trec->get_size());
	//} else
	//return var_addr == mem_addr;
}

/**
 * Determine the address of this variable
 *
 * @param ctxt a pin process context
 * @param mem_addr the prospective memory address
 * @param trec the type record for this variable
 * @return true if the variable is at the prospective address, otherwise false
 */
Generic var_record::get_var_address(const CONTEXT *ctxt, type_record *trec)
{

	Generic var_addr = get_base_address(ctxt);

	var_addr = deref_if_by_reference(trec, var_addr);

	return var_addr;
}

/**
 * Determine if this variable points to the specified address
 *
 * @param ctxt a pin process context
 * @param mem_addr the prospective memory address
 * @param trec the type record for this variable
 * @return true if the variable is at the prospective address, otherwise false
 */
bool var_record::points_to_address(const CONTEXT *ctxt, Generic mem_addr, type_record *trec)
{
	DEBUGL(LOG("Enter points_to_address\n"));
	DEBUGL(LOG("trec = " + hexstr(trec) + " type name = " + *trec->get_name() + "\n"));
	Generic *var_addr;

	var_addr = (Generic*)get_base_address(ctxt);

	DEBUGL(LOG("*var_addr: " + hexstr(var_addr) + " mem_addr: " + hexstr(mem_addr) +  " size: " + decstr(trec->get_size()) + "\n"));
	return *var_addr == mem_addr;
}

/**
 * Get the base address of this variable in a given execution context
 * @param ctxt a pin process context
 * @return the base address of this variable in a given execution context
 */
Generic var_record::get_base_address(const CONTEXT *ctxt)
{
	Generic base_address;

	if (is_local || is_param) {

		// location is the offset from the frame pointer

		base_address = PIN_GetContextReg( ctxt, REG_GBP) + location;
		DEBUGL(LOG("Base address for a local or param is: " + hexstr(base_address) + "\n"));
	} else {
		base_address = location + data_base_address;           // location is the actual address
		DEBUGL(LOG("Base address for a global is: " + hexstr(base_address) + "\n"));
	}

	return base_address;
}

/**
 * Get the scope name of a var_record for a local variable
 *
 * @param vrec a variable record
 * @return a true if this variable is in this subprogram's scope, otherwise false
 */
bool var_record::get_scope(var_record *vrec)
{
	bool is_in_scope = false;

	if (is_subprog)
		for (map<string, symbol_table_record*>::iterator it = local_var_table->begin();
		     !is_in_scope && it != local_var_table->end();
		     it++) {

			var_record* tvrec = (var_record*)it->second;

			is_in_scope = (vrec == tvrec);
		}

	return is_in_scope;
}

static jmp_buf current_env;

static void segv_handler(int signum)
{

	DEBUGL(LOG("Caught a SIGSEGV in dereference_memory()"));
	longjmp(current_env, 1);

}

/**
 * Get the value at an address based on a particular type
 *
 * @param trec a type record indicating the type of the value stored at the address
 * @param addr a memory address
 * @return a string containg the value stored at the address
 */
string var_record::read_singleton_value(type_record *trec, Generic addr)
{
	string value = MEM_ADDR_ERROR(addr);
	string type_name = *(trec->get_name());

	DEBUGL(LOG("In var_record::read_singleton_value\n"));
	DEBUGL(LOG("Type is " + type_name + "\n"));
	DEBUGL(LOG("Reading " + decstr(trec->get_size()) + " bytes at address: " + MEM_ADDR_STR(addr) + "\n"));


	// set up a signal handler for a segmentation fault in case the address (addr) is not valid

	struct sigaction new_action, old_action;

	new_action.sa_handler = segv_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction(SIGSEGV, NULL, &old_action);

	sigaction(SIGSEGV, &new_action, NULL);

	// Save the current state using setjmp(), if a SIGSEGV is thrown, then segv_handler is called
	// segv_handler will LOG the error and call longjmp() to return control to the
	// if statement with have a return code of 1
	// In this case, the value of the memory location will be the default string defined above
	// This prevents the analysis from failing when accessing the monitored programs
	// variables that might not be set up properly yet or might be errors. The program
	// itself may error, but the analysis will not.

	if (!setjmp(current_env)) {

		ostringstream convert;
		if (addr == 0)
			value = NULL_STR;
		else if (trec->get_is_pointer() || trec->get_is_array()) {

			//  return the actual pointer value since this pointer has already been dereferenced once

			value = MEM_ADDR_STR(addr);
		} else if (type_name.find("long long") != string::npos) {

			// interpret data as long long

			if (type_name.find("unsigned") != string::npos) {

				unsigned long long *ptr = (unsigned long long*)addr;
				DEBUGL(LOG("Interpreting data as an unsigned long long\n"));
				convert << *ptr;
			} else {

				long long *ptr = (long long*)addr;
				DEBUGL(LOG("Interpreting data as a long long\n"));
				convert << *ptr;
			}

			value = convert.str();

		} else if (type_name.find("long") != string::npos) {

			// interpret data as long

			if (type_name.find("unsigned") != string::npos) {
				unsigned long *ptr = (unsigned long*)addr;
				DEBUGL(LOG("Interpreting data as an unsigned long, value is " + decstr(*ptr) + "\n"));
				convert << *ptr;
			} else {
				long *ptr = (long*)addr;
				DEBUGL(LOG("Interpreting data as a long, value is " + decstr(*ptr) + "\n"));
				convert << *ptr;
			}

			value = convert.str();

		} else if (type_name.find("short") != string::npos) {

			// interpret data as short

			if (type_name.find("unsigned") != string::npos) {
				unsigned short *ptr = (unsigned short*)addr;
				DEBUGL(LOG("Interpreting data as an unsigned short, value is " + decstr(*ptr) + "\n"));
				convert << *ptr;
			} else {
				short *ptr = (short*)addr;
				DEBUGL(LOG("Interpreting data as a short, value is " + decstr(*ptr) + "\n"));
				convert << *ptr;
			}

			value = convert.str();

		} else if (type_name.find("char") != string::npos) {
			// interpret data as a char

			if (type_name.find("unsigned") != string::npos) {
				unsigned char *ptr = (unsigned char*)addr;
				DEBUGL(LOG("Interpreting data as an unsigned, value is " + decstr(*ptr) + "\n"));
				convert << *ptr;
			} else {
				char *ptr = (char*)addr;
				DEBUGL(LOG("Interpreting data as a byte, value is " + decstr(*ptr) + "\n"));
				convert << *ptr;
			}

			value = convert.str();

			if (value[0] == '\0')
				value = "\\0";

		} else if (type_name.find("int") != string::npos) {
			// interpret data as an int

			if (type_name.find("unsigned") != string::npos) {
				unsigned int *ptr = (unsigned int*)addr;
				DEBUGL(LOG("Interpreting data as an unsigned int, value is " + decstr(*ptr) + "\n"));
				convert << *ptr;
			} else {
				int *ptr = (int*)addr;
				DEBUGL(LOG("Interpreting data as an int, value is " + decstr(*ptr) + "\n"));
				convert << *ptr;
			}

			value = convert.str();

		} else
			value = "<multielement>";
	}

	// reset the signal handler for SIGSEGV to the original handler.

	sigaction(SIGSEGV, &old_action, NULL);

	return value;

}

/**
 * Get a value at an address contained withing a struct data structure
 *
 * @param ttab a type table
 * @param trec a type record indicating the struct type of the value stored at the address
 * @param addr a memory address
 * @param ctxt a PIN runtime context
 * @return a string containg the value stored at the address
 */
string var_record::read_struct_value(type_table *ttab, type_record *trec, Generic addr, CONTEXT *ctxt)
{
	DEBUGL(LOG("read_struct_value: Reading a value in a struct at address: " + hexstr(addr)));
	if (is_first_access())
		return "<multielement>";

	var_table *memtab = get_member_table();
	string value = "";

	for (std::map<std::string, symbol_table_record*>::iterator it = memtab->begin();
	     it != memtab->end();
	     it++) {

		var_record *mvrec = (var_record*)it->second;
		type_record *mtrec = ttab->get(mvrec->get_type());

		if (mvrec->is_at_address(ctxt, addr, mtrec)) {
			value = read_value(ttab, mtrec, addr, ctxt);
			break;
		}
	}

	return value;
}
/**
 * Get a value at an address contained within an array
 *
 * @param ttab a type table
 * @param trec a type record indicating the array type of the value stored at the address
 * @param addr a memory address
 * @param ctxt a PIN runtime context
 * @return a string containg the value stored at the address
 */
string var_record::read_array_value(type_table *ttab, type_record *trec, Generic addr, CONTEXT *ctxt)
{
	DEBUGL(LOG("read_array_value: Reading a value in a struct at address: " + hexstr(addr)));
	if (is_first_access())
		return "<multielement>";


	type_record *atrec = ttab->get(*trec->get_base_type());
	Generic element_size = atrec->get_size();
	symbol_table_record_factory factory;

	int index = (addr - get_base_address(ctxt)) / element_size;
	DEBUGL(LOG("Computed index = " + decstr(index) + "\n"));
	addr +=  (element_size * index);

	return read_value(ttab, atrec, addr, ctxt);
}

string read_c_string(Generic addr, type_record *trec)
{
	DEBUGL(LOG("Reading C string at addr " + MEM_ADDR_STR(addr) + "\n"));
	string value;

	struct sigaction new_action, old_action;

	new_action.sa_handler = segv_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction(SIGSEGV, NULL, &old_action);

	sigaction(SIGSEGV, &new_action, NULL);

	// Save the current state using setjmp(), if a SIGSEGV is thrown, then segv_handler is called
	// segv_handler will LOG the error and call longjmp() to return control to the
	// if statement with have a return code of 1
	// In this case, the value of the memory location will be the default string defined above
	// This prevents the analysis from failing when accessing the monitored programs
	// variables that might not be set up properly yet or might be errors. The program
	// itself may error, but the analysis will not.

	if (!setjmp(current_env)) {
		char *ptr = (char*)addr;
		value = "";
		int upb = VACCS_MAX_STRING_LENGTH;
		for (int i = 0; *ptr != '\0' && i < upb; i++, ptr++)
			value += *ptr;

		if (*ptr != '\0')
			value += "...";
		else if (value.length() == 0)
			value = NULL_STR;
	} else {
		value = MEM_ADDR_ERROR(addr);
	}

	// reset the signal handler for SIGSEGV to the original handler.

	sigaction(SIGSEGV, &old_action, NULL);

	return value;
}

/**
 * Get the value at an address based on a particular type
 *
 * @param trec a type record indicating the type of the value stored at the address
 * @param addr a memory address
 * @return a string containg the value stored at the address
 */
string var_record::read_value(type_table *ttab, type_record *trec, Generic addr, CONTEXT *ctxt)
{
	string value;
	string type_name = *(trec->get_name());

	DEBUGL(LOG("In var_record::read_value\n"));
	DEBUGL(LOG("Type is " + type_name + "\n"));
	DEBUGL(LOG("Reading " + decstr(trec->get_size()) + " bytes at address: " + MEM_ADDR_STR(addr) + "\n"));

	if (addr == 0)
		value = NULL_STR;
	else if (trec->get_is_pointer()) {
		DEBUGL(LOG("Found a pointer\n"));
		type_record *btrec = ttab->get(*trec->get_base_type());
		string bt_name = *btrec->get_name();

		// handle a char* as a string

		if (!btrec->get_is_pointer() && !btrec->get_is_array() && bt_name.find("char") != string::npos) {
			DEBUGL(LOG("Found a character string\n"));
			value = read_c_string(addr,trec);
		} else if (btrec->get_is_struct())
			value = read_struct_value(ttab, btrec, addr, ctxt);
		else {
			DEBUGL(LOG("Found a non-character pointer to an atomic value\n"));
			bool is_segv;
			addr = dereference_memory((Generic*)addr, &is_segv);               // get the pointer value
			if (is_segv)
				value = MEM_ADDR_ERROR(addr);
			else
				value = MEM_ADDR_STR(addr);
		}
		DEBUGL(LOG("value = " + value + "\n"));
	} else if (trec->get_is_array()) {
		DEBUGL(LOG("Found an array\n"));
		type_record *btrec = ttab->get(*trec->get_base_type());
		string bt_name = *btrec->get_name();

		// read a char array as a string

		if (!btrec->get_is_pointer() && !btrec->get_is_array() && bt_name.find("char") != string::npos) {
			DEBUGL(LOG("Found a character string\n"));
			value = read_c_string(addr,trec);
		} else {
			DEBUGL(LOG("Found a non-character array\n"));
			value = read_array_value(ttab, btrec, addr, ctxt);
		}
	} else if (trec->get_is_struct())
		value = read_struct_value(ttab, trec, addr, ctxt);
	else {
		DEBUGL(LOG("Found a singleton value\n"));
		value = read_singleton_value(trec, addr);
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
void var_record::propagate_local_info(type_table *ttab, Generic location, bool is_local, bool is_param)
{
	this->is_local = is_local;
	this->is_param = is_param;
	this->location += location;

	DEBUGL(LOG("New updated address = " + hexstr(this->location) + "\n"));
	type_record *trec = ttab->get(type);
	if (trec->get_is_struct()) {
		symbol_table_factory factory;
		member_table = (var_table*)factory.copy_symbol_table(VAR_TABLE, trec->get_member_table());
		member_table->propagate_local_info(ttab, this->location, this->is_local, this->is_param);
	}
}

void var_record::debug_emit(string var)
{
	DEBUGL(LOG("var_record for " + var + "\n"));
	DEBUGL(LOG("\t id = " + decstr(id) + "\n"));
	DEBUGL(LOG("\t is_local = " + decstr((int)is_local) + "\n"));
	DEBUGL(LOG("\t is_param = " + decstr((int)is_param) + "\n"));
	DEBUGL(LOG("\t file = " + decl_file + "\n"));
	DEBUGL(LOG("\t line = " + decstr(decl_line) + "\n"));
	DEBUGL(LOG("\t low_pc = " + hexstr(low_pc) + "\n"));
	DEBUGL(LOG("\t high_pc = " + hexstr(high_pc) + "\n"));
	DEBUGL(LOG("\t location = " + hexstr(location) + "\n"));
	DEBUGL(LOG("\t type id = " + type + "\n"));
	DEBUGL(LOG("\t is_subprog = " + decstr((int)is_subprog) + "\n"));
}

/**
 * Create member tables for structure variables
 *
 * @param type_table a type table
 */
void var_record::create_member_tables(type_table *ttab)
{
	DEBUGL(LOG("var_table::create_member_tables\n"));
	if (is_subprog)
		local_var_table->create_member_tables(ttab);
	else {
		type_record *trec = ttab->get(type);
		if (trec->get_is_struct()) {
			symbol_table_factory factory;
			DEBUGL(LOG("Propagate local info from address " + hexstr(location) + "\n"));
			member_table = (var_table*)factory.copy_symbol_table(VAR_TABLE, trec->get_member_table());
			member_table->propagate_local_info(ttab, location, is_local, is_param);
		}
	}
}

/**
 * Copy constructor (deep copy)
 *
 * @param vtab a variable table to copy
 */
var_table::var_table(var_table *vtab) :
	symbol_table(VAR_TABLE)
{

	DEBUGL(LOG("Copying var_table\n"));
	symbol_table_record_factory factory;
	for (map<string, symbol_table_record*>::iterator it = vtab->begin();
	     it != vtab->end();
	     it++ ) {
		DEBUGL(LOG("Calling factory to copy var_record for " + it->first + "\n"));
		var_record *vrec = (var_record*)factory.copy_symbol_table_record(VAR_RECORD, (var_record*)it->second);
		DEBUGL(LOG("Adding var_record for " + it->first + "\n"));

		put(it->first, vrec);
	}

}

var_table::~var_table()
{
}

/**
 * Propagate local information about variables to the structure members
 *
 * @param ttab a type table
 * @param is_local is the variable declared locally
 * @param is_param is the variable a formal parameter
 */
void var_table::propagate_local_info(type_table *ttab, Generic location, bool is_local, bool is_param)
{
	for (map<string, symbol_table_record*>::iterator it = begin();
	     it != end();
	     it++ ) {
		var_record *vrec = (var_record*)it->second;
		DEBUGL(LOG("Propagate local info for variable " + it->first + "\n"));
		vrec->propagate_local_info(ttab, location, is_local, is_param);
	}
}

/**
 * Create member tables for structure variables
 *
 * @param type_table a type table
 */
void var_table::create_member_tables(type_table *ttab)
{
	DEBUGL(LOG("var_table::create_member_tables\n"));
	for (map<string, symbol_table_record*>::iterator it = begin();
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
void var_table::write(NATIVE_FD fd)
{
	USIZE dsize =  sizeof(id); assert(OS_WriteFD(fd, &id, &dsize).generic_err == OS_RETURN_CODE_NO_ERROR);

	size_t num = size();

	dsize = sizeof(num); assert(OS_WriteFD(fd, &num, &dsize).generic_err == OS_RETURN_CODE_NO_ERROR);

	for (map<string, symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		var_record* vrec = (var_record*)it->second;
		vrec->write(it->first, fd);
	}
}
