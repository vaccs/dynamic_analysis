#ifndef VACCS_ERROR_MESSAGE_H
#define VACCS_ERROR_MESSAGE_H

#include <string>
#include "pin.H"
#include <iostream>

using namespace std;

#define DELIMITER ":"
#define VACCS_ERROR_LEVEL_WARNING "warning"
#define VACCS_ERROR_LEVEL_ERROR "error"
#define VACCS_ERROR_LEVEL_INFO "info"

#define ACCESS_FAULT_ID "accessFault"
#define ACCESS_MSG "invalid memory access"
#define ILL_FAULT_ID "illFault"
#define ILL_MSG "illegal instruction"
#define STK_FAULT_ID "stkFault"
#define STK_MSG "corrupted stack detected"
#define RBP_FAULT_ID "rbpFault"
#define RBP_MSG "stack smashing of old frame pointer detected"
#define RA_FAULT_ID "raFault"
#define RA_MSG "stack smashing of return address detected"
#define PROT_FAULT_ID "protFault"
#define PROT_MSG "memory protection violation"
#define PAGE_FAULT_ID "pageFault"
#define PAGE_MSG "invalid memory page"
#define ALIGN_FAULT_ID "alignFault"
#define ALIGN_MSG "invalid memory page"
#define PRIV_FAULT_ID "privFault"
#define PRIV_MSG "invalid memory page"
#define DIV_FAULT_ID "divFault"
#define DIV_MSG "integer divide by zero"
#define OVER_FAULT_ID "overFault"
#define OVER_MSG "integer overflow"
#define BND_FAUlT_ID "bndFault"
#define BND_MSG "array out-of-bounds"
#define FDIV_FAULT_ID "fdivFault" 
#define FDIV_MSG "floating-point divide by zero"
#define FOVER_FAULT_ID "foverFault"
#define FOVER_MSG "floating-point overflow"
#define FUNDR_FAULT_ID "fundrFault"
#define FUNDR_MSG "floating-point underflow"
#define FEXT_FAULT_ID "fextFault"
#define FEXT_MSG "floating-point inexact result"
#define FINV_FAULT_ID "finvFault"
#define FINV_MSG "floating-point invalid operation"
#define FNRM_FAULT_ID "fnrmFault"
#define FNRM_MSG "floating-point denormalized operand"
#define FSTK_FAULT_ID "fstkFault"
#define FSTK_MSG "floating-point stack error"
#define FUNK_FAULT_ID "funkFault"
#define FUNK_MSG "floating-point unknown error"
#define UNK_FAULT_ID "unkFault"
#define UNK_MSG "unknow error"
#define DOUBLE_FREE_ID "doubleFree"
#define DOUBLE_FREE_MSG "attempt to free a block multiple times"
#define INV_FREE_ID "invalidFree"
#define INV_FREE_MSG "attempt to free an invalid block"
#define GARBAGE_BLOCK_ID "garbageBlock"
#define GARBAGE_BLOCK_MSG "allocated block never freed"
#define DANGLING_REF_ID "danglingPointer"
#define DANGLING_REF_MSG "pointer to freed block"

class vaccs_error_message
{
private:
	string file_name;
	int line;
	string severity;
	string message;
	string id;

public:
	vaccs_error_message();
	~vaccs_error_message();

	string get_file_name() const
	{
		return file_name;
	}

	vaccs_error_message* add_file_name(string file_name)
	{
		this->file_name = file_name;
		return this;
	}

	int get_line() const
	{
		return line;
	}

	vaccs_error_message* add_line(int line)
	{
		this->line = line;
		return this;
	}

	string get_severity() const
	{
		return severity;
	}

	vaccs_error_message* add_severity(string severity)
	{
		this->severity = severity;
		return this;
	}

	string get_message() const
	{
		return message;
	}

	vaccs_error_message* add_message(string message)
	{
		this->message = message;
		return this;
	}

	string get_id() const
	{
		return id;
	}

	vaccs_error_message* add_id(string id)
	{
		this->id = id;
		return this;
	}

	string get_vaccs_formatted_error_message() {
		return file_name + DELIMITER + decstr(line) + DELIMITER + message + DELIMITER + " (" + id + ") ";
	}

	void emit_vaccs_formatted_error_message() {
		cerr << get_vaccs_formatted_error_message() << "\n";
	}
};

#endif
