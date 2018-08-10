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
	is_local = false;
	is_param = false;
	decl_line = -1;
	low_pc = -1;
	high_pc = -1;
	type = void_type;
	location = 0;
}

var_record::~var_record() {}
/**
 * Write a var table record to a file
 *
 * @param fn the file name for the cu
 * @param fp a file pointer
 */
void var_record::write(std::string key,FILE *fp) {
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	size_t length = key.length();
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(key.c_str(), length, 1, fp) == 1);
	assert(fwrite(&is_local, sizeof(is_local), 1, fp) == 1);
	assert(fwrite(&is_param, sizeof(is_param), 1, fp) == 1);
	length = decl_file.length();
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(decl_file.c_str(), length, 1, fp) == 1);
	assert(fwrite(&decl_line,sizeof(decl_line),1,fp) == 1);
	assert(fwrite(&low_pc, sizeof(low_pc), 1, fp) == 1);
	assert(fwrite(&high_pc, sizeof(high_pc), 1, fp) == 1);
	assert(fwrite(&location, sizeof(location), 1, fp) == 1);
	length = type.length();
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(type.c_str(), length, 1, fp) == 1);
	assert(fwrite(&is_subprog,sizeof(is_subprog),1,fp) == 1);

	if (local_var_table != NULL) {
		local_var_table->write(fp);
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
 * @return true if the variable is at the prospective address, otherwise false
 */
bool var_record::is_at_address(const CONTEXT *ctxt,Generic mem_addr, type_record *trec) {
	DEBUGL(LOG("Enter is_at_address\n"));
	DEBUGL(LOG("trec = 0x" + hexstr(trec) + "\n"));
	Generic var_addr;

	if (is_local || is_param) {
		var_addr = PIN_GetContextReg( ctxt, REG_GBP) + location; // location is the offset from the frame pointer
	} else
	    var_addr = location; // location is the actual address

	DEBUGL(LOG("var_addr: 0x" +hexstr(var_addr) + " mem_addr: 0x" + hexstr(mem_addr) +  " size: " + decstr(trec->get_size())+ "\n"));
	if (trec->get_is_array()) { // is this an access to some element of an array
		return (mem_addr >= var_addr) && (mem_addr < var_addr + trec->get_size());
	} else
		return var_addr == mem_addr;
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
		Generic *ptr = (Generic*)addr;
		convert << *ptr;
		value = convert.str();
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
			LOG("Type not supported: " + type_name);
			PIN_ExitProcess(-1);
		}
	}

	return value;
}

var_table::~var_table() {}

/**
 * Write the var table to a file
 *
 * @param fp a file pointer
 */
void var_table::write(FILE *fp) {
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	size_t num = size();
	assert(fwrite(&num,sizeof(size_t), 1, fp) == 1);

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		var_record* vrec = (var_record*)it->second;
		vrec->write(it->first,fp);
	}
}

