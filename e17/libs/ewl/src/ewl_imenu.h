#ifndef __EWL_IMENU_H__
#define __EWL_IMENU_H__

#define Ewl_IMenu Ewl_Menu_Base
#define EWL_IMENU(menu) ((Ewl_IMenu *) menu)

Ewl_Widget     *ewl_imenu_new(char *image, char *title);
void            ewl_imenu_init(Ewl_IMenu * menu, char *image, char *title);

#endif				/* __EWL_IMENU_H__ */
