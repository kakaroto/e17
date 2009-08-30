
#ifndef EON_TEXT_H_
#define EON_TEXT_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_TEXT_STR_CHANGED "strChanged"
#define EON_TEXT_X_CHANGED "xChanged"
#define EON_TEXT_Y_CHANGED "yChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_TEXT_STR;
extern Ekeko_Property_Id EON_TEXT_X;
extern Ekeko_Property_Id EON_TEXT_Y;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Text_Private Eon_Text_Private;
struct _Eon_Text
{
	Eon_Shape parent;
	Eon_Text_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_text_type_get(void);
EAPI void eon_text_str_set(Eon_Text *t, const char *str);
EAPI void eon_text_x_rel_set(Eon_Text *t, int x);
EAPI void eon_text_x_set(Eon_Text *t, int x);
EAPI void eon_text_y_set(Eon_Text *t, int y);
EAPI void eon_text_y_rel_set(Eon_Text *t, int y);

#endif /* EON_TEXT_H_ */
