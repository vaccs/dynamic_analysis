/*
 * dwreader.cpp
 *
 *  Created on: Jul 2, 2018
 *      Author: carr
 */

#include <vaccs_read/vaccs_dw_reader.h>

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <inttypes.h>
#include <string>
#include <cassert>

#include <tables/cu_table.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * Constructor
 */
vaccs_dw_reader::vaccs_dw_reader() {
	fp = NULL;
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
	assert(fread(&id, sizeof(id), 1, fp) == 1 && id == VAR_RECORD);

	size_t length;;
	assert(fread(&length, sizeof(length), 1, fp) == 1);

	char *key;
	assert((key = (char *)malloc(length+1)) != NULL);
	assert(fread(key, length, 1, fp) == 1);
	key[length] = '\0';

	bool is_local;
	assert(fread(&is_local, sizeof(is_local), 1, fp) == 1);

	bool is_param;
	assert(fread(&is_param, sizeof(is_param), 1, fp) == 1);

	assert(fread(&length, sizeof(length), 1, fp) == 1);

	char *fn;
	assert((fn = (char *)malloc(length+1)) != NULL);
	assert(fread(fn, length, 1, fp) == 1);
	fn[length] = '\0';

	Generic decl_line;
	assert(fread(&decl_line,sizeof(decl_line),1,fp) == 1);

	Generic low_pc;
	assert(fread(&low_pc, sizeof(low_pc), 1, fp) == 1);

	Generic high_pc;
	assert(fread(&high_pc, sizeof(high_pc), 1, fp) == 1);

	Generic location;
	assert(fread(&location, sizeof(location), 1, fp) == 1);

	assert(fread(&length, sizeof(length), 1, fp) == 1);

	char *type;
	assert((type = (char *)malloc(length+1)) != NULL);
	assert(fread(type, length, 1, fp) == 1);
	type[length] = '\0';

	bool is_subprog;
	assert(fread(&is_subprog,sizeof(is_subprog),1,fp) == 1);

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
	if (fread(&id, sizeof(int), 1, fp) == 1 &&
			id == VAR_TABLE) {
		size_t size;
		assert(fread(&size,sizeof(size),1,fp) == 1 && size >= 0);
		for (unsigned long i = 0; i < size; i++)
			read_var_record();
	} else {
		fprintf(stderr,"Malformed vaccs dw file (must be a Var Table): %s, %d\n",file_name.c_str(),id);
	}


}

/**
 * Read a record in the type table
 */
void vaccs_dw_reader::read_type_record() {
	int id;
	assert(fread(&id, sizeof(id), 1, fp) == 1 && id == TYPE_RECORD);

	size_t length;;
	assert(fread(&length, sizeof(length), 1, fp) == 1);

	char *key;
	assert((key = (char *)malloc(length+1)) != NULL);
	assert(fread(key, length, 1, fp) == 1);
	key[length] = '\0';

	Generic size;
	assert(fread(&size,sizeof(size),1,fp) == 1);

	assert(fread(&length, sizeof(length), 1, fp) == 1);

	char *name;
	assert((name = (char *)malloc(length+1)) != NULL);
	assert(fread(name, length, 1, fp) == 1);
	name[length] = '\0';

	bool is_array;
	assert(fread(&is_array, sizeof(is_array), 1, fp) == 1);

	Generic upper_bound;
	assert(fread(&upper_bound, sizeof(upper_bound), 1, fp) == 1);

	assert(fread(&length, sizeof(length), 1, fp) == 1);

	char *base_type;
	if (length > 0) {
		assert((base_type = (char *)malloc(length+1)) != NULL);
		assert(fread(base_type, length, 1, fp) == 1);
		base_type[length] = '\0';
	} else {
		base_type = NULL;
	}

	std::string type(key);
	type_record *trec = (type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);

	if (is_array)
		current_cu_rec->get_type_table()->put(type,
				trec->add_size(size)
					->add_name(name)
					->add_is_array()
					->add_upper_bound(upper_bound,false)
					->add_base_type(base_type));
	else
		current_cu_rec->get_type_table()->put(type,
						trec->add_size(size)
							->add_name(name));
}

/**
 * Read a type table
 */
void vaccs_dw_reader::read_type_table() {
	int id;
	if (fread(&id, sizeof(int), 1, fp) == 1 &&
			id == TYPE_TABLE) {
		size_t size;
		assert(fread(&size,sizeof(size),1,fp) == 1 && size >= 0);
		for (unsigned long i = 0; i < size; i++)
			read_type_record();
	} else {
		fprintf(stderr,"Malformed vaccs dw file (must be a Type Table): %s, %d\n",file_name.c_str(),id);
	}

}


/**
 * Read a top level DIE for a compilation unit
 *
 * @param node a DW_TAG_compilation_unit DIE
 */
void vaccs_dw_reader::read_cu_record() {

	int id;
	assert(fread(&id, sizeof(id), 1, fp) == 1 && id == CU_RECORD);
	size_t length;
	assert(fread(&length, sizeof(length), 1, fp) == 1 && length >= 0);

	char *key;
	assert((key = (char *)malloc(length+1)) != NULL);
	assert(fread(key, length, 1, fp) == 1);
	key[length] = '\0';

	Generic low_pc;
	assert(fread(&low_pc, sizeof(low_pc), 1, fp) == 1);

	Generic high_pc;
	assert(fread(&high_pc, sizeof(high_pc), 1, fp) == 1);


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

	assert((fp= fopen(file_name.c_str(), "r")) != NULL);

	int id;
	if (fread(&id, sizeof(int), 1, fp) == 1 &&
			id == CU_TABLE) {
		cutab =  (cu_table*)table_factory.make_symbol_table(CU_TABLE);
		size_t size;
		assert(fread(&size,sizeof(size),1,fp) == 1 && size >= 0);
		for (unsigned long i = 0; i < size; i++)
			read_cu_record();
	} else {
		fprintf(stderr,"Malformed vaccs dw file (must start with a CU Table): %s, %d\n",file_name.c_str(),id);
	}


}
