#ifndef _EV_E_H
#define _EV_E_H

char *ThemeFile;

Evas_List *visible_elements_get(void);
Demo_Edje *edje_part_create(Etk_Tree *output, Etk_Mdi_Area *mdi_area,
	const char *file, char *name);

#endif
