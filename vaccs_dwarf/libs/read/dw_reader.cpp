/*
 * dwreader.cpp
 *
 *  Created on: Jul 2, 2018
 *      Author: carr
 */

#include "dw_reader.h"
#include <util/general.h>

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

#include <read/base_expr_context.h>

#include <sys/types.h>
#include <unistd.h>
#include "../tables/vdw_cu_table.h"

/**
 * Constructor
 */
dw_reader::dw_reader() {
	cutab = NULL;
	current_cu_rec = NULL;
	last_array_rec = NULL;
	last_struct_rec = NULL;
	var_table_stack.push((vdw_var_table*)table_factory.make_symbol_table(VAR_TABLE));
}

/**
 * Destructor
 */
dw_reader::~dw_reader() {
}

std::string get_at_name(const dwarf::die& node) {
	std::string name = "<none>";
	if (node.has(dwarf::DW_AT::name))
		name = at_name(node);

	return name;
}

std::string get_at_type(const dwarf::die& node) {
	std::string type = void_type;
	if (node.has(dwarf::DW_AT::type))
		type = std::to_string(at_type(node).get_section_offset());

	return type;
}

Generic get_at_byte_size(const dwarf::die& node) {
	Generic val = sizeof(Generic);
	if (node.has(dwarf::DW_AT::byte_size))
		val = (Generic)at_byte_size(node,&dwarf::no_expr_context);

	return val;
}

Generic get_at_upper_bound(const dwarf::die& node) {
	Generic upb = 1;
	if (node.has(dwarf::DW_AT::upper_bound))
		upb = at_upper_bound(node,&dwarf::no_expr_context);

	return upb;
}

Generic get_at_low_pc(const dwarf::die& node) {
	Generic low_pc = 0;
	if (node.has(dwarf::DW_AT::low_pc))
		low_pc = at_low_pc(node);

	return low_pc;
}

Generic get_at_high_pc(const dwarf::die& node) {
	Generic high_pc = 0;
	if (node.has(dwarf::DW_AT::high_pc))
		high_pc = at_high_pc(node);

	return high_pc;
}

uint64_t get_decl_file_index(const dwarf::die& node) {
	uint64_t index = 0;
	if (node.has(dwarf::DW_AT::decl_file))
		index = node[dwarf::DW_AT::decl_file].as_uconstant();

	return index;
}

Generic get_decl_line(const dwarf::die& node) {
	Generic line = 0;
	if (node.has(dwarf::DW_AT::decl_line))
		line = node[dwarf::DW_AT::decl_line].as_uconstant();

	return line;
}


/**
 * Build a record in the type table for a base type
 *
 * @param node a DW_TAG_base_type DIE
 */
void dw_reader::process_base_type_die(const dwarf::die &node) {

	vdw_type_record *trec = (vdw_type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);
	std::string type = std::to_string(node.get_section_offset());
	std::string name = get_at_name(node);
	Generic size = get_at_byte_size(node);

	current_cu_rec->get_type_table()->put(type,trec->add_name(name)->add_size(size));
}

/**
 * Build a record in the type table for a typedef
 *
 * @param node a DW_TAG_typedef_ DIE
 */
void dw_reader::process_typedef_die(const dwarf::die &node) {

	vdw_type_record *trec = (vdw_type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);
	std::string type = std::to_string(node.get_section_offset());
	std::string name = get_at_name(node);
	Generic size = get_at_byte_size(node);

	current_cu_rec->get_type_table()->put(type,trec->add_name(name)->add_size(size)
			->add_is_typedef());
}

/**
 * Build a record in the type table for a const
 *
 * @param node a DW_TAG_const_type_ DIE
 */
void dw_reader::process_const_type_die(const dwarf::die &node) {

	vdw_type_record *trec = (vdw_type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);
	std::string type = std::to_string(node.get_section_offset());
	std::string base_type = get_at_type(node);

	current_cu_rec->get_type_table()->put(type,trec->add_base_type(base_type)->add_is_const());
}

/**
 * Set the upper bound for an array type
 *
 * @param node a subrange die.
 */
void dw_reader::set_upper_bound(const dwarf::die& node) {

	Generic upper_bound = get_at_upper_bound(node);
	last_array_rec->add_upper_bound(upper_bound,true);
        DEBUGL(fprintf(stderr,"Array size = %ld, upper bound = %ld\n",
                last_array_rec->get_size(),last_array_rec->get_upper_bound()));

}

/**
 * Build a record in the type table for an array type
 *
 * @param node a DW_TAG_array_type DIE
 */
void dw_reader::process_array_type_die(const dwarf::die &node) {

	vdw_type_record *trec = (vdw_type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);
	std::string type = std::to_string(node.get_section_offset());

	std::string base_type = get_at_type(node);

	// resolved later
	std::string name = "<array>";
	Generic size = sizeof(Generic);
	trec = trec->add_name(name)
			   ->add_is_array()
			   ->add_base_type(base_type)
			   ->add_size(size);
	current_cu_rec->get_type_table()->put(type,trec);
	last_array_rec = trec;
}

/**
 * Build a record in the type table for a pointer type
 *
 * @param node a DW_TAG_pointer_type DIE
 */
void dw_reader::process_pointer_type_die(const dwarf::die &node) {

	vdw_type_record *trec = (vdw_type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);

	std::string type = std::to_string(node.get_section_offset());

	std::string base_type = get_at_type(node);

	// resolved later
	std::string name = "<pointer>";
	Generic size = get_at_byte_size(node);
	current_cu_rec->get_type_table()->put(type,trec->add_name(name)
			->add_is_pointer()
			->add_base_type(base_type)
			->add_size(size));
}

/**
 * Add a member for a struct type
 *
 * @param node a member die.
 */
void dw_reader::add_struct_member(const dwarf::die& node) {

	vdw_var_record *vrec = (vdw_var_record*)record_factory.make_symbol_table_record(VAR_RECORD);

	std::string type = get_at_type(node);
	std::string name = get_at_name(node);
	uint64_t index = get_decl_file_index(node);
	std::string decl_file = current_cu.get_line_table().get_file(index)->path;
	Generic decl_line = get_decl_line(node);
	dwarf::expr_result result = at_data_member_location(node, new base_expr_context(getpid()), 0,0);
	Generic location = result.value;

	last_struct_rec->get_member_table()->put(name,vrec->add_type(type)->add_decl_file(decl_file)->add_decl_line(decl_line)->
			add_location(location));

}

/**
 * Build a record in the type table for a struct type
 *
 * @param node a DW_TAG_struct_type DIE
 */
void dw_reader::process_struct_type_die(const dwarf::die &node) {

	vdw_type_record *trec = (vdw_type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);
	std::string type = std::to_string(node.get_section_offset());
	std::string name = "struct " + get_at_name(node);
	Generic size = get_at_byte_size(node);
	vdw_var_table *memtab = (vdw_var_table*)table_factory.make_symbol_table(VAR_TABLE);

	current_cu_rec->get_type_table()->put(type,trec->add_name(name)
			->add_is_struct()
			->add_size(size)
			->add_member_table(memtab));
	last_struct_rec = trec;
}

bool address_in_subprogram(Generic address,
	Generic low_pc,
	Generic high_pc) {
		return address >= low_pc && address < (low_pc + high_pc);
}

Generic dw_reader::get_start_pc_from_line_table(std::string name,
	Generic low_pc,
	Generic high_pc) {

	dwarf::line_table lt = current_cu.get_line_table();
	Generic addr = 0;
	bool second_line = false; // The first executable line is the function decl
														// The second one will be the first line of code in the function

	for (const auto& entry : lt)
		if(address_in_subprogram(entry.address,low_pc,high_pc) && entry.is_stmt) {
			if (second_line) {
				addr = entry.address;
				DEBUGL(fprintf(stderr,"Line %d starts procedure %s\n",entry.line,name.c_str()));
				break;
			} else
				second_line = true;
		}

	return addr;
}
/**
 * Build a record in the variable table for a subprogram
 *
 * @param node a DW_TAG_subprogram DIE
 */
void dw_reader::process_subprogram_die(const dwarf::die &node) {

	vdw_var_record *vrec = (vdw_var_record*)record_factory.make_symbol_table_record(VAR_RECORD);

	std::string type = get_at_type(node);

	std::string name = get_at_name(node);
	uint64_t index = get_decl_file_index(node);
	std::string decl_file =current_cu.get_line_table().get_file(index)->path;
	Generic decl_line = get_decl_line(node);
	Generic low_pc = get_at_low_pc(node);
	Generic high_pc = get_at_high_pc(node);
	vdw_var_table *local_var_table = (vdw_var_table*)table_factory.make_symbol_table(VAR_TABLE);
	Generic start_pc = get_start_pc_from_line_table(name,low_pc,high_pc);

	var_table_stack.top()->put(name,vrec->add_is_subprog()->add_type(type)->add_decl_line(decl_line)->
											  add_low_pc(low_pc)->add_high_pc(high_pc)->add_decl_file(decl_file)->
											  add_local_var_table(local_var_table)->add_start_pc(start_pc));

	var_table_stack.push(local_var_table);
}

/**
 * Build a record in the variable table for a variable
 *
 * @param node a DW_TAG_variable DIE
 */
void dw_reader::process_variable_die(const dwarf::die &node, bool is_param) {

	if (node.has(dwarf::DW_AT::location)) {
		vdw_var_record *vrec = (vdw_var_record*)record_factory.make_symbol_table_record(VAR_RECORD);

		std::string type = get_at_type(node);
		std::string name = get_at_name(node);
		uint64_t index = get_decl_file_index(node);
		std::string decl_file = current_cu.get_line_table().get_file(index)->path;
		Generic decl_line = get_decl_line(node);
		bool is_local = (var_table_stack.size() > 1) && !is_param;
		dwarf::expr_result result = node[dwarf::DW_AT::location].as_exprloc().evaluate(new base_expr_context(getpid()));
		Generic location = result.value;

		vrec = vrec->add_type(type)
				->add_decl_file(decl_file)
				->add_decl_line(decl_line)
				->add_location(location);

		if (is_param)
			var_table_stack.top()->put(name,vrec->add_is_param());
		else if (is_local)
			var_table_stack.top()->put(name,vrec->add_is_local());
		else
			var_table_stack.top()->put(name,vrec);
	}
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
	case dwarf::DW_TAG::pointer_type:
		dw_reader::process_pointer_type_die(node);
		break;
	case dwarf::DW_TAG::structure_type:
		dw_reader::process_struct_type_die(node);
		break;
	case dwarf::DW_TAG::member:
		dw_reader::add_struct_member(node);
		break;
	case dwarf::DW_TAG::typedef_:
		dw_reader::process_typedef_die(node);
		break;
	case dwarf::DW_TAG::const_type:
		dw_reader::process_const_type_die(node);
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

	current_cu_rec = (vdw_cu_record*)record_factory.make_symbol_table_record(CU_RECORD);

	Generic low_pc = get_at_low_pc(node);
	Generic high_pc = get_at_high_pc(node);
	vdw_var_table *global_var_table = var_table_stack.top();

	cutab->put(path,current_cu_rec->add_low_pc(low_pc)
			->add_high_pc(high_pc)
			->add_var_table(global_var_table)
			->add_type_table((vdw_type_table*)table_factory.make_symbol_table(TYPE_TABLE)));

	// add a void type
	vdw_type_record *trec = (vdw_type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);
	current_cu_rec->get_type_table()->put(void_type,
			trec->add_name(void_type)->add_size(sizeof(Generic)));

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

	cutab =  (vdw_cu_table*)table_factory.make_symbol_table(CU_TABLE);

	for (auto cu : dw.compilation_units()) {
		current_cu = cu;
		try {
			dw_reader::process_cu_die(cu.root());
		} catch(std::exception& e) {
			fprintf(stderr,"Exception: %s\n", e.what());
		}
	}


	cutab->resolve_base_types();

	// add some of the global symbols to the first compilation unit
	vdw_cu_record *curec = (vdw_cu_record *)cutab->begin()->second;

	//  add a type record for a FILE* so that the variable as a type
	vdw_type_record *trec = (vdw_type_record*)record_factory.make_symbol_table_record(TYPE_RECORD);
	std::string stdout_type = "FILE*";
	std::string stdout_name = "FILE*";
	Generic stdout_size = sizeof(Generic);

	curec->get_type_table()->put(stdout_type,trec->add_name(stdout_name)->add_size(stdout_size));
    for (auto &sec : ef.sections()) {
             if (sec.get_hdr().type != elf::sht::symtab && sec.get_hdr().type != elf::sht::dynsym)
                     continue;
             for (auto sym : sec.as_symtab()) {
                     auto &d = sym.get_data();

                     if (sym.get_name() == "stdout") {
                    	 std::string file = "__NOCSOURCE__";
                    	 vdw_var_record *vrec = (vdw_var_record*)record_factory.make_symbol_table_record(VAR_RECORD);
                    	 vrec = vrec->add_type(stdout_type)
                    			 ->add_decl_file(file)
								 ->add_decl_line(0)
								 ->add_location((Generic)d.value);
                    	 curec->get_var_table()->put("stdout",vrec);
                     }
              }
     }


}
