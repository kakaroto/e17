
#ifndef LINUX_2_6
#define LINUX_2_6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int cpu_usage_get(void);

long mem_real_get(void);
long mem_swap_get(void);
int  mem_real_usage_get(void);
int  mem_swap_usage_get(void);

int  net_in_usage_get(void);
long net_bytes_in_get(void);
int  net_out_usage_get(void);
long net_bytes_out_get(void);


#endif /* LINUX_2_6 */
