/*
 * IntegerOverflow.cpp
 *
 *  Created on: Oct 18, 2016
 *      Author: haoli
 */

#include <iostream>
#include "pin.H"
#include <bitset>
#include <set>
#include <fstream>

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
std::set<INT32> line_numbers;
INT32 current_line;
std::ofstream output;
bool Is_System_Image(IMG img){
	if(strncmp(IMG_Name(img).c_str(),"/home",5)){
		return true;
	}
	else{
		return false;
	}
}
VOID checkLineNumber(ADDRINT ip){
	INT32 * 	column = (INT32*) malloc(sizeof(INT32));
	INT32 * 	line = (INT32*) malloc(sizeof(INT32));
	std::string 	fileName;
	PIN_LockClient();
	PIN_GetSourceLocation(ip,column,line,&fileName);
	PIN_UnlockClient();
	//std::cout<<"line "<<*line<<std::endl;
	if(*line>0) current_line = *line;
}
// This function is called before every instruction is executed
VOID checkOverflowBefore(CONTEXT * ctxt,CHAR* ins) {
	//ADDRINT flag = (ADDRINT)PIN_GetContextReg( ctxt, REG_AppFlags());
	//std::cout<<"Instruction:\t"<<ins<<std::endl;
	//std::cout<<"Flag before:\t"<<std::bitset<16>(flag)<<std::endl;
}
VOID checkOverflowAfter(CONTEXT * ctxt,ADDRINT ip,CHAR* assembly) {
	ADDRINT flag = (ADDRINT)PIN_GetContextReg( ctxt, REG_AppFlags());

	//std::cout<<"Flag after:\t"<<std::bitset<16>(flag)<<std::endl;
	if(flag&1){
		INT32 * 	column = (INT32*) malloc(sizeof(INT32));
			INT32 * 	line = (INT32*) malloc(sizeof(INT32));
			std::string 	fileName;
			PIN_LockClient();
			PIN_GetSourceLocation(ip,column,line,&fileName);
			PIN_UnlockClient();
			line_numbers.insert(current_line);
		std::cout<<"Integer Overflow at line "<<dec<<current_line<<":"<<assembly<<std::endl;
//		PIN_UnlockClient();
	}

}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
    // Insert a call to docount before every instruction, no arguments are passed
	if(INS_Category	(ins)==9||true ){
		//std::cout<<"category:"<<CATEGORY_StringShort(INS_Category(ins)) <<"  "<<INS_Disassemble(ins)<<std::endl;
		char* currentInstruction = (char*) malloc(1000);
		strcpy(currentInstruction,INS_Disassemble(ins).c_str());
		//std::cout<<"current instruction:"<<currentInstruction<<std::endl;

		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)checkOverflowBefore,IARG_CONTEXT,IARG_ADDRINT, currentInstruction, IARG_END);
		int linenumber;
		string filename;
		PIN_GetSourceLocation(INS_Address(ins), NULL, &linenumber, &filename);
		if(INS_HasFallThrough(ins)){
			INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)checkOverflowAfter,
								IARG_CONTEXT,
								 IARG_INST_PTR,
								 IARG_ADDRINT, currentInstruction,
								 IARG_END);
			INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)checkLineNumber, IARG_INST_PTR, IARG_END);
		}

	}

}

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "inscount.out", "specify output file name");
VOID InstructionInstrumentation(IMG img, VOID *v){
	if(Is_System_Image(img)) return;
		for(SEC sec = IMG_SecHead(img);SEC_Valid(sec);sec = SEC_Next(sec)){
			for(RTN rtn = SEC_RtnHead(sec);RTN_Valid(rtn);rtn = RTN_Next(rtn)){
				RTN_Open(rtn);
				for(INS ins = RTN_InsHead(rtn);INS_Valid(ins);ins = INS_Next(ins)){
					Instruction(ins,0);
				}
				RTN_Close(rtn);
			}
		}

}
// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
	output<<"integer overflow at ";
	for(std::set<INT32>::iterator it= line_numbers.begin();it!=line_numbers.end();it++){
		output<<*it<<" ";
	}
	output<<std::endl;
	output<<"Total integer overflow "<<line_numbers.size()<<std::endl;
    output.close();

}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char * argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();
    PIN_InitSymbols();
    output.open("integer_overflow.txt");
    // Register Instruction to be called to instrument instructions
    IMG_AddInstrumentFunction(InstructionInstrumentation, 0);


    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);

    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}
