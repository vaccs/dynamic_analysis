/*
 * This file is the source code to do dynamic analysis for the project VACCS
 * It provides data for the address space visualization.
 */
#include <stdio.h>
#include "pin.H"
#include <sys/stat.h>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include "return.h"
#include "malloc_free.h"
#include "InstructionInstrumentation.h"
#include "global.h"
#include "functionInvocation.h"
#include <vaccs_read/vaccs_dw_reader.h>

/*call stack*/
std::stack<function_invocation_transaction> invocation_stack;
/*file output variable*/
std::ofstream ou_function_invocation;
std::ofstream ou_return;
std::ofstream ou_malloc;
std::ofstream ou_free;
std::ofstream ou_push;
std::ofstream ou_pop;
std::ofstream ou_read_direct;
std::ofstream ou_write_direct;
std::ofstream ou_read_indirect;
std::ofstream ou_write_indirect;
std::ofstream ou_register;
std::ofstream pas_output;


/*
 * global variables CHANFGEING
 */

std::vector<std::string> assembly_codes;


char* current_function_name;
int current_invocation_id;
int timestamp;
push_transaction push_event;
pop_transaction pop_event;
malloc_transaction malloc_event;
return_transaction return_event;








/*
 *
 *
 *
 *
 *
 */
//MYSQL *con;



/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

#include <unistd.h>
/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
void setup_output_files(char* filename){
	std::string filename_string = std::string(filename);
	int index = -1;
	for(unsigned int i=0;i<filename_string.size();i++){
		if(filename[i]=='/'){
			index = i;
		}
	}
	std::string application_name;
	if(index == -1){
		application_name = string(filename_string);
	}
	else{
		application_name = filename_string.substr(index+1,filename_string.size()-index-1);
	}


	application_name.append(".csv");
	cout<<application_name<<std::endl;
	pas_output.open(application_name.c_str());
	ou_function_invocation.open("function_invocation.csv");
	ou_malloc.open("malloc.csv");
	ou_free.open("free.csv");
	ou_push.open("push.csv");
	ou_pop.open("pop.csv");
	ou_read_direct.open("read_direct.csv");
	ou_write_direct.open("write_direct.csv");
	ou_read_indirect.open("read_indirect.csv");
	ou_write_indirect.open("write_indirect.csv");
	ou_return.open("return.csv");
	ou_register.open("register.csv");

}
void finalize_outputfiles(){
	printf("saving files\n");
	ou_function_invocation.close();
	ou_malloc.close();
	ou_free.close();
	ou_push.close();
	ou_pop.close();
	ou_read_direct.close();
	ou_write_direct.close();
	ou_read_indirect.close();
	ou_write_indirect.close();
	ou_return.close();
	ou_register.close();
	pas_output.close();
}
void initialize(){

		 /*
		  * Initialize Global Variables
		  */
		current_EBP = 0;
		timestamp = 0;
		current_function_name = 0;
		current_invocation_id =0;
		//file output initialization

}


VOID Fini(INT32 code, VOID *v)
{

	finalize_outputfiles();
//	while(!invocation_stack.empty()){
//		std::cout<<invocation_stack.top().function_name<<std::endl;
//		invocation_stack.pop();
//	}
}
int main(int argc, char *argv[])
{


	 /*
	  * Pin Initialization
	  */
	initialize();
	setup_output_files(argv[6]);

   /*
    * read dwarf information from executable
    */
   std::string vfn(argv[6]);
   vfn.append(".vdw");
	vaccs_dw_reader *vdr;
	vdr = new vaccs_dw_reader();
	vdr->add_file_name(vfn);

	vdr->read_vaccs_dw_info();
 
	PIN_InitSymbols();
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    IMG_AddInstrumentFunction(FunctionInvocatioinImage,0);

    IMG_AddInstrumentFunction(InstructionInstrumentation,0);
	IMG_AddInstrumentFunction(ReturnImage, 0);

    IMG_AddInstrumentFunction(MallocAndFreeImage, 0);



    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}
