/*
 * This file is the source code to do dynamic analysis for the project VACCS
 * It provides data for the address space visualization.
 */
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
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
#include "section_info.h"
#include <util/general.h>
#include <vaccs_read/vaccs_dw_reader.h>
#include <io/vaccs_record_factory.h>
#include <io/arch_record.h>
#include <io/binary_record.h>
#include <io/ccode_record.h>
#include <io/cmd_line_record.h>
#include <io/func_inv_record.h>
#include <io/output_record.h>
#include <tables/frame.h>
#include <tables/heap.h>

vaccs_dw_reader *vdr = NULL;
NATIVE_FD vaccs_fd = -1;
int vaccs_stdout = -1;
extern heap_map *heap_m;
using namespace std;


/*call stack*/
stack<function_invocation_transaction> invocation_stack;

/* runtime stack of variables */
runtime_stack *stack_model;

/*file output variable*/
ofstream ou_function_invocation;
ofstream ou_return;
ofstream ou_malloc;
ofstream ou_free;
ofstream ou_push;
ofstream ou_pop;
ofstream ou_read_direct;
ofstream ou_write_direct;
ofstream ou_read_indirect;
ofstream ou_write_indirect;
ofstream ou_register;
ofstream pas_output;


/*
 * global variables CHANFGEING
 */

vector<std::string> assembly_codes;


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

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
void setup_output_files(char* filename){
   string vfn(filename);
   vfn.append(".vaccs");
  	assert(OS_OpenFD(vfn.c_str(), OS_FILE_OPEN_TYPE_CREATE | OS_FILE_OPEN_TYPE_WRITE | OS_FILE_OPEN_TYPE_TRUNCATE,
            OS_FILE_PERMISSION_TYPE_WRITE | OS_FILE_PERMISSION_TYPE_READ,
            &vaccs_fd).generic_err == OS_RETURN_CODE_NO_ERROR);

   string stdout_fn("/tmp/vaccs.stdout");

   assert((vaccs_stdout = open(stdout_fn.c_str(),O_CREAT | O_RDONLY | O_NONBLOCK,
            S_IRUSR | S_IWUSR)) != -1);
	//pas_output.open(application_name.c_str());
	//ou_function_invocation.open("function_invocation.csv");
	//ou_malloc.open("malloc.csv");
	//ou_free.open("free.csv");
	//ou_push.open("push.csv");
	//ou_pop.open("pop.csv");
	//ou_read_direct.open("read_direct.csv");
	//ou_write_direct.open("write_direct.csv");
	//ou_read_indirect.open("read_indirect.csv");
	//ou_write_indirect.open("write_indirect.csv");
	//ou_return.open("return.csv");
	//ou_register.open("register.csv");

}

void finalize_outputfiles(){
	//ou_function_invocation.close();
	//ou_malloc.close();
	//ou_free.close();
	//ou_push.close();
	//ou_pop.close();
	//ou_read_direct.close();
	//ou_write_direct.close();
	//ou_read_indirect.close();
	//ou_write_indirect.close();
	//ou_return.close();
	//ou_register.close();
	//pas_output.close();

   OS_CloseFD(vaccs_fd);
   close(vaccs_stdout);
}

void initialize(){

		 /*
		  * Initialize Global Variables
		  */
		current_EBP = 0;
		timestamp = 0;
		current_function_name = 0;
		current_invocation_id =0;

                heap_m = new heap_map();

		//file output initialization

}


VOID Fini(INT32 code, VOID *v)
{

	finalize_outputfiles();
	delete stack_model;
}

void emit_arch() {

	vaccs_record_factory factory;
	arch_record *rec;

#ifdef __x86_64
   DEBUGL(cerr << "Architecture: x86_64\n");
	rec = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))->add_arch_type(VACCS_ARCH_X86_64);
#else
   DEBUGL(cerr << "Architecture: ia32\n");
	rec = ((arch_record*)factory.make_vaccs_record(VACCS_ARCH))->add_arch_type(VACCS_ARCH_I386);
#endif

	rec->write(vaccs_fd);

   delete rec;

}

void emit_binary(string binary) {
   DEBUGL(cerr << "Binary: " + binary + "\n");
	vaccs_record_factory factory;
   binary_record *brec = ((binary_record*)factory.make_vaccs_record(VACCS_BINARY))
         ->add_bin_file_name(binary.c_str());
   brec->write(vaccs_fd);

   delete brec;
}

void emit_cmd_line(int argc, int file_index, char **argv) {
	string cmd_line;
	for (int i = file_index; i < argc; i++)
		cmd_line.append(argv[i]).append(" ");

	DEBUGL(cerr << "Command line = " + cmd_line + "\n");

	vaccs_record_factory factory;
	cmd_line_record *rec = ((cmd_line_record*)factory.make_vaccs_record(VACCS_CMD_LINE))
			->add_cmd_line(cmd_line.c_str());

	rec->write(vaccs_fd);

   delete rec;
}

void emit_c_code(vaccs_dw_reader *vdr) {
	cu_table *cutab = vdr->get_cutab();
	vaccs_record_factory factory;

	DEBUGL(cerr << "C source code\n");
	DEBUGL(cerr << "-------------\n");
	for (map<std::string,symbol_table_record*>::iterator it = cutab->begin(); it != cutab->end(); it++) {
		DEBUGL(cerr << "Found a file " +  it->first + "\n");
		ifstream cfile(it->first.c_str(), std::ifstream::in);
		string ccode;
		ccode_record *rec;
		int i = 1;
		while (getline(cfile,ccode)) {
			DEBUGL(cerr << "Line " + decstr(i) + ": " + ccode + "\n");
			rec = ((ccode_record*)factory.make_vaccs_record(VACCS_CCODE))->add_c_file_name(it->first.c_str())
					->add_c_line_num(i)
					->add_c_start_pos(0)
					->add_c_src_line(ccode.c_str());
			rec->write(vaccs_fd);

         delete rec;

			i++;
		}
		cfile.close();
	}

}

void emit_initial_function_call() { 

    vaccs_record_factory factory;
 
    DEBUGL(cerr << "Call to function\n");
    DEBUGL(cerr << "\tEvent num: " + decstr(timestamp++) + "\n");
    DEBUGL(cerr << "\tFunction name: __NOFUNCTION__\n");
    DEBUGL(cerr << "\tFunc line: 0\n");
    DEBUGL(cerr << "\tInv line: 0\n");
    DEBUGL(cerr << "\tFunc file: __NOCSOURCE__\n");
    DEBUGL(cerr << "\tInv file: __NOCSOURCE\n");
    DEBUGL(cerr << "\tCallee address: 0x0\n\n");
    func_inv_record *frec = (func_inv_record*)factory.make_vaccs_record(VACCS_FUNCTION_INV);
    DEBUGL(cerr << "frec = 0x" + hexstr(frec) + "\n");
    frec = frec->add_event_num(timestamp++)
      ->add_func_name(NOFUNCNAME)
      ->add_func_line_num(0)
      ->add_inv_line_num(0)
      ->add_c_func_file(NOCSOURCE)
      ->add_c_inv_file(NOCSOURCE)
      ->add_address(0);

    DEBUGL(cerr << "Built frec\n");
    frec->write(vaccs_fd);
    delete frec;
    DEBUGL(cerr << "Wrote frec\n");
}

int main(int argc, char *argv[])
{

    DEBUGL(cerr << "Begin pas_analysis\n");

     /*
      * Pin Initialization
      */

    initialize();

    int i;
    for (i = 0; i < argc && strncmp(argv[i],"--",2); i++);

    PIN_InitSymbols();
    if( PIN_Init(argc,argv) ) {
        return Usage();
    }
 
    setup_output_files(argv[++i]);

   /*
    * read dwarf information from executable
    */
    string vfn(argv[i]);
    vfn.append(".vdw");
    vdr = new vaccs_dw_reader();
    vdr->add_file_name(vfn);

    vdr->read_vaccs_dw_info();
 
    stack_model = (new runtime_stack())->add_cu_table(vdr->get_cutab());
    emit_arch();
    emit_cmd_line(argc,i,argv);

    string bn(argv[i]);
    emit_binary(bn);

    emit_c_code(vdr);

    IMG_AddInstrumentFunction(dump_section_info,0);
    IMG_AddInstrumentFunction(FunctionInvocatioinImage,0);

    IMG_AddInstrumentFunction(InstructionInstrumentation,0);
    IMG_AddInstrumentFunction(ReturnImage, 0);

    IMG_AddInstrumentFunction(MallocAndFreeImage, 0);

    PIN_AddFiniFunction(Fini, 0);

    //emit_initial_function_call();

    DEBUGL(cerr << "Starting program\n");

    // Never returns
    PIN_StartProgram();

    return 0;
}
