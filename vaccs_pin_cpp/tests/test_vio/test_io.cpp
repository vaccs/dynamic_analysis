/*
 * test_io.cpp
 *
 *  Created on: Jul 5, 2018
 *      Author: carr
 */

#define CATCH_CONFIG_MAIN

#include <cstdio>
#include <cstring>

#include <test/catch.hpp>

#include <io/vaccs_record_factory.h>
#include <io/vaccs_record_reader.h>
#include <io/arch_record.h>
#include <io/asm_record.h>
#include <io/binary_record.h>
#include <io/ccode_record.h>
#include <io/cmd_line_record.h>
#include <io/func_inv_record.h>
#include <io/output_record.h>
#include <io/register_record.h>
#include <io/return_addr_record.h>
#include <io/return_record.h>
#include <io/sd_clear_record.h>
#include <io/sd_czero_record.h>
#include <io/sd_lock_record.h>
#include <io/sd_set_record.h>
#include <io/sd_unlock_record.h>
#include <io/section_record.h>
#include <io/var_access_record.h>

FILE *fp;

bool open_vaccs_file(const char * filename, const char *mode) {
	return ((fp = fopen(filename, mode)) != 0);
}

bool close_vaccs_file(void) {
	return fclose(fp) == 0;
}

TEST_CASE("Test arch_record i386","[arch_record]") {
	vaccs_record_factory factory;
	arch_record *rec = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))->add_arch_type(VACCS_ARCH_I386)
           ->add_heap_start(0x56000)->add_heap_end(0x755fff);
	REQUIRE( rec != nullptr );
	REQUIRE(rec->get_arch_type() == VACCS_ARCH_I386);
        REQUIRE(rec->get_heap_start() == 0x560000);
        REQUIRE(rec->get_heap_end() == 0x755fff);

	REQUIRE(open_vaccs_file("input/i386.vaccs","w"));

	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/i386.vaccs", "r"));

	vaccs_record_reader *vrr= (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp);

	REQUIRE(vrr != nullptr);

	arch_record *rec2 = (arch_record *)vrr->read_next_vaccs_record();

	REQUIRE(close_vaccs_file());

	REQUIRE(rec2 != nullptr);
	REQUIRE(rec2->get_id() == VACCS_ARCH);
	REQUIRE(rec2->get_arch_type() == rec->get_arch_type());
        REQUIRE(rec2->get_heap_start() == rec->get_heap_start());
        REQUIRE(rec2->get_heap_end() == rec->get_heap_end());

	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("Test arch_record x86_64","[arch_record") {
	vaccs_record_factory factory;
	arch_record *rec = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))->add_arch_type(VACCS_ARCH_X86_64);
           ->add_heap_start(0x555555756000)->add_heap_end(0x5555d5755fff);
	REQUIRE( rec != nullptr );
	REQUIRE(rec->get_arch_type() == VACCS_ARCH_X86_64);
        REQUIRE(rec->get_heap_start() == 0x5555557560000);
        REQUIRE(rec->get_heap_end() == 0x5555d5755fff);

	REQUIRE(open_vaccs_file("input/x86_64.vaccs","w"));

	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/x86_64.vaccs", "r"));

	vaccs_record_reader *vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp);
	REQUIRE(vrr != nullptr);

	arch_record *rec2 = (arch_record *)vrr->read_next_vaccs_record();

	REQUIRE(close_vaccs_file());

	REQUIRE(rec2 != nullptr);
	REQUIRE(rec2->get_id() == VACCS_ARCH);
	REQUIRE(rec2->get_arch_type() == rec->get_arch_type());
        REQUIRE(rec2->get_heap_start() == rec->get_heap_start());
        REQUIRE(rec2->get_heap_end() == rec->get_heap_end());

	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_asm_record","[asm_record]") {
	vaccs_record_factory factory;
	asm_record* rec;
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_line_num(10)
			->add_c_line_num(2)
			->add_asm_file_name("/home/carr/test.s")
			->add_c_file_name("/home/carr/test.c")
			->add_asm_inst("\tmov %eax, %ebx")) != nullptr);


	REQUIRE(rec->get_id() == VACCS_ASM);
	REQUIRE(rec->get_asm_line_num() == 10);
	REQUIRE(rec->get_c_line_num() == 2);
	REQUIRE(strncmp(rec->get_asm_file_name(),"/home/carr/test.s",PATH_MAX) == 0);
	REQUIRE(strncmp(rec->get_c_file_name(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(strncmp(rec->get_asm_inst(),"\tmov %eax, %ebx",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/asm.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/asm.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	arch = (arch_record*)vrr->read_next_vaccs_record();
	REQUIRE(arch != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	asm_record *rec2;
	REQUIRE((rec2 = (asm_record*)vrr->read_next_vaccs_record()) != nullptr);

	REQUIRE(close_vaccs_file());

	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(rec2->get_asm_line_num() == rec->get_asm_line_num());
	REQUIRE(rec2->get_c_line_num() == rec->get_c_line_num());
	REQUIRE(strncmp(rec2->get_asm_file_name(),rec->get_asm_file_name(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_c_file_name(),rec->get_c_file_name(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_asm_inst(),rec->get_asm_inst(),VACCS_MAX_SRC_LINE_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_bin_record(void)", "[bin_record]") {

	vaccs_record_factory factory;
	binary_record *rec;

	REQUIRE((rec = ((binary_record*)factory.make_vaccs_record(VACCS_BINARY))
												->add_bin_file_name("/home/carr/test"))!= nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_BINARY);
	REQUIRE(strncmp(rec->get_bin_file_name(),"/home/carr/test",PATH_MAX) == 0);

	REQUIRE(open_vaccs_file("input/bin.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/bin.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	binary_record *rec2;
	REQUIRE((rec2 = (binary_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());

	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(strncmp(rec2->get_bin_file_name(),rec->get_bin_file_name(),PATH_MAX) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_c_src_record(void)","[ccode_record]") {

	vaccs_record_factory factory;
	ccode_record *rec;

	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
			->add_c_file_name("/home/carr/test.c")
			->add_c_line_num(10)
			->add_c_src_line("x = y;")
			->add_c_start_pos(2)) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_CCODE);
	REQUIRE(rec->get_c_line_num() == 10);
	REQUIRE(rec->get_c_start_pos() == 2);
	REQUIRE(strncmp(rec->get_c_file_name(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(strncmp(rec->get_c_src_line(),"x = y;",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/c_src.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/c_src.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	ccode_record *rec2;
	REQUIRE((rec2 = (ccode_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());

	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(rec2->get_c_line_num() == rec->get_c_line_num());
	REQUIRE(rec2->get_c_start_pos() == rec->get_c_start_pos());
	REQUIRE(strncmp(rec2->get_c_file_name(),rec->get_c_file_name(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_c_src_line(),rec->get_c_src_line(),VACCS_MAX_SRC_LINE_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_cmd_line_record(void)", "[bin_record]") {

	vaccs_record_factory factory;
	cmd_line_record *rec;

	REQUIRE((rec = ((cmd_line_record*)factory.make_vaccs_record(VACCS_CMD_LINE))
												->add_cmd_line("run aparam"))!= nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_CMD_LINE);
	REQUIRE(strncmp(rec->get_cmd_line(),"run aparam",PATH_MAX) == 0);

	REQUIRE(open_vaccs_file("input/cmd.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/cmd.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	cmd_line_record *rec2;
	REQUIRE((rec2 = (cmd_line_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());

	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(strncmp(rec2->get_cmd_line(),rec->get_cmd_line(),PATH_MAX) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("void test_vaccs_make_func_inv_record(void)","[func_inv_record]") {

	vaccs_record_factory factory;
	func_inv_record *rec;

	REQUIRE((rec = ((func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV))
			->add_event_num(10)
			->add_address(0x123ABCDE)
			->add_func_line_num(15)
			->add_inv_line_num(37)
			->add_c_func_file("/home/carr/test.c")
			->add_c_inv_file("/home/carr/test.c")
			->add_func_name("test"))!= nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_FUNCTION_INV);

	REQUIRE(rec->get_event_num() == 10);
	REQUIRE(rec->get_func_line_num() == 15);
	REQUIRE(rec->get_inv_line_num() == 37);
	REQUIRE(strncmp(rec->get_c_func_file(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(strncmp(rec->get_c_inv_file(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(rec->get_address() == 0x123ABCDE);

	REQUIRE(open_vaccs_file("input/inv.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/inv.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr  = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	func_inv_record *rec2;
	REQUIRE((rec2 = (func_inv_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());

	REQUIRE(rec2->get_id() == rec->get_id());

	REQUIRE(rec2->get_event_num() == rec->get_event_num());
	REQUIRE(rec2->get_func_line_num()== rec->get_func_line_num());
	REQUIRE(rec2->get_inv_line_num()== rec->get_inv_line_num());
	REQUIRE(strncmp(rec2->get_c_func_file(),rec->get_c_func_file(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_c_inv_file(),rec->get_c_inv_file(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_func_name(),rec->get_func_name(),VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(rec2->get_address() == rec->get_address());

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_output_record(void)", "[bin_record]") {

	vaccs_record_factory factory;
	output_record *rec;

	REQUIRE((rec = ((output_record*)factory.make_vaccs_record(VACCS_OUTPUT))
												->add_output("Hello World!")
												->add_event_num(34))!= nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_OUTPUT);
	REQUIRE(strncmp(rec->get_output(),"Hello World!",PATH_MAX) == 0);
	REQUIRE(rec->get_event_num() == 34);

	REQUIRE(open_vaccs_file("input/out.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/out.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	output_record *rec2;
	REQUIRE((rec2 = (output_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());

	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(strncmp(rec2->get_output(),rec->get_output(),PATH_MAX) == 0);
	REQUIRE(rec2->get_event_num() == rec->get_event_num());

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_register_record(void)","[register_record]") {

	vaccs_record_factory factory;
	register_record *rec;

	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
												->add_c_file_name("/home/carr/register.c")
												->add_c_line_num(31)
												->add_event_num(22)
												->add_register_name("%eax")
												->add_value(0xfff))!= nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_REGISTER);
;
	REQUIRE(rec->get_event_num() == 22);
	REQUIRE(rec->get_c_line_num() == 31);
	REQUIRE(rec->get_value() == 0xfff);
	REQUIRE(strncmp(rec->get_c_file_name(),"/home/carr/register.c",PATH_MAX) == 0);
	REQUIRE(strncmp(rec->get_register_name(),"%eax",VACCS_MAX_REGISTER_NAME_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/reg.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/reg.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	register_record *rec2;
	REQUIRE((rec2 = (register_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_event_num() == rec->get_event_num());
	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(rec2->get_c_line_num()== rec->get_c_line_num());
	REQUIRE(rec2->get_value() == rec->get_value());
	REQUIRE(strncmp(rec2->get_c_file_name(),rec->get_c_file_name(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_register_name(),rec->get_register_name(),VACCS_MAX_REGISTER_NAME_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_return_address_record(void)","[return_addr_record]") {

	vaccs_record_factory factory;
	return_addr_record *rec;

	REQUIRE((rec = ((return_addr_record*)factory.make_vaccs_record(VACCS_RETURN_ADDR))
			->add_c_func_name("foo")
			->add_dynamic_link(0x12345)
			->add_return_address(0x67890)) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_RETURN_ADDR);
	REQUIRE(rec->get_dynamic_link() == 0x12345);
	REQUIRE(rec->get_return_address() == 0x67890);
	REQUIRE(strncmp(rec->get_c_func_name(),"foo",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/ret_addr.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/ret_addr.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	return_addr_record *rec2;
	REQUIRE((rec2 = (return_addr_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(rec2->get_dynamic_link() == rec->get_dynamic_link());
	REQUIRE(rec2->get_return_address() == rec->get_return_address());
	REQUIRE(strncmp(rec2->get_c_func_name(),rec->get_c_func_name(),VACCS_MAX_VARIABLE_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_return_record(void)","[return_record") {

	vaccs_record_factory factory;
	return_record *rec;

	REQUIRE((rec = ((return_record*)factory.make_vaccs_record(VACCS_RETURN))
			->add_event_num(3145)) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_RETURN);
	REQUIRE(rec->get_event_num() == 3145);

	REQUIRE(open_vaccs_file("input/ret.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/ret.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	return_record *rec2;
	REQUIRE((rec2 = (return_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(rec2->get_event_num() == rec->get_event_num());

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}


TEST_CASE("test_vaccs_make_sd_clear_record(void)","[sd_clear_record]") {

	vaccs_record_factory factory;
	sd_clear_record *rec;

	REQUIRE((rec = ((sd_clear_record*)factory.make_vaccs_record(VACCS_SD_CLEAR))
			->add_event_num(52)
			->add_c_file_name("/home/carr/sd.c")
			->add_c_line_num(17)
			->add_scope("*G*")
			->add_variable("sd_x")) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_SD_CLEAR);
	REQUIRE(rec->get_event_num() == 52);
	REQUIRE(rec->get_c_line_num() == 17);
	REQUIRE(strncmp(rec->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(strncmp(rec->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(rec->get_variable(),"sd_x",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/sd_clear.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/sd_clear.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	sd_clear_record *rec2;
	REQUIRE((rec2 = (sd_clear_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(rec2->get_event_num()
					== rec->get_event_num());
	REQUIRE(rec2->get_c_line_num()
					== rec->get_c_line_num());
	REQUIRE(strncmp(rec2->get_c_file_name(),rec->get_c_file_name(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_scope(),rec->get_scope(),VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(rec2->get_variable(),rec->get_variable(),VACCS_MAX_VARIABLE_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("void test_vaccs_make_sd_corezero_record(void)","[sd_czero_record]") {

	vaccs_record_factory factory;
	sd_czero_record *rec;

	REQUIRE((rec = ((sd_czero_record*)factory.make_vaccs_record(VACCS_SD_CZERO))
			->add_event_num(37)
			->add_c_file_name("/home/carr/sd.c")
			->add_c_line_num(85)) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_SD_CZERO);
	REQUIRE(rec->get_event_num() == 37);
	REQUIRE(rec->get_c_line_num() == 85);
	REQUIRE(strncmp(rec->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);

	REQUIRE(open_vaccs_file("input/sd_zero.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/sd_zero.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	sd_czero_record *rec2;
	REQUIRE((rec2 = (sd_czero_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(
			rec2->get_event_num()
					== rec->get_event_num());
	REQUIRE(
			rec2->get_c_line_num()
					== rec->get_c_line_num());
	REQUIRE(
			strncmp(rec2->get_c_file_name(),rec->get_c_file_name(),PATH_MAX) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_sd_lock_record(void)","[sd_lock_record]") {

	vaccs_record_factory factory;
	sd_lock_record *rec;

	REQUIRE((rec = ((sd_lock_record*)factory.make_vaccs_record(VACCS_SD_LOCK))
			->add_event_num(52)
			->add_c_file_name("/home/carr/sd.c")
			->add_c_line_num(17)
			->add_scope("*G*")
			->add_variable("sd_x")) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_SD_LOCK);
	REQUIRE(rec->get_event_num() == 52);
	REQUIRE(rec->get_c_line_num() == 17);
	REQUIRE(strncmp(rec->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(strncmp(rec->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(rec->get_variable(),"sd_x",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/sd_lock.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/sd_lock.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	sd_lock_record *rec2;
	REQUIRE((rec2 = (sd_lock_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(rec2->get_event_num()
					== rec->get_event_num());
	REQUIRE(rec2->get_c_line_num()
					== rec->get_c_line_num());
	REQUIRE(strncmp(rec2->get_c_file_name(),rec->get_c_file_name(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_scope(),rec->get_scope(),VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(rec2->get_variable(),rec->get_variable(),VACCS_MAX_VARIABLE_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_sd_set_record(void)","[sd_set_record]") {

	vaccs_record_factory factory;
	sd_set_record *rec;

	REQUIRE((rec = ((sd_set_record*)factory.make_vaccs_record(VACCS_SD_SET))
			->add_event_num(52)
			->add_c_file_name("/home/carr/sd.c")
			->add_c_line_num(17)
			->add_scope("*G*")
			->add_variable("sd_x")) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_SD_SET);
	REQUIRE(rec->get_event_num() == 52);
	REQUIRE(rec->get_c_line_num() == 17);
	REQUIRE(strncmp(rec->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(strncmp(rec->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(rec->get_variable(),"sd_x",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/sd_set.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/sd_set.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	sd_set_record *rec2;
	REQUIRE((rec2 = (sd_set_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(rec2->get_event_num()
					== rec->get_event_num());
	REQUIRE(rec2->get_c_line_num()
					== rec->get_c_line_num());
	REQUIRE(strncmp(rec2->get_c_file_name(),rec->get_c_file_name(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_scope(),rec->get_scope(),VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(rec2->get_variable(),rec->get_variable(),VACCS_MAX_VARIABLE_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_sd_unlock_record(void)","[sd_unlock_record]") {

	vaccs_record_factory factory;
	sd_unlock_record *rec;

	REQUIRE((rec = ((sd_unlock_record*)factory.make_vaccs_record(VACCS_SD_UNLOCK))
			->add_event_num(52)
			->add_c_file_name("/home/carr/sd.c")
			->add_c_line_num(17)
			->add_scope("*G*")
			->add_variable("sd_x")) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_SD_UNLOCK);
	REQUIRE(rec->get_event_num() == 52);
	REQUIRE(rec->get_c_line_num() == 17);
	REQUIRE(strncmp(rec->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(strncmp(rec->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(rec->get_variable(),"sd_x",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/sd_unlock.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/sd_unlock.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	sd_unlock_record *rec2;
	REQUIRE((rec2 = (sd_unlock_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(rec2->get_event_num()
					== rec->get_event_num());
	REQUIRE(rec2->get_c_line_num()
					== rec->get_c_line_num());
	REQUIRE(strncmp(rec2->get_c_file_name(),rec->get_c_file_name(),PATH_MAX) == 0);
	REQUIRE(strncmp(rec2->get_scope(),rec->get_scope(),VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(rec2->get_variable(),rec->get_variable(),VACCS_MAX_VARIABLE_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_section_record(void)","[section_record]") {

	vaccs_record_factory factory;
	section_record *rec;

	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|SectID|Type|File Address|Per|File Off.|File Size|Flags|Section Name")) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_SECTION);

	REQUIRE(strncmp(rec->get_info(),"|SectID|Type|File Address|Per|File Off.|File Size|Flags|Section Name",VACCS_MAX_OUTPUT_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/section.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/section.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	section_record *rec2;
	REQUIRE((rec2 = (section_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_id() == rec->get_id());

	REQUIRE(strncmp(rec2->get_info(),rec->get_info(),VACCS_MAX_OUTPUT_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_make_var_access_record(void)","[var_access_record]") {

	vaccs_record_factory factory;
	var_access_record *rec;

	REQUIRE((rec = ((var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS))
			->add_address(0x1234567)
			->add_c_file_name("/home/carr/sd.c")
			->add_c_line_num(17)
			->add_event_num(52)
			->add_name("x")
			->add_points_to(0x89ABCDEF)
			->add_scope("*G*")
			->add_type("int")
			->add_value("37")) != nullptr);


	REQUIRE(rec != NULL);
	REQUIRE(rec->get_id() == VACCS_VAR_ACCESS);
	REQUIRE(rec->get_event_num() == 52);
	REQUIRE(rec->get_c_line_num() == 17);
	REQUIRE(
			strncmp(rec->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(rec->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(rec->get_address() == 0x1234567);
	REQUIRE(
			strncmp(rec->get_type(),"int",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			strncmp(rec->get_name(),"x",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(rec->get_points_to() == 0x89ABCDEF);
	REQUIRE(
			strncmp(rec->get_value(),"37",VACCS_MAX_VALUE_LENGTH) == 0);

	REQUIRE(open_vaccs_file("input/var_access.vaccs", "w"));

	arch_record* arch;
	REQUIRE ((arch = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
										->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	arch->write(fp);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/var_access.vaccs", "r"));
	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((arch = (arch_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(arch->get_id() == VACCS_ARCH);
	REQUIRE(arch->get_arch_type() == VACCS_ARCH_X86_64);

	var_access_record *rec2;
	REQUIRE((rec2 = (var_access_record*)vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(close_vaccs_file());
	REQUIRE(rec2->get_id() == rec->get_id());
	REQUIRE(
			rec2->get_event_num()
					== rec->get_event_num());
	REQUIRE(
			rec2->get_c_line_num()
					== rec->get_c_line_num());
	REQUIRE(
			strncmp(rec2->get_c_file_name(),rec->get_c_file_name(),PATH_MAX) == 0);
	REQUIRE(
			strncmp(rec2->get_scope(),rec->get_scope(),VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			rec2->get_address()
					== rec->get_address());
	REQUIRE(
			strncmp(rec2->get_type(),rec->get_type(),VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			strncmp(rec2->get_name(),rec->get_name(),VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			rec2->get_points_to()
					== rec->get_points_to());
	REQUIRE(
			strncmp(rec2->get_value(),rec->get_value(),VACCS_MAX_VALUE_LENGTH) == 0);

	delete arch;
	delete rec;
	delete rec2;
	delete vrr;
}

TEST_CASE("test_vaccs_several_records(void)") {

	REQUIRE(open_vaccs_file("input/test.vaccs", "w"));

	vaccs_record_factory factory;

	vaccs_record *rec;

	REQUIRE((rec = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
			->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);

	rec->write(fp);

	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("/home/carr/test.s")
			->add_asm_inst("\tmov %eax, %ebx")
			->add_asm_line_num(10)
			->add_c_file_name("/home/carr/test.c")
			->add_c_line_num(2)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
				->add_asm_file_name("/home/carr/test.s")
				->add_asm_inst("\tadd %eax, %ebx")
				->add_asm_line_num(11)
				->add_c_file_name("/home/carr/test.c")
				->add_c_line_num(3)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
				->add_asm_file_name("/home/carr/test.s")
				->add_asm_inst("\tsub %eax, %ebx")
				->add_asm_line_num(12)
				->add_c_file_name("/home/carr/test.c")
				->add_c_line_num(3)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
				->add_asm_file_name("/home/carr/test.s")
				->add_asm_inst("\tmov %ebx, %eax")
				->add_asm_line_num(13)
				->add_c_file_name("/home/carr/test.c")
				->add_c_line_num(3)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((binary_record*)factory.make_vaccs_record(VACCS_BINARY))
			->add_bin_file_name("/home/carr/test")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
			->add_c_file_name("/home/carr/test.c")
			->add_c_line_num(2)
			->add_c_src_line("x = y;")
			->add_c_start_pos(1)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
					->add_c_file_name("/home/carr/test.c")
					->add_c_line_num(3)
					->add_c_src_line("y = x + x - x;")
					->add_c_start_pos(1)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV))
			->add_address(0x1A2B3C)
			->add_c_func_file("/home/carr/test.c")
			->add_c_inv_file("/home/carr/test.c")
			->add_event_num(10)
			->add_func_line_num(15)
			->add_func_name("test")
			->add_inv_line_num(37)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((cmd_line_record*)factory.make_vaccs_record(VACCS_CMD_LINE))
			->add_cmd_line("/home/carr/test")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|SectID|Type|File Address|Per|File Off.|File Size|Flags|Section Name")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
					->add_info("|0x00000001|regular| |---|0x00000000|0x00000000|0x00000000|sign.")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((output_record*)factory.make_vaccs_record(VACCS_OUTPUT))
							->add_output( "Hello world!!")
							->add_event_num(0x134L)) != nullptr);
	rec->write(fp);

	REQUIRE((rec =  ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
									->add_event_num(22)
									->add_c_line_num(31)
									->add_c_file_name("/home/carr/register.c")
									->add_register_name("%eax")
									->add_value(0xfff)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
											->add_event_num(23)
											->add_c_line_num(35)
											->add_c_file_name("/home/carr/register.c")
											->add_register_name("%ebx")
											->add_value(0x0)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((return_record*)factory.make_vaccs_record(VACCS_RETURN))
			->add_event_num(3145)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((return_addr_record*)factory.make_vaccs_record(VACCS_RETURN_ADDR))
				->add_c_func_name("foo")
				->add_return_address(0x67890)
				->add_dynamic_link(0x12345)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((sd_clear_record*)factory.make_vaccs_record(VACCS_SD_CLEAR))
					->add_c_file_name("/home/carr/sd.c")
					->add_c_line_num(17)
					->add_event_num(52)
					->add_scope("*G*")
					->add_variable("sd_x")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((sd_czero_record*)factory.make_vaccs_record(VACCS_SD_CZERO))
					->add_c_file_name("/home/carr/sd.c")
					->add_c_line_num(85)
					->add_event_num(37)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((sd_lock_record*)factory.make_vaccs_record(VACCS_SD_LOCK))
					->add_c_file_name("/home/carr/sd.c")
					->add_c_line_num(17)
					->add_event_num(52)
					->add_scope("*G*")
					->add_variable("sd_x")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((sd_set_record*)factory.make_vaccs_record(VACCS_SD_SET))
					->add_c_file_name("/home/carr/sd.c")
					->add_c_line_num(17)
					->add_event_num(52)
					->add_scope("*G*")
					->add_variable("sd_x")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((sd_unlock_record*)factory.make_vaccs_record(VACCS_SD_UNLOCK))
					->add_c_file_name("/home/carr/sd.c")
					->add_c_line_num(17)
					->add_event_num(52)
					->add_scope("*G*")
					->add_variable("sd_x")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS))
					->add_c_file_name("/home/carr/sd.c")
					->add_c_line_num(17)
					->add_event_num(52)
					->add_scope("*G*")
					->add_name("x")
					->add_address(0x1234567)
					->add_points_to(0x89ABCDEF)
					->add_type("int")
					->add_value("37")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS))
					->add_c_file_name("/home/carr/sd.c")
					->add_c_line_num(17)
					->add_event_num(52)
					->add_scope("*G*")
					->add_name("y")
					->add_address(0x89ABC)
					->add_points_to(0x89ABCDEF)
					->add_type("short")
					->add_value("12")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS))
					->add_c_file_name("/home/carr/sd.c")
					->add_c_line_num(17)
					->add_event_num(52)
					->add_scope("*G*")
					->add_name("zx")
					->add_address(0xDEF012)
					->add_points_to(0x89ABCDEF)
					->add_type("long")
					->add_value("101")) != nullptr);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/test.vaccs", "r"));

	vaccs_record_reader *vrr;
	REQUIRE((vrr = (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp)) != nullptr);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(((arch_record*)rec)->get_id() == VACCS_ARCH);
	REQUIRE(((arch_record*)rec)->get_arch_type() == VACCS_ARCH_X86_64);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((arch_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 10);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 2);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"/home/carr/test.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"\tmov %eax, %ebx",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 11);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 3);
	REQUIRE(
			strncmp(((asm_record*)rec)->get_asm_file_name(),"/home/carr/test.s",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((asm_record*)rec)->get_c_file_name(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"\tadd %eax, %ebx",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 12);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 3);
	REQUIRE(
			strncmp(((asm_record*)rec)->get_asm_file_name(),"/home/carr/test.s",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((asm_record*)rec)->get_c_file_name(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((asm_record*)rec)->get_asm_inst(),"\tsub %eax, %ebx",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 13);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 3);
	REQUIRE(
			strncmp(((asm_record*)rec)->get_asm_file_name(),"/home/carr/test.s",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((asm_record*)rec)->get_c_file_name(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((asm_record*)rec)->get_asm_inst(),"\tmov %ebx, %eax",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((binary_record*)rec)->get_id() == VACCS_BINARY);
	REQUIRE(
			strncmp(((binary_record*)rec)->get_bin_file_name(),"/home/carr/test",PATH_MAX) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((ccode_record*)rec)->get_id() == VACCS_CCODE);
	REQUIRE(((ccode_record*)rec)->get_c_line_num() == 2);
	REQUIRE(((ccode_record*)rec)->get_c_start_pos() == 1);
	REQUIRE(
			strncmp(((ccode_record*)rec)->get_c_file_name(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((ccode_record*)rec)->get_c_src_line(),"x = y;",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((ccode_record*)rec)->get_id() == VACCS_CCODE);
	REQUIRE(((ccode_record*)rec)->get_c_line_num() == 3);
	REQUIRE(((ccode_record*)rec)->get_c_start_pos() == 1);
	REQUIRE(
			strncmp(((ccode_record*)rec)->get_c_file_name(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((ccode_record*)rec)->get_c_src_line(),"y = x + x - x;",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((func_inv_record*)rec)->get_id() == VACCS_FUNCTION_INV);
	REQUIRE(((func_inv_record*)rec)->get_event_num() == 10);
	REQUIRE(((func_inv_record*)rec)->get_func_line_num() == 15);
	REQUIRE(((func_inv_record*)rec)->get_inv_line_num() == 37);
	REQUIRE(
			strncmp(((func_inv_record*)rec)->get_c_func_file(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((func_inv_record*)rec)->get_c_inv_file(),"/home/carr/test.c",PATH_MAX) == 0);
	REQUIRE(((func_inv_record*)rec)->get_address() == (vaccs_address_t )0x1A2B3C);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((cmd_line_record*)rec)->get_id() == VACCS_CMD_LINE);
	REQUIRE(
			strncmp(((cmd_line_record*)rec)->get_cmd_line(),"/home/carr/test",PATH_MAX) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|SectID|Type|File Address|Per|File Off.|File Size|Flags|Section Name",VACCS_MAX_OUTPUT_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000001|regular| |---|0x00000000|0x00000000|0x00000000|sign.",VACCS_MAX_OUTPUT_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_OUTPUT);
	REQUIRE(((output_record*)rec)->get_event_num() == 0x134L);
	REQUIRE(
			strncmp(((output_record*)rec)->get_output(),"Hello world!!",VACCS_MAX_OUTPUT_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 22);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 31);
	REQUIRE(((register_record*)rec)->get_value() == 0xfff);
	REQUIRE(
			strncmp(((register_record*)rec)->get_c_file_name(),"/home/carr/register.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((register_record*)rec)->get_register_name(),"%eax",VACCS_MAX_REGISTER_NAME_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 23);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 35);
	REQUIRE(((register_record*)rec)->get_value() == 0x0);
	REQUIRE(
			strncmp(((register_record*)rec)->get_c_file_name(),"/home/carr/register.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((register_record*)rec)->get_register_name(),"%ebx",VACCS_MAX_REGISTER_NAME_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_RETURN);
	REQUIRE(((return_record*)rec)->get_event_num() == 3145);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_RETURN_ADDR);
	REQUIRE(((return_addr_record*)rec)->get_dynamic_link() == 0x12345);
	REQUIRE(((return_addr_record*)rec)->get_return_address() == 0x67890);
	REQUIRE(
			strncmp(((return_addr_record*)rec)->get_c_func_name(),"foo",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_SD_CLEAR);
	REQUIRE(((sd_clear_record*)rec)->get_event_num() == 52);
	REQUIRE(((sd_clear_record*)rec)->get_c_line_num() == 17);
	REQUIRE(
			strncmp(((sd_clear_record*)rec)->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((sd_clear_record*)rec)->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			strncmp(((sd_clear_record*)rec)->get_variable(),"sd_x",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_SD_CZERO);
	REQUIRE(((sd_czero_record*)rec)->get_event_num() == 37);
	REQUIRE(((sd_czero_record*)rec)->get_c_line_num() == 85);
	REQUIRE(
			strncmp(((sd_czero_record*)rec)->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_SD_LOCK);
	REQUIRE(((sd_lock_record*)rec)->get_event_num() == 52);
	REQUIRE(((sd_lock_record*)rec)->get_c_line_num() == 17);
	REQUIRE(
			strncmp(((sd_lock_record*)rec)->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((sd_lock_record*)rec)->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			strncmp(((sd_lock_record*)rec)->get_variable(),"sd_x",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_SD_SET);
	REQUIRE(((sd_set_record*)rec)->get_event_num() == 52);
	REQUIRE(((sd_set_record*)rec)->get_c_line_num() == 17);
	REQUIRE(
			strncmp(((sd_set_record*)rec)->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((sd_set_record*)rec)->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			strncmp(((sd_set_record*)rec)->get_variable(),"sd_x",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_SD_UNLOCK);
	REQUIRE(((sd_unlock_record*)rec)->get_event_num() == 52);
	REQUIRE(((sd_unlock_record*)rec)->get_c_line_num() == 17);
	REQUIRE(
			strncmp(((sd_unlock_record*)rec)->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((sd_unlock_record*)rec)->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			strncmp(((sd_unlock_record*)rec)->get_variable(),"sd_x",VACCS_MAX_VARIABLE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_VAR_ACCESS);
	REQUIRE(((var_access_record*)rec)->get_event_num() == 52);
	REQUIRE(((var_access_record*)rec)->get_c_line_num() == 17);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_address() == 0x1234567);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_type(),"int",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_name(),"x",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_points_to() == 0x89ABCDEF);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_value(),"37",VACCS_MAX_VALUE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_VAR_ACCESS);
	REQUIRE(((var_access_record*)rec)->get_event_num() == 52);
	REQUIRE(((var_access_record*)rec)->get_c_line_num() == 17);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_address() == 0x89ABC);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_type(),"short",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_name(),"y",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_points_to() == 0x89ABCDEF);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_value(),"12",VACCS_MAX_VALUE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) != nullptr);
	REQUIRE(rec != NULL);
	REQUIRE(((section_record*)rec)->get_id() == VACCS_VAR_ACCESS);
	REQUIRE(((var_access_record*)rec)->get_event_num() == 52);
	REQUIRE(((var_access_record*)rec)->get_c_line_num() == 17);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_c_file_name(),"/home/carr/sd.c",PATH_MAX) == 0);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_scope(),"*G*",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_address() == 0xDEF012);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_type(),"long",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_name(),"zx",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_points_to() == 0x89ABCDEF);
	REQUIRE(
			strncmp(((var_access_record*)rec)->get_value(),"101",VACCS_MAX_VALUE_LENGTH) == 0);

	REQUIRE((rec = vrr->read_next_vaccs_record()) == nullptr);

	REQUIRE(close_vaccs_file());
}

TEST_CASE("test_build_sign_file(void)") {

	REQUIRE(open_vaccs_file("input/sign-output.vaccs", "w"));

	vaccs_record_factory factory;

	vaccs_record *rec;

	REQUIRE((rec = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))
			->add_arch_type(VACCS_ARCH_X86_64)) != nullptr);


	rec->write(fp);

	REQUIRE((rec = ((cmd_line_record*)factory.make_vaccs_record(VACCS_CMD_LINE))
			->add_cmd_line("sign")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((binary_record*)factory.make_vaccs_record(VACCS_BINARY))
			->add_bin_file_name("sign")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
			->add_c_file_name("sign.c")
			->add_c_line_num(1)
			->add_c_src_line("#include <stdio.h>")
			->add_c_start_pos(1)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
			->add_c_file_name("sign.c")
			->add_c_line_num(2)
			->add_c_src_line("int main(){")
			->add_c_start_pos(1)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_c_src_line("  int i = -3;")
			->add_c_start_pos(1)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
			->add_c_file_name("sign.c")
			->add_c_line_num(4)
			->add_c_src_line("  unsigned short u;")
			->add_c_start_pos(1)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
			->add_c_file_name("sign.c")
			->add_c_line_num(5)
			->add_c_src_line("  u=i;")
			->add_c_start_pos(1)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
			->add_c_file_name("sign.c")
			->add_c_line_num(6)
			->add_c_src_line(" ")
			->add_c_start_pos(1)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))
			->add_c_file_name("sign.c")
			->add_c_line_num(7)
			->add_c_src_line("}")
			->add_c_start_pos(1)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("0x80483dc <+6>: movl   $0xfffffffd, -0x4(%ebp)   ; imm = 0xFFFFFFFD")
			->add_asm_line_num(1)
			->add_c_file_name("sign.c")
			->add_c_line_num(3)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst(" ")
			->add_asm_line_num(2)
			->add_c_file_name("sign.c")
			->add_c_line_num(5)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("sign`main:")
			->add_asm_line_num(3)
			->add_c_file_name("sign.c")
			->add_c_line_num(5)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("0x80483e3 <+13>: movl   -0x4(%ebp), %eax")
			->add_asm_line_num(4)
			->add_c_file_name("sign.c")
			->add_c_line_num(5)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("0x80483e6 <+16>: movw   %ax, -0x6(%ebp)")
			->add_asm_line_num(5)
			->add_c_file_name("sign.c")
			->add_c_line_num(5)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("0x80483ea <+20>: movl   $0x0, %eax")
			->add_asm_line_num(6)
			->add_c_file_name("sign.c")
			->add_c_line_num(5)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("6")
			->add_asm_line_num(7)
			->add_c_file_name("sign.c")
			->add_c_line_num(7)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("7   	}")
			->add_asm_line_num(8)
			->add_c_file_name("sign.c")
			->add_c_line_num(7)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst(" ")
			->add_asm_line_num(9)
			->add_c_file_name("sign.c")
			->add_c_line_num(7)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("sign`main:")
			->add_asm_line_num(10)
			->add_c_file_name("sign.c")
			->add_c_line_num(7)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("0x80483ef <+25>: leave")
			->add_asm_line_num(11)
			->add_c_file_name("sign.c")
			->add_c_line_num(7)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((asm_record*)factory.make_vaccs_record(VACCS_ASM))
			->add_asm_file_name("sign.s")
			->add_asm_inst("0x80483f0 <+26>: retl")
			->add_asm_line_num(12)
			->add_c_file_name("sign.c")
			->add_c_line_num(7)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|SectID|Type|File Address|Per|File Off.|File Size|Flags|Section Name")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000001|regular| |---|0x00000000|0x00000000|0x00000000|sign.")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000002|regular|[0x0000000008048154-0x0000000008048167)|r--|0x00000154|0x00000013|0x00000002|sign..interp")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000003|regular|[0x0000000008048168-0x0000000008048188)|r--|0x00000168|0x00000020|0x00000002|sign..note.ABI-tag")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000004|regular|[0x0000000008048188-0x00000000080481ac)|r--|0x00000188|0x00000024|0x00000002|sign..note.gnu.build-id")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000005|regular|[0x00000000080481ac-0x00000000080481cc)|r--|0x000001ac|0x00000020|0x00000002|sign..gnu.hash")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000006|elf-dynamic-symbols|[0x00000000080481cc-0x000000000804820c)|r--|0x000001cc|0x00000040|0x00000002|sign..dynsym")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000007|regular|[0x000000000804820c-0x0000000008048251)|r--|0x0000020c|0x00000045|0x00000002|sign..dynstr")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000008|regular|[0x0000000008048252-0x000000000804825a)|r--|0x00000252|0x00000008|0x00000002|sign..gnu.version")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000009|regular|[0x000000000804825c-0x000000000804827c)|r--|0x0000025c|0x00000020|0x00000002|sign..gnu.version_r")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000000a|elf-relocation-entries|[0x000000000804827c-0x0000000008048284)|r--|0x0000027c|0x00000008|0x00000002|sign..rel.dyn")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000000b|elf-relocation-entries|[0x0000000008048284-0x000000000804828c)|r--|0x00000284|0x00000008|0x00000042|sign..rel.plt")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000000c|regular|[0x000000000804828c-0x00000000080482af)|r-x|0x0000028c|0x00000023|0x00000006|sign..init")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000000d|regular|[0x00000000080482b0-0x00000000080482d0)|r-x|0x000002b0|0x00000020|0x00000006|sign..plt")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000000e|regular|[0x00000000080482d0-0x00000000080482d8)|r-x|0x000002d0|0x00000008|0x00000006|sign..plt.got")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000000f|code|[0x00000000080482e0-0x0000000008048462)|r-x|0x000002e0|0x00000182|0x00000006|sign..text")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000010|regular|[0x0000000008048464-0x0000000008048478)|r-x|0x00000464|0x00000014|0x00000006|sign..fini")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000011|regular|[0x0000000008048478-0x0000000008048484)|r--|0x00000478|0x0000000c|0x00000002|sign..rodata")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000012|regular|[0x0000000008048484-0x00000000080484b0)|r--|0x00000484|0x0000002c|0x00000002|sign..eh_frame_hdr")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000013|eh-frame|[0x00000000080484b0-0x0000000008048570)|r--|0x000004b0|0x000000c0|0x00000002|sign..eh_frame")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000014|regular|[0x0000000008049f0c-0x0000000008049f10)|rw-|0x00000f0c|0x00000004|0x00000003|sign..init_array")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000015|regular|[0x0000000008049f10-0x0000000008049f14)|rw-|0x00000f10|0x00000004|0x00000003|sign..fini_array")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000016|elf-dynamic-link-info|[0x0000000008049f14-0x0000000008049ffc)|rw-|0x00000f14|0x000000e8|0x00000003|sign..dynamic")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000017|regular|[0x0000000008049ffc-0x000000000804a000)|rw-|0x00000ffc|0x00000004|0x00000003|sign..got")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000018|regular|[0x000000000804a000-0x000000000804a010)|rw-|0x00001000|0x00000010|0x00000003|sign..got.plt")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000019|data|[0x000000000804a010-0x000000000804a014)|rw-|0x00001010|0x00000004|0x00000003|sign..data")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000001a|zero-fill|[0x000000000804a014-0x000000000804a018)|rw-|0x00001014|0x00000000|0x00000003|sign..bss")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000001b|regular| |---|0x00001014|0x0000002c|0x00000030|sign..comment")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000001c|dwarf-aranges| |---|0x00001040|0x00000020|0x00000000|sign..debug_aranges")) != nullptr);
	 rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000001d|dwarf-info| |---|0x00001060|0x0000033c|0x00000000|sign..debug_info")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000001e|dwarf-abbrev| |---|0x0000139c|0x000000ef|0x00000000|sign..debug_abbrev")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x0000001f|dwarf-line| |---|0x0000148b|0x000000bf|0x00000000|sign..debug_line")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000020|dwarf-str| |---|0x0000154a|0x000002dc|0x00000030|sign..debug_str")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000021|elf-symbol-table| |---|0x00001828|0x00000430|0x00000000|sign..symtab")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000022|regular| |---|0x00001c58|0x000001b9|0x00000000|sign..strtab")) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((section_record*)factory.make_vaccs_record(VACCS_SECTION))
			->add_info("|0x00000023|regular| |---|0x00001e11|0x00000145|0x00000000|sign..shstrtab")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("eax")
			->add_value(0xf7fb3d9cL)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("ebx")
			->add_value(0x00000000L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("ecx")
			->add_value(0x03f94b96L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("edx")
			->add_value(0xffffd074L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("edi")
			->add_value(0x00000000L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("esi")
			->add_value(0xf7fb2000L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("ebp")
			->add_value(0xffffd048L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("esp")
			->add_value(0xffffd040L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("eip")
			->add_value(0x080483dcL)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("eflags")
			->add_value(0x00000282L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("cs")
			->add_value(0x00000023L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("fs")
			->add_value(0x00000000L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("gs")
			->add_value(0x00000063L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("ss")
			->add_value(0x0000002bL)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("ds")
			->add_value(0x0000002bL)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(3)
			->add_event_num(0)
			->add_register_name("es")
			->add_value(0x0000002bL)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV))
			->add_event_num(0)
			->add_func_line_num(3)
			->add_inv_line_num(0)
			->add_c_func_file("sign.c")
			->add_c_inv_file("sign.c")
			->add_func_name("ld-linux.so.2`tunable_list")
			->add_address(0x0L)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV))
			->add_event_num(0)
			->add_func_line_num(3)
			->add_inv_line_num(0)
			->add_c_func_file("sign.c")
			->add_c_inv_file("sign.c")
			->add_func_name("sign`_start")
			->add_address(0x0L)) != nullptr);

	rec->write(fp);
	REQUIRE((rec = ((func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV))
			->add_event_num(0)
			->add_func_line_num(3)
			->add_inv_line_num(0)
			->add_c_func_file("sign.c")
			->add_c_inv_file("sign.c")
			->add_func_name("libc.so.6`__libc_start_main")
			->add_address(0x0L)) != nullptr);

	rec->write(fp);
	REQUIRE((rec = ((func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV))
			->add_event_num(0)
			->add_func_line_num(3)
			->add_inv_line_num(0)
			->add_c_func_file("sign.c")
			->add_c_inv_file("sign.c")
			->add_func_name("sign`main")
			->add_address(0xffffd048L)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((return_addr_record*)factory.make_vaccs_record(VACCS_RETURN_ADDR))
			->add_c_func_name("main")
			->add_dynamic_link(0x00000000L)
			->add_return_address(0xf7e242c3L)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS))
			->add_event_num(0)
			->add_c_line_num(3)
			->add_c_file_name("sign.c")
			->add_scope("main")
			->add_address(0xffffd044L)
			->add_type("int")
			->add_name("i")
			->add_points_to(0x0L)
			->add_value("-134537216")) != nullptr);

	rec->write(fp);

	REQUIRE((rec = ((var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS))
			->add_event_num(0)
			->add_c_line_num(3)
			->add_c_file_name("sign.c")
			->add_scope("main")
			->add_address(0xffffd042L)
			->add_type("unsigned short")
			->add_name("u")
			->add_points_to(0x0L)
			->add_value("63483")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(5)
			->add_event_num(1)
			->add_register_name("eip")
			->add_value(0x080483e3L)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((return_addr_record*)factory.make_vaccs_record(VACCS_RETURN_ADDR))
			->add_c_func_name("main")
			->add_dynamic_link(0x00000000L)
			->add_return_address(0xf7e242c3L)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS))
			->add_event_num(1)
			->add_c_line_num(5)
			->add_c_file_name("sign.c")
			->add_scope("main")
			->add_address(0xffffd044L)
			->add_type("int")
			->add_name("i")
			->add_points_to(0x0L)
			->add_value("3")) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(7)
			->add_event_num(2)
			->add_register_name("eax")
			->add_value(0x00000000L)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((register_record*)factory.make_vaccs_record(VACCS_REGISTER))
			->add_c_file_name("sign.c")
			->add_c_line_num(7)
			->add_event_num(2)
			->add_register_name("eip")
			->add_value(0x080483efL)) != nullptr);
	rec->write(fp);

	REQUIRE((rec = ((return_addr_record*)factory.make_vaccs_record(VACCS_RETURN_ADDR))
			->add_c_func_name("main")
			->add_dynamic_link(0x00000000L)
			->add_return_address(0xf7e242c3L)) != nullptr);
	rec->write(fp);
	REQUIRE((rec = ((var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS))
			->add_event_num(2)
			->add_c_line_num(7)
			->add_c_file_name("sign.c")
			->add_scope("main")
			->add_address(0xffffd042L)
			->add_type("unsigned short")
			->add_name("u")
			->add_points_to(0x0L)
			->add_value("65533")) != nullptr);
	rec->write(fp);

	REQUIRE(close_vaccs_file());

	REQUIRE(open_vaccs_file("input/sign-output.vaccs","r"));

	vaccs_record_reader *vrr= (new vaccs_record_reader())->add_factory(&factory)->add_fp(fp);

	rec = vrr->read_next_vaccs_record();
	REQUIRE(((section_record*)rec)->get_id() == VACCS_ARCH);
	REQUIRE(((arch_record *)rec)->get_arch_type() == VACCS_ARCH_X86_64);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_CMD_LINE);
	REQUIRE(strncmp(((cmd_line_record *)rec)->get_cmd_line(),"sign",PATH_MAX) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_BINARY);
	REQUIRE(strncmp(((binary_record*)rec)->get_bin_file_name(),"sign",PATH_MAX) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_CCODE);
	REQUIRE(((ccode_record*)rec)->get_c_line_num() == 1);
	REQUIRE(((ccode_record*)rec)->get_c_start_pos() == 1);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_src_line(),"#include <stdio.h>",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((ccode_record*)rec)->get_id() == VACCS_CCODE);
	REQUIRE(((ccode_record*)rec)->get_c_line_num() == 2);
	REQUIRE(((ccode_record*)rec)->get_c_start_pos() == 1);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_src_line(),"int main(){",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((ccode_record*)rec)->get_id() == VACCS_CCODE);
	REQUIRE(((ccode_record*)rec)->get_c_line_num() == 3);
	REQUIRE(((ccode_record*)rec)->get_c_start_pos() == 1);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_src_line(),"  int i = -3;",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((ccode_record*)rec)->get_id() == VACCS_CCODE);
	REQUIRE(((ccode_record*)rec)->get_c_line_num() == 4);
	REQUIRE(((ccode_record*)rec)->get_c_start_pos() == 1);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_src_line(),"  unsigned short u;",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((ccode_record*)rec)->get_id() == VACCS_CCODE);
	REQUIRE(((ccode_record*)rec)->get_c_line_num() == 5);
	REQUIRE(((ccode_record*)rec)->get_c_start_pos() == 1);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_src_line(),"  u=i;",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((ccode_record*)rec)->get_id() == VACCS_CCODE);
	REQUIRE(((ccode_record*)rec)->get_c_line_num() == 6);
	REQUIRE(((ccode_record*)rec)->get_c_start_pos() == 1);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_src_line()," ",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((ccode_record*)rec)->get_id() == VACCS_CCODE);
	REQUIRE(((ccode_record*)rec)->get_c_line_num() == 7);
	REQUIRE(((ccode_record*)rec)->get_c_start_pos() == 1);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((ccode_record*)rec)->get_c_src_line(),"}",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 1);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"0x80483dc <+6>: movl   $0xfffffffd, -0x4(%ebp)   ; imm = 0xFFFFFFFD",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 2);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 5);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst()," ",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 3);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 5);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"sign`main:",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 4);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 5);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"0x80483e3 <+13>: movl   -0x4(%ebp), %eax",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 5);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 5);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"0x80483e6 <+16>: movw   %ax, -0x6(%ebp)",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 6);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 5);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"0x80483ea <+20>: movl   $0x0, %eax",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 7);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 7);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"6",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 8);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 7);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"7   	}",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 9);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 7);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst()," ",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 10);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 7);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"sign`main:",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 11);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 7);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"0x80483ef <+25>: leave",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((asm_record*)rec)->get_id() == VACCS_ASM);
	REQUIRE(((asm_record*)rec)->get_asm_line_num() == 12);
	REQUIRE(((asm_record*)rec)->get_c_line_num() == 7);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_file_name(),"sign.s",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((asm_record*)rec)->get_asm_inst(),"0x80483f0 <+26>: retl",VACCS_MAX_SRC_LINE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|SectID|Type|File Address|Per|File Off.|File Size|Flags|Section Name",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000001|regular| |---|0x00000000|0x00000000|0x00000000|sign.",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000002|regular|[0x0000000008048154-0x0000000008048167)|r--|0x00000154|0x00000013|0x00000002|sign..interp",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000003|regular|[0x0000000008048168-0x0000000008048188)|r--|0x00000168|0x00000020|0x00000002|sign..note.ABI-tag",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000004|regular|[0x0000000008048188-0x00000000080481ac)|r--|0x00000188|0x00000024|0x00000002|sign..note.gnu.build-id",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000005|regular|[0x00000000080481ac-0x00000000080481cc)|r--|0x000001ac|0x00000020|0x00000002|sign..gnu.hash",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000006|elf-dynamic-symbols|[0x00000000080481cc-0x000000000804820c)|r--|0x000001cc|0x00000040|0x00000002|sign..dynsym",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000007|regular|[0x000000000804820c-0x0000000008048251)|r--|0x0000020c|0x00000045|0x00000002|sign..dynstr",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000008|regular|[0x0000000008048252-0x000000000804825a)|r--|0x00000252|0x00000008|0x00000002|sign..gnu.version",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000009|regular|[0x000000000804825c-0x000000000804827c)|r--|0x0000025c|0x00000020|0x00000002|sign..gnu.version_r",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000000a|elf-relocation-entries|[0x000000000804827c-0x0000000008048284)|r--|0x0000027c|0x00000008|0x00000002|sign..rel.dyn",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000000b|elf-relocation-entries|[0x0000000008048284-0x000000000804828c)|r--|0x00000284|0x00000008|0x00000042|sign..rel.plt",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000000c|regular|[0x000000000804828c-0x00000000080482af)|r-x|0x0000028c|0x00000023|0x00000006|sign..init",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000000d|regular|[0x00000000080482b0-0x00000000080482d0)|r-x|0x000002b0|0x00000020|0x00000006|sign..plt",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000000e|regular|[0x00000000080482d0-0x00000000080482d8)|r-x|0x000002d0|0x00000008|0x00000006|sign..plt.got",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000000f|code|[0x00000000080482e0-0x0000000008048462)|r-x|0x000002e0|0x00000182|0x00000006|sign..text",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000010|regular|[0x0000000008048464-0x0000000008048478)|r-x|0x00000464|0x00000014|0x00000006|sign..fini",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000011|regular|[0x0000000008048478-0x0000000008048484)|r--|0x00000478|0x0000000c|0x00000002|sign..rodata",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000012|regular|[0x0000000008048484-0x00000000080484b0)|r--|0x00000484|0x0000002c|0x00000002|sign..eh_frame_hdr",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000013|eh-frame|[0x00000000080484b0-0x0000000008048570)|r--|0x000004b0|0x000000c0|0x00000002|sign..eh_frame",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000014|regular|[0x0000000008049f0c-0x0000000008049f10)|rw-|0x00000f0c|0x00000004|0x00000003|sign..init_array",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000015|regular|[0x0000000008049f10-0x0000000008049f14)|rw-|0x00000f10|0x00000004|0x00000003|sign..fini_array",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000016|elf-dynamic-link-info|[0x0000000008049f14-0x0000000008049ffc)|rw-|0x00000f14|0x000000e8|0x00000003|sign..dynamic",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000017|regular|[0x0000000008049ffc-0x000000000804a000)|rw-|0x00000ffc|0x00000004|0x00000003|sign..got",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000018|regular|[0x000000000804a000-0x000000000804a010)|rw-|0x00001000|0x00000010|0x00000003|sign..got.plt",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000019|data|[0x000000000804a010-0x000000000804a014)|rw-|0x00001010|0x00000004|0x00000003|sign..data",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000001a|zero-fill|[0x000000000804a014-0x000000000804a018)|rw-|0x00001014|0x00000000|0x00000003|sign..bss",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000001b|regular| |---|0x00001014|0x0000002c|0x00000030|sign..comment",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000001c|dwarf-aranges| |---|0x00001040|0x00000020|0x00000000|sign..debug_aranges",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000001d|dwarf-info| |---|0x00001060|0x0000033c|0x00000000|sign..debug_info",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000001e|dwarf-abbrev| |---|0x0000139c|0x000000ef|0x00000000|sign..debug_abbrev",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x0000001f|dwarf-line| |---|0x0000148b|0x000000bf|0x00000000|sign..debug_line",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000020|dwarf-str| |---|0x0000154a|0x000002dc|0x00000030|sign..debug_str",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000021|elf-symbol-table| |---|0x00001828|0x00000430|0x00000000|sign..symtab",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000022|regular| |---|0x00001c58|0x000001b9|0x00000000|sign..strtab",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((section_record*)rec)->get_id() == VACCS_SECTION);
	REQUIRE(strncmp(((section_record*)rec)->get_info(),"|0x00000023|regular| |---|0x00001e11|0x00000145|0x00000000|sign..shstrtab",VACCS_MAX_OUTPUT_LENGTH) == 0);


	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"eax",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0xf7fb3d9cL);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"ebx",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x00000000L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"ecx",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x03f94b96L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"edx",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0xffffd074L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"edi",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x00000000L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"esi",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0xf7fb2000L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"ebp",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0xffffd048L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"esp",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0xffffd040L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"eip",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x080483dcL);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"eflags",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x00000282L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"cs",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x00000023L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"fs",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x00000000L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"gs",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x00000063L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"ss",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x0000002bL);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"ds",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x0000002bL);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 0);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"es",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x0000002bL);


	rec = vrr->read_next_vaccs_record(); REQUIRE(((func_inv_record*)rec)->get_id() == VACCS_FUNCTION_INV);
	REQUIRE(((func_inv_record*)rec)->get_event_num() == 0);
	REQUIRE(((func_inv_record*)rec)->get_func_line_num() == 3);
	REQUIRE(((func_inv_record*)rec)->get_inv_line_num() == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_c_func_file(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_c_inv_file(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_func_name(),"ld-linux.so.2`tunable_list",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((func_inv_record*)rec)->get_address() == 0x0L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((func_inv_record*)rec)->get_id() == VACCS_FUNCTION_INV);
	REQUIRE(((func_inv_record*)rec)->get_event_num() == 0);
	REQUIRE(((func_inv_record*)rec)->get_func_line_num() == 3);
	REQUIRE(((func_inv_record*)rec)->get_inv_line_num() == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_c_func_file(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_c_inv_file(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_func_name(),"sign`_start",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((func_inv_record*)rec)->get_address() == 0x0L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((func_inv_record*)rec)->get_id() == VACCS_FUNCTION_INV);
	REQUIRE(((func_inv_record*)rec)->get_event_num() == 0);
	REQUIRE(((func_inv_record*)rec)->get_func_line_num() == 3);
	REQUIRE(((func_inv_record*)rec)->get_inv_line_num() == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_c_func_file(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_c_inv_file(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_func_name(),"libc.so.6`__libc_start_main",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((func_inv_record*)rec)->get_address() == 0x0L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((func_inv_record*)rec)->get_id() == VACCS_FUNCTION_INV);
	REQUIRE(((func_inv_record*)rec)->get_event_num() == 0);
	REQUIRE(((func_inv_record*)rec)->get_func_line_num() == 3);
	REQUIRE(((func_inv_record*)rec)->get_inv_line_num() == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_c_func_file(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_c_inv_file(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((func_inv_record*)rec)->get_func_name(),"sign`main",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((func_inv_record*)rec)->get_address() == 0xffffd048L);


	rec = vrr->read_next_vaccs_record(); REQUIRE(((return_addr_record*)rec)->get_id() == VACCS_RETURN_ADDR);
	REQUIRE(((return_addr_record*)rec)->get_dynamic_link() == 0x00000000L);
	REQUIRE(((return_addr_record*)rec)->get_return_address() ==0xf7e242c3L);
	REQUIRE(strncmp(((return_addr_record*)rec)->get_c_func_name(),"main",VACCS_MAX_VARIABLE_LENGTH) == 0);


	rec = vrr->read_next_vaccs_record(); REQUIRE(((var_access_record*)rec)->get_id() == VACCS_VAR_ACCESS);
	REQUIRE(((var_access_record*)rec)->get_event_num() == 0);
	REQUIRE(((var_access_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((var_access_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((var_access_record*)rec)->get_scope(),"main",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_address() == 0xffffd044L);
	REQUIRE(strncmp(((var_access_record*)rec)->get_type(),"int",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(((var_access_record*)rec)->get_name(),"i",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_points_to() == 0x0L);
	REQUIRE(strncmp(((var_access_record*)rec)->get_value(),"-134537216",VACCS_MAX_OUTPUT_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((var_access_record*)rec)->get_id() == VACCS_VAR_ACCESS);
	REQUIRE(((var_access_record*)rec)->get_event_num() == 0);
	REQUIRE(((var_access_record*)rec)->get_c_line_num() == 3);
	REQUIRE(strncmp(((var_access_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((var_access_record*)rec)->get_scope(),"main",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_address() == 0xffffd042L);
	REQUIRE(strncmp(((var_access_record*)rec)->get_type(),"unsigned short",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(((var_access_record*)rec)->get_name(),"u",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_points_to() == 0x0L);
	REQUIRE(strncmp(((var_access_record*)rec)->get_value(),"63483",VACCS_MAX_OUTPUT_LENGTH) == 0);


	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 1);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 5);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"eip",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x080483e3L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((return_addr_record*)rec)->get_id() == VACCS_RETURN_ADDR);
	REQUIRE(((return_addr_record*)rec)->get_dynamic_link() == 0x00000000L);
	REQUIRE(((return_addr_record*)rec)->get_return_address() ==0xf7e242c3L);
	REQUIRE(strncmp(((return_addr_record*)rec)->get_c_func_name(),"main",VACCS_MAX_VARIABLE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((var_access_record*)rec)->get_id() == VACCS_VAR_ACCESS);
	REQUIRE(((var_access_record*)rec)->get_event_num() == 1);
	REQUIRE(((var_access_record*)rec)->get_c_line_num() == 5);
	REQUIRE(strncmp(((var_access_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((var_access_record*)rec)->get_scope(),"main",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_address() == 0xffffd044L);
	REQUIRE(strncmp(((var_access_record*)rec)->get_type(),"int",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(((var_access_record*)rec)->get_name(),"i",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_points_to() == 0x0L);
	REQUIRE(strncmp(((var_access_record*)rec)->get_value(),"3",VACCS_MAX_OUTPUT_LENGTH) == 0);


	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 2);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 7);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"eax",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x00000000L);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((register_record*)rec)->get_id() == VACCS_REGISTER);
	REQUIRE(((register_record*)rec)->get_event_num() == 2);
	REQUIRE(((register_record*)rec)->get_c_line_num() == 7);
	REQUIRE(strncmp(((register_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((register_record*)rec)->get_register_name(),"eip",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((register_record*)rec)->get_value() == 0x080483efL);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((return_addr_record*)rec)->get_id() == VACCS_RETURN_ADDR);
	REQUIRE(((return_addr_record*)rec)->get_dynamic_link() == 0x00000000L);
	REQUIRE(((return_addr_record*)rec)->get_return_address() ==0xf7e242c3L);
	REQUIRE(strncmp(((return_addr_record*)rec)->get_c_func_name(),"main",VACCS_MAX_VARIABLE_LENGTH) == 0);

	rec = vrr->read_next_vaccs_record(); REQUIRE(((var_access_record*)rec)->get_id() == VACCS_VAR_ACCESS);
	REQUIRE(((var_access_record*)rec)->get_event_num() == 2);
	REQUIRE(((var_access_record*)rec)->get_c_line_num() == 7);
	REQUIRE(strncmp(((var_access_record*)rec)->get_c_file_name(),"sign.c",PATH_MAX) == 0);
	REQUIRE(strncmp(((var_access_record*)rec)->get_scope(),"main",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_address() == 0xffffd042L);
	REQUIRE(strncmp(((var_access_record*)rec)->get_type(),"unsigned short",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(strncmp(((var_access_record*)rec)->get_name(),"u",VACCS_MAX_VARIABLE_LENGTH) == 0);
	REQUIRE(((var_access_record*)rec)->get_points_to() == 0x0L);
	REQUIRE(strncmp(((var_access_record*)rec)->get_value(),"65533",VACCS_MAX_OUTPUT_LENGTH) == 0);

	REQUIRE(close_vaccs_file());
}


