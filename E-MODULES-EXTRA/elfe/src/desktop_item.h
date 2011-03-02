#ifndef _DESKTOP_ITEM_H_
#define _DESKTOP_ITEM_H_

Evas_Object *elfe_desktop_item_add(Evas_Object *parent,
                                   int row, int col,
                                   const char *name,
                                   Elfe_Desktop_Item_Type type,
                                   E_Gadcon *gc);
void elfe_desktop_item_pos_get(Evas_Object *obj, int *col, int *row);


#endif /* _DESKTOP_ITEM_H_ */
