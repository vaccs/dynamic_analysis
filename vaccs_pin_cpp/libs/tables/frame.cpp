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

#include <sstream>
#include <tables/frame.h>
#include <tables/deref.h>
#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/var_access_record.h>
#include <tables/heap.h>

/**
 * Create an instance of a frame_record and initialize instance variables.
 */
frame_record::frame_record()
{
	variable_name = "";
	value = "";
	points_to_value = "";
}

/**
 * Create and instance of a stack frame
 */
frame::frame() : list()
{
	is_first_access = true;
	is_before_stack_setup = true;
}

/**
 * Create an instance of a runtime stack and initialize the instance variables
 */
runtime_stack::runtime_stack()
{
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
runtime_stack *runtime_stack::add_cu_table(cu_table *cutab)
{

	DEBUGL(LOG("In runtime_stack::add_cu_table for globals\n"));

	this->cutab = cutab;
	global_frame = (new frame())->add_name("*G*");

	for (map<string, symbol_table_record*>::iterator it = cutab->begin(); it != cutab->end(); it++) {

		cu_record *curec = (cu_record*)it->second;
		var_table *vtab = curec->get_var_table();

		for (map<string, symbol_table_record*>::iterator vit = vtab->begin(); vit != vtab->end(); vit++) {

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
frame *runtime_stack::push(string name, Generic ip, CONTEXT *ctx)
{
	DEBUGL(LOG("In runtime_stack::push for routine" + name + "\n"));

	var_table *vtab = cutab->get_function_var_table(ip);

	frame *new_frame = (new frame())->add_name(name)->add_context(ctx);

	if (vtab != NULL)
		for (map<string, symbol_table_record*>::iterator it = vtab->begin(); it != vtab->end(); it++) {

			var_record *vrec = (var_record*)it->second;

			if (!vrec->get_is_subprog()) {

				DEBUGL(LOG("Add variable " + it->first + " for frame " + name + "\n"));

				type_table *ttab = cutab->get_type_table(vrec->get_type());
				type_record *trec = ttab->get(vrec->get_type());
				Generic addr = vrec->deref_if_by_reference(trec, vrec->get_base_address(ctx));
				string value = vrec->read_value(ttab, trec, addr, ctx);

				frame_record *frec = (new frame_record())->add_variable_name(it->first)
				                     ->add_var_record(vrec)
				                     ->add_value(value);

				if (trec->get_is_pointer()) {
					bool is_segv;
					Generic ptr_addr = dereference_memory((Generic*)addr, &is_segv);

					string points_to_value;
					if (!is_segv) {
						type_record *btrec = ttab->get(*trec->get_base_type());
						points_to_value = vrec->read_value(ttab, btrec, ptr_addr, ctx);
					} else
						points_to_value = MEM_ADDR_ERROR(addr);

					frec->add_points_to_value(points_to_value);

				}
				new_frame->add_frame_record(frec);
			}
		}

	push_front(new_frame);

	return new_frame;
}

/**
 * Pop a frame from the runtime stack
 */
void runtime_stack::pop()
{
	pop_front();
}

/**
 * Compute a list of variables that reference a particular updated memory location
 *
 * @param addr the address of a memory location in memory
 * @param cutab the compilation unit table;
 * @return a list of variables that reference the given memory location
 */
list<var_upd_record*> *runtime_stack::addr_get_updated_variables(Generic addr, cu_table *cutab)
{

	DEBUGL(LOG("In runtime_stack::addr_get_updated_variables, addr = " + MEM_ADDR_STR(addr) + "\n"));

	list<var_upd_record*> *vlist = new list<var_upd_record*>();

	/* check the entire stack for a variable at this address */

	for (list<frame*>::iterator it = begin(); it != end(); it++)
		vlist->splice(vlist->end(), *get_updated_variables_from_frame(cutab, *it, addr));

	/* check globals for a variable at this address */

	vlist->splice(vlist->end(), *get_updated_variables_from_frame(cutab, global_frame, addr));

	return vlist;
}


string* convert_buffer_to_string(const char *buffer, const type_record *trec)
{

	return NULL;
}

list<var_upd_record *> *runtime_stack::get_updated_array_elements(cu_table *cutab, frame *fr,
                                                                  frame_record *frec,
                                                                  var_record *vrec,
                                                                  Generic addr,
                                                                  string prefix)
{

	list<var_upd_record *> *vlist = new list<var_upd_record *>();
	type_table *ttab = cutab->get_type_table(vrec->get_type());
	type_record *trec = ttab->get(vrec->get_type());
	string scope = cutab->get_scope(vrec);

	Generic element_size = ttab->get(*trec->get_base_type())->get_size();
	symbol_table_record_factory factory;

	if (fr->get_is_first_access() || vrec->is_first_access()) {

		//
		// On first access write out the entire array so the visualization has the inital values
		//
		vrec->clear_first_access();

		Generic array_addr = vrec->get_base_address(fr->get_context());

		var_upd_record *vurec = (new var_upd_record())
		                        ->add_variable_name(frec->get_variable_name())
		                        ->add_var_record(vrec)
														->add_type_name(*trec->get_name())
		                        ->add_context(fr->get_context())
		                        ->add_address(array_addr)
		                        ->add_scope(scope)
		                        ->add_prefix(prefix)
		                        ->add_value("<multielement>");

		vlist->push_back(vurec);

		string base_type = *ttab->get(vrec->get_type())->get_base_type();
		for (unsigned int i = 0; i <= trec->get_upper_bound(); i++) {
			var_record *varec = (var_record*)factory.make_symbol_table_record(VAR_RECORD);
			varec = varec->add_decl_file(vrec->get_decl_file())
			        ->add_decl_line(vrec->get_decl_line())
			        ->add_type(base_type)
			        ->add_location(vrec->get_location() + (element_size * i));

			if (vrec->get_is_local())
				varec = varec->add_is_local();
			else if (vrec->get_is_param())
				varec = varec->add_is_param();

			Generic addr = array_addr + (element_size * i);
			var_upd_record *nvurec = (new var_upd_record())
			                         ->add_variable_name(vurec->get_variable_name())
			                         ->add_var_record(varec)
														   ->add_type_name(base_type)
			                         ->add_context(fr->get_context())
			                         ->add_address(addr)
			                         ->add_prefix(vurec->get_variable_name() + ":[" + decstr(i) + "]")
			                         ->add_scope(scope)
			                         ->add_value(varec->read_value(ttab, ttab->get(base_type), addr,
			                                                       fr->get_context()));
			vlist->push_back(nvurec);

		}
	} else {
		int index = (addr - vrec->get_base_address(fr->get_context()))
		            / element_size;
		DEBUGL(LOG("Computed index = " + decstr(index) + "\n"));
		var_record *varec = (var_record*)factory.make_symbol_table_record(VAR_RECORD);
		string base_type = *ttab->get(vrec->get_type())->get_base_type();
		varec = varec->add_decl_file(vrec->get_decl_file())
		        ->add_decl_line(vrec->get_decl_line())
		        ->add_type(base_type)
		        ->add_location(vrec->get_location() + (element_size * index));

		if (vrec->get_is_local())
			varec = varec->add_is_local();
		else if (vrec->get_is_param())
			varec = varec->add_is_param();

		var_upd_record *nvurec = (new var_upd_record())
		                         ->add_variable_name(frec->get_variable_name())
		                         ->add_var_record(varec)
														 ->add_type_name(base_type)
		                         ->add_context(fr->get_context())
		                         ->add_address(addr)
		                         ->add_scope(scope)
		                         ->add_prefix(frec->get_variable_name() + ":[" + decstr(index) + "]")
		                         ->add_value(varec->read_value(ttab, ttab->get(base_type), addr,
		                                                       fr->get_context()));

		vlist->push_back(nvurec);

	}

	return vlist;
}

list<var_upd_record *> *runtime_stack::get_updated_struct_members(cu_table *cutab, frame *fr,
                                                                  frame_record *frec,
                                                                  var_record *vrec,
                                                                  Generic addr,
                                                                  string prefix)
{

	list<var_upd_record *> *vlist = new list<var_upd_record *>();
	type_table *ttab = cutab->get_type_table(vrec->get_type());
  type_record *trec = ttab->get(vrec->get_type());
	var_table *memtab = vrec->get_member_table();
	string scope = cutab->get_scope(vrec);

	DEBUGL(LOG("In get_updated_struct_members: scope = "+scope+"\n"));

	if (fr->get_is_first_access() || vrec->is_first_access()) {

		//
		// On first access write out the entire structure so the visualization has the inital values
		//

		Generic struct_addr = vrec->get_base_address(fr->get_context());

		var_upd_record *srec = (new var_upd_record())
		                       ->add_variable_name(prefix + frec->get_variable_name())
		                       ->add_var_record(vrec)
													 ->add_type_name(*trec->get_name())
		                       ->add_context(fr->get_context())
		                       ->add_address(struct_addr)
		                       ->add_scope(scope)
		                       ->add_prefix(prefix)
		                       ->add_value("<multielement>");

		vlist->push_back(srec);

		Generic member_address = struct_addr;

		//
		// Emit the values of all structure members
		//

		for (std::map<std::string, symbol_table_record*>::iterator it = memtab->begin();
		     it != memtab->end();
		     it++) {

			var_record *mvrec = (var_record*)it->second;
			type_record *mtrec = ttab->get(mvrec->get_type());

			var_upd_record *vurec = (new var_upd_record())
			                        ->add_variable_name(it->first)
													    ->add_type_name(*mtrec->get_name())
			                        ->add_var_record(mvrec)
			                        ->add_context(fr->get_context())
			                        ->add_address(member_address)
			                        ->add_scope(scope)
			                        ->add_prefix(prefix + *trec->get_name() + ":")
			                        ->add_value(mvrec->read_value(ttab, mtrec,
			                                                      member_address, fr->get_context()));
			vlist->push_back(vurec);
			member_address += ttab->get(mvrec->get_type())->get_size();

		}

		vrec->clear_first_access();

	} else {

		//
		// if this is not the first time this variable has been written only find
		// the particular struct element that is updated
		//

		for (std::map<std::string, symbol_table_record*>::iterator it = memtab->begin();
		     it != memtab->end();
		     it++) {

			var_record *mvrec = (var_record*)it->second;
			type_record *mtrec = ttab->get(mvrec->get_type());

			//
			// find the structure element that is updated
			//

			if (mvrec->is_at_address(fr->get_context(), addr, mtrec)) {
				var_upd_record *vurec = (new var_upd_record())
				                        ->add_variable_name(it->first)
																->add_type_name(*mtrec->get_name())
				                        ->add_var_record(mvrec)
				                        ->add_context(fr->get_context())
				                        ->add_address(addr)
				                        ->add_scope(scope)
				                        ->add_prefix(prefix + *trec->get_name() + ":")
				                        ->add_value(mvrec->read_value(ttab, mtrec, addr,
				                                                      fr->get_context()));
				vlist->push_back(vurec);
				break;
			}
		}
	}

	return vlist;
}

/**
 * Check a single frame for variables that have been updated
 *
 * @param cutab a compilation unit table
 * @param fr a stack or global variable frame
 * @param addr the address that has been updated, if addr = 0 then all locations are checked
 * @return a list of variables in this frame whose value hase changed
 */
list<var_upd_record *> *runtime_stack::get_updated_variables_from_frame(cu_table *cutab, frame *fr,
                                                                        Generic addr)
{

	DEBUGL(LOG("In runtime_stack::get_updated_variables_from_frame\n"));

	if (fr->get_is_first_access())
		DEBUGL(LOG("First access for frame of " + fr->get_name() + "\n"));
	list<var_upd_record*> *vlist = new list<var_upd_record*>();

	for (list<frame_record*>::iterator it2 = fr->begin(); it2 != fr->end(); it2++) {
		frame_record *frec = *it2;
		DEBUGL(LOG("Checking if variable " + frec->get_variable_name() + " has been updated\n"));
		type_record *trec = cutab->get_type_record(frec->get_var_record()->get_type());
		var_record *vrec = frec->get_var_record();
		type_table *ttab = cutab->get_type_table(vrec->get_type());

		//
		// If addr = 0, make sure we check this variable (all variables are checked in this case)
		//
		if (addr == 0 || fr->get_is_first_access())
			addr = vrec->get_var_address(fr->get_context(), trec);

		if (fr->get_is_first_access() || vrec->is_at_address(fr->get_context(), addr, trec)) {

			if (trec->get_is_struct()) {

				vlist->splice(vlist->end(), *get_updated_struct_members(cutab, fr, frec, vrec, addr, ""));

			} else if (trec->get_is_array()) {

				vlist->splice(vlist->end(), *get_updated_array_elements(cutab, fr, frec, vrec, addr, ""));

			} else {
				string new_value = vrec->read_value(ttab, trec, addr, fr->get_context());

				DEBUGL(LOG("New value = " + new_value + ", old value = " + frec->get_value() + "\n"));

				//
				// Check if there is a new value for this variable stored in the stack frame
				// if get_all_values is true, then a record is generated for every variable
				//
				if (fr->get_is_first_access() || new_value != frec->get_value()) {

					DEBUGL(LOG("Found new value: " + new_value + " for " + frec->get_variable_name() + "\n"));

					frec->add_value(new_value);
					var_upd_record *vurec = (new var_upd_record())
					                        ->add_variable_name(frec->get_variable_name())
																	->add_type_name(*trec->get_name())
					                        ->add_var_record(frec->get_var_record())
					                        ->add_context(fr->get_context())
					                        ->add_address(addr)
					                        ->add_scope(cutab->get_scope(vrec))
					                        ->add_value(new_value);

					if (trec->get_is_pointer()) {

						unsigned long pointer_val = 0;
						stringstream(frec->get_points_to_value()) >> pointer_val;
						DEBUGL(LOG("points_to value converted from " + frec->get_points_to_value() +
						           " to " + hexstr(pointer_val)));
						vurec->add_points_to(pointer_val);
					}

					vlist->push_back(vurec);

				}
			}
		}

	}

	if (fr->get_is_first_access())
		fr->clear_is_first_access();

	return vlist;
}

/**
 * Compute a list of variables that have been updated. Search the stack and global data area for any updated variable.
 * This method is used after a library routine has been called
 *
 * @param cutab a compilation unit table
 * @return a list of variables that have been updated
 */
list<var_upd_record *> *runtime_stack::get_all_updated_variables(cu_table *cutab)
{

	DEBUGL(LOG("In runtime_stack::get_all_updated_variables\n"));

	list<var_upd_record*> *vlist = new list<var_upd_record*>();

	for (list<frame*>::iterator it = begin(); it != end(); it++)
		vlist->splice(vlist->end(), *get_updated_variables_from_frame(cutab, *it, 0));

	vlist->splice(vlist->end(), *get_updated_variables_from_frame(cutab, global_frame, 0));

	return vlist;
}

list<var_upd_record *> *runtime_stack::get_updated_points_to_from_struct(
	cu_table *cutab,
	frame *fr,
	frame_record *frec,
	var_record *vrec,
	Generic addr,
	string prefix)
{

	list<var_upd_record *> *vlist = new list<var_upd_record *>();
	type_table *ttab = cutab->get_type_table(vrec->get_type());
	type_record *trec = ttab->get(vrec->get_type());
	var_table *memtab = vrec->get_member_table();
	string scope = cutab->get_scope(vrec);

	DEBUGL(LOG("In get_updated_points_to_from_struct: scope = "+scope+"\n"));
	//
	// if this is not the first time this variable has been written only find
	// the particular struct element that is updated
	//

	for (std::map<std::string, symbol_table_record*>::iterator it = memtab->begin();
	     it != memtab->end();
	     it++) {

		var_record *mvrec = (var_record*)it->second;
		type_record *mtrec = ttab->get(mvrec->get_type());

		if (mtrec->get_is_pointer()) {

			DEBUGL(LOG("Found a pointer variable: " + frec->get_variable_name() + "\n"));

			Generic addr = mvrec->get_base_address(fr->get_context());
			bool is_segv;

			Generic ptr_addr = dereference_memory((Generic*)addr, &is_segv);

			string new_value;

			if (is_segv) {
				DEBUGL(LOG("Pointer variable: " + frec->get_variable_name() + " is not a valid memory location\n"));
				new_value = MEM_ADDR_ERROR(addr);
			} else {
				dereference_memory((Generic*)ptr_addr, &is_segv);

				if (is_segv) {
					DEBUGL(LOG("Pointer variable: " + frec->get_variable_name() +
					           " does not point to a valid memory location\n"));
					new_value = MEM_ADDR_ERROR(ptr_addr);
				} else {
					DEBUGL(LOG("Pointer variable: " + frec->get_variable_name() + " points to a valid memory location\n"));
					type_record *btrec = cutab->get_type_record(*mtrec->get_base_type());

					new_value = mvrec->read_value(ttab, btrec, ptr_addr, fr->get_context());
				}
			}

			var_upd_record *vurec = (new var_upd_record())
			                        ->add_variable_name(it->first)
															->add_type_name(*mtrec->get_name())
			                        ->add_var_record(mvrec)
			                        ->add_context(fr->get_context())
			                        ->add_address(addr)
			                        ->add_scope(scope)
			                        ->add_prefix(prefix + *trec->get_name() + ":")
															->add_value(hexstr(ptr_addr))
															->add_points_to(ptr_addr)
															->add_points_to_value(new_value)
															->add_update_is_points_to();

			vlist->push_back(vurec);
		}
	}

	return vlist;
}

/**
 * Check a single frame for variables whose points to location has been updated
 *
 * @param cutab a compilation unit table
 * @param fr a stack or global variable frame
 * @return a list of variables in this frame whose value hase changed
 */
list<var_upd_record *> *runtime_stack::get_updated_points_to_frame(cu_table *cutab, frame *fr)
{

	DEBUGL(LOG("In runtime_stack::get_updated_points_to_frame\n"));

	list<var_upd_record*> *vlist = new list<var_upd_record*>();
	for (list<frame_record*>::iterator it2 = fr->begin(); it2 != fr->end(); it2++) {
		frame_record *frec = *it2;
		type_record *trec = cutab->get_type_record(frec->get_var_record()->get_type());

		//
		// Check if the value pointed to has changed
		//

		var_record *vrec = frec->get_var_record();
		Generic addr = vrec->get_base_address(fr->get_context());

		if (trec->get_is_pointer()) {

			DEBUGL(LOG("Found a pointer variable: " + frec->get_variable_name() + "\n"));

			bool is_segv;

			Generic ptr_addr = dereference_memory((Generic*)addr, &is_segv);

			string new_value;

			if (is_segv) {
				DEBUGL(LOG("Pointer variable: " + frec->get_variable_name() + " is not a valid memory location\n"));
				new_value = MEM_ADDR_ERROR(addr);
			} else {
				dereference_memory((Generic*)ptr_addr, &is_segv);

				if (is_segv) {
					DEBUGL(LOG("Pointer variable: " + frec->get_variable_name() +
					           " does not point to a valid memory location\n"));
					new_value = MEM_ADDR_ERROR(ptr_addr);
				} else {
					DEBUGL(LOG("Pointer variable: " + frec->get_variable_name() + " points to a valid memory location\n"));
					type_table *ttab = cutab->get_type_table(vrec->get_type());

					type_record *btrec = cutab->get_type_record(*trec->get_base_type());

					new_value = vrec->read_value(ttab, btrec, ptr_addr, fr->get_context());
				}
			}

			DEBUGL(LOG("Checking if what variable " + frec->get_variable_name() + " points to has been updated\n"));
			if (new_value != frec->get_points_to_value()) {

				DEBUGL(LOG("Found new points_to value: " + new_value + " for " + frec->get_variable_name() +
				           " old points_to value = " + frec->get_points_to_value() + "\n"));

				frec->add_points_to_value(new_value);

				var_upd_record *vurec = (new var_upd_record())
				                        ->add_variable_name(frec->get_variable_name())
				                        ->add_var_record(frec->get_var_record())
													      ->add_type_name(*trec->get_name())
				                        ->add_context(fr->get_context())
				                        ->add_address(addr)
				                        ->add_scope(cutab->get_scope(vrec))
				                        ->add_value(frec->get_value())
				                        ->add_update_is_points_to()
				                        ->add_points_to(ptr_addr)
				                        ->add_points_to_value(new_value);

				vlist->push_back(vurec);
			}
		} else if (trec->get_is_struct())
			vlist->splice(vlist->end(), *get_updated_points_to_from_struct(cutab, fr, frec,
			                                                               vrec, addr, ""));
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
list<var_upd_record*> *runtime_stack::get_all_updated_points_to(cu_table *cutab)
{

	DEBUGL(LOG("In runtime_stack::get_all_updated_points_to\n"));

	list<var_upd_record*> *vlist = new list<var_upd_record*>();

	/* check the entire stack for */

	for (list<frame*>::iterator it = begin(); it != end(); it++)
		vlist->splice(vlist->end(), *get_updated_points_to_frame(cutab, *it));

	vlist->splice(vlist->end(), *get_updated_points_to_frame(cutab, global_frame));

	return vlist;
}

void var_upd_record::write(NATIVE_FD vaccs_fd, string fileName, int line, cu_table *cutab, int timestamp)
{
	vaccs_record_factory factory;
	var_access_record *varec;

	DEBUGL(LOG( "Write to variable " + prefix + variable_name + "\n"));
	DEBUGL(LOG( "\tEvent num: " + decstr(timestamp) + "\n"));
	DEBUGL(LOG( "\tC file name: " + fileName + "\n"));
	DEBUGL(LOG( "\tScope: " + scope + "\n"));
	DEBUGL(LOG( "\tAddress: " + hexstr(address) + "\n"));
	DEBUGL(LOG( "\tType: " + type_name + "\n"));
	DEBUGL(LOG( "\tValue: " + value + "\n"));
	DEBUGL(LOG( "\tPoints to: " + hexstr(points_to) + "\n"));
	DEBUGL(LOG( "\tPoints to value: " + points_to_value + "\n"));

	varec = (var_access_record*)factory.make_vaccs_record(VACCS_VAR_ACCESS);
	varec = varec->add_event_num(timestamp)
	        ->add_c_line_num(line)
	        ->add_c_file_name(fileName.c_str())
	        ->add_scope(scope.c_str())
	        ->add_address(address)
	        ->add_name((prefix + variable_name).c_str())
	        ->add_type(type_name.c_str());

	if (update_is_points_to)
		varec = varec->add_value(points_to_value.c_str())
		        ->add_points_to(points_to);
	else if (type_name.find("*") != string::npos)
		varec = varec->add_value(value.c_str())
		        ->add_points_to(points_to);
	else
		varec = varec->add_value(value.c_str())
		        ->add_points_to(-1);

	varec->write(vaccs_fd);
	delete varec;
}
