/******************************************************************************
*
* File: abbrv_table.c
*
* Summary: This file contains the code for building tables for mapping 
*          dwarf abbreviations to their format
*
*
******************************************************************************/

#include <stdlib.h>

#include <abbrv_table.h>

static SymTable abbrv_table; // The symbol table for holding dwarf abbreviations

STATIC(void, free_abbrv_real_tag, (Generic str));


/**
* 
* Initialize the abbreviation symbol table
*
* @param size the size of the abbreviation table
*
*/
void abbrv_table_init(int size) {
	
   abbrv_table = SymInit(size);
    
   SymInitField(abbrv_table,SYM_ABBRV_TAG_FIELD,NULL,free_abbrv_real_tag);
}

/**
* 
* Free the real_tag_field
*
* @param str the string to free
*
*/
static void free_abbrv_real_tag(Generic str) {

   if (str != NULL)
      free((char *)str);
}

/**
* 
* Free the abbreviation symbol table
*
*/
void abbrv_table_fini() {

   SymKill(abbrv_table);
   abbrv_table = NULL;
}

/**
* 
* Look for an abbreviation in the symbol table. If it is not there, insert it. Return the index.
*
* @param abbrv_tag the tag to find in the symbol table
* @return the index in the symbol table for the tag
*
*/
int abbrv_table_index(char *abbrv_tag) {

   return SymIndex(abbrv_table,abbrv_tag);

}

/**
* 
* Look for an abbreviation in the symbol table. If it is not there, return -1, otherwise return the index.
*
* @param abbrv_tag the tag to find in the symbol table
* @return the index in the symbol table for the tag, -1 if not found
*
*/
int abbrv_table_query_index(char *abbrv_tag) {

   return SymQueryIndex(abbrv_table,abbrv_tag);

}

/**
* 
* Put the real tag field value for an abbreviation name in the symbol table
*
* @param abbrv_tag the tag to find in the symbol table
* @param real_tag the name of the real tag
*
*/
int abbrv_table_put_tag(int tag_index, real_tag_type real_tag) {

   if (SymFieldExists(abbrv_table,SYM_ABBRV_TAG_FIELD)) {
      SymPutFieldByIndex(abbrv_table,tag_index,SYM_ABBRV_TAG_FIELD,(Generic)real_tag);
      return 1;
   }
   else {
      return 0;
   }
}

/**
* 
* Get the real tag field value for an abbreviation name in the symbol table
*
* @param abbrv_tag the tag to find in the symbol table
* @return the name of the real tag, NULL if no such field
*
*/
real_tag_type abbrv_table_get_tag(int tag_index) {
   if (SymFieldExists(abbrv_table,SYM_ABBRV_TAG_FIELD)) {
      return (real_tag_type)SymGetFieldByIndex(abbrv_table,tag_index,SYM_ABBRV_TAG_FIELD);
   }
   else {
      return NULL;
   }
}
