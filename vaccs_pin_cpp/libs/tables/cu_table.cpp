/******************************************************************************
 *
 * File: cu_table.cpp
 *
 * Summary: This file contains the code for building records of debug
 *          information for a compilation unit
 *
 *
 ******************************************************************************/

#include <tables/cu_table.h>
#include <tables/type_table.h>

#include <tables/symbol_table_factory.h>

#include <cstdio>
#include <cstdlib>
#include <cassert>



cu_record::cu_record() :
		symbol_table_record(CU_RECORD) {
	vtab = NULL;
	ttab = NULL;
	low_pc = -1;
	high_pc = -1;
}

cu_record::~cu_record() {}

/**
 * Check if a pc is in this compilation unit
 *
 * @param pc the pc to check
 * @return true if the pc is found in this compilation unit, otherwise false.
 */
bool cu_record::pc_in_range(Generic pc) {
	return (pc >= low_pc && pc <= high_pc);
}

/**
 * Translate a memory access by an instruction in this compilation unit to a variable name
 *
 * @param inst_addr the address of the instruction making the memory access
 * @param mem_addr the memory address being accessed
 * @return the variable record of the variable being accessed if possible to determine, otherwise
 * 			NULL
 */
var_record* cu_record::translate_address_to_variable(Generic inst_addr, Generic mem_addr) {
	var_record *vrec = NULL;

	for (std::map<std::string,symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {
			var_record* tvrec = (var_record*)it->second;
			if (tvrec->get_is_subprog() && tvrec->pc_in_range(inst_addr))
				vrec = tvrec->find_address(mem_addr);
	}

	return vrec;
}

/**
 * Write a compilation unit record to a file
 *
 * @param fp a file pointer
 */
void cu_record::write(std::string key,FILE *fp) {
	printf("Begin cu_record::write()\n");

	assert(fwrite(&id, sizeof(id), 1, fp) == 1);
	size_t length = key.length();
	assert(fwrite(&length, sizeof(length), 1, fp) == 1);
	assert(fwrite(key.c_str(), length, 1, fp) == 1);
	assert(fwrite(&low_pc, sizeof(low_pc), 1, fp) == 1);
	assert(fwrite(&high_pc, sizeof(high_pc), 1, fp) == 1);

	ttab->write(fp);
	vtab->write(fp);

	printf("End cu_record::write()\n");

}

cu_table::~cu_table() {}

/**
 * Translate a memory access to a variable name
 *
 * @param inst_addr the address of the instruction making the memory access
 * @param mem_addr the memory address being accessed
 * @return the variable record of the variable being accessed if possible to determine, otherwise
 * 			NULL
 */
var_record* cu_table::translate_address_to_variable(Generic inst_addr, Generic mem_addr) {
	var_record *vrec = NULL;

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		cu_record* curec = (cu_record*)it->second;
		if (curec->pc_in_range(inst_addr)) {
			vrec = curec->translate_address_to_variable(inst_addr,mem_addr);
			break;
		}
	}

	return vrec;
}

/**
 * Write the compilation unit table to a file
 *
 * @param fp a file pointer
 */
void cu_table::write(FILE *fp) {
	printf("Begin cu_table::write()\n");
	assert(fwrite(&id, sizeof(id), 1, fp) == 1);

	size_t num = size();
	assert(fwrite(&num,sizeof(size_t), 1, fp) == 1);

	for (std::map<std::string,symbol_table_record*>::iterator it = begin(); it != end(); it++) {
		cu_record* curec = (cu_record*)it->second;
		curec->write(it->first,fp);
	}
	printf("End cu_table::write()\n");
}


