/*
 * database_access_api.h
 *
 *  Created on: Aug 26, 2016
 *      Author: haoli
 */
#include "vaccs_analysis.h"
#include "global.h"

#ifndef DATABASE_ACCESS_API_H_
#define DATABASE_ACCESS_API_H_
/*
 *
 */
void store_push_transaction(push_transaction push_event);
/*
 *
 */
void store_pop_transaction(pop_transaction pop_event);
/*
 *
 */
void store_malloc_transaction(malloc_transaction malloc_event);
/*
 *
 */
void store_return_transaction(return_transaction return_event);
/*
 *
 */
void store_free_transaction(free_transaction free_event);
/*
 *
 */
void store_function_invocation_transaction(function_invocation_transaction function_invocation_event);
/*
 *
 */
void store_program_invocation_transaction(program_invocation_transaction program_invocation_event);
/*
 *
 */
void store_program_exit_transaction(program_exit_transaction program_exit_event);
/*
 *
 */
void store_directed_variable_access_transactions(directed_variable_access_transaction directed_variable_access_event);
/*
 *
 */
void store_indirected_variable_access_transactions(indirected_variable_access_transaction indirected_variable_access_event);

void test_mysql();
void write_push_transaction();
/*
 *
 */
void write_pop_transaction();
/*
 *
 */
void write_malloc_transaction();
/*
 *
 */
void write_return_transaction();
/*
 *
 */
void write_free_transaction();
/*
 *
 */
void write_function_invocation_transaction();
/*
 *
 */
void write_program_invocation_transaction();
/*
 *
 */
void write_program_exit_transaction();
/*
 *
 */
void write_directed_variable_access_transactions();
/*
 *
 */
void write_indirected_variable_access_transactions();


#endif /* DATABASE_ACCESS_API_H_ */
