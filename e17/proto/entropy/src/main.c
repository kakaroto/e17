#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "include/plugin_base.h"
#include <Ecore.h>
#include "include/entropy.h"
#include <Ewl.h>

void
entropy_sigseg_act(int x, siginfo_t *info, void *data)
{
   void *array[255];
   size_t size;
   
   write(2, "**** SEGMENTATION FAULT ****\n", 29);
   write(2, "**** Printing Backtrace... *****\n\n", 34);

   usleep(50000000);
}


int main() {
	struct sigaction action;
	
	action.sa_sigaction = entropy_sigseg_act;
	action.sa_flags = SA_ONSTACK | SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGSEGV, &action, NULL);

	
	entropy_core* core = entropy_core_init();

	for (;;) {}

	return 0;
}
