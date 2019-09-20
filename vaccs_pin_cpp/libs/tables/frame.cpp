/*
 * =====================================================================================
 *
 *       Filename:  frame.cpp
 *
 *    Description:  This file implements a stack frame so that there is a mapping from
 *          addresses to variables.
 *
 *        Version:  1.0
 *        Created:  11/13/2018 04:50:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steven Carr
 *        Company:  Western Michigan University
 *
 * =====================================================================================
 */

#include <tables/frame.h>
#include <tables/deref.h>
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/var_access_record.h>
#include <tables/heap.h>

/**
 * Create an instance of a frame_record and initialize instance variables.
 */
frame_record::frame_record() {
   variable_name = "";
   value = "";
   points_to_value = "";
}

/**
 * Create and instance of a stack frame
 */
frame::frame() : list() { }

/**
 * Create an instance of a runtime stack and initialize the instance variables
 */
runtime_stack::runtime_stack() {
   cutab = NULL;
   global_frame = NULL;
}

/**
 * Add a compilation unit table to the runtime_stack. This is where we
 * get the stack compilation information from to build the runtime stack
 *
 * @param cutab a compilation unit table
 * @return a pointer to this runtime_stack
 */
runtime_stack *runtime_stack::add_cu_table(cu_table *cutab) {

   DEBUGL(LOG("In runtime_stack::add_cu_table for globals\n"));

   this->cutab = cutab;
   global_frame = (new frame())->add_name("*G*");

   for (map<string,symbol_table_record*>::iterator it = cutab->begin(); it != cutab->end(); it++) {

      cu_record *curec = (cu_record *)it->second;
      var_table *vtab = curec->get_var_table();

      for (map<string,symbol_table_record*>::iterator vit = vtab->begin(); vit != vtab->end(); vit++) {

         var_record *vrec = (var_record*)vit->second;

         if (!vrec->get_is_subprog()) {
            DEBUGL(LOG("Add variable " + vit->first + " for frame *G*\n"));
            frame_record *frec = (new frame_record())->add_variable_name(vit->first)->add_var_record(vrec);
            global_frame->push_back(frec);
         }
      }
   }
   return this;
}

/**
 * Push a frame on the runtime stack containing a map from addresses to
 * var_records for each local variable and parameter in the routine.
 *
 * @param name the name of the function
 * @param ip address in the function
 * @param ctx the pin context for this stack frame
 */
void runtime_stack::push(string name,Generic ip,CONTEXT *ctx) {
   DEBUGL(LOG("In runtime_stack::push for routine" + name + "\n"));

   var_table *vtab = cutab->get_function_var_table(ip);

   frame *new_frame = (new frame())->add_name(name)->add_context(ctx);

   if (vtab != NULL)
      for (map<string,symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {

      var_record *vrec = (var_record*)it->second;

      if (!vrec->get_is_subprog()) {

         DEBUGL(LOG("Add variable " + it->first + " for frame " + name + "\n"));

         type_table *ttab = cutab->get_type_table(vrec->get_type());
         type_record *trec = ttab->get(vrec->get_type());
         Generic addr = vrec->deref_if_by_reference(trec,vrec->get_base_address(ctx));
         string value = vrec->read_value(ttab,trec,addr);

         frame_record *frec = (new frame_record())->add_variable_name(it->first)
                                    ->add_var_record(vrec)
                                    ->add_value(value);

         if (trec->get_is_pointer()) {
            bool is_segv;
            Generic ptr_addr = dereference_memory((Generic*)addr,&is_segv);

            string points_to_value;
            if (!is_segv) {
               type_record *btrec = ttab->get(*trec->get_base_type());
               points_to_value = vrec->read_value(ttab,btrec,ptr_addr);
            } else if (addr == 0) {
               points_to_value = "<null>";
            } else {
               points_to_value = "<unknown>";
            }

            frec->add_points_to_value(points_to_value);

         }
         new_frame->add_frame_record(frec);
       }
   }

   push_front(new_frame);
}

/**
 * Pop a frame from the runtime stack
 */
void runtime_stack::pop() {
   pop_front();
}

/**
* Compute a list of variables that reference a particular updated memory location
*
* @param addr the address of a memory location in memory
* @param cutab the compilation unit table;
* @return a list of variables that reference the given memory location
*/
list<var_upd_record*> *runtime_stack::addr_get_updated_variables(Generic addr,cu_table *cutab) {

   DEBUGL(LOG("In runtime_stack::get_variables_accessed, addr = "+hexstr(addr)+"\n"));

   list<var_upd_record*> *vlist = new list<var_upd_record*>();

    /* check the entire stack for a variable at this address */

   for (list<frame*>::iterator it = begin(); it != end(); it++) {
      vlist->splice(vlist->end(),*get_updated_variables_from_frame(cutab,*it,addr)); 
   }

    /* check globals for a variable at this address */

   vlist->splice(vlist->end(),*get_updated_variables_from_frame(cutab,global_frame,addr));

   return vlist;
}


string* convert_buffer_to_string(const char *buffer, const type_record *trec) {

   return NULL;
}

/**
 * Check a single frame for variables that have been updated
 *
 * @param cutab a compilation unit table
 * @param fr a stack or global variable frame
 * @param addr the addres that has been updated, if addr = 0 then all locations are checked
 * @return a list of variables in this frame whose value hase changed
 */
list<var_upd_record *> *runtime_stack::get_updated_variables_from_frame(cu_table *cutab,frame *fr,
      Generic addr) {

   DEBUGL(LOG("In runtime_stack::get_updated_variables_from_frame\n"));

  list<var_upd_record*> *vlist = new list<var_upd_record*>();

  for (list<frame_record*>::iterator it2 = fr->begin(); it2 != fr->end(); it2++) {
      frame_record *frec = *it2;
      DEBUGL(LOG("Checking if variable " + frec->get_variable_name() + " has been updated\n"));
      type_record *trec = cutab->get_type_record(frec->get_var_record()->get_type());
      var_record *vrec = frec->get_var_record();
      type_table *ttab = cutab->get_type_table(vrec->get_type());

      //
      // If addr = 0, make sure we check this variable (all variables are check in this case)
      //
      if (addr == 0)
         addr = vrec->get_base_address(fr->get_context());

      if (vrec->is_at_address(fr->get_context(),addr,trec)) {

         string new_value = vrec->read_value(ttab,trec,addr);

         DEBUGL(LOG("New value = "+new_value+", old value = "+frec->get_value()+"\n"));

         //
         // Check if there is a new value for this variable stored in the stack frame
         //
         if (new_value != frec->get_value()) {
   
            DEBUGL(LOG("Found new value: "+new_value+" for "+frec->get_variable_name()+"\n"));

            frec->add_value(new_value);
            var_upd_record *vurec = (new var_upd_record())
               ->add_variable_name(frec->get_variable_name())
               ->add_var_record(frec->get_var_record())
               ->add_context(fr->get_context())
               ->add_address(addr)
               ->add_scope(cutab->get_scope(vrec))
               ->add_value(new_value);

            vlist->push_back(vurec);

         }
      }
   }

  return vlist;
}

/**
 * Compute a list of variables that have been updated. Search the stack and global data area for any updated variable.
 * This method is used after a library routine has been called
 *
 * @param cutab a compilation unit table
 * @return a list of variables that have been updated  
 */
list<var_upd_record *> *runtime_stack::get_all_updated_variables(cu_table *cutab) {

   DEBUGL(LOG("In runtime_stack::get_all_updated_variables\n"));

   list<var_upd_record*> *vlist = new list<var_upd_record*>();

   for (list<frame*>::iterator it = begin(); it != end(); it++) {
      vlist->splice(vlist->end(),*get_updated_variables_from_frame(cutab,*it,0)); 
   }

   vlist->splice(vlist->end(),*get_updated_variables_from_frame(cutab,global_frame,0));

   return vlist;
}

/**
 * Check a single frame for variables whose points to location has been updated
 * 
 * @param cutab a compilation unit table
 * @param fr a stack or global variable frame
 * @return a list of variables in this frame whose value hase changed
 */
list<var_upd_record *> *runtime_stack::get_updated_points_to_frame(cu_table *cutab,frame *fr) {

   DEBUGL(LOG("In runtime_stack::get_updated_points_to_frame\n"));

   list<var_upd_record*> *vlist = new list<var_upd_record*>();
   for (list<frame_record*>::iterator it2 = fr->begin(); it2 != fr->end(); it2++) {
      frame_record *frec = *it2;
      type_record *trec = cutab->get_type_record(frec->get_var_record()->get_type());

       //
       // Check if the value pointed to has changed
       //
       
      if (trec->get_is_pointer()) {

         DEBUGL(LOG("Found a pointer varialble: "+frec->get_variable_name()+"\n"));

         var_record *vrec = frec->get_var_record();
         Generic addr = vrec->get_base_address(fr->get_context());
         bool is_segv;

         Generic ptr_addr = dereference_memory((Generic*)addr,&is_segv);

         string new_value;

         if (is_segv) {
            DEBUGL(LOG("Pointer variable: "+frec->get_variable_name()+" does not point to a valid memory location\n"));
            if (addr == 0)
               new_value = "<null>";
            else
               new_value = "<unknown>";
         } else {
            DEBUGL(LOG("Pointer varialble: "+frec->get_variable_name()+" points to a valid memory location\n"));
            type_table *ttab = cutab->get_type_table(vrec->get_type());

            type_record *btrec = cutab->get_type_record(*trec->get_base_type());

            new_value = vrec->read_value(ttab,btrec,ptr_addr);
         }

         DEBUGL(LOG("Checking if what variable "+frec->get_variable_name()+" points to has been updated\n"));
         if (new_value != frec->get_points_to_value()) {

            DEBUGL(LOG("Found new points_to value: "+new_value+" for "+frec->get_variable_name()+"\n"));

            frec->add_points_to_value(new_value);

            var_upd_record *vurec = (new var_upd_record())
               ->add_variable_name(frec->get_variable_name())
               ->add_var_record(frec->get_var_record())
               ->add_context(fr->get_context())
               ->add_address(addr)
               ->add_scope(cutab->get_scope(vrec))
               ->add_value(frec->get_value())
               ->add_points_to(ptr_addr)
               ->add_points_to_value(new_value);

            vlist->push_back(vurec);
         }
      }
   }

   return vlist;
}
 
/**
* Compute a list of all pointers accessible from the stack or static data area whose points_to location has changed 
*
* @param cutab a table of compilation units
* @return a list of variables that on the stack or in global memory that could
* have possibly been accessed
*/
list<var_upd_record*> *runtime_stack::get_all_updated_points_to(cu_table *cutab) {

   DEBUGL(LOG("In runtime_stack::get_all_updated_points_to\n"));

   list<var_upd_record*> *vlist = new list<var_upd_record*>();

  /* check the entire stack for */

   for (list<frame*>::iterator it = begin(); it != end(); it++) {
      vlist->splice(vlist->end(),*get_updated_points_to_frame(cutab,*it)); 
   }

   vlist->splice(vlist->end(),*get_updated_points_to_frame(cutab,global_frame));

   return vlist;
}

void var_upd_record::write(NATIVE_FD vaccs_fd,string fileName,int line,cu_table *cutab,int timestamp) {
   vaccs_record_factory factory;
   var_access_record *varec;

   string scope = cutab->get_scope(vrec);

   DEBUGL(LOG( "Write to variable " + variable_name + "\n"));
   DEBUGL(LOG( "\tEvent num: " + decstr(timestamp) + "\n"));
   DEBUGL(LOG( "\tC file name: " + fileName + "\n"));
   DEBUGL(LOG( "\tScope: " + scope + "\n"));
   DEBUGL(LOG( "\tAddress: " + hexstr(address) + "\n"));
   DEBUGL(LOG( "\tType: " + type_name + "\n"));
   DEBUGL(LOG( "\tValue: " + value + "\n"));
   DEBUGL(LOG( "\tPoints to: " + hexstr(points_to) + "\n"));
   DEBUGL(LOG( "\tPoints to value: "+points_to_value + "\n"));

   varec = (var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS);
   varec = varec->add_event_num(timestamp++)
      ->add_c_line_num(line)
      ->add_c_file_name(fileName.c_str())
      ->add_scope(scope.c_str())
      ->add_address(address)
      ->add_name(variable_name.c_str())
      ->add_type(type_name.c_str());

   if (type_name.find("*") != string::npos)
      varec = varec->add_points_to(points_to)
                   ->add_value(points_to_value.c_str());
   else 
      varec = varec->add_value(value.c_str());

   varec->write(vaccs_fd);
   delete varec;
}

