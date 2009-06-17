/* Evolve
 * Copyright (C) 2007-2008 Hisham Mardam-Bey 
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

#include <stdio.h>

#include "evolve_private.h"

int main(int argc, char **argv)
{
   Evolve *ev;   
   Etk_Widget *g_table;
   Etk_Widget *p_table;
   Etk_Widget *c_table;
   Etk_Widget *s_table;   
   
   etk_init(argc, argv);
   evolve_init();
   evolve_gui_init();
   
   ev = evolve_eet_load(PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet");
   if (!ev)
     {
	fprintf(stderr, "Can't load eet file! %s\n", PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet");
	exit(-1);
     }
   
   evolve_render(ev);
   
   g_table = evolve_etk_widget_find(ev, "general_table");
   p_table = evolve_etk_widget_find(ev, "packing_table");
   c_table = evolve_etk_widget_find(ev, "common_table");
   s_table = evolve_etk_widget_find(ev, "signal_table");
   
   evolve_gui_prop_dialog_tables_set(g_table, p_table, c_table, s_table);
   
   etk_main();
   evolve_shutdown();
   etk_shutdown();
   
   return 0;
}
