/*
 * This file is the source code to do dynamic analysis for the project VACCS
 * It provides data for the address space visualization.
 */
#include <stdio.h>
#include <sys/utsname.h>
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
#include <io/vaccs_record_factory.h>
#include <io/arch_record.h>
#include <io/binary_record.h>
#include <io/ccode_record.h>
#include <io/cmd_line_record.h>

vaccs_dw_reader *vdr;
FILE *vfp;

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

   std::string vfn(filename);
   vfn.append(".vaccs");
  	vfp = fopen(vfn.c_str(), "w");
	assert(vfp != NULL);


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
   fclose(vfp);
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

void emit_arch() {

	vaccs_record_factory factory;
	arch_record *rec;

#ifdef __x86_64
   cout << "Architecture: x86_64" << endl;
	rec = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))->add_arch_type(VACCS_ARCH_X86_64);
#else
   cout << "Architecture: ia32" << endl;
	rec = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))->add_arch_type(VACCS_ARCH_I386);
#endif

	rec->write(vfp);

   delete rec;

}

void emit_binary(std::string binary) {
   cout << "Binary: " << binary << endl;
	vaccs_record_factory factory;
   binary_record *brec = ((binary_record*)factory.make_vaccs_record(VACCS_BINARY))
         ->add_bin_file_name(binary.c_str());
   brec->write(vfp);

   delete brec;
}

void emit_cmd_line(int argc, int file_index, char **argv) {
	std::string cmd_line;
	for (int i = file_index; i < argc; i++)
		cmd_line.append(argv[i]).append(" ");

	printf("Command line = %s\n",cmd_line.c_str());

	vaccs_record_factory factory;
	cmd_line_record *rec = ((cmd_line_record*)factory.make_vaccs_record(VACCS_CMD_LINE))
			->add_cmd_line(cmd_line.c_str());

	rec->write(vfp);

   delete rec;
}

void emit_c_code(vaccs_dw_reader *vdr) {
	cu_table *cutab = vdr->get_cutab();
	vaccs_record_factory factory;

	printf("C source code\n");
	printf("-------------\n");
	for (std::map<std::string,symbol_table_record*>::iterator it = cutab->begin(); it != cutab->end(); it++) {
		printf("Found a file %s\n", it->first.c_str());
		std::ifstream cfile(it->first.c_str(), std::ifstream::in);
		std::string ccode;
		ccode_record *rec;
		int i = 1;
		while (std::getline(cfile,ccode)) {
			printf("Line %d: %s\n",i,ccode.c_str());
			rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))->add_c_file_name(it->first.c_str())
					->add_c_line_num(i)
					->add_c_start_pos(0)
					->add_c_src_line(ccode.c_str());
			rec->write(vfp);

         delete rec;

			i++;
		}
		cfile.close();
	}
}

int main(int argc, char *argv[])
{


	 /*
	  * Pin Initialization
	  */
	initialize();

   int i;
   for (i = 0; i < argc && strncmp(argv[i],"--",2); i++);
	setup_output_files(argv[++i]);

   /*
    * read dwarf information from executable
    */
   std::string vfn(argv[i]);
   vfn.append(".vdw");
	vdr = new vaccs_dw_reader();
	vdr->add_file_name(vfn);

	vdr->read_vaccs_dw_info();
 
   emit_arch();
   emit_cmd_line(argc,i,argv);

   std::string bn(argv[i]);
   emit_binary(bn);

   emit_c_code(vdr);

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
