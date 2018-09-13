/*
 * dwreader.cpp
 *
 *  Created on: Jul 2, 2018
 *      Author: carr
 */

#include <vaccs_read/vaccs_dw_reader.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cerrno>
#include <fcntl.h>
#include <inttypes.h>
#include <string>
#include <cassert>

#include <tables/cu_table.h>
#include <sys/types.h>
#include <unistd.h>

#include <pin.H>

/**
 * Constructor
 */
vaccs_dw_reader::vaccs_dw_reader() {
	fd = -1;
	cutab = NULL;
	current_cu_rec = NULL;
	var_table_stack.push((var_table*)table_factory.make_symbol_table(VAR_TABLE));
}

/**
 * Destructor
 */
vaccs_dw_reader::~vaccs_dw_reader() {
}


/**
 * Read a record in the variable table for a variable
 */
void vaccs_dw_reader::read_var_record() {
	int id;
	USIZE size = sizeof(id);
	assert(OS_ReadFD(fd, &size, &id).generic_err == OS_RETURN_CODE_NO_ERROR && id == VAR_RECORD);

	size_t length;;
	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);

	char *key;
	assert((key = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,key).generic_err == OS_RETURN_CODE_NO_ERROR);
	key[length] = '\0';

	bool is_local;
	size =  sizeof(is_local); assert(OS_ReadFD(fd,&size,&is_local).generic_err == OS_RETURN_CODE_NO_ERROR);

	bool is_param;
	size =  sizeof(is_param); assert(OS_ReadFD(fd,&size,&is_param).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);

	char *fn;
	assert((fn = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,fn).generic_err == OS_RETURN_CODE_NO_ERROR);
	fn[length] = '\0';

	Generic decl_line;
	size = sizeof(decl_line); assert(OS_ReadFD(fd, &size, &decl_line).generic_err == OS_RETURN_CODE_NO_ERROR);

	Generic low_pc;
	size =  sizeof(low_pc); assert(OS_ReadFD(fd,&size,&low_pc).generic_err == OS_RETURN_CODE_NO_ERROR);

	Generic high_pc;
	size =  sizeof(high_pc); assert(OS_ReadFD(fd,&size,&high_pc).generic_err == OS_RETURN_CODE_NO_ERROR);

	Generic location;
	size =  sizeof(location); assert(OS_ReadFD(fd,&size,&location).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);

	char *type;
	assert((type = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,type).generic_err == OS_RETURN_CODE_NO_ERROR);
	type[length] = '\0';

	bool is_subprog;
	size = sizeof(is_subprog); assert(OS_ReadFD(fd, &size, &is_subprog).generic_err == OS_RETURN_CODE_NO_ERROR);

	std::string name(key);
	std::string stype(type);

	var_record *vrec = (var_record*)record_factory.make_symbol_table_record(VAR_RECORD);

	std::string decl_file(fn);
	vrec = vrec->add_decl_file(decl_file)
			->add_decl_line(decl_line)
			->add_low_pc(low_pc)
			->add_high_pc(high_pc)
			->add_type(stype)
			->add_location(location);

	if (is_local)
		vrec = vrec->add_is_local();

	if (is_param)
		vrec = vrec->add_is_param();

	if (is_subprog)
		vrec = vrec->add_is_subprog();

	var_table_stack.top()->put(name,vrec);

	if (is_subprog) {
		var_table *local_var_table = (var_table*)table_factory.make_symbol_table(VAR_TABLE);
		vrec = vrec->add_local_var_table(local_var_table);
		var_table_stack.push(local_var_table);
		read_var_table();
		var_table_stack.pop();
	}
}

/**
 * Read a var table
 */
void vaccs_dw_reader::read_var_table() {
	int id;
	USIZE size = sizeof(int);
	if (OS_ReadFD(fd, &size, &id).generic_err == OS_RETURN_CODE_NO_ERROR &&
			id == VAR_TABLE) {
		size_t num_vrecs;
		size = sizeof(num_vrecs);
		assert(OS_ReadFD(fd,&size,&num_vrecs).generic_err == OS_RETURN_CODE_NO_ERROR && num_vrecs >= 0);
		for (unsigned long i = 0; i < num_vrecs; i++)
			read_var_record();
	} else {
		LOG("Malformed vaccs dw file (must be a Var Table): " + file_name + ", Id = " + decstr(id) + "\n");
	}


}

/**
 * Read a record in the type table
 */
void vaccs_dw_reader::read_type_record() {
	int id;
	USIZE size = sizeof(id);
	assert(OS_ReadFD(fd,&size,&id).generic_err == OS_RETURN_CODE_NO_ERROR && id == TYPE_RECORD);

	size_t length;;
	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);

	char *key;
	assert((key = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,key).generic_err == OS_RETURN_CODE_NO_ERROR);
	key[length] = '\0';

	Generic type_size;
	size = sizeof(type_size); assert(OS_ReadFD(fd,&size,&type_size).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);

	char *name;
	assert((name = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,name).generic_err == OS_RETURN_CODE_NO_ERROR);
	name[length] = '\0';

	bool is_array;
	size =  sizeof(is_array); assert(OS_ReadFD(fd,&size,&is_array).generic_err == OS_RETURN_CODE_NO_ERROR);

	Generic upper_bound;
	size =  sizeof(upper_bound); assert(OS_ReadFD(fd,&size,&upper_bound).generic_err == OS_RETURN_CODE_NO_ERROR);

	bool is_pointer;
	size =  sizeof(is_pointer); assert(OS_ReadFD(fd,&size,&is_pointer).generic_err == OS_RETURN_CODE_NO_ERROR);

	bool is_struct;
	size =  sizeof(is_struct); assert(OS_ReadFD(fd,&size,&is_struct).generic_err == OS_RETURN_CODE_NO_ERROR);

	bool is_typedef;
	size =  sizeof(is_struct); assert(OS_ReadFD(fd,&size,&is_typedef).generic_err == OS_RETURN_CODE_NO_ERROR);

	bool is_const;
	size =  sizeof(is_struct); assert(OS_ReadFD(fd,&size,&is_const).generic_err == OS_RETURN_CODE_NO_ERROR);

	size =  sizeof(length); assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR);

	char *base_type;
	if (length > 0) {
		assert((base_type = (char *)malloc(length+1)) != NULL);
		size =  length; assert(OS_ReadFD(fd,&size,base_type).generic_err == OS_RETURN_CODE_NO_ERROR);
		base_type[length] = '\0';
	} else {
		base_type = NULL;
	}

	std::string type(key);
	type_record *trec = (type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);

	trec = trec->add_size(type_size)->add_name(name);

	if (is_array)
		trec = trec->add_is_array()
			->add_upper_bound(upper_bound,false)
			->add_base_type(base_type);

	if (is_pointer)
		trec = trec->add_is_pointer()
			->add_base_type(base_type);

	if (is_typedef)
		trec = trec->add_is_typedef();

	if (is_const)
		trec = trec->add_is_const();

	current_cu_rec->get_type_table()->put(type,trec);
	if (is_struct) {

		DEBUGL(cerr << "Reading member table for structure for type: " + type + "\n");
		trec = trec->add_is_struct();
		var_table *memtab = (var_table*)table_factory.make_symbol_table(VAR_TABLE);
		trec = trec->add_member_table(memtab);
		var_table_stack.push(memtab);
		read_var_table();
		var_table_stack.pop();

	}


}

/**
 * Read a type table
 */
void vaccs_dw_reader::read_type_table() {
	int id;
	USIZE size = sizeof(int);
	if (OS_ReadFD(fd,&size,&id).generic_err == OS_RETURN_CODE_NO_ERROR &&
			id == TYPE_TABLE) {
		size_t num_trecs;
		size = sizeof(num_trecs);
		assert(OS_ReadFD(fd,&size,&num_trecs).generic_err == OS_RETURN_CODE_NO_ERROR && num_trecs >= 0);
		for (unsigned long i = 0; i < num_trecs; i++)
			read_type_record();
	} else {
		LOG("Malformed vaccs dw file (must be a Type Table): " + file_name + ", Id = " + decstr(id) + "\n");
	}

}


/**
 * Read a top level DIE for a compilation unit
 *
 * @param node a DW_TAG_compilation_unit DIE
 */
void vaccs_dw_reader::read_cu_record() {

	int id;
	USIZE size = sizeof(id);
	assert(OS_ReadFD(fd,&size,&id).generic_err == OS_RETURN_CODE_NO_ERROR && id == CU_RECORD);
	size_t length;
	size = sizeof(length);
	assert(OS_ReadFD(fd,&size,&length).generic_err == OS_RETURN_CODE_NO_ERROR && length >= 0);

	char *key;
	assert((key = (char *)malloc(length+1)) != NULL);
	size =  length; assert(OS_ReadFD(fd,&size,key).generic_err == OS_RETURN_CODE_NO_ERROR);
	key[length] = '\0';

	Generic low_pc;
	size =  sizeof(low_pc); assert(OS_ReadFD(fd,&size,&low_pc).generic_err == OS_RETURN_CODE_NO_ERROR);

	Generic high_pc;
	size =  sizeof(high_pc); assert(OS_ReadFD(fd,&size,&high_pc).generic_err == OS_RETURN_CODE_NO_ERROR);


	std::string path(key);

	current_cu_rec = (cu_record*)record_factory.make_symbol_table_record(id);

	cutab->put(path,current_cu_rec->add_low_pc(low_pc)
			->add_high_pc(high_pc)
			->add_var_table(var_table_stack.top())
			->add_type_table((type_table*)table_factory.make_symbol_table(TYPE_TABLE)));

	read_type_table();
	read_var_table();
}
/**
 * Read the DWARF information and store it in symbol tables
 */
void vaccs_dw_reader::read_vaccs_dw_info(void) {

	DEBUGL(cerr << "Reading vaccs dwarf info\n");
	assert((OS_OpenFD(file_name.c_str(), OS_FILE_OPEN_TYPE_READ, OS_FILE_PERMISSION_TYPE_READ, &fd)).generic_err
			== OS_RETURN_CODE_NO_ERROR);

	int id;
	USIZE size = sizeof(int);
	if (OS_ReadFD(fd, &size, &id).generic_err == OS_RETURN_CODE_NO_ERROR &&
			id == CU_TABLE) {
		cutab =  (cu_table*)table_factory.make_symbol_table(CU_TABLE);
		size_t num_curecs;
		size = sizeof(num_curecs);
		assert(OS_ReadFD(fd,&size,&num_curecs).generic_err == OS_RETURN_CODE_NO_ERROR && num_curecs >= 0);
		for (unsigned long i = 0; i < num_curecs; i++)
			read_cu_record();

		DEBUGL(cerr << "Creating Member Tables\n");
		cutab->create_member_tables();
		DEBUGL(cerr << "Done Creating Member Tables\n");

	} else {
		LOG("Malformed vaccs dw file (must start with a CU Table): " + file_name + ", Id = " + decstr(id) + "\n");
	}


}
