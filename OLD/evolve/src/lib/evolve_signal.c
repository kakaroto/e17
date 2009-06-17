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

#include <string.h>
#include <dlfcn.h>

#include "evolve_private.h"

extern void *handle;

/* create a new internal evovle signal */
Evolve_Signal *evolve_signal_new(char *name, char *emission, Evolve *evolve)
{
   Evolve_Signal *esig;
   
   esig = calloc(1, sizeof(Evolve_Signal));
   esig->name = strdup(name);
   esig->emit = strdup(emission);
   esig->evolve = evolve;
   eina_hash_add(evolve->emissions, emission, esig);
   return esig;
}

/* callback that runs for internal signals and in turn calls wigget specific callbacks for that signal */
void evolve_signal_emit_cb(void *data, Etk_Object *obj)
{
   Evolve_Signal *esig;
   Eina_List *cbs;
   Eina_List *l;
   
   esig = data;
   
   if (!(cbs = eina_hash_find(esig->evolve->callbacks, esig->emit)))
     return;
   
   for (l = cbs; l; l = l->next)
     {
	Evolve_Signal_Callback *cb;
	
	cb = l->data;
	if(cb->func)
	  cb->func(esig->emit, cb->data);
     }
}

/* connect a callback to a custom evolve signal */
void evolve_signal_connect(Evolve *evolve, char *emission, void (*callback)(char *emission, void *data), void *data)
{
   Eina_List *l = NULL;
   Evolve_Signal_Callback *sig_cb;
   
   if (!evolve || !emission || !callback)
     return;
      
   sig_cb = calloc(1, sizeof(Evolve_Signal_Callback));   
   sig_cb->func = callback;
   sig_cb->data = data;   
   
   if ((l = eina_hash_find(evolve->callbacks, emission)))
     l = eina_list_append(l, sig_cb);     
   else
     {
	l = eina_list_append(l, sig_cb);
	eina_hash_add(evolve->callbacks, emission, l);
     }
}
