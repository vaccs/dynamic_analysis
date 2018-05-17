/******************************************************************************
*
* File: type_table.c
*
* Summary: This file contains the code for building tables for mapping 
*          dwarf types to their attributes
*
*
******************************************************************************/

#include <stdlib.h>

#include <type_table.h>


/**
* 
* Initialize the type symbol table
*
* @param type_table the symbol table for type information
* @param size the size of the type table
*
*/
SymTable type_table_init(int size) {
	
   SymTable type_table = SymInit(size);
    
   SymInitField(type_table,SYM_TYPE_SIZE,NULL,NULL);

   return type_table;
}

/**
* 
* Free the type symbol table
*
* @param type_table the symbol table for type information
*/
void type_table_fini(SymTable type_table) {

   SymKill(type_table);
   type_table = NULL;
}

/**
* 
* Look for a type in the symbol table. If it is not there, insert it. Return the index.
*
* @param type_table the symbol table for type information
* @param type_name the name to find in the symbol table
* @return the index in the symbol table for the name
*
*/
int type_table_index(SymTable type_table,char *type_name) {

   return SymIndex(type_table,type_name);

}

/**
* 
* Look for a type in the symbol table. If it is not there, return -1, otherwise return the index.
*
* @param type_table the symbol table for type information
* @param type_name the name to find in the symbol table
* @return the index in the symbol table for the name, -1 if not found
*
*/
int type_table_query_index(SymTable type_table,char *type_name) {

   return SymQueryIndex(type_table,type_name);

}

/**
* 
* Put the size field value for a type name in the symbol table
*
* @param type_table the symbol table for type information
* @param index the index in the symbol table for this type
* @param type_name the name of the type 
* @return 1 if the operation is successful, 0 otherwise
*
*/
int type_table_put_size(SymTable type_table,int index, long size) {

   if (SymFieldExists(type_table,SYM_TYPE_SIZE)) {
      SymPutFieldByIndex(type_table,index,SYM_TYPE_SIZE,(Generic)size);
      return 1;
   }
   else {
      return 0;
   }
}

/**
* 
* Get the size field value for a type name in the symbol table
*
* @param type_table the symbol table for type information
* @param index the index of the type to find in the symbol table
* @return the size of the type, -1 if no such field
*
*/
long type_table_get_size(SymTable type_table,int index) {
   if (SymFieldExists(type_table,SYM_TYPE_SIZE)) {
      return (long)SymGetFieldByIndex(type_table,index,SYM_TYPE_SIZE);
   }
   else {
      return -1;
   }
}
