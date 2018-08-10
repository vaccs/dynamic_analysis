/*
 * global.cpp
 *
 *  Created on: Aug 15, 2016
 *      Author: haoli
 */

#include <cstdlib>
#include "global.h"
#include <iostream>
#include <fstream>
#include <util/general.h>
#include <strings.h>

using namespace std;

bool Is_System_Image(IMG img){
	if(strncmp(IMG_Name(img).c_str(),"/home",5)){
		return true;
	}
	else{
		return false;
	}
}
void get_registers(const CONTEXT * ctxt,int id){
	/***EAX,EBX,ECX,EDX,EBP,EIP***/
	//ADDRINT EAX = (ADDRINT)PIN_GetContextReg( ctxt, REG_GAX);
	//ADDRINT EBX = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBX);
	//ADDRINT ECX = (ADDRINT)PIN_GetContextReg( ctxt, REG_GCX);
	//ADDRINT EDX = (ADDRINT)PIN_GetContextReg( ctxt, REG_GDX);
	//ADDRINT EBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GBP);
	//ADDRINT EIP = (ADDRINT)PIN_GetContextReg( ctxt, REG_INST_PTR);
	//ADDRINT RBP = (ADDRINT)PIN_GetContextReg( ctxt, REG_GR_BASE);
	//ou_register<<dec<<id<<","<<hex<<EAX<<","<<EBX<<","<<ECX<<","<<EDX<<","<<EBP
			//<<","<<EIP<<","<<EBP<<std::endl;

}

ADDRINT read_memory_as_address(ADDRINT addr) {
   ADDRINT buf;
   DEBUGL(LOG("Address being read: 0x" + hexstr(addr) + "\n"));

   if (addr != 0)
      bcopy((void *)addr,&buf,sizeof(ADDRINT));
   else
      buf = 0;
   return buf;
}

void print_global_context(){
	if(current_function_name!=0)
		DEBUGL(LOG("Invocation ID:"+decstr(current_invocation_id)+ "function "+
               current_function_name +" was called. Frame pointer was "+
               hexstr(current_EBP)+"\n"));
}
char* process_string_for_csv(char* string){
	int length = strlen(string);
	int i;
	int number_of_prime = 0;
	for(i=0;i<length;i++){
		if(string[i]=='"') number_of_prime++;

	}
	char* csv_string = (char*) malloc(length+number_of_prime+2);
	csv_string[0] = '"';
	int j = 1;
	for(i = 0;i<length;i++){
		if(string[i]!='"'){
			csv_string[j] = string[i];
			j++;
		}
		else{
			csv_string[j] = string[i];
			j++;
			csv_string[j] = string[i];
		}
	}
	csv_string[j] = '"';
	return csv_string;
}
