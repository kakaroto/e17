#ifndef MISC_HOUSEKEEPING_H
#define MISC_HOUSEKEEPING_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

void __attribute__ ((constructor)) eflplusplus_init(void);
void __attribute__ ((destructor)) eflplusplus_fini(void);

#endif
