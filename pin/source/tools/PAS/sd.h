/*
 * sd.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#ifndef SD_H
#define SD_H

#include "pin.H"

VOID vaccs_declare_as_secure_data(ADDRINT addr);
VOID vaccs_sd_clear_set_check(ADDRINT addr, UINT32 c, UINT64 n);
VOID vaccs_sd_clear_set_memset(ADDRINT addr, UINT32 c, UINT64 n);
VOID vaccs_sd_clear_set_bzero(ADDRINT addr, UINT64 n);
VOID vaccs_sd_clear_set_bcopy(ADDRINT addr1, ADDRINT addr2, UINT64 n);
VOID vaccs_sd_clear_set_memcpy(ADDRINT addr1, ADDRINT addr2, UINT64 n);
VOID vaccs_sd_clear_set_mem_write(ADDRINT ip, ADDRINT addr, const CONTEXT *ctxt, UINT32 size);
VOID vaccs_sd_clear_set_instrument_inst(INS ins, VOID * v);
VOID vaccs_sd_lock(ADDRINT addr, UINT64 n);
VOID vaccs_sd_unlock(ADDRINT addr, UINT64 n);
VOID vaccs_sd_corezero(UINT32 resource, ADDRINT rlim);
VOID SecureDataImage(IMG img, VOID *v);

#endif /* SD_H */
