/*
 * vaccs_record_factory.cpp
 *
 *  Created on: Feb 21, 2018
 *      Author: carr
 */

#include <cstdio>
#include <cassert>

#include <io/vaccs_record_factory.h>
#include <io/vaccs_record.h>
#include <io/arch_record.h>
#include <io/asm_record.h>
#include <io/binary_record.h>
#include <io/ccode_record.h>
#include <io/cmd_line_record.h>
#include <io/func_inv_record.h>
#include <io/output_record.h>
#include <io/register_record.h>
#include <io/return_record.h>
#include <io/return_addr_record.h>
#include <io/sd_clear_record.h>
#include <io/sd_czero_record.h>
#include <io/sd_lock_record.h>
#include <io/sd_set_record.h>
#include <io/sd_unlock_record.h>
#include <io/section_record.h>
#include <io/var_access_record.h>
#include <io/malloc_record.h>
#include <io/free_record.h>

/**
 * Create a vaccs_record object
 *
 * @param id the type of analysis record
 * @return a pointer to vaccs_record object
 */
vaccs_record *vaccs_record_factory::make_vaccs_record(vaccs_id_t id) {
	vaccs_record *rec = NULL;

	switch (id) {
	case VACCS_ARCH:
		rec = new arch_record();
		break;
	case VACCS_ASM:
		rec = new asm_record();
		break;
	case VACCS_BINARY:
		rec = new binary_record();
		break;
	case VACCS_CCODE:
		rec = new ccode_record();
		break;
	case VACCS_CMD_LINE:
		rec = new cmd_line_record();
		break;
	case VACCS_FUNCTION_INV:
		rec = new func_inv_record();
		break;
	case VACCS_OUTPUT:
		rec = new output_record();
		break;
	case VACCS_REGISTER:
		rec = new register_record();
		break;
	case VACCS_RETURN:
		rec = new return_record();
		break;
	case VACCS_RETURN_ADDR:
		rec = new return_addr_record();
		break;
	case VACCS_SD_CLEAR:
		rec = new sd_clear_record();
		break;
	case VACCS_SD_CZERO:
		rec = new sd_czero_record();
		break;
	case VACCS_SD_LOCK:
		rec = new sd_lock_record();
		break;
	case VACCS_SD_SET:
		rec = new sd_set_record();
		break;
	case VACCS_SD_UNLOCK:
		rec = new sd_unlock_record();
		break;
	case VACCS_SECTION:
		rec = new section_record();
		break;
	case VACCS_VAR_ACCESS:
		rec = new var_access_record();
		break;
	case VACCS_MALLOC:
		rec = new malloc_record();
		break;
	case VACCS_FREE:
		rec = new free_record();
		break;
	default:
		break;
	}

	assert(rec != NULL);

	return rec;
}
