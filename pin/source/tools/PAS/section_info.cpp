#include <util/general.h>
#include "global.h"
#include "section_info.h"
#include <io/vaccs_record.h>
#include <io/vaccs_record_factory.h>
#include <io/section_record.h>
#include <string>
#include <sstream>

Generic text_base_address = BASE_ADDRESS; // Base address for offsets for .text addresses (no ASLR)
Generic data_base_address = BASE_ADDRESS; // Base address for offsets of global data (no ASLR)

//extern bool Is_System_Image(IMG img);;
VOID dump_section_info(IMG img, VOID *v){
   static bool header_added = false;
   vaccs_record_factory factory;
   section_record *srec;
   if (!header_added) {
      srec = ((section_record *)factory.make_vaccs_record(VACCS_SECTION))
         ->add_info("|Type|Address|Size|Section Name");
      srec->write(vaccs_fd);
      DEBUGL(LOG("Section header:\n"));
      DEBUGL(LOG("\t|Type|Address|Size|Section Name\n"));
      header_added = true;
   }
	if(Is_System_Image(img)) return;

	for(SEC sec = IMG_SecHead(img);SEC_Valid(sec);sec = SEC_Next(sec)){
      if (!SEC_Name(sec).empty()) {
         DEBUGL(LOG("Section name: " + SEC_Name(sec) + "\n"));
         DEBUGL(LOG("\tType: " + decstr(SEC_Type(sec)) + "\n"));
         DEBUGL(LOG("\tAddress: " + hexstr(SEC_Address(sec)) + "\n"));
         DEBUGL(LOG("\tSize: " + decstr(SEC_Size(sec)) + "\n"));

         stringstream info;
         info << "|" << (Generic)SEC_Type(sec)
            << "|0x" << hex << (Generic)SEC_Address(sec) 
            << "|" << dec << SEC_Size(sec) << "|" << SEC_Name(sec);
         DEBUGL(LOG("\tRecord: " + info.str() + "\n"));
         srec = ((section_record *)factory.make_vaccs_record(VACCS_SECTION))
            ->add_info(info.str().c_str());
         srec->write(vaccs_fd);

      } 
	}

}
