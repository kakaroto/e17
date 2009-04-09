#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Enesim_Cpu **_cpus = NULL;
static unsigned int _num_cpus;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_cpu_init(void)
{
	enesim_cpu_smp_init();
	/* TODO any other target platform init */
}
void enesim_cpu_shutdown(void)
{

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Retrieve all the Enesim_Cpu available on the system
 */
EAPI Enesim_Cpu ** enesim_cpu_get(int *num)
{
	if (!num)
		return NULL;
	*num = _num_cpus;
	return _cpus;
}
/**
 * Register a new cpu
 */
EAPI void enesim_cpu_register(char *name, Enesim_Cpu_Create create,
		Enesim_Cpu_Delete delete, Enesim_Cpu_Run run, Eina_Bool host)
{
	Enesim_Cpu *cpu;

	if (!create || !delete || !run || !name)
		return;

	cpu = create();
	cpu->name = strdup(name);
	cpu->create = create;
	cpu->delete = delete;
	cpu->run = run;
	cpu->host = host;
	cpu->id = _num_cpus;

	_cpus = realloc(_cpus, (_num_cpus + 1) * sizeof(Enesim_Cpu *));
	_cpus[_num_cpus] = cpu;
	_num_cpus++;
}
/**
 *
 */
EAPI const char * enesim_cpu_name_get(Enesim_Cpu *cpu)
{
	return cpu->name;
}
/**
 *
 */
EAPI Eina_Bool enesim_cpu_is_host(Enesim_Cpu *cpu)
{
	return cpu->host;
}
/**
 *
 */
EAPI unsigned int enesim_cpu_id_get(Enesim_Cpu *cpu)
{
	return cpu->id;
}
