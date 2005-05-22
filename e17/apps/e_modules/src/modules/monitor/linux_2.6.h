
#ifndef LINUX_2_6
#define LINUX_2_6

#include <stdio.h>

int cpu_usage_get(void);

int  net_in_usage_get(void);
long net_bytes_in_get(void);
int  net_out_usage_get(void);
long net_bytes_out_get(void);


#endif /* LINUX_2_6 */
