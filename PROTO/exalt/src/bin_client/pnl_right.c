/*
 * =====================================================================================
 *
 *       Filename:  pnl_right.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/02/09 17:55:11 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "pnl_right.h"
#include "config.h"

void pnl_right_create()
{
    pnl_right.btn_center = elm_button_add(win);
    elm_table_pack(table, pnl_right.btn_center, 7, 1, 16, 10);
    evas_object_size_hint_weight_set(pnl_right.btn_center, 1.0, 1.0);

    pnl_right.btn_wired = pnl_right.btn_center;

    pnl_right.background = elm_icon_add(win);
    elm_icon_file_set(pnl_right.background, ICONS_LOGO, NULL);
    elm_icon_scale_set(pnl_right.background, 1,1);
    elm_table_pack(table, pnl_right.background, 7, 1, 16, 10);
    evas_object_show(pnl_right.background);
    evas_object_size_hint_weight_set(pnl_right.background, 1.0, 1.0);
    evas_object_size_hint_align_set(pnl_right.background, -1.0, -1.0);

}

