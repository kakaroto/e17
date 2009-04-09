/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

#ifndef ENESIM_ROTATOR_H_
#define ENESIM_ROTATOR_H_

typedef enum _Enesim_Rotator_Angle
{
	ENESIM_ROTATOR_NONE,
	ENESIM_ROTATOR_90,
	ENESIM_ROTATOR_180,
	ENESIM_ROTATOR_270,
	ENESIM_ROTATOR_ANGLES,
} Enesim_Rotator_Angle;

typedef void (*Enesim_Rotator)(void *s, uint32_t x, uint32_t y, uint32_t len, void *dst);

#endif /* ENESIM_ROTATOR_H_ */
