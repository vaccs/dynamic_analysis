/*
 * sd.cpp
 *
 */
#include "fileops.h"
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include "global.h"
#include <util/general.h>
#include <util/vaccs_config.h>
#include <tables/cu_table.h>
#include <tables/deref.h>
#include <tables/type_table.h>
#include <tables/var_table.h>
#include <tables/frame.h>
#include <tables/fd_table.h>

#include <io/vaccs_record_factory.h>
#include <io/file_open_record.h>
#include <io/file_close_record.h>
#include <io/file_read_record.h>
#include <io/file_write_record.h>

#include <list>

using namespace std;

extern runtime_stack * stack_model;
extern vaccs_config *vcfg;
extern list<ADDRINT> call_inst_ip; // managed in function_call.cpp

fd_table fdtab;

static list<fd_record*> last_file_op;

VOID vaccs_monitor_open_before(VOID *path, INT32 flags, INT32 mode)
{

  DEBUGL(LOG("In vaccs_monitor_open, flags = " + hexstr(flags) + ", mode = " + hexstr(mode) + "\n"));

  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0) {
    DEBUGL(LOG("Call from line = " + decstr(line) + "\n"));
    fd_record *fdr = (new fd_record())
                     ->add_path((const char*)path)
                     ->add_flags(flags)
                     ->add_mode(mode);


    last_file_op.push_front(fdr);
  }
}

VOID vaccs_monitor_open_after(INT32 fd)
{

  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0 && fd != -1) {
    DEBUGL(LOG("Call from line = " + decstr(line) + "\n"));
    struct stat lstat_info, fstat_info;
    fd_record *fdr = last_file_op.front();

    if (!lstat(fdr->get_path().c_str(), &lstat_info) && !fstat(fd, &fstat_info)) {

      if (S_ISLNK(lstat_info.st_mode))
        fdr->add_is_sym_link();

      fdtab.put((const long)fd, fdr);

      vaccs_record_factory factory;
      file_open_record *forec = ((file_open_record*)factory.make_vaccs_record(VACCS_FILE_OPEN))
                                ->add_event_num(timestamp++)
                                ->add_c_line_num(line)
                                ->add_c_file_name(file_name.c_str())
                                ->add_file_descriptor(fd)
                                ->add_path(fdr->get_path().c_str())
                                ->add_flags(fdr->get_flags())
                                ->add_mode(fdr->get_mode());
      if (fdr->get_is_sym_link())
        forec->add_is_sym_link();

      forec->write(vaccs_fd);
      delete forec;
    }
    last_file_op.pop_front();
  }
}

VOID vaccs_monitor_close(INT32 fd)
{


  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0) {
    fdtab.erase((const long)fd);
    vaccs_record_factory factory;
    file_close_record *fcrec = ((file_close_record*)factory.make_vaccs_record(VACCS_FILE_CLOSE))
                               ->add_event_num(timestamp++)
                               ->add_c_line_num(line)
                               ->add_c_file_name(file_name.c_str())
                               ->add_file_descriptor(fd);

    fcrec->write(vaccs_fd);
    delete fcrec;;
  }
}

VOID vaccs_monitor_fopen_before(VOID *path, VOID *mode)
{
  string s_mode((char*)mode);

  DEBUGL(LOG("In vaccs_monitor_open, mode = " + s_mode + "\n"));

  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0) {
    DEBUGL(LOG("Call from line = " + decstr(line) + "\n"));
    fd_record *fdr = (new fd_record())
                     ->add_path((const char*)path)
                     ->add_str_mode(s_mode);


    last_file_op.push_front(fdr);
  }
}

VOID vaccs_monitor_fopen_after(VOID *fp)
{

  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0 && fp != NULL) {
    DEBUGL(LOG("Call from line = " + decstr(line) + "\n"));
    fd_record *fdr = last_file_op.front();
    fdtab.put((const long)fp, fdr);

    vaccs_record_factory factory;
    file_open_record *forec = ((file_open_record*)factory.make_vaccs_record(VACCS_FILE_OPEN))
                              ->add_event_num(timestamp++)
                              ->add_c_line_num(line)
                              ->add_c_file_name(file_name.c_str())
                              ->add_file_descriptor((long)fp)
                              ->add_path(fdr->get_path().c_str())
                              ->add_flags(fdr->get_flags())
                              ->add_mode(fdr->get_mode());
    forec->write(vaccs_fd);
    delete forec;

    last_file_op.pop_front();
  }
}

VOID vaccs_monitor_fclose(VOID *fp)
{

  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0) {
    fdtab.erase((const long)fp);
    vaccs_record_factory factory;
    file_close_record *fcrec = ((file_close_record*)factory.make_vaccs_record(VACCS_FILE_CLOSE))
                               ->add_event_num(timestamp++)
                               ->add_c_line_num(line)
                               ->add_c_file_name(file_name.c_str())
                               ->add_file_descriptor((long)fp);

    fcrec->write(vaccs_fd);
    delete fcrec;;
  }
}

VOID vaccs_monitor_read(INT32 fd)
{ 
  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0) {
    fdtab.erase((const long)fd);
    vaccs_record_factory factory;
    file_read_record *fcrec = ((file_read_record*)factory.make_vaccs_record(VACCS_FILE_READ))
                               ->add_event_num(timestamp++)
                               ->add_c_line_num(line)
                               ->add_c_file_name(file_name.c_str())
                               ->add_file_descriptor(fd);

    fcrec->write(vaccs_fd);
    delete fcrec;
  }

}

VOID vaccs_monitor_fread(VOID *fp)
{ 
  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0) {
    fdtab.erase((const long)fp);
    vaccs_record_factory factory;
    file_read_record *fcrec = ((file_read_record*)factory.make_vaccs_record(VACCS_FILE_READ))
                               ->add_event_num(timestamp++)
                               ->add_c_line_num(line)
                               ->add_c_file_name(file_name.c_str())
                               ->add_file_descriptor((long)fp);

    fcrec->write(vaccs_fd);
    delete fcrec;;
  }

}

VOID vaccs_monitor_write(INT32 fd)
{ 
  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0) {
    fdtab.erase((const long)fd);
    vaccs_record_factory factory;
    file_write_record *fcrec = ((file_write_record*)factory.make_vaccs_record(VACCS_FILE_WRITE))
                               ->add_event_num(timestamp++)
                               ->add_c_line_num(line)
                               ->add_c_file_name(file_name.c_str())
                               ->add_file_descriptor(fd);

    fcrec->write(vaccs_fd);
    delete fcrec;;
  }

}

VOID vaccs_monitor_fwrite(VOID *fp)
{ 
  INT32 column;
  INT32 line;
  string file_name;

  PIN_LockClient();
  PIN_GetSourceLocation(call_inst_ip.front(), &column, &line, &file_name);
  PIN_UnlockClient();

  if (line != 0) {
    fdtab.erase((const long)fp);
    vaccs_record_factory factory;
    file_write_record *fcrec = ((file_write_record*)factory.make_vaccs_record(VACCS_FILE_WRITE))
                               ->add_event_num(timestamp++)
                               ->add_c_line_num(line)
                               ->add_c_file_name(file_name.c_str())
                               ->add_file_descriptor((long)fp);

    fcrec->write(vaccs_fd);
    delete fcrec;;
  }

}

/* ===================================================================== */
/* Analysis routine for setuid                                           */
/* ===================================================================== */
VOID FileOpsImage(IMG img, VOID *v)
{
  RTN open_rtn = RTN_FindByName(img, "open@plt");

  if (RTN_Valid(open_rtn)) {
    DEBUGL(LOG("Instrumenting open\n"));
    RTN_Open(open_rtn);

    RTN_InsertCall(open_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_monitor_open_before,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
                   IARG_END);
    RTN_InsertCall(open_rtn, IPOINT_AFTER, (AFUNPTR)vaccs_monitor_open_after,
                   IARG_FUNCRET_EXITPOINT_VALUE,
                   IARG_END);
    RTN_Close(open_rtn);
  }

  RTN close_rtn = RTN_FindByName(img, "close@plt");

  if (RTN_Valid(close_rtn)) {
    DEBUGL(LOG("Instrumenting close"));
    RTN_Open(close_rtn);

    RTN_InsertCall(close_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_monitor_close,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_END);
    RTN_Close(close_rtn);
  }

  RTN read_rtn = RTN_FindByName(img, "read@plt");

  if (RTN_Valid(read_rtn)) {
    DEBUGL(LOG("Instrumenting read"));
    RTN_Open(read_rtn);

    RTN_InsertCall(close_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_monitor_read,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_END);
    RTN_Close(read_rtn);
  }

  RTN write_rtn = RTN_FindByName(img, "write@plt");

  if (RTN_Valid(write_rtn)) {
    DEBUGL(LOG("Instrumenting write"));
    RTN_Open(write_rtn);

    RTN_InsertCall(close_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_monitor_write,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_END);
    RTN_Close(write_rtn);
  }

  RTN fopen_rtn = RTN_FindByName(img, "fopen@plt");

  if (RTN_Valid(fopen_rtn)) {
    DEBUGL(LOG("Instrumenting fopen\n"));
    RTN_Open(fopen_rtn);

    RTN_InsertCall(fopen_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_monitor_fopen_before,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
                   IARG_END);
    RTN_InsertCall(fopen_rtn, IPOINT_AFTER, (AFUNPTR)vaccs_monitor_fopen_after,
                   IARG_FUNCRET_EXITPOINT_VALUE,
                   IARG_END);
    RTN_Close(fopen_rtn);
  }

  RTN fclose_rtn = RTN_FindByName(img, "fclose@plt");

  if (RTN_Valid(fclose_rtn)) {
    DEBUGL(LOG("Instrumenting fclose"));
    RTN_Open(fclose_rtn);

    RTN_InsertCall(fclose_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_monitor_fclose,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_END);
    RTN_Close(fclose_rtn);
  }

  RTN fread_rtn = RTN_FindByName(img, "fread@plt");

  if (RTN_Valid(fread_rtn)) {
    DEBUGL(LOG("Instrumenting fread"));
    RTN_Open(fread_rtn);

    RTN_InsertCall(fread_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_monitor_fread,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_END);
    RTN_Close(fread_rtn);
  }

  RTN fwrite_rtn = RTN_FindByName(img, "fwrite@plt");

  if (RTN_Valid(fwrite_rtn)) {
    DEBUGL(LOG("Instrumenting fwrite"));
    RTN_Open(fwrite_rtn);

    RTN_InsertCall(fwrite_rtn, IPOINT_BEFORE, (AFUNPTR)vaccs_monitor_fwrite,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                   IARG_END);
    RTN_Close(fwrite_rtn);
  }
}
