#ifndef __PROC_H__
#define __PROC_H__

#define PROC_CPU_TOTAL 0
#define PROC_CPU_USER  1
#define PROC_CPU_NICE  2
#define PROC_CPU_SYS   3
#define PROC_CPU_IDLE  4

#define PROC_MEM_TOTAL   0
#define PROC_MEM_USER    1
#define PROC_MEM_SHARED  2
#define PROC_MEM_BUF     3
#define PROC_MEM_FREE    4
#define PROC_MEM_USED    5

#define PROC_SWAP_TOTAL  0
#define PROC_SWAP_USED   1
#define PROC_SWAP_FREE   2

#define PROC_CPU_SIZE    5
#define PROC_MEM_SIZE    6
#define PROC_SWAP_SIZE   3

typedef struct _ProcInfo ProcInfo;

struct _ProcInfo
{
   unsigned            cpu[PROC_CPU_SIZE];
   unsigned            cpu_now[PROC_CPU_SIZE];
   unsigned            cpu_last[PROC_CPU_SIZE];

   unsigned            mem[PROC_MEM_SIZE];
   unsigned            swap[PROC_SWAP_SIZE];
};

void                proc_read_cpu(ProcInfo *);
void                proc_read_mem(ProcInfo *);

#endif
