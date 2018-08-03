/*
 * dwreader.cpp
 *
 *  Created on: Jul 2, 2018
 *      Author: carr
 */

#include "dw_reader.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <inttypes.h>
#include <string>
#include <cstdint>

#include <libelfin/elf/elf++.hh>
#include <libelfin/dwarf/dwarf++.hh>
#include <libelfin/dwarf/data.hh>

#include <tables/cu_table.h>
#include <read/base_expr_context.h>

#include <sys/types.h>
#include <unistd.h>

/**
 * Constructor
 */
dw_reader::dw_reader() {
	cutab = NULL;
	current_cu_rec = NULL;
	last_array_rec = NULL;
	var_table_stack.push((var_table*)table_factory.make_symbol_table(VAR_TABLE));
}

/**
 * Destructor
 */
dw_reader::~dw_reader() {
}

/**
 * Build a record in the type table for a base type
 *
 * @param node a DW_TAG_base_type DIE
 */
void dw_reader::process_base_type_die(const dwarf::die &node) {

	type_record *trec = (type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);
	std::string type = std::to_string(node.get_section_offset());
	std::string name = at_name(node);
	Generic size = (Generic)at_byte_size(node,&dwarf::no_expr_context);

	current_cu_rec->get_type_table()->put(type,trec->add_name(name)->add_size(size));
}

/**
 * Get the upper bound for an array type
 *
 * @param node an array_type die.
 * @return the upper_bound if the next die is a subrange, otherwise return -1 (unknown)
 */
void dw_reader::set_upper_bound(const dwarf::die& node) {

	Generic upper_bound = at_upper_bound(node,&dwarf::no_expr_context);
	last_array_rec->add_upper_bound(upper_bound,true);

}

/**
 * Build a record in the type table for an array type
 *
 * @param node a DW_TAG_array_type DIE
 */
void dw_reader::process_array_type_die(const dwarf::die &node) {

	type_record *trec = (type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);
	std::string type = std::to_string(node.get_section_offset());
	std::string base_type = std::to_string(at_type(node).get_section_offset());
	type_record *btrec = current_cu_rec->get_type_table()->get(base_type);
	std::string name = *btrec->get_name();
	Generic size = btrec->get_size();
	current_cu_rec->get_type_table()->put(type,trec->add_name(name)
			->add_is_array()
			->add_base_type(base_type)
			->add_size(size));
	last_array_rec = trec;
}

/**
 * Build a record in the variable table for a subprogram
 *
 * @param node a DW_TAG_subprogram DIE
 */
void dw_reader::process_subprogram_die(const dwarf::die &node) {

	var_record *vrec = (var_record*)record_factory.make_symbol_table_record(VAR_RECORD);

	std::string type = void_type;

	if (node.has(dwarf::DW_AT::type))
		type = std::to_string(at_type(node).get_section_offset());

	std::string name = at_name(node);
	uint64_t index = node[dwarf::DW_AT::decl_file].as_uconstant();
	std::string decl_file =current_cu.get_line_table().get_file(index)->path;
	Generic decl_line = node[dwarf::DW_AT::decl_line].as_uconstant();
	Generic low_pc = at_low_pc(node);
	Generic high_pc = at_high_pc(node);
	var_table *local_var_table = (var_table*)table_factory.make_symbol_table(VAR_TABLE);

	var_table_stack.top()->put(name,vrec->add_is_subprog()->add_type(type)->add_decl_line(decl_line)->
											  add_low_pc(low_pc)->add_high_pc(high_pc)->add_decl_file(decl_file)->
											  add_local_var_table(local_var_table));

	var_table_stack.push(local_var_table);
}

/**
 * Build a record in the variable table for a variable
 *
 * @param node a DW_TAG_variable DIE
 */
void dw_reader::process_variable_die(const dwarf::die &node, bool is_param) {

	var_record *vrec = (var_record*)record_factory.make_symbol_table_record(VAR_RECORD);

	std::string type = std::to_string(at_type(node).get_section_offset());
	std::string name = at_name(node);
	uint64_t index = node[dwarf::DW_AT::decl_file].as_uconstant();
	std::string decl_file = current_cu.get_line_table().get_file(index)->path;
	Generic decl_line = node[dwarf::DW_AT::decl_line].as_uconstant();
	bool is_local = (var_table_stack.size() > 1) && !is_param;
	dwarf::expr_result result = node[dwarf::DW_AT::location].as_exprloc().evaluate(new base_expr_context(getpid()));
	Generic location = result.value;

	if (is_param)
		var_table_stack.top()->put(name,vrec->add_type(type)->add_decl_file(decl_file)->add_decl_line(decl_line)->
				add_is_param()->add_location(location));
	else if (is_local)
		var_table_stack.top()->put(name,vrec->add_type(type)->add_decl_file(decl_file)->add_decl_line(decl_line)->
				add_is_local()->add_location(location));
	else
		var_table_stack.top()->put(name,vrec->add_type(type)->add_decl_file(decl_file)->add_decl_line(decl_line)->
				add_location(location));
}

/**
 * Walk the DIE tree to build the tables of information needed for program analysis
 *
 * @param node a DIE
 */
void dw_reader::process_die_tree(const dwarf::die &node) {
	switch (node.tag) {
	case dwarf::DW_TAG::base_type:
		dw_reader::process_base_type_die(node);
		break;
	case dwarf::DW_TAG::array_type:
		dw_reader::process_array_type_die(node);
		break;
	case dwarf::DW_TAG::subprogram:
		dw_reader::process_subprogram_die(node);
		break;
	case dwarf::DW_TAG::variable:
		dw_reader::process_variable_die(node,false);
		break;
	case dwarf::DW_TAG::formal_parameter:
		dw_reader::process_variable_die(node,true);
		break;
	case dwarf::DW_TAG::subrange_type:
		dw_reader::set_upper_bound(node);
		break;
	default:
		break;
	}

	for (auto &child : node)
		dw_reader::process_die_tree(child);

	if (node.tag == dwarf::DW_TAG::subprogram)
		var_table_stack.pop();
}

/**
 * Process a top level DIE for a compilation unit
 *
 * @param node a DW_TAG_compilation_unit DIE
 */
void dw_reader::process_cu_die(const dwarf::die &node) {

	std::string path = to_string(node.resolve(dwarf::DW_AT::comp_dir)) + "/"
			+ to_string(node.resolve(dwarf::DW_AT::name));

	current_cu_rec = (cu_record*)record_factory.make_symbol_table_record(CU_RECORD);

	Generic low_pc = (Generic) at_low_pc(node);
	Generic high_pc = (Generic) at_high_pc(node);
	var_table *global_var_table = var_table_stack.top();

	cutab->put(path,current_cu_rec->add_low_pc(low_pc)
			->add_high_pc(high_pc)
			->add_var_table(global_var_table)
			->add_type_table((type_table*)table_factory.make_symbol_table(TYPE_TABLE)));

	for (auto &child : node)
		dw_reader::process_die_tree(child);
}
/**
 * Read the DWARF information and store it in symbol tables
 */
void dw_reader::read_dw_info(void) {
	int fd;

	fd = open(file_name.c_str(), O_RDONLY);

	if (fd < 0) {
		fprintf(stderr, "%s: %s\n", file_name.c_str(), strerror(errno));
		exit(-1);
	}

	elf::elf ef(elf::create_mmap_loader(fd));
	dwarf::dwarf dw(dwarf::elf::create_loader(ef)); // @suppress("Invalid arguments")

	cutab =  (cu_table*)table_factory.make_symbol_table(CU_TABLE);

	for (auto cu : dw.compilation_units()) {
		current_cu = cu;
		try {
			dw_reader::process_cu_die(cu.root());
		} catch(std::exception& e) {
			fprintf(stderr,"Exception: %s\n", e.what());
		}
	}

}
