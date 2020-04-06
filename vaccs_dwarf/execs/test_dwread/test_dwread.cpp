/*
 * test_dwread.cpp
 *
 *  Created on: Jul 9, 2018
 *      Author: carr
 */

#define CATCH_CONFIG_MAIN

#include "../../libs/test/catch.hpp"

#include <libelfin/dwarf/dwarf++.hh>

#include "../../libs/tables/vdw_cu_table.h"
#include <tables/symbol_table_factory.h>

#include <read/dw_reader.h>

TEST_CASE("test_cu_table_record") {
	symbol_table_factory tab_factory;
	symbol_table_record_factory rec_factory ;

	vdw_cu_record *curec;
	REQUIRE((curec = (vdw_cu_record*)rec_factory.make_symbol_table_record(CU_RECORD)) != nullptr);

	dwarf::line_table lt;
	REQUIRE((curec->add_high_pc(0x12345L)
			->add_low_pc(0x98765L)
			->add_type_table((vdw_type_table*)0x34567L)
			->add_var_table((vdw_var_table*)0x87654L)) != nullptr);

	REQUIRE(curec->get_high_pc() == 0x12345L);
	REQUIRE(curec->get_low_pc() == 0x98765L);
	REQUIRE(curec->get_type_table() == (vdw_type_table*)0x34567L);
	REQUIRE(curec->get_var_table() == (vdw_var_table*)0x87654L);

	vdw_cu_table *cutab;
	REQUIRE((cutab = (vdw_cu_table*)tab_factory.make_symbol_table(CU_TABLE)) != nullptr);

	REQUIRE(cutab->put("/tmp/steve",curec));

	REQUIRE(cutab->get("/tmp/steve") == curec);
	REQUIRE(cutab->get("bogus") == nullptr);

}

TEST_CASE("test_type_table") {
	symbol_table_factory tfac;
	symbol_table_record_factory rfac;

	vdw_type_record *trec;
	REQUIRE((trec = (vdw_type_record*)rfac.make_symbol_table_record(TYPE_RECORD)) != nullptr);

	REQUIRE((trec->add_base_type("int")
			->add_is_array()
			->add_name("a")
			->add_size(4)
			->add_upper_bound(34,false)) != nullptr);

	REQUIRE((*trec->get_base_type()) == "int");
	REQUIRE(trec->get_is_array());
	REQUIRE((*trec->get_name()) == "a");
	REQUIRE(trec->get_size() == 4);
	REQUIRE(trec->get_upper_bound() == 34);

	vdw_type_table *tt;
	REQUIRE((tt = (vdw_type_table*)tfac.make_symbol_table(TYPE_TABLE)) != nullptr);

	REQUIRE(tt->put("0xef",trec));

	REQUIRE(tt->get("0xef") == trec);
	REQUIRE(tt->get("bogus") == nullptr);
}

TEST_CASE("test_var_table") {
	symbol_table_factory tfac;
	symbol_table_record_factory rfac;

	vdw_var_record *vrec;
	REQUIRE((vrec = (vdw_var_record*)rfac.make_symbol_table_record(VAR_RECORD)) != nullptr);

	vdw_var_table *vt;
	REQUIRE((vt = (vdw_var_table*)tfac.make_symbol_table(VAR_TABLE)) != nullptr);

	std::string fn = "/temp/steve.c";
	std::string ty = "short int";

	REQUIRE((vrec->add_decl_file(fn)
			->add_decl_line(7)
			->add_high_pc(0x987654)
			->add_is_subprog()
			->add_local_var_table(vt)
			->add_location(-36)
			->add_low_pc(0x123456)
			->add_type(ty)) != nullptr);

	REQUIRE(vrec->get_decl_file() == fn);
	REQUIRE(vrec->get_decl_line() == 7);
	REQUIRE(vrec->get_high_pc() == 0x987654);
	REQUIRE(!vrec->get_is_local());
	REQUIRE(vrec->get_is_subprog());
	REQUIRE(vrec->get_location() == -36);
	REQUIRE(vrec->get_low_pc() == 0x123456);
	REQUIRE(vrec->get_type() == ty);

	REQUIRE(vt->put("foo",vrec));

	REQUIRE(vt->get("foo") == vrec);

	REQUIRE(vt->get("main") == nullptr);
}

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

	dwr->read_dw_info();

	vdw_cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	CHECK(cutab->size() == 1);

	vdw_cu_record* curec;
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

	vdw_cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	vdw_cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	vdw_type_table *ttab;
	REQUIRE((ttab = curec->get_type_table()) != nullptr);

	CHECK(ttab->size() == 13);

	vdw_type_record *trec;

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

	vdw_cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	vdw_cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	vdw_var_table *vtab;
	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	CHECK(vtab->size() == 4);

	vdw_var_record *vrec;
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

	vdw_cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	vdw_cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	vdw_var_table *vtab;
	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	vdw_var_record *vrec;
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
	CHECK(vrec->get_location() ==-52);

}

TEST_CASE("test_dwarf_local_foo_var_info") {
	std::string fn = "input/PAS_Example";
	dw_reader *dwr;

	REQUIRE((dwr = new dw_reader()) != nullptr);
	REQUIRE((dwr->add_file_name(fn)) != nullptr);

	dwr->read_dw_info();

	vdw_cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	vdw_cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	vdw_var_table *vtab;
	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	vdw_var_record *vrec;
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

	vdw_cu_table* cutab;
	REQUIRE((cutab = dwr->get_cutab()) != nullptr);

	vdw_cu_record* curec;
	REQUIRE((curec = cutab->get("/home/carr/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/source/tools/PAS/tests/PAS_Example.c")) != nullptr);

	vdw_var_table *vtab;
	REQUIRE((vtab = curec->get_var_table()) != nullptr);

	vdw_var_record *vrec;
	REQUIRE((vrec = vtab->get("main")) != nullptr);

	REQUIRE((vtab = vrec->get_local_var_table()) != nullptr);

	CHECK(vtab->size() == 0);
}


