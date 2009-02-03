/* EXCHANGE - a library to interact with exchange.enlightenment.org
 * Copyright (C) 2008 Massimiliano Calamelli
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef _EXCHANGE_SMART_H
#define _EXCHANGE_SMART_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


unsigned char exchange_smart_init(void);
void exchange_smart_shutdown(void);

EAPI Evas_Object* exchange_smart_object_add(Evas *evas);
EAPI unsigned char exchange_smart_object_run(Evas_Object *obj);
EAPI unsigned char exchange_smart_object_remote_group_set(Evas_Object *obj, const char *group);
EAPI unsigned char exchange_smart_object_local_path_set(Evas_Object *obj, const char *user_dir);
EAPI unsigned char exchange_smart_object_offset_set(Evas_Object *obj, int x, int y);
EAPI unsigned char exchange_smart_object_offset_get(Evas_Object *obj, int *x, int *y);
EAPI unsigned char exchange_smart_object_apply_cb_set(Evas_Object *obj, void (*func)(const char *path, void *data), void *data);

/**
 * @file exchange_smart.h
 * @brief This file contain a complete evas smart object ready to use.
 * See the source of exchange_smart_test.c for a complete usage example.
 *
 * Example:
 * @code
 * #include <Exchange.h>
 *
 * ...
 * // This function is called when the user press the 'use' button
 * static void
 * on_apply(const char *path, void *data)
 * {
      printf("THEME SELECTED: %s [%p]\n", path, data);
 * }
 *
 * ...
 * // Create a new smart object
 * Evas_Object *exsm;
 * exsm = exchange_smart_object_add(evas);
 * exchange_smart_object_remote_group_set(exsm, "Border");
 * exchange_smart_object_local_path_set(exsm, "/<home>/.e/e/themes", "<e_prefix>/data/themes/");
 * exchange_smart_object_mode_set(exsm, EXCHANGE_SMART_SHOW_BOTH);
 * exchange_smart_object_apply_cb_set(exsm, on_apply, NULL);
 *
 * // Make the smart object run now!
 * exchange_smart_object_run(exsm);
 *
 * @endcode
 */

#endif /* _EXCHANGE_SMART_H */
