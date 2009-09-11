#ifndef EON_STRIPES_H_
#define EON_STRIPES_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_STRIPES_COLOR1;
extern Ekeko_Property_Id EON_STRIPES_COLOR2;
extern Ekeko_Property_Id EON_STRIPES_THICKNESS1;
extern Ekeko_Property_Id EON_STRIPES_THICKNESS2;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Stripes_Private Eon_Stripes_Private;
struct _Eon_Stripes
{
	Eon_Paint parent;
	Eon_Stripes_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_stripes_type_get(void);
EAPI Eon_Stripes * eon_stripes_new(void);
EAPI Eon_Color eon_stripes_color1_get(Eon_Stripes *sq);
EAPI Eon_Color eon_stripes_color2_get(Eon_Stripes *sq);
EAPI void eon_stripes_color1_set(Eon_Stripes *sq, Eon_Color color);
EAPI void eon_stripes_color2_set(Eon_Stripes *sq, Eon_Color color);

EAPI void eon_stripes_thickness1_set(Eon_Stripes *s, float th);
EAPI float eon_stripes_thickness1_get(Eon_Stripes *s);
EAPI void eon_stripes_thickness2_set(Eon_Stripes *s, float th);
EAPI float eon_stripes_thickness2_get(Eon_Stripes *s);

#endif /* EON_STRIPES_H_ */
