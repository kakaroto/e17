#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef Enesim_Rotator Enesim_Rotator_Lut[ENESIM_FORMATS][ENESIM_ROTATOR_ANGLES][ENESIM_FORMATS];
Enesim_Rotator_Lut *_rotators;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_rotator_init(void)
{
	int numcpu;

	enesim_cpu_get(&numcpu);
	_rotators = malloc(sizeof(Enesim_Rotator_Lut) * numcpu);
}
void enesim_rotator_shutdown(void)
{
	free(_rotators);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_rotator_register(Enesim_Cpu *cpu, Enesim_Rotator rot,
		Enesim_Format sfmt, Enesim_Rotator_Angle angle,
		Enesim_Converter_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Rotator_Lut *t;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_rotators[cpuid];
	*t[sfmt][angle][dfmt] = rot;
}

EAPI Eina_Bool enesim_rotator_op_get(Enesim_Operator *op,
		Enesim_Cpu *cpu, Enesim_Format sfmt, Enesim_Rotator_Angle angle,
		Enesim_Converter_Format dfmt)
{
	unsigned int cpuid;
	Enesim_Rotator_Lut *t;
	Enesim_Rotator r;

	cpuid = enesim_cpu_id_get(cpu);
	t = &_rotators[cpuid];
	r = *t[sfmt][angle][dfmt];
	if (r)
	{
		op->id = ENESIM_OPERATOR_ROTATOR;
		op->cb = r;
		op->cpu = cpu;
		return EINA_TRUE;
	}
	else
		return EINA_FALSE;
}
