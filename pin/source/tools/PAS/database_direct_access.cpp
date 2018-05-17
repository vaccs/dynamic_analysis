/*
 * database_direct_access.cpp
 *
 *  Created on: Sep 8, 2016
 *      Author: haoli
 */
#include "database_direct_access.h"

void test_mysql()
{
	int d[2];
	pipe(d);
	printf("MySQL client version: %s\n", mysql_get_client_info());
}
void write_push_transaction(){

}
/*
 *
 */
void write_pop_transaction(){

}
/*
 *
 */
void write_malloc_transaction(){

}
/*
 *
 */
void write_return_transaction(){

}
/*
 *
 */
void write_free_transaction(){

}
/*
 *
 */
void write_function_invocation_transaction(){

}
/*
 *
 */
void write_program_invocation_transaction(){

}
/*
 *
 */
void write_program_exit_transaction(){

}
/*
 *
 */
void write_directed_variable_access_transactions(){

}
/*
 *
 */
void write_indirected_variable_access_transactions(){

}
