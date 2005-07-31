
#ifndef LINUX_2_6
#define LINUX_2_6

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Ecore.h>

int cpu_usage_get(void);

long mem_real_get(void);
long mem_swap_get(void);
int  mem_real_usage_get(void);
int  mem_swap_usage_get(void);

void mem_real_ignore_buffers_set(int ignore_buffers);
void mem_real_ignore_cached_set(int ignore_cached);

int  net_in_usage_get(void);
long net_bytes_in_get(void);
int  net_out_usage_get(void);
long net_bytes_out_get(void);

int  net_interfaces_get(Ecore_List * ifaces);
void net_interface_set(char* interface_name);

#endif /* LINUX_2_6 */
