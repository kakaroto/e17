/*
 * eon_external.h
 *
 *  Created on: 14-jul-2009
 *      Author: jl
 */

#ifndef EON_EXTERNAL_H_
#define EON_EXTERNAL_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_EXTERNAL_FILE_CHANGED "fileChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_EXTERNAL_FILE;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_External_Private Eon_External_Private;
struct _Eon_External
{
	Ekeko_Object parent;
	Eon_External_Private *private;
};

typedef struct _Eon_Parser
{
	Eina_Bool (*file_load)(Eon_Canvas *c, const char *file);
	Eina_Bool (*tree_get)(Eon_External *e, const char *file);
	Eina_Bool (*subtree_get)(Eon_External *e, const char *file);
} Eon_Parser;
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_external_type_get(void);
EAPI Eon_External * eon_external_new(Eon_Canvas *c);
EAPI void eon_external_file_set(Eon_External *e, const char *file);


#endif /* EON_EXTERNAL_H_ */
