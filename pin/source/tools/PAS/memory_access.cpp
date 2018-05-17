/*
 * memory_access.cpp
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#include "memory_access.h"
#include <iostream>
#include  <iomanip>
#include <fstream>
#include "global.h"
/* ===================================================================== */
/* Instrumentation the memory operation to record the memory accesses     */
/* ===================================================================== */
std::ofstream* output;
char* get_address_dereference(char* string){
	int length = strlen(string);
	char* output = (char*) malloc(length+1);

	int start = -1;
	int end = -1;

	for(int i=0;i<length;i++){
		if(string[i]=='['){
			start = i;
		}

		else if(string[i] == ']'){
			end = i;
			break;
		}

	}
	if(start == -1 || end == -1) return 0;
	strncpy(output,string+start+1,end-start-1);
	return output;
}
bool is_indirect(char* assembly){
	if(get_address_dereference(assembly)!=0){
		if(strchr(get_address_dereference(assembly),'*')!=NULL) return true;
	}
	return false;
}
// Print a memory read record
ADDRINT current_EBP;
PIN_LOCK lock;
VOID BeforeMemRead(VOID* assembly,ADDRINT ip, VOID * addr,const CONTEXT * ctxt,UINT32 size)
{
	char* assembly_code = (char*) assembly;
	assembly_code = process_string_for_csv(assembly_code);
    std::cout<<hex<<ip<<": R "<<addr<<" "<<dec<<size<<std::endl;
    int id = timestamp++;
    get_registers(ctxt,id);
	INT32 * 	column = (INT32*) malloc(sizeof(INT32));
	INT32 * 	line = (INT32*) malloc(sizeof(INT32));
	std::string 	fileName;

	PIN_LockClient();
	PIN_GetSourceLocation(ip,column,line,&fileName);
	PIN_UnlockClient();
    printf("\n");
    int invocation_id = invocation_stack.top().id;
    std::cout<<"line number "<<dec<<*line<<std::endl;
    current_EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
    char* location = (char*) malloc(size);
    printf("value before read :");
    if(is_indirect(assembly_code)) {
    	 ou_read_indirect<<"line:"<<dec<<*line<<",\t id:"<<id<<","<<assembly_code<<","<<hex<<addr<<","<<dec<<size<<",";
    	 pas_output<<"pin_pointer_access~!~"<<dec<<*line<<"|"<<id<<"|"<<assembly_code<<"|"<<hex<<addr<<"|"<<dec<<size<<"|";
    	 for(unsigned int i=0;i<size;i++){
    	         	location[i] = ((char*)addr)[i];
    	         	printf("%x ",location[i] & 0xff);
    	         	ou_read_indirect<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);

    	 }
    	 for(int i= size-1;i>=0;i--){
    	 			// pas_output<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
    	 		 }
    	 ou_read_indirect<<","<<dec<<invocation_id<<","<<hex<<current_EBP<<std::endl;
    	 pas_output<<"|"<<dec<<invocation_id<<"|"<<hex<<current_EBP<<std::endl;
    }
    else{
    	ou_read_direct<<"line:"<<dec<<*line<<",\t id:"<<id<<","<<assembly_code<<","<<hex<<addr<<","<<dec<<size<<",";
    	pas_output<<"pin_variable_access~!~"<<dec<<*line<<"|"<<id<<"|"<<assembly_code<<"|"<<hex<<addr<<"|"<<dec<<size<<"|";
    	for(unsigned int i=0;i<size;i++){
    	    	         	location[i] = ((char*)addr)[i];
    	    	         	printf("%x ",location[i] & 0xff);
    	    	         	ou_read_direct<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
    	 }
    	for(int i= size-1;i>=0;i--){
    		// pas_output<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
    	}
    	 ou_read_direct<<","<<dec<<invocation_id<<","<<hex<<current_EBP<<std::endl;
    	 pas_output<<"|"<<dec<<invocation_id<<"|"<<hex<<current_EBP<<std::endl;
    }


    print_global_context();


}
VOID AfterMemRead(VOID * ip, VOID * addr,const CONTEXT * ctxt,UINT32 size)
{

    char* location = (char*) malloc(size);
    printf("value after read :");
    for(unsigned int i=0;i<size;i++){
    	location[i] = ((char*)addr)[i];
    	printf("%x ",location[i] & 0xff);
    }
    printf("\n");


}

// Print a memory write record
VOID BeforeMemWrite(VOID* assembly,ADDRINT  ip, VOID * addr,const CONTEXT * ctxt, UINT32 size)
{
	if(size==0) pas_output<<"error here\n";
	INT32 * 	column = (INT32*) malloc(sizeof(INT32));
	INT32 * 	line = (INT32*) malloc(sizeof(INT32));
	std::string 	fileName;

	PIN_LockClient();
	PIN_GetSourceLocation(ip,column,line,&fileName);
	PIN_UnlockClient();
	char* assembly_code = (char*) assembly;
	assembly_code = process_string_for_csv(assembly_code);

	std::cout<<hex<<ip<<": W "<<addr<<" "<<dec<<size<<std::endl;
    int id = timestamp++;
    get_registers(ctxt,id);
    current_EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
    if(is_indirect(assembly_code)) {
    	ou_write_indirect<<"line:"<<dec<<*line<<",\t id:"<<id<<",\t"<<assembly_code<<",\t"<<hex<<addr<<",\t"<<dec<<size<<",\t";
    	pas_output<<"pin_pointer_access~!~"<<dec<<*line<<"|"<<id<<"|"<<assembly_code<<"|"<<hex<<addr<<"|"<<dec<<size<<"|";
    	char* location = (char*) malloc(size);
    	printf("value before write :");
    	for(unsigned int i=0;i<size;i++){
    		location[i] = ((char*)addr)[i];
    		printf("%x ",location[i] & 0xff);
    		ou_write_indirect<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
    	}
    	for(int i= size-1;i>=0;i--){
    		 pas_output<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
    	}
    	ou_write_indirect<<",\t";
    	pas_output<<"|";
    	printf("\n");
    }
    else{
    	ou_write_direct<<"line:"<<dec<<*line<<",\t id:"<<id<<",\t"<<assembly_code<<",\t"<<hex<<addr<<",\t"<<dec<<size<<",\t";
    	pas_output<<"pin_variable_access~!~"<<dec<<*line<<"|"<<id<<"|"<<assembly_code<<"|"<<hex<<addr<<"|"<<dec<<size<<"|";
    	char* location = (char*) malloc(size);
    	printf("value before write :");
    	for(unsigned int i=0;i<size;i++){
    	   		location[i] = ((char*)addr)[i];
    	   		printf("%x ",location[i] & 0xff);
    	   		ou_write_direct<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
    	}
    	for(int i= size-1;i>=0;i--){
    		 pas_output<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
    	}
    	ou_write_direct<<",\t";
    	pas_output<<"|";
    	printf("\n");
    }


}
VOID AfterMemWrite(VOID* assembly,VOID * ip, VOID * addr,const CONTEXT * ctxt, UINT32 size)
{
	 int invocation_id = invocation_stack.top().id;
	 char* assembly_code = (char*) assembly;
	char* location = (char*) malloc(size);
	current_EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
	printf("value after write :");
	 if(is_indirect(assembly_code)) {
		 for(unsigned int i=0;i<size;i++){
		 	    	location[i] = ((char*)addr)[i];
		 	    	printf("%x ",location[i] & 0xff);
		 	    	ou_write_indirect<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
		 }
		 for(int i= size-1;i>=0;i--){
		 	 pas_output<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
		 }
		 printf("\n");
		 ou_write_indirect<<","<<dec<<invocation_id<<",\t"<<hex<<current_EBP<<std::endl;
		 pas_output<<"|"<<dec<<invocation_id<<"|"<<hex<<current_EBP;
	 }
	 else{
		 for(unsigned int i=0;i<size;i++){
		 		 	    	location[i] = ((char*)addr)[i];
		 		 	    	printf("%x ",location[i] & 0xff);
		 		 	    	ou_write_direct<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);

		 }
		 for(int i= size-1;i>=0;i--){
			 pas_output<< setfill('0') << setw(2) <<hex<<(location[i] & 0xff);
		 }
		 printf("\n");
		 ou_write_direct<<","<<dec<<invocation_id<<",\t"<<hex<<current_EBP<<std::endl;
		 pas_output<<"|"<<dec<<invocation_id<<"|"<<hex<<current_EBP;
	 }

	 pas_output<<"\n";

}

// Is called for every instruction and instruments reads and writes
VOID MemoryAccessInstruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
    // prefixed instructions appear as predicated instructions in Pin.

				UINT32 memOperands = INS_MemoryOperandCount(ins);
				char* assembly_code = (char*) malloc(strlen(INS_Disassemble(ins).c_str())+1);
				strcpy(assembly_code,INS_Disassemble(ins).c_str());
				// Iterate over each memory operand of the instruction.
				for (UINT32 memOp = 0; memOp < memOperands; memOp++)
				{
					if (INS_MemoryOperandIsRead(ins, memOp))
					{

				        INS_InsertPredicatedCall(
				                ins, IPOINT_BEFORE, (AFUNPTR)BeforeMemRead,
				                IARG_PTR,assembly_code,
				                IARG_INST_PTR,
				                IARG_MEMORYOP_EA, memOp	,
								IARG_CONTEXT,
								IARG_MEMORYREAD_SIZE,
				                IARG_END);
//				        IPOINT where = IPOINT_AFTER;
//				        if (!INS_HasFallThrough(ins))
//				        	where = IPOINT_TAKEN_BRANCH;
//				        INS_InsertPredicatedCall(
//				        				                ins, where, (AFUNPTR)AfterMemRead,
//				        				                IARG_INST_PTR,
//				        				                IARG_MEMORYOP_EA, memOp	,IARG_CONTEXT,IARG_MEMORYREAD_SIZE,
//				        				                IARG_END);

				        }

				        // Note that in some architectures a single memory operand can be
				        // both read and written (for instance incl (%eax) on IA-32)
				        // In that case we instrument it once for read and once for write.
				        if (INS_MemoryOperandIsWritten(ins, memOp))
				        {
				            INS_InsertPredicatedCall(
				                ins, IPOINT_BEFORE, (AFUNPTR)BeforeMemWrite,
				                IARG_PTR,assembly_code,
				                IARG_INST_PTR,
				                IARG_MEMORYOP_EA, memOp,
								IARG_CONTEXT,
								IARG_MEMORYWRITE_SIZE,
				                IARG_END);
				            IPOINT where = IPOINT_AFTER;
				            if (!INS_HasFallThrough(ins))
				                        where = IPOINT_TAKEN_BRANCH;
				            INS_InsertPredicatedCall(
				            				                ins, where, (AFUNPTR)AfterMemWrite,
															IARG_PTR,assembly_code,
				            				                IARG_INST_PTR,
				            				                IARG_MEMORYOP_EA,
															memOp,IARG_CONTEXT,
															IARG_MEMORYWRITE_SIZE,
				            				                IARG_END);
				        }
				    }
}
