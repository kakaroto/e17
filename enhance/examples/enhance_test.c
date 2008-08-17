/* Enhance
 * Copyright (C) 2006-2008 Hisham Mardam-Bey, Samuel Mendes, Ugo Riboni, Simon Treny
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
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <Enhance.h>
#include <Etk.h>

#include <string.h>

Enhance *en;

void
rip_only_clicked(Etk_Object *obj, void **data)
{
   char *foo = *data;
   printf("rip_only_clicked %s\n", foo);
}
    
int
on_window1_delete_event(void *data)
{
   etk_main_quit();
   enhance_shutdown();
   return 1;
}

int main(int argc, char **argv)
{      
   char *string1;
   char *string2;
   char *data;
   char **data2;
   
   string1 = strdup("this is some data\n");
   string2 = strdup("we should be seeing this\n");
   
   data = string1;
   
   enhance_init();
   etk_init(&argc, &argv);
   
   en = enhance_new();      
   enhance_callback_data_set(en, "rip_only_clicked", &data);

   /* if you're trying the dialog test, show it explicitly! */
   enhance_file_load(en, "window1", "buttons.glade");
   
   data2 = enhance_callback_data_get(en, "rip_only_clicked");
   *data2 = string2;

   Etk_Widget *entry;
   entry = enhance_var_get(en, "entry1");
   etk_widget_show(entry);
   
   etk_main();   
   etk_shutdown();
   
   enhance_free(en);  

   free(string1);
   free(string2);
   enhance_shutdown();
   etk_shutdown();
   
   return 0;
}
