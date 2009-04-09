#ifndef CPU_H_
#define CPU_H_

struct _Enesim_Cpu
{
	char *name;
	unsigned int id;
	Enesim_Cpu_Create create;
	Enesim_Cpu_Delete delete;
	Enesim_Cpu_Run run;
	Eina_Bool host;
};

void enesim_cpu_init(void);
void enesim_cpu_shutdown(void);

void enesim_cpu_smp_init(void);

#endif /* CPU_H_ */
