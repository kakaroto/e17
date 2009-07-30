#ifndef EON_SCRIPT_H_
#define EON_SCRIPT_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_SCRIPT_FILE_CHANGED "fileChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_SCRIPT_FILE;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Script_Private Eon_Script_Private;
struct _Eon_Script
{
	Ekeko_Object parent;
	Eon_Script_Private *prv;
};

typedef struct _Eon_Script_Module
{
	void * (*init)(void);
	void (*shutdown)(void *data);
	Eina_Bool (*load)(void *data, const char *file);
	Eina_Bool (*execute)(void *data, const char *fname, Ekeko_Object *ref);
} Eon_Script_Module;
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_script_type_get(void);
EAPI Eon_Script * eon_script_new(void);
EAPI void eon_script_file_set(Eon_Script *e, const char *file);

#endif /* EON_SCRIPT_H_ */
