
/*
 * The spacer class is a simple widget that has no visible parts, it is used
 * to tweak spacing in an app.
 */

#ifndef __EWL_SPACER_H__
#define __EWL_SPACER_H__

typedef Ewl_Widget Ewl_Spacer;

#define EWL_SPACER(spacer) ((Ewl_Spacer *) spacer)

Ewl_Widget     *ewl_spacer_new();
void            ewl_spacer_init(Ewl_Spacer * s);

#endif				/* __EWL_SPACER_H__ */
