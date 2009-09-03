
#ifndef EON_TEXT_H_
#define EON_TEXT_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_TEXT_STR_CHANGED "strChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_TEXT_STR;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Text_Private Eon_Text_Private;
struct _Eon_Text
{
	Eon_Square parent;
	Eon_Text_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_text_type_get(void);
EAPI void eon_text_string_set(Eon_Text *t, const char *str);
EAPI const char * eon_text_string_get(Eon_Text *t);

#endif /* EON_TEXT_H_ */
