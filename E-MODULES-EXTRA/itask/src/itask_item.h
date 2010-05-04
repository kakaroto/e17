#ifndef ITASK_ITEM_H
#define ITASK_ITEM_H

EAPI Itask_Item *itask_item_new(Itask *it, E_Border *bd);
EAPI int    itask_item_realize(Itask_Item *ic);
EAPI void   itask_icon_signal_emit(Itask_Item *ic, char *sig, char *src);
EAPI void 	itask_item_set_icon(Itask_Item *ic);
EAPI void   itask_item_del_icon(Itask_Item *ic);
EAPI void   itask_item_set_label(Itask_Item *ic);
EAPI void   itask_item_add_to_bar(Itask_Item *ic);
EAPI void   itask_item_remove_from_bar(Itask_Item *ic);
EAPI void   itask_item_remove(Itask_Item *ic);
EAPI void   itask_item_swap_to_bar(Itask_Item *ic);
EAPI void   itask_item_swap_oldest_from_bar(Itask *it);
EAPI void   itask_item_swap_youngest_from_menu(Itask *it);
EAPI int    itask_item_add_check(Itask *it, E_Border *bd);
/* EAPI int    itask_item_space_left(Itask *it, int add); */
#endif /*E_MOD_ITEM_H_*/
