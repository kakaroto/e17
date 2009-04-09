#ifndef ENESIM_REPEATER_H_
#define ENESIM_REPEATER_H_


/**
 * @defgroup Enesim_Repeater_Group Enesim_Repeater
 * @{
 */
typedef void (*Enesim_Repeater)(void *src, uint32_t x, uint32_t y, uint32_t len, void *dst);

typedef enum _Enesim_Repeater_Direction
{
	ENESIM_REPEATER_DIRECTION_NONE,/**< Don't repeat on any axis */
	ENESIM_REPEATER_DIRECTION_X, /**< Repeat on X axis */
	ENESIM_REPEATER_DIRECTION_Y, /**< Repeat on Y axis */
	ENESIM_REPEATER_DIRECTION_XY,
	ENESIM_REPEATER_DIRECTIONS,
} Enesim_Repeater_Direction;

EAPI void enesim_repeater_register(Enesim_Cpu *cpu, Enesim_Repeater rep,
		Enesim_Format sfmt, Enesim_Direction dir,
		Enesim_Format dfmt);
EAPI Eina_Bool enesim_repeater_operation_get(Enesim_Operator *op,
		Enesim_Cpu *cpu, Enesim_Format sfmt,
		Enesim_Repeater_Direction dir, Enesim_Format dfmt);
/**
 * @}
 */
#endif /* ENESIM_REPEATER_H_ */
