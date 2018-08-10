/*
 * global.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#ifndef GLOBAL_H_
#define GLOBAL_H_

#include<stack>
#include<string>
#include<set>
#include<iostream>
#include "pin.H"
#include <vaccs_read/vaccs_dw_reader.h>

#define NOCSOURCE "__NOCSOURCE__"
#define NOASMSOURCE "__NOASMSOURCE__"
#define NOFUNCNAME "__NOFUNCTIONNAME__"

#define OLD_FRAME_PTR_OFFSET 0
#ifdef __x86_64
#define RETURN_ADDRESS_OFFSET 8
#else
#define RETURN_ADDRESS_OFFSET 4
#endif 


ADDRINT read_memory_as_address(ADDRINT addr); 

// file output variables

//extern std::ofstream ou_function_invocation;
//extern std::ofstream ou_malloc;
//extern std::ofstream ou_free;
//extern std::ofstream ou_push;
//extern std::ofstream ou_pop;
//extern std::ofstream ou_read_direct;
//extern std::ofstream ou_write_direct;
//extern std::ofstream ou_read_indirect;
//extern std::ofstream ou_write_indirect;
//extern std::ofstream ou_return;
//extern std::ofstream ou_register;
//extern std::ofstream pas_output;




extern ADDRINT current_EBP;
extern char* current_function_name;
extern int current_invocation_id;
extern int timestamp;


bool Is_System_Image(IMG img);
void print_global_context();
char* process_string_for_csv(char* output_string);
typedef uint64_t memory_address;
/*
 *
 *
 *
 */
typedef void* raw_data;
/*
 *
 *
 *
 *
 */
typedef struct{
	int id;
	memory_address frame_pointer;
	int id_function_invocation_happened_in;
	memory_address heap_address;
	int deallocated_size;
}free_transaction;

extern void get_registers(const CONTEXT * ctxt,int timestamp);
/*
 *
 *
 *
 *
 *
 */


typedef struct{
	int id;
	memory_address frame_pointer;
	int id_function_invocation_happened_in;
	memory_address heap_address;
	int allocated_size;

}malloc_transaction;

/*
 *
 *
 *
 *
 *
 */

typedef struct{
	int id;
	memory_address frame_pointer;
	int id_function_invocation_happened_in;
	memory_address new_ebp_value;


}EBP_transaction;

/*
 *
 *
 *
 *
 *
 *
 */
typedef struct{
	int id;
	memory_address frame_pointer;
	int id_function_invocation_happened_in;
	memory_address new_esp_value;


}ESP_transaction;

/*
 *
 *
 *
 *
 */

typedef struct{
	int id;
	memory_address frame_pointer;
	int id_function_invocation_happened_in;
	memory_address sp_before;
	memory_address sp_after;

}push_transaction;

/*
 *
 *
 *
 *
 *
 */

typedef struct{
	int id;
	memory_address frame_pointer;
	int id_function_invocation_happened_in;
	memory_address sp_before;
	memory_address sp_after;
}pop_transaction;

/*
 *
 *
 *
 *
 *
 *
 */
typedef struct{
	int id;
	char* function_name;
	memory_address frame_pointer;
}function_invocation_transaction;

/*
 *
 *
 *
 *
 *
 */


typedef struct{
	int id;
	char* program_name;
	memory_address initial_address;


}program_invocation_transaction;


/*
 *
 *
 *
 *
 *
 *
 */

typedef struct{
	int id;
	char* program_name;
	int exit_status;
	int line_number;

}program_exit_transaction;




typedef struct{
	int id;
	memory_address frame_pointer;
	int id_function_invocation_happened_in;
	memory_address sp_before;
	memory_address sp_after;

}return_transaction;

/*
 *
 *
 *
 *
 *
 */


typedef struct{
	int id;
	memory_address frame_pointer;
	int id_function_invocation_happened_in;
	memory_address address_accessed;
	int target_size;
	raw_data new_value;
	raw_data current_value;

}directed_variable_access_transaction;

/*
 *
 *
 *
 *
 */
typedef struct{
	int id;
	memory_address frame_pointer;
	int id_function_invocation_happened_in;
	memory_address address_accessed;
	int target_size;
	raw_data new_value;
	raw_data current_value;



}indirected_variable_access_transaction;


extern push_transaction push_event;
extern pop_transaction pop_event;
extern malloc_transaction malloc_event;
extern return_transaction return_event;
extern std::stack<function_invocation_transaction> invocation_stack;
extern function_invocation_transaction function_invocation_event;
extern std::vector<std::string> assembly_codes;
extern vaccs_dw_reader *vdr;
extern NATIVE_FD vaccs_fd;
using namespace std;

#endif /* GLOBAL_H_ */
