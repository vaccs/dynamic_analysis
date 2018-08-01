/*
 * test_dwread.cpp
 *
 *  Created on: Jul 9, 2018
 *      Author: carr
 */

#define CATCH_CONFIG_MAIN

#include "../../libs/test/catch.hpp"

#include <tables/cu_table.h>
#include <tables/symbol_table_factory.h>

#include <read/dw_reader.h>
#include <vaccs_read/vaccs_dw_reader.h>

#include <cstdio>
#include <cstdlib>

std::string hex_to_decimal_string(std::string hex_str) {
	unsigned long value;
	std::istringstream iss(hex_str);
	iss >> std::hex >> value;

	return std::to_string(value);
}

TEST_CASE("test_dwarf_cu_info") {

	std::string fn = "input/PAS_Example";
	dw_reader *dwr;

	REQUIRE((dwr = new dw_reader()) != nullptr);
	REQUIRE((dwr->add_file_name(fn)) != nullptr);

	printf("Before read\n");
	dwr->read_dw_info();
	printf("Afterpf  read\n");


	cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	CHECK(cutab->size() == 1);

	cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);
	CHECK(curec->get_low_pc() == 0x00400546L);
	CHECK(curec->get_high_pc() == curec->get_low_pc() + 139);
	CHECK(curec->get_var_table() != nullptr);
	CHECK(curec->get_type_table() != nullptr);

	FILE *fp = fopen("input/PAS_Example.vdw","w");
	cutab->write(fp);
	fclose(fp);

	vaccs_dw_reader *vdr;
	fn = "input/PAS_Example.vdw";
	REQUIRE((vdr = new vaccs_dw_reader()) != nullptr);
	REQUIRE((vdr->add_file_name(fn)) != nullptr);

	vdr->read_vaccs_dw_info();

	REQUIRE((cutab = vdr->get_cutab()) != nullptr);

	CHECK(cutab->size() == 1);

	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);
	CHECK(curec->get_low_pc() == 0x00400546L);
	CHECK(curec->get_high_pc() == curec->get_low_pc() + 139);
	CHECK(curec->get_var_table() != nullptr);
	CHECK(curec->get_type_table() != nullptr);

}

TEST_CASE("test_dwarf_type_info") {
	std::string fn = "input/PAS_Example";
	dw_reader *dwr;

	REQUIRE((dwr = new dw_reader()) != nullptr);
	REQUIRE((dwr->add_file_name(fn)) != nullptr);

	dwr->read_dw_info();

	cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	type_table *ttab;
	REQUIRE((ttab = curec->get_type_table()) != nullptr);

	CHECK(ttab->size() == 13);

	type_record *trec;

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x2d"))) != nullptr);
	CHECK((*trec->get_name()) == "long unsigned int");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x34"))) != nullptr);
	CHECK((*trec->get_name()) == "unsigned char");
	CHECK(trec->get_size() == 0x1);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x3b"))) != nullptr);
	CHECK((*trec->get_name()) == "short unsigned int");
	CHECK(trec->get_size() == 0x2);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x42"))) != nullptr);
	CHECK((*trec->get_name()) == "unsigned int");
	CHECK(trec->get_size() == 0x4);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x49"))) != nullptr);
	CHECK((*trec->get_name()) == "signed char");
	CHECK(trec->get_size() == 0x1);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x50"))) != nullptr);
	CHECK((*trec->get_name()) == "short int");
	CHECK(trec->get_size() == 0x2);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x57"))) != nullptr);
	CHECK((*trec->get_name()) == "int");
	CHECK(trec->get_size() == 0x4);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x5e"))) != nullptr);
	CHECK((*trec->get_name()) == "long int");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x65"))) != nullptr);
	CHECK((*trec->get_name()) == "sizetype");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x6c"))) != nullptr);
	CHECK((*trec->get_name()) == "char");
	CHECK(trec->get_size() == 0x1);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x73"))) != nullptr);
	CHECK((*trec->get_name()) == "long long unsigned int");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x7a"))) != nullptr);
	CHECK((*trec->get_name()) == "long long int");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0xb8"))) != nullptr);
	CHECK((*trec->get_base_type()) == hex_to_decimal_string("0x57"));
	CHECK(trec->get_is_array());

	FILE *fp = fopen("input/PAS_Example.vdw","w");
	cutab->write(fp);
	fclose(fp);

	vaccs_dw_reader *vdr;
	fn = "input/PAS_Example.vdw";
	REQUIRE((vdr = new vaccs_dw_reader()) != nullptr);
	REQUIRE((vdr->add_file_name(fn)) != nullptr);

	vdr->read_vaccs_dw_info();

	REQUIRE((cutab = vdr->get_cutab()) != nullptr);
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	REQUIRE((ttab = curec->get_type_table()) != nullptr);

	CHECK(ttab->size() == 13);

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x2d"))) != nullptr);
	CHECK((*trec->get_name()) == "long unsigned int");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x34"))) != nullptr);
	CHECK((*trec->get_name()) == "unsigned char");
	CHECK(trec->get_size() == 0x1);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x3b"))) != nullptr);
	CHECK((*trec->get_name()) == "short unsigned int");
	CHECK(trec->get_size() == 0x2);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x42"))) != nullptr);
	CHECK((*trec->get_name()) == "unsigned int");
	CHECK(trec->get_size() == 0x4);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x49"))) != nullptr);
	CHECK((*trec->get_name()) == "signed char");
	CHECK(trec->get_size() == 0x1);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x50"))) != nullptr);
	CHECK((*trec->get_name()) == "short int");
	CHECK(trec->get_size() == 0x2);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x57"))) != nullptr);
	CHECK((*trec->get_name()) == "int");
	CHECK(trec->get_size() == 0x4);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x5e"))) != nullptr);
	CHECK((*trec->get_name()) == "long int");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x65"))) != nullptr);
	CHECK((*trec->get_name()) == "sizetype");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x6c"))) != nullptr);
	CHECK((*trec->get_name()) == "char");
	CHECK(trec->get_size() == 0x1);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x73"))) != nullptr);
	CHECK((*trec->get_name()) == "long long unsigned int");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0x7a"))) != nullptr);
	CHECK((*trec->get_name()) == "long long int");
	CHECK(trec->get_size() == 0x8);
	CHECK(!trec->get_is_array());

	REQUIRE((trec = ttab->get(hex_to_decimal_string("0xb8"))) != nullptr);
	CHECK((*trec->get_base_type()) == hex_to_decimal_string("0x57"));
	CHECK(trec->get_is_array());

}

TEST_CASE("test_dwarf_global_var_info") {
	std::string fn = "input/PAS_Example";
	dw_reader *dwr;

	REQUIRE((dwr = new dw_reader()) != nullptr);
	REQUIRE((dwr->add_file_name(fn)) != nullptr);

	dwr->read_dw_info();

	cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	var_table *vtab;
	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	CHECK(vtab->size() == 4);

	var_record *vrec;
	REQUIRE((vrec = vtab->get("bar")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x6);
	CHECK(vrec->get_is_subprog());
	CHECK(vrec->get_low_pc() == 0x00400546);
	CHECK(vrec->get_high_pc() == 0x00400546+101);
	CHECK(vrec->get_local_var_table() != nullptr);
	CHECK(vrec->get_type() == void_type);

	REQUIRE((vrec = vtab->get("foo")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0xe);
	CHECK(vrec->get_is_subprog());
	CHECK(vrec->get_low_pc() == 0x004005ab);
	CHECK(vrec->get_high_pc() == 0x004005ab+7);
	CHECK(vrec->get_local_var_table() != nullptr);
	CHECK(vrec->get_type() == void_type);

	REQUIRE((vrec = vtab->get("main")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x12);
	CHECK(vrec->get_is_subprog());
	CHECK(vrec->get_low_pc() == 0x004005b2);
	CHECK(vrec->get_high_pc() == 0x004005b2+31);
	CHECK(vrec->get_local_var_table() != nullptr);
	CHECK(vrec->get_type() == hex_to_decimal_string("0x57"));

	REQUIRE((vrec = vtab->get("a")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x5);
	CHECK(!vrec->get_is_subprog());
	CHECK(!vrec->get_is_local());
	CHECK(vrec->get_type() == hex_to_decimal_string("0xb8"));

	FILE *fp = fopen("input/PAS_Example.vdw","w");
	cutab->write(fp);
	fclose(fp);

	vaccs_dw_reader *vdr;
	fn = "input/PAS_Example.vdw";
	REQUIRE((vdr = new vaccs_dw_reader()) != nullptr);
	REQUIRE((vdr->add_file_name(fn)) != nullptr);

	vdr->read_vaccs_dw_info();

	REQUIRE((cutab = vdr->get_cutab()) != nullptr);

	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);
;
	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	CHECK(vtab->size() == 4);

	REQUIRE((vrec = vtab->get("bar")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x6);
	CHECK(vrec->get_is_subprog());
	CHECK(vrec->get_low_pc() == 0x00400546);
	CHECK(vrec->get_high_pc() == 0x00400546+101);
	CHECK(vrec->get_local_var_table() != nullptr);
	CHECK(vrec->get_type() == void_type);

	REQUIRE((vrec = vtab->get("foo")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0xe);
	CHECK(vrec->get_is_subprog());
	CHECK(vrec->get_low_pc() == 0x004005ab);
	CHECK(vrec->get_high_pc() == 0x004005ab+7);
	CHECK(vrec->get_local_var_table() != nullptr);
	CHECK(vrec->get_type() == void_type);

	REQUIRE((vrec = vtab->get("main")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x12);
	CHECK(vrec->get_is_subprog());
	CHECK(vrec->get_low_pc() == 0x004005b2);
	CHECK(vrec->get_high_pc() == 0x004005b2+31);
	CHECK(vrec->get_local_var_table() != nullptr);
	CHECK(vrec->get_type() == hex_to_decimal_string("0x57"));

	REQUIRE((vrec = vtab->get("a")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x5);
	CHECK(!vrec->get_is_subprog());
	CHECK(!vrec->get_is_local());
	CHECK(vrec->get_type() == hex_to_decimal_string("0xb8"));

}

TEST_CASE("test_dwarf_local_bar_var_info") {
	std::string fn = "input/PAS_Example";
	dw_reader *dwr;

	REQUIRE((dwr = new dw_reader()) != nullptr);
	REQUIRE((dwr->add_file_name(fn)) != nullptr);

	dwr->read_dw_info();

	cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	var_table *vtab;
	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	var_record *vrec;
	REQUIRE((vrec = vtab->get("bar")) != nullptr);

	REQUIRE((vtab = vrec->get_local_var_table()) != nullptr);

	CHECK(vtab->size() == 2);

	REQUIRE((vrec = vtab->get("b")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x7);
	CHECK(!vrec->get_is_subprog());
	CHECK(vrec->get_is_local());
	CHECK(vrec->get_type() == hex_to_decimal_string("0xb8"));
	CHECK(vrec->get_location() == -48);

	REQUIRE((vrec = vtab->get("i")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x8);
	CHECK(!vrec->get_is_subprog());
	CHECK(vrec->get_is_local());
	CHECK(vrec->get_type() == hex_to_decimal_string("0x57"));
	CHECK(vrec->get_location() == -52);

	FILE *fp = fopen("input/PAS_Example.vdw","w");
	cutab->write(fp);
	fclose(fp);

	vaccs_dw_reader *vdr;
	fn = "input/PAS_Example.vdw";
	REQUIRE((vdr = new vaccs_dw_reader()) != nullptr);
	REQUIRE((vdr->add_file_name(fn)) != nullptr);

	vdr->read_vaccs_dw_info();

	REQUIRE((cutab = vdr->get_cutab()) != nullptr);

	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	REQUIRE((vrec = vtab->get("bar")) != nullptr);

	REQUIRE((vtab = vrec->get_local_var_table()) != nullptr);

	CHECK(vtab->size() == 2);

	REQUIRE((vrec = vtab->get("b")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x7);
	CHECK(!vrec->get_is_subprog());
	CHECK(vrec->get_is_local());
	CHECK(vrec->get_type() == hex_to_decimal_string("0xb8"));
	CHECK(vrec->get_location() == -48);

	REQUIRE((vrec = vtab->get("i")) != nullptr);
	CHECK(vrec->get_decl_file() == "/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c");
	CHECK(vrec->get_decl_line() == 0x8);
	CHECK(!vrec->get_is_subprog());
	CHECK(vrec->get_is_local());
	CHECK(vrec->get_type() == hex_to_decimal_string("0x57"));
	CHECK(vrec->get_location() == -52);

}

TEST_CASE("test_dwarf_local_foo_var_info") {
	std::string fn = "input/PAS_Example";
	dw_reader *dwr;

	REQUIRE((dwr = new dw_reader()) != nullptr);
	REQUIRE((dwr->add_file_name(fn)) != nullptr);

	dwr->read_dw_info();

	cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	var_table *vtab;
	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	var_record *vrec;
	REQUIRE((vrec = vtab->get("foo")) != nullptr);

	REQUIRE((vtab = vrec->get_local_var_table()) != nullptr);

	CHECK(vtab->size() == 0);

	FILE *fp = fopen("input/PAS_Example.vdw","w");
	cutab->write(fp);
	fclose(fp);

	vaccs_dw_reader *vdr;
	fn = "input/PAS_Example.vdw";
	REQUIRE((vdr = new vaccs_dw_reader()) != nullptr);
	REQUIRE((vdr->add_file_name(fn)) != nullptr);

	vdr->read_vaccs_dw_info();

	REQUIRE((cutab = vdr->get_cutab()) != nullptr);

	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	REQUIRE((vrec = vtab->get("foo")) != nullptr);

	REQUIRE((vtab = vrec->get_local_var_table()) != nullptr);

	CHECK(vtab->size() == 0);

}

TEST_CASE("test_dwarf_local_main_var_info") {
	std::string fn = "input/PAS_Example";
	dw_reader *dwr;

	REQUIRE((dwr = new dw_reader()) != nullptr);
	REQUIRE((dwr->add_file_name(fn)) != nullptr);

	dwr->read_dw_info();

	cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	var_table *vtab;
	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	var_record *vrec;
	REQUIRE((vrec = vtab->get("main")) != nullptr);

	REQUIRE((vtab = vrec->get_local_var_table()) != nullptr);

	CHECK(vtab->size() == 0);

	FILE *fp = fopen("input/PAS_Example.vdw","w");
	cutab->write(fp);
	fclose(fp);

	vaccs_dw_reader *vdr;
	fn = "input/PAS_Example.vdw";
	REQUIRE((vdr = new vaccs_dw_reader()) != nullptr);
	REQUIRE((vdr->add_file_name(fn)) != nullptr);

	vdr->read_vaccs_dw_info();

	REQUIRE((cutab = vdr->get_cutab()) != nullptr);

	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	REQUIRE((vrec = vtab->get("main")) != nullptr);

	REQUIRE((vtab = vrec->get_local_var_table()) != nullptr);

	CHECK(vtab->size() == 0);


}


