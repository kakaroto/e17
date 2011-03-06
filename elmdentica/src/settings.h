/* vim:tabstop=4
 * Copyright © 2009-2010 Rui Miguel Silva Seabra <rms@1407.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ELM_DENTICA_SETTINGS
#define ELM_DENTICA_SETTINGS

typedef struct _settings {
	Eina_Bool online;
	int browser;
	char *browser_name;
	char *browser_cmd;
	int max_messages;
	Eina_Bool rel_timestamps;
	Ecore_Timer *rel_ts_timer;
    int update_interval;
    int update_interval_val;
    Ecore_Timer *update_timer;
} Settings;

void ed_settings_init(int argc, char ** argv);
void ed_settings_shutdown(void);

void on_settings(void *data, Evas_Object *obj, void *event_info);

#endif
