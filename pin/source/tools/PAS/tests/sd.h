/*
 * =====================================================================================
 *
 *       Filename:  sd.h
 *
 *    Description:  Provides the stub for detecting the declartion of a
 *                  variable as secure data.
 *
 *        Version:  1.0
 *        Created:  12/11/2019 03:49:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Steve Carr (), steve.carr@wmich.edu
 *   Organization:  Department of Computer Science
 *
 * =====================================================================================
 */

#include <stdlib.h>

#define VACCSSensitiveAnnotate(var) _vaccs_declare_as_secure_data(&var)

void _vaccs_declare_as_secure_data(void *address) { address = NULL; return;}
