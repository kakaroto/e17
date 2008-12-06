/*  Copyright (C) 2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of places.
 *  places is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  places is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with places.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef E_MOD_PLACES_H
#define E_MOD_PLACES_H


typedef struct _Volume
{
   const char *udi;
   const char *label;
   unsigned char mounted;
   const char *mount_point;
   const char *fstype;
   long long size;
   
   const char *bus;
   const char *drive_type;
   const char *model;
   const char *vendor;
   const char *serial;
   unsigned char removable;
   unsigned char requires_eject;
   
   E_DBus_Signal_Handler *sh_prop;
   unsigned char valid;
   unsigned char to_mount;
   unsigned char force_open;
   Evas_Object *obj;
   const char *icon;
   
}Volume;

void places_init(void);
void places_shutdown(void);
void places_update_all_gadgets(void);
void places_fill_box(Evas_Object *box);
void places_empty_box(Evas_Object *box);

void places_generate_menu(void *data, E_Menu *em);
void places_augmentation(void *data, E_Menu *em);


#endif
