#include <signal.h>
#include <setjmp.h>
#include <tables/deref.h>
#include <pin.H>

static jmp_buf current_env;

static void segv_handler(int signum) {

   DEBUGL(LOG("Caught a SIGSEGV in dereference_memory()"));
   longjmp(current_env,1);

}

Generic dereference_memory (Generic *addr,bool *is_segv) {

   Generic value = -1;
   struct sigaction new_action, old_action;

   new_action.sa_handler = segv_handler;
   sigemptyset (&new_action.sa_mask);
   new_action.sa_flags = 0;

   *is_segv = false;
   sigaction (SIGSEGV, NULL, &old_action);

   sigaction (SIGSEGV, &new_action, NULL);
   if (!setjmp(current_env))
      value = *addr;
   else
      *is_segv = true;

   sigaction(SIGSEGV,&old_action,NULL);

   return value;
}
