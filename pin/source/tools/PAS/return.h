/*
 * return.h
 *
 *  Created on: Jul 19, 2016
 *      Author: haoli
 */
#include "pin.H"

#ifndef RETURN_H_
#define RETURN_H_
VOID BeforeReturn(CONTEXT * ctxt);
VOID AfterReturn( const CONTEXT * ctxt);
VOID ReturnImage(IMG img, VOID *v);

#endif /* RETURN_H_ */
