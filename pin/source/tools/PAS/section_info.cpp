#include <util/general.h>
#include "global.h"
#include "section_info.h"

//extern bool Is_System_Image(IMG img);;
VOID dump_section_info(IMG img, VOID *v){
	if(Is_System_Image(img)) return;
		for(SEC sec = IMG_SecHead(img);SEC_Valid(sec);sec = SEC_Next(sec)){
         DEBUGL(cout << "Section name: " << SEC_Name(sec) << endl);
         DEBUGL(cout << '\t' << "Type: " << SEC_Type(sec) << endl);
         DEBUGL(cout << '\t' << "Address: 0x" << hex << SEC_Address(sec) << endl);
         DEBUGL(cout << '\t' << "Size: " << dec << SEC_Size(sec) << endl);
		}

}
