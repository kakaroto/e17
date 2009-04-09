#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef Enesim_Reflector Enesim_Reflector_Lut[ENESIM_FORMATS][ENESIM_DIRECTIONS][ENESIM_FORMATS];

Enesim_Reflector_Lut *_reflectors;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_reflector_init(void)
{
	int numcpu;

	enesim_cpu_get(&numcpu);
	_reflectors = malloc(sizeof(Enesim_Reflector_Lut) * numcpu);
}
void enesim_reflector_shutdown(void)
{
	free(_reflectors);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_reflector_register(Enesim_Cpu *cpu, Enesim_Reflector rfl,
		Enesim_Format sfmt, Enesim_Direction dir,
		Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Reflector_Lut *t;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_reflectors[cpuid];
	*t[sfmt][dir][dfmt] = rfl;
}

EAPI Eina_Bool enesim_reflector_op_get(Enesim_Operator *op, Enesim_Cpu *cpu,
		Enesim_Format sfmt, Enesim_Direction dir, Enesim_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Reflector_Lut *t;
	Enesim_Reflector r;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_reflectors[cpuid];
	r = *t[sfmt][dir][dfmt];
	if (r)
	{
		op->id = ENESIM_OPERATOR_REFLECTOR;
		op->cb = r;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
		return EINA_FALSE;
}
