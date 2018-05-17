/*
 * database_access_api.cpp
 *
 *  Created on: Aug 26, 2016
 *      Author: haoli
 */

/*
 *
 */

#include "database_access_api.h"


#define COMMENTS_DISBLED	0

void store_push_transaction(push_transaction push_event){
#if COMMENTS_DISBLED
	write_push_transaction();
#endif
}
/*
 *
 */
void store_pop_transaction(pop_transaction pop_event){
#if COMMENTS_DISBLED
	write_pop_transaction();
	test_mysql();
#endif
}
/*
 *
 */
void store_malloc_transaction(malloc_transaction malloc_event){
#if COMMENTS_DISBLED
	write_malloc_transaction();
#endif
}
/*
 *
 */
void store_return_transaction(return_transaction return_event){
#if COMMENTS_DISBLED
	write_return_transaction();
#endif
}
/*
 *
 */
void store_free_transaction(free_transaction free_event){
#if COMMENTS_DISBLED
	write_free_transaction();
#endif
}
/*
 *
 */
void store_function_invocation_transaction(function_invocation_transaction function_invocation_event){
#if COMMENTS_DISBLED
	write_function_invocation_transaction();
#endif
}
/*
 *
 */
void store_program_invocation_transaction(program_invocation_transaction program_invocation_event){
#if COMMENTS_DISBLED
	write_program_invocation_transaction();
#endif
}
/*
 *
 */
void store_program_exit_transaction(program_exit_transaction program_exit_event){
#if COMMENTS_DISBLED
	write_program_exit_transaction();
#endif
}
/*
 *
 */
void store_directed_variable_access_transactions(directed_variable_access_transaction directed_variable_access_event){
#if COMMENTS_DISBLED
	write_directed_variable_access_transactions();
#endif
}
/*
 *
 */
void store_indirected_variable_access_transactions(indirected_variable_access_transaction indirected_variable_access_event){
#if COMMENTS_DISBLED
	write_indirected_variable_access_transactions();
#endif
}
