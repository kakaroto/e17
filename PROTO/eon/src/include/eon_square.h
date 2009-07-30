/*
 * eon_square.h
 *
 *  Created on: 26-feb-2009
 *      Author: jl
 */

#ifndef EON_SQUARE_H_
#define EON_SQUARE_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_SQUARE_X_CHANGED "xChanged"
#define EON_SQUARE_Y_CHANGED "yChanged"
#define EON_SQUARE_W_CHANGED "wChanged"
#define EON_SQUARE_H_CHANGED "hChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_SQUARE_X;
extern Ekeko_Property_Id EON_SQUARE_Y;
extern Ekeko_Property_Id EON_SQUARE_W;
extern Ekeko_Property_Id EON_SQUARE_H;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Square_Private Eon_Square_Private;
struct _Eon_Square
{
	Eon_Shape parent;
	Eon_Square_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_square_type_get(void);
EAPI void eon_square_x_rel_set(Eon_Square *r, int x);
EAPI void eon_square_x_set(Eon_Square *r, int x);
EAPI void eon_square_y_set(Eon_Square *r, int y);
EAPI void eon_square_y_rel_set(Eon_Square *r, int y);
EAPI void eon_square_w_set(Eon_Square *r, int w);
EAPI void eon_square_w_rel_set(Eon_Square *r, int w);
EAPI void eon_square_h_set(Eon_Square *r, int h);
EAPI void eon_square_h_rel_set(Eon_Square *r, int h);
EAPI void eon_square_coords_get(Eon_Square *s, Eon_Coord *x, Eon_Coord *y, Eon_Coord *w, Eon_Coord *h);


#endif /* EON_SQUARE_H_ */
