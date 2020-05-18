/*
 * This file is the source code to do dynamic analysis for the project VACCS
 * It provides data for the address space visualization.
 */
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <signal.h>
#include "pin.H"
#include <sys/stat.h>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include "return.h"
#include "malloc_free.h"
#include "sd.h"
#include "setuid.h"
#include "InstructionInstrumentation.h"
#include "global.h"
#include "functionInvocation.h"
#include "section_info.h"
#include "fileops.h"
#include <util/general.h>
#include <util/c_string_utils.h>
#include <util/vaccs_config.h>
#include <vaccs_read/vaccs_dw_reader.h>
#include <io/vaccs_record_factory.h>
#include <io/arch_record.h>
#include <io/binary_record.h>
#include <io/ccode_record.h>
#include <io/cmd_line_record.h>
#include <io/func_inv_record.h>
#include <io/output_record.h>
#include <io/return_record.h>
#include <tables/frame.h>
#include <tables/heap.h>
#include <util/memory_info.h>

vaccs_dw_reader *vdr = NULL;
NATIVE_FD vaccs_fd = -1;
int vaccs_stdout = -1;
extern heap_map *heap_m;
memory_info *memmap;
using namespace std;
bool no_reg = false;
vaccs_config *vcfg;

KNOB<BOOL> user_code_only(KNOB_MODE_WRITEONCE, "pintool", "user-code-only", "0", "analyze user code only");
KNOB<BOOL> monitor_registers(KNOB_MODE_WRITEONCE, "pintool", "monitor-registers", "0", "analyze registers");
KNOB<BOOL> malloc_free(KNOB_MODE_WRITEONCE, "pintool", "malloc-free", "0", "analyze calls to malloc and free");
KNOB<BOOL> secure_data(KNOB_MODE_WRITEONCE, "pintool", "secure-data", "0", "analyze secure data");
KNOB<BOOL> file_ops(KNOB_MODE_WRITEONCE, "pintool", "file-ops", "0", "analyze file operations");
KNOB<string> config_file(KNOB_MODE_WRITEONCE, "pintool", "vaccs-config", "default", "set vaccs configuration file");

/*call stack*/
stack<function_invocation_transaction> invocation_stack;

/* runtime stack of variables */
runtime_stack *stack_model;

/*file output variable*/
ofstream ou_function_invocation;
ofstream ou_return;
ofstream ou_malloc;
ofstream ou_free;
ofstream ou_push;
ofstream ou_pop;
ofstream ou_read_direct;
ofstream ou_write_direct;
ofstream ou_read_indirect;
ofstream ou_write_indirect;
ofstream ou_register;
ofstream pas_output;

/*
 * global variables CHANFGEING
 */

vector<std::string> assembly_codes;

char *current_function_name;
int current_invocation_id;
int timestamp;
push_transaction push_event;
pop_transaction pop_event;
malloc_transaction malloc_event;
return_transaction return_event;

/*
 *
 *
 *
 *
 *
 */
//MYSQL *con;

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
  PIN_ERROR("This Pintool prints a trace of memory addresses\n" + KNOB_BASE::StringKnobSummary() + "\n");
  return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
void setup_output_files(char *filename)
{
  string vfn(filename);

  vfn.append(".vaccs");
  assert(OS_OpenFD(vfn.c_str(), OS_FILE_OPEN_TYPE_CREATE | OS_FILE_OPEN_TYPE_WRITE | OS_FILE_OPEN_TYPE_TRUNCATE,
                   OS_FILE_PERMISSION_TYPE_WRITE | OS_FILE_PERMISSION_TYPE_READ,
                   &vaccs_fd)
             .generic_err == OS_RETURN_CODE_NO_ERROR);

  string stdout_fn("/tmp/vaccs.stdout");

  assert((vaccs_stdout = open(stdout_fn.c_str(), O_CREAT | O_RDONLY | O_NONBLOCK,
                              S_IRUSR | S_IWUSR)) != -1);
  //pas_output.open(application_name.c_str());
  //ou_function_invocation.open("function_invocation.csv");
  //ou_malloc.open("malloc.csv");
  //ou_free.open("free.csv");
  //ou_push.open("push.csv");
  //ou_pop.open("pop.csv");
  //ou_read_direct.open("read_direct.csv");
  //ou_write_direct.open("write_direct.csv");
  //ou_read_indirect.open("read_indirect.csv");
  //ou_write_indirect.open("write_indirect.csv");
  //ou_return.open("return.csv");
  //ou_register.open("register.csv");
}

void finalize_outputfiles()
{
  //ou_function_invocation.close();
  //ou_malloc.close();
  //ou_free.close();
  //ou_push.close();
  //ou_pop.close();
  //ou_read_direct.close();
  //ou_write_direct.close();
  //ou_read_indirect.close();
  //ou_write_indirect.close();
  //ou_return.close();
  //ou_register.close();
  //pas_output.close();

  OS_CloseFD(vaccs_fd);
  close(vaccs_stdout);
}

VOID Fini(INT32 code, VOID *v);

static string get_signal_string(EXCEPTION_CODE ex_code) {
  string ex_msg;
  switch (ex_code) {
    case EXCEPTCODE_ACCESS_INVALID_ADDRESS:
    case EXCEPTCODE_RECEIVED_ACCESS_FAULT:
      ex_msg = "invalid memory address";
      break;
    case EXCEPTCODE_ACCESS_DENIED:
      ex_msg = "memory protection violation";
      break;
    case EXCEPTCODE_ACCESS_INVALID_PAGE:
      ex_msg = "invalid memory page";
      break;
    case EXCEPTCODE_ACCESS_MISALIGNED:
      ex_msg = "memory access misaligned";
      break;
    case EXCEPTCODE_ILLEGAL_INS:
      ex_msg = "illegal instruction";
      break;
    case EXCEPTCODE_PRIVILEGED_INS:
      ex_msg = "privileged instruction";
      break;
    case EXCEPTCODE_INT_DIVIDE_BY_ZERO:
      ex_msg = "integer divide by zero";
      break;
    case EXCEPTCODE_INT_OVERFLOW_TRAP:
      ex_msg = "integer overflow";
      break;
    case EXCEPTCODE_INT_BOUNDS_EXCEEDED:
      ex_msg = "array out-of-bounds";
      break;
    case EXCEPTCODE_X87_DIVIDE_BY_ZERO:
    case EXCEPTCODE_SIMD_DIVIDE_BY_ZERO:
      ex_msg = "floating-point divide by zero";
      break;
    case EXCEPTCODE_X87_OVERFLOW:
    case EXCEPTCODE_SIMD_OVERFLOW:
      ex_msg = "floating-point overflow";
      break;
    case EXCEPTCODE_X87_UNDERFLOW:
    case EXCEPTCODE_SIMD_UNDERFLOW:
      ex_msg = "floating-point underflow";
      break;
    case EXCEPTCODE_X87_INEXACT_RESULT:
    case EXCEPTCODE_SIMD_INEXACT_RESULT:
      ex_msg = "floating-point inexact result";
      break;
    case EXCEPTCODE_X87_INVALID_OPERATION:
    case EXCEPTCODE_SIMD_INVALID_OPERATION:
      ex_msg = "floating-point invalid operation";
      break;
    case EXCEPTCODE_X87_DENORMAL_OPERAND:
    case EXCEPTCODE_SIMD_DENORMAL_OPERAND:
      ex_msg = "floating-point denormalized operand";
      break;
    case EXCEPTCODE_X87_STACK_ERROR:
      ex_msg = "floating-point stack error";
       break;
    case EXCEPTCODE_RECEIVED_AMBIGUOUS_X87:
    case EXCEPTCODE_RECEIVED_AMBIGUOUS_SIMD:
      ex_msg = "floating-point unknown error";
       break;
    default:
      ex_msg = "unknown error";
       break;
  }

  return ex_msg;
}

static BOOL signal_handler(THREADID id, INT32 signum, CONTEXT *ctxt, BOOL hasHandler, const EXCEPTION_INFO *einfo, VOID *val)
{
  ADDRINT ip = PIN_GetContextReg(ctxt, REG_INST_PTR);

  INT32 column;
  INT32 line;
  string fileName;

  PIN_LockClient();
  PIN_GetSourceLocation(ip, &column, &line, &fileName);
  PIN_UnlockClient();

  ADDRINT exAddr = PIN_GetExceptionAddress(einfo);
  string last_func = stack_model->get_last_user_function_called();
  DEBUGL(LOG("Caught a signal in analysis of program -- aborting\n"));
  if (line == 0)
    cerr << "VACCS: Application raised " << get_signal_string(PIN_GetExceptionCode(einfo)) << " fault trying to access address " 
         << hexstr(exAddr) << " in unknown file at ip = " << hexstr(ip) << ". Last known program function: " << last_func << "\n";
  else
    cerr << "VACCS: Application raised " << get_signal_string(PIN_GetExceptionCode(einfo)) << " fault in file " << fileName << " at line " 
         << line << " in function " << last_func << " trying to access address " << hexstr(exAddr) << "\n";
  Fini(0, NULL);
  exit(-1);
}


void initialize()
{

  /*
   * Initialize Global Variables
   */
  current_EBP = 0;
  timestamp = 0;
  current_function_name = 0;
  current_invocation_id = 0;

  if (!user_code_only && !monitor_registers && !malloc_free && !secure_data && !file_ops)
  {
    string config_file_name;
    if (config_file.Value() == "default")
    {
      string vpas(getenv("VPAS"));
      config_file_name = vpas + "/vaccs.cfg";
    }
    else
    {
      config_file_name = config_file.Value();
    }
    vcfg = new vaccs_config(config_file_name);
  }
  else
  {
    vcfg = new vaccs_config();
    if (user_code_only)
      vcfg->add_user_code_only();
    if (monitor_registers)
      vcfg->add_monitor_registers();
    if (malloc_free)
      vcfg->add_malloc_free();
    if (secure_data)
      vcfg->add_secure_data();
    if (file_ops)
      vcfg->add_file_ops();
  }

  DEBUGL(vcfg->dump_vaccs_config());

  pid_t pid = getpid();
  stringstream ss;
  ss << pid;
  string spid;
  ss >> spid;
  string map_file = "/proc/" + spid + "/maps";
  heap_m = new heap_map(map_file);
  memmap = new memory_info(map_file);
  DEBUGL(LOG("Done reading file\n"));
}

VOID Fini(INT32 code, VOID *v)
{

  // this is a return for main() which does not happen
  vaccs_record_factory factory;
  return_record *rrec = (return_record *)factory.make_vaccs_record(VACCS_RETURN);

  rrec = rrec->add_event_num(timestamp++);
  rrec->write(vaccs_fd);

  // this is a return for _start which does not happen

  if (!vcfg->get_user_code_only())
  {
    rrec->add_event_num(timestamp++);
    rrec->write(vaccs_fd);
  }

  delete rrec;
  finalize_outputfiles();
  delete stack_model;
  delete heap_m;
  delete vcfg;
}

void emit_arch()
{

  vaccs_record_factory factory;
  arch_record *rec;

#ifdef __x86_64
  DEBUGL(LOG("Architecture: x86_64, heap_start = " + hexstr(memmap->get_heap_begin()) +
             " heap_end = " + hexstr(memmap->get_heap_end()) + " stack_begin = " +
             hexstr(memmap->get_stack_begin()) + " stack end " +
             hexstr(memmap->get_stack_end()) + ": \n"));
  rec = ((arch_record *)factory.make_vaccs_record(VACCS_ARCH))
            ->add_arch_type(VACCS_ARCH_X86_64)
            ->add_heap_start(memmap->get_heap_begin())
            ->add_heap_end(memmap->get_heap_end())
            ->add_stack_start(memmap->get_stack_begin())
            ->add_stack_end(memmap->get_stack_end());
#else
  DEBUGL(LOG("Architecture: ia32, heap_start = " + hexstr(heap_m->get_heap_start()) +
             " heap_end = " + hexstr(heap_m->get_heap_end()) + ": \n"));
  rec = ((arch_record *)factory.make_vaccs_record(VACCS_ARCH))
            ->add_arch_type(VACCS_ARCH_I386)
            ->add_heap_start(heap_m->get_heap_start())
            ->add_heap_end(heap_m->get_heap_end());
#endif

  rec->write(vaccs_fd);

  delete rec;
}

void emit_binary(string binary)
{
  DEBUGL(LOG("Binary: " + binary + "\n"));
  vaccs_record_factory factory;
  binary_record *brec = ((binary_record *)factory.make_vaccs_record(VACCS_BINARY))
                            ->add_bin_file_name(binary.c_str());
  brec->write(vaccs_fd);

  delete brec;
}

void emit_cmd_line(int argc, int file_index, char **argv)
{
  string cmd_line;

  for (int i = file_index; i < argc; i++)
    cmd_line.append(argv[i]).append(" ");

  DEBUGL(LOG("Command line = " + cmd_line + "\n"));

  vaccs_record_factory factory;
  cmd_line_record *rec = ((cmd_line_record *)factory.make_vaccs_record(VACCS_CMD_LINE))
                             ->add_cmd_line(cmd_line.c_str());

  rec->write(vaccs_fd);

  delete rec;
}

void emit_c_code(vaccs_dw_reader *vdr)
{
  cu_table *cutab = vdr->get_cutab();
  vaccs_record_factory factory;

  DEBUGL(LOG("C source code\n"));
  DEBUGL(LOG("-------------\n"));
  for (map<std::string, symbol_table_record *>::iterator it = cutab->begin(); it != cutab->end(); it++)
  {
    DEBUGL(LOG("Found a file " + it->first + "\n"));
    ifstream cfile(it->first.c_str(), std::ifstream::in);
    string ccode;
    ccode_record *rec;
    int i = 1;
    while (getline(cfile, ccode))
    {
      DEBUGL(LOG("Line " + decstr(i) + ": " + ccode + "\n"));
      rec = ((ccode_record *)factory.make_vaccs_record(VACCS_CCODE))->add_c_file_name(it->first.c_str())->add_c_line_num(i)->add_c_start_pos(0)->add_c_src_line(ccode.c_str());
      rec->write(vaccs_fd);

      delete rec;

      i++;
    }
    cfile.close();
  }
}

int main(int argc, char *argv[])
{

  DEBUGL(LOG("Begin pas_analysis\n"));

  /*
   * Pin Initialization
   */

  int i;
  for (i = 0; i < argc && strncmp(argv[i], "--", 2); i++)
    ;

  //DEBUGL(no_reg = true);
  PIN_InitSymbols();
  if (PIN_Init(argc, argv))
    return Usage();

  PIN_InterceptSignal(SIGSEGV,signal_handler,0);
  PIN_InterceptSignal(SIGILL,signal_handler,0);
  PIN_InterceptSignal(SIGBUS,signal_handler,0);
  PIN_InterceptSignal(SIGFPE,signal_handler,0);
  PIN_InterceptSignal(SIGSYS,signal_handler,0);
  PIN_InterceptSignal(SIGXCPU,signal_handler,0);
  PIN_InterceptSignal(SIGXFSZ,signal_handler,0);

  initialize();
  setup_output_files(argv[++i]);

  /*
   * read dwarf information from executable
   */
  string vfn(argv[i]);
  vfn.append(".vdw");
  vdr = new vaccs_dw_reader();
  vdr->add_file_name(vfn);

  DEBUGL(LOG("Reading dwarf info\n"));
  vdr->read_vaccs_dw_info();

  DEBUGL(LOG("Building dwarf tables\n"));
  stack_model = (new runtime_stack())->add_cu_table(vdr->get_cutab());
  DEBUGL(LOG("Begin analysis\n"));
  emit_arch();
  emit_cmd_line(argc, i, argv);

  string bn(argv[i]);
  emit_binary(bn);

  emit_c_code(vdr);

  IMG_AddInstrumentFunction(dump_section_info, 0);
  IMG_AddInstrumentFunction(FunctionInvocatioinImage, 0);

  IMG_AddInstrumentFunction(InstructionInstrumentation, 0);
  IMG_AddInstrumentFunction(ReturnImage, 0);

  if (vcfg->get_malloc_free())
    IMG_AddInstrumentFunction(MallocAndFreeImage, 0);

  if (vcfg->get_secure_data())
    IMG_AddInstrumentFunction(SecureDataImage, 0);

  if (vcfg->get_file_ops())
    IMG_AddInstrumentFunction(FileOpsImage, 0);

  PIN_AddFiniFunction(Fini, 0);

  DEBUGL(LOG("Starting program\n"));
  vcfg->dump_vaccs_config();

  // Never returns
  PIN_StartProgram();

  return 0;
}
