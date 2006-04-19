#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "include/plugin_base.h"
#include <Ecore.h>
#include "include/entropy.h"
#include "entropy_alert.h"
#include <Ecore_X.h>

#ifdef __GLIBC__
#include <execinfo.h>
#endif


void
entropy_sigseg_act(int x, siginfo_t *info, void *data)
{
   void* array[255];
   size_t size;
	
   write(2, "**** SEGMENTATION FAULT ****\n", 29);
   write(2, "**** Printing Backtrace... *****\n\n", 34);

#ifdef __GLIBC__
  
   size = backtrace(array, 255);
   backtrace_symbols_fd(array, size, 2);

#endif

   ecore_x_pointer_ungrab();
   ecore_x_keyboard_ungrab();
   ecore_x_ungrab();
   ecore_x_sync();

   entropy_alert_show("This is very bad. Entropy has segfaulted.\n"
		"This is not meant to happen and is likely a sign of a\n"
		"bug in Entropy or the libraries it relies on.\n"
		"\n"
		"You can gdb attach to this process now to try debug it\n"
		"or you could exit, or just hit restart to try and get\n"
		"your file manager back the way it was. This may not always\n"
		"work perfectly, but it is the best we can do for now.\n"
		"\n"
		"Please help us debug this by compiling Entropy and\n"
		"all its dependency libraries with gdb debugging enabled\n"
		"and gdb attach to Entropy when this happens and try figure out\n"
		"what's going on and set us gdb backtraces, variable dumps\n"
		"etc.");

   exit (-11);
}


int main(int argc, char** argv) {
	entropy_alert_init(NULL);
	
	struct sigaction action;
	
	action.sa_sigaction = entropy_sigseg_act;
	action.sa_flags = SA_ONSTACK | SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGSEGV, &action, NULL);

   action.sa_sigaction = entropy_sigseg_act;
      action.sa_flags = SA_ONSTACK | SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
        sigemptyset(&action.sa_mask);
    sigaction(SIGABRT, &action, NULL);


	
	entropy_core_init(argc,argv);

	return 0;
}
