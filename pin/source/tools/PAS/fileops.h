/*
 * sd.h
 *
 *  Created on: Jul 20, 2016
 *      Author: haoli
 */
#ifndef FILEOPS_H
#define FILEOPS_H

#include "pin.H"

VOID vaccs_monitor_open_before(VOID *path,INT32 flags,INT32 mode);
VOID vaccs_monitor_open_after(INT32 fd);
VOID vaccs_monitor_close(INT32 fd);
VOID vaccs_monitor_read(INT32 fd);
VOID vaccs_monitor_write(INT32 fd);
VOID vaccs_monitor_fopen_before(VOID *path,VOID *mode);
VOID vaccs_monitor_fopen_after(VOID *fp);
VOID vaccs_monitor_fclose(VOID *fp);
VOID vaccs_monitor_fread(VOID *fp);
VOID vaccs_monitor_fwrite(VOID *fp);
VOID FileOpsImage(IMG img, VOID *v);

#endif /* FILEOPS_H */
