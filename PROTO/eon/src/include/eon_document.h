/*
 * eon_document.h
 *
 *  Created on: 03-feb-2009
 *      Author: jl
 */

#ifndef EON_DOCUMENT_H_
#define EON_DOCUMENT_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_DOCUMENT_SIZE_CHANGED "sizeChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_DOCUMENT_SIZE;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Document_Private Eon_Document_Private;
struct _Eon_Document
{
	Ekeko_Object parent;
	Eon_Document_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_document_type_get(void);
EAPI Eon_Document * eon_document_new(const char *name, int w, int h, const char *options);
EAPI Eon_Canvas * eon_document_canvas_get(Eon_Document *d);
EAPI Ekeko_Object * eon_document_object_get_by_id(Eon_Document *d, const char *id);
EAPI void eon_document_size_get(Eon_Document *d, int *w, int *h);
EAPI void eon_document_resize(Eon_Document *d, int w, int h);
EAPI void eon_document_pause(Eon_Document *d);
EAPI void eon_document_play(Eon_Document *d);

#endif /* EON_DOCUMENT_H_ */
