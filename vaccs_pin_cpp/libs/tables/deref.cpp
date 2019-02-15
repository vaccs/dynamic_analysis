#include <signal.h>
#include <setjmp.h>

#include <pin.H>

static jmp_buf current_env;

static void segv_handler(int signum) {

  DEBUGL(LOG("Caught a SIGSEGV in dereference_memory()"))
  longjmp(current_env,1);

}

Generic dereference_memory (Generic *addr) {
  Generic value = -1;
  struct sigaction new_action, old_action;

  new_action.sa_handler = segv_handler;
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;
  sigaction (SIGSEGV, NULL, &old_action);

  sigaction (SIGSEGV, &new_action, NULL);
  if (!setjmp(curent_env))
    value = *addr;

  sigaction(SIGSEGV,&old_action,NULL)

  return value;
}
