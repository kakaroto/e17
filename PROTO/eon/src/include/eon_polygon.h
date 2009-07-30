/*
 * eon_polygon.h
 *
 *  Created on: 05-jun-2009
 *      Author: jl
 */

#ifndef EON_POLYGON_H_
#define EON_POLYGON_H_
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Polygon_Private Eon_Polygon_Private;
struct _Eon_Polygon
{
	Eon_Shape parent;
	Eon_Polygon_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_polygon_type_get(void);
EAPI Eon_Polygon * eon_polygon_new(Eon_Canvas *c);

#endif /* EON_POLYGON_H_ */
