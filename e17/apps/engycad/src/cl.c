
#include "engy.h"
#include <locale.h>
#include <iconv.h>

#define FONTNAME "verdana"


typedef struct _CL CL;

struct _CL {
   char * text;
   char * hint;
   int len;
   int cpos;
   int bcpos;
   int hintlen;
   Evas_Object * obg;
   Evas_Object * otext;
   Evas_Object * ocu;
   Evas_Object * oclip;
   int voffs;
   int hoffs;
   Evas_Coord x;
   Evas_Coord y;
   Evas_Coord w;
   Evas_Coord h;
   Evas_Coord d;
};


Evas_Object * o_cl;
iconv_t dcd;
Evas_Object * current_editbox;
Evas_List * history = NULL;
Evas_List * curr_h = NULL;


Evas_Smart *smart;

Evas_Smart * cl_smart_get(void);
Evas_Object * cl_otext_new(Evas * e);
Evas_Object * cl_obg_new(Evas * e);
Evas_Object * cl_ocu_new(Evas * e);
Evas_Object * cl_oclip_new(Evas * e);
void cl_add(Evas_Object *o);
void cl_del(Evas_Object *o);
void cl_layer_set(Evas_Object *o, int l);
void cl_raise(Evas_Object *o);
void cl_lower(Evas_Object *o);
void cl_stack_above(Evas_Object *o, Evas_Object *above);
void cl_stack_below(Evas_Object *o, Evas_Object *below);
void cl_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
void cl_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
void cl_show(Evas_Object *o);
void cl_hide(Evas_Object *o);
void cl_color_set(Evas_Object *o, int r, int g, int b, int a);
void cl_clip_set(Evas_Object *o, Evas_Object *clip);
void cl_clip_unset(Evas_Object *o);

void cl_history_up(Evas_Object *o);
void cl_history_down(Evas_Object *o);
void cl_autocomplete(Evas_Object *o);

void bg_key_down_cb(void * data,  
	Evas *e, 
	Evas_Object *obj, 
	void *event_info);

void bg_main_key_down_cb(void * data,  
	Evas *e, 
	Evas_Object *obj, 
	void *event_info);

void bg_mouse_in(void * data,  
	Evas *e, 
	Evas_Object *obj, 
	void *event_info);

void bg_mouse_out(void * data,  
	Evas *e, 
	Evas_Object *obj, 
	void *event_info);


char * my_iconv(iconv_t dcd, const char *s);


int engy_cl_init(void)
{
   Evas * e;
   Evas_Object * o, * o1;
   
   char *s, *p, *from = NULL;
   setlocale(LC_ALL, "");
   s = setlocale(LC_MESSAGES, NULL);
   p = strrchr(s, '.');
   if (p)
   {
      from = p+1;
      dcd = iconv_open("utf8", from);
   }
   else
   {
      dcd = iconv_open("utf8", "koi8-r");
   }
	     
   
   e = shell->evas;

   o = cl_new(0 | 0x100);
   cl_cursor_pos_set(o, 1000);
   evas_object_color_set(o, 200, 200, 200, 200); // premul
   evas_object_show(o);
   
   o_cl = o;
   
/*   o = cl_new(0);

   evas_object_layer_set(o, 100);
   evas_object_resize(o, 340, 50);
   evas_object_color_set(o, 255, 255, 255, 1);
   evas_object_color_set(o, 0, 0, 0, 2);
   evas_object_color_set(o, 0, 0, 0, 3);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, 100, 20);
   evas_object_show(o);
   
   cl_text_set(o, "Здравствуйте!");
   cl_cursor_pos_set(o, 12);
//   cl_cursor_del(o);
//   cl_cursor_del(o);
//   cl_cursor_bsp(o);
//   cl_cursor_bsp(o);
//   cl_cursor_pos_set(o, 1);
//   cl_cursor_insert(o, "moo");
   cl_refresh(o);
//   evas_object_move(o, 20, 5);
   */
   return 0;
}


Evas_Smart *
cl_smart_get()
{
   if (smart) return smart;
   
   smart = evas_smart_new ("cl",
	   cl_add,
	   cl_del,
	   cl_layer_set,
	   cl_raise,
	   cl_lower,
	   cl_stack_above,
	   cl_stack_below,
	   cl_move,
	   cl_resize,
	   cl_show,
	   cl_hide,
	   cl_color_set,
	   cl_clip_set,
	   cl_clip_unset,
	   NULL
	   );
   return smart;
}


Evas_Object * cl_new(int opt)
{
   Evas_Object * ores;
   Evas * e;
   CL * cl;

   e = shell->evas;
   ores = evas_object_smart_add(e, cl_smart_get());

   cl = evas_object_smart_data_get(ores);

   evas_object_data_set(cl->obg, "parent", ores);

   if (opt & 0xff)
   {
      evas_object_text_font_set(cl->otext, FONTNAME, opt & 0xff);
   }

/*   if (opt & 0x100)// primary
   {
      evas_object_event_callback_add(cl->obg, EVAS_CALLBACK_KEY_DOWN, 
	      bg_main_key_down_cb, NULL);
      evas_object_focus_set(cl->obg, 1);
   }
   else // secondary
   {  
      evas_object_event_callback_add(cl->obg, EVAS_CALLBACK_KEY_DOWN, 
	      bg_key_down_cb, NULL);
      evas_object_event_callback_add(cl->obg, EVAS_CALLBACK_MOUSE_IN, 
	      bg_mouse_in, NULL);
      evas_object_event_callback_add(cl->obg, EVAS_CALLBACK_MOUSE_OUT, 
	      bg_mouse_out, NULL);
   }*/
  
   cl_hint_set(ores, "");

   return ores;
}


Evas_Object * cl_otext_new(Evas * e)
{
   Evas_Object * ores;

   ores = evas_object_text_add(e);
   evas_object_text_font_set(ores, FONTNAME, 10);
   evas_object_color_set(ores, 0, 0, 0, 255);
   evas_object_layer_set(ores, 101);
   evas_object_pass_events_set(ores, 1);

   return ores;
}

Evas_Object * cl_obg_new(Evas * e)
{
   Evas_Object * ores;

   ores = evas_object_rectangle_add(e);
   evas_object_resize(ores, 400, 50);
   evas_object_color_set(ores, ALPHA1, ALPHA1, ALPHA1, ALPHA1); // premul
   evas_object_layer_set(ores, 100);
   evas_object_pass_events_set(ores, 1);
   
   return ores;
}

Evas_Object * cl_ocu_new(Evas * e)
{
   Evas_Object * ores;

   ores = evas_object_rectangle_add(e);
   evas_object_resize(ores, 1, 20);
   evas_object_color_set(ores, 0, 0, 0, 255);
   evas_object_layer_set(ores, 102);
   evas_object_pass_events_set(ores, 1);

   return ores;
}

Evas_Object * cl_oclip_new(Evas * e)
{
   Evas_Object * ores;
   
   ores = evas_object_rectangle_add(e);
   evas_object_resize(ores, 492, 42);
   evas_object_color_set(ores, 255, 255, 255, 255);
   evas_object_layer_set(ores, 103);
   evas_object_pass_events_set(ores, 1);
   
   return ores;
}

void cl_add(Evas_Object *_o)
{
   Evas_Object * o;
   Evas * e;
   CL * cl;

   cl = (CL*)malloc(sizeof(CL));
   if (!cl)
      return;
   memset(cl, 0, sizeof(CL));

   cl->hoffs = 4;
   cl->voffs = 4;
   cl->d = 0;
   
   e = evas_object_evas_get(_o);
   
   o = cl_obg_new(e);
   cl->obg = o;
   
   o = cl_otext_new(e);
   cl->otext = o;
 
   o = cl_ocu_new(e);
   cl->ocu = o;

   o = cl_oclip_new(e);
   cl->oclip = o;

   evas_object_clip_set(cl->obg, cl->oclip);
   evas_object_clip_set(cl->otext, cl->oclip);
   evas_object_clip_set(cl->ocu, cl->oclip);

   cl->text = strdup("");   
   cl->hint = strdup("");

   evas_object_smart_data_set(_o, cl);
}

void cl_del(Evas_Object *_o)
{
   CL * cl;
   
   cl = evas_object_smart_data_get(_o);
   evas_object_del(cl->obg);
   evas_object_del(cl->otext);
   evas_object_del(cl->ocu);
   evas_object_del(cl->oclip);
   if(cl->text)
	   free(cl->text);
   if(cl->hint)
	   free(cl->hint);
   free(cl);
}

void cl_layer_set(Evas_Object *o, int l)
{
   CL * cl;
   
   cl = evas_object_smart_data_get(o);
   evas_object_layer_set(cl->obg, l);
   evas_object_layer_set(cl->otext, l+1);
   evas_object_layer_set(cl->ocu, l+2);
}

void cl_raise(Evas_Object *o)
{
   
}

void cl_lower(Evas_Object *o)
{
   
}

void cl_stack_above(Evas_Object *o, Evas_Object *above)
{
   
}

void cl_stack_below(Evas_Object *o, Evas_Object *below)
{
   
}

void cl_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
   CL * cl;
   
   cl = evas_object_smart_data_get(o);
   if (!cl)
      return;
  
   cl->x = x;
   cl->y = y;
   evas_object_move(cl->obg, x, y);
   evas_object_move(cl->otext, x + cl->hoffs, y + cl->voffs);
   evas_object_move(cl->oclip, x, y);
}

void cl_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   CL * cl;
   
   cl = evas_object_smart_data_get(o);
   if (!cl) return;
  
   cl->w = w;
   cl->h = h;
   evas_object_resize(cl->obg, w, h);
   evas_object_resize(cl->oclip, w, h);
   cl_refresh(o);
}

void cl_show(Evas_Object *o)
{
   CL * cl;

   cl = evas_object_smart_data_get(o);
   if (!cl) return;

   evas_object_show(cl->obg);
   evas_object_show(cl->otext);
   evas_object_show(cl->ocu);
   evas_object_show(cl->oclip);
}

void cl_hide(Evas_Object *o)
{
   CL * cl;
   
   cl = evas_object_smart_data_get(o);
   if (!cl) return;

   evas_object_hide(cl->obg);
   evas_object_hide(cl->otext);
   evas_object_hide(cl->ocu);
   evas_object_hide(cl->oclip);
}

void cl_color_set(Evas_Object *o, int r, int g, int b, int a)
{ 
   CL * cl;
   static int trigger = 0;
   
   cl = evas_object_smart_data_get(o);
   if (!cl) return;

   if (0 ==  (g | b | a))
   {
      trigger = r;
      return;
   }

   switch(trigger)
   {
       case 3: evas_object_color_set(cl->obg, r*a/255, g*a/255, b*a/255, a);
	       break;
       case 2: evas_object_color_set(cl->otext, r*a/255, g*a/255, b*a/255, a);
	       break;
       case 1: evas_object_color_set(cl->ocu, r*a/255, g*a/255, b*a/255, a);
	       break;
       case 0: evas_object_color_set(cl->oclip, r*a/255, g*a/255, b*a/255, a);
	       break;
   }
}

void cl_clip_set(Evas_Object *o, Evas_Object *clip)
{
   
}

void cl_clip_unset(Evas_Object *o)
{

}

/*void bg_key_down_cb(void * data,
	Evas *e,
	Evas_Object *obj,
	void *event_info)
{
   Ecore_X_Event_Key_Down * ee;
   Evas_Object * oparent;

   ee =(Ecore_X_Event_Key_Down*) ecore_event_current_event_get();
   if(!ee)
      return;
   
   oparent = (Evas_Object *)evas_object_data_get(obj, "parent");
   if (!oparent)
      return;

//   printf("%s %s\n", ee->keyname, ee->key_compose);
   
   if (!strcmp(ee->keyname, "BackSpace")) cl_cursor_bsp(oparent);
   else if (!strcmp(ee->keyname, "Delete")) cl_cursor_del(oparent);
   else if (!strcmp(ee->keyname, "Left")) cl_cursor_pos_bwd(oparent);
   else if (!strcmp(ee->keyname, "Right")) cl_cursor_pos_fwd(oparent);
   else if (!strcmp(ee->keyname, "Home")) cl_cursor_pos_set(oparent, 0);
   else if (!strcmp(ee->keyname, "End")) cl_cursor_pos_set(oparent, 0xffff);
//   else if (!strcmp(ee->keyname, "Tab")) engy_panel_feed(ee);
//   else if (!strcmp(ee->keyname, "Return")) engy_panel_feed(ee);
//   else if (!strcmp(ee->keyname, "KP_Enter")) engy_panel_feed(ee);
//   else if (!strcmp(ee->keyname, "Up")) engy_panel_feed(ee);
//   else if (!strcmp(ee->keyname, "Down")) engy_panel_feed(ee);
//   else if (! ee->key_compose) engy_panel_feed(ee);
   else 
   {	// insert input
      char * nt;

      if (!ee->key_compose)
	 return;
      
      nt = my_iconv(dcd, ee->key_compose);
      cl_cursor_insert(oparent, nt);
      free(nt);
   }

   cl_refresh(oparent);
}*/

int check_alias(const char *keyname, Evas_Modifier *mods)
{
	int mask = 0;
	char *s;
	char buf[4096];

	if(evas_key_modifier_is_set(mods, "Shift"))
		mask |= 1;
	if(evas_key_modifier_is_set(mods, "Control"))
		mask |= 2;
	if(evas_key_modifier_is_set(mods, "Alt"))
		mask |= 4;

	switch(mask)
	{
		case 0: return 0; break;
		case 1: s = "shift"; break;
		case 2: s = "ctrl"; break;
		case 3: s = "ctrl+shift"; break;
		case 4: s = "alt"; break;
		case 5: s = "alt+shift"; break;
		case 6: s = "ctrl+alt"; break;
		case 7: s = "ctrl+alt+shift"; break;
	}
	sprintf(buf, "%s+%s", s, keyname);
	s = _alias(buf);
	if(s)
	{
		gui_put_string(s);
		return 1;
	}

	return 0;
}

void cl_handle_key(Evas_Object *o, void *event_info)
{
	Evas_Object *oparent;
   
   Evas_Event_Key_Down * ee;

   oparent = o_cl;
   ee =(Evas_Event_Key_Down*) event_info;
   if(!ee)
      return;

   if(!strcmp(ee->keyname, "Escape"))
	   magnet_off_all();
   else if (!strcmp(ee->keyname, "BackSpace")) 
	   cl_cursor_bsp(oparent);
   else if (!strcmp(ee->keyname, "Delete")) 
	   cl_cursor_del(oparent);
   else if (!strcmp(ee->keyname, "Left")) 
	   cl_cursor_pos_bwd(oparent);
   else if (!strcmp(ee->keyname, "Right")) 
	   cl_cursor_pos_fwd(oparent);
   else if (!strcmp(ee->keyname, "Home")) 
	   cl_cursor_pos_set(oparent, 0);
   else if (!strcmp(ee->keyname, "End")) 
	   cl_cursor_pos_set(oparent, 0xffff);
   else if (!strcmp(ee->keyname, "Tab")) 
	   cl_autocomplete(oparent);
   else if (!strcmp(ee->keyname, "Up"))
	   cl_history_up(oparent);
   else if (!strcmp(ee->keyname, "Down"))
	   cl_history_down(oparent);
   else if (!strcmp(ee->keyname, "Return") || 
		   !strcmp(ee->keyname, "KP_Enter")) 
   {
	char *p, *s, *al;
	cl_text_get(oparent, &p);

	if(!curr_h)
	{
		s = DUP(p);
		history = evas_list_prepend(history, s);
	}
	else
	{
		curr_h = history;
		IF_FREE(curr_h->data);
		curr_h->data = DUP(p);
	}

	curr_h = NULL;

	s = DUP(p);
	al = _alias(s);
	if(!al)
	{
		gui_put_string(s);
	}
	else
	{
		gui_put_string(al);
		FREE(s);
	}

	cl_text_set(oparent, "");
   }
   else if(!check_alias(ee->keyname, ee->modifiers)) 
   {	// insert input
      char * nt;

      if(!ee->string)
	      return;

      nt = my_iconv(dcd, ee->string);
      cl_cursor_insert(oparent, nt);
      free(nt);
   }

   cl_refresh(oparent);
}
/*
void bg_mouse_in(void * data,  
	Evas *e, 
	Evas_Object *obj, 
	void *event_info)
{
   CL * cl;
   
   cl = evas_object_smart_data_get(o_cl);
   
   evas_object_focus_set(cl->obg, 0);   
   evas_object_focus_set(obj, 1);
}

void bg_mouse_out(void * data,  
	Evas *e, 
	Evas_Object *obj, 
	void *event_info)
{
   CL * cl;
   
   cl = evas_object_smart_data_get(o_cl);
   
   evas_object_focus_set(obj, 0);
   evas_object_focus_set(cl->obg, 1);   
}
*/


/*************************************************************/


void 
cl_text_set(Evas_Object * _o, const char * str)
{
   CL * cl;
   int sl, pos = 0, d = 0, i = 0;
   
   cl = evas_object_smart_data_get(_o);
   if (cl->text)
      free(cl->text);
   cl->text = NULL;
   if (!str)
      return;

   cl->text = strdup(str);
   sl = strlen(cl->text);

   while (pos < sl)
   {
      pos = evas_string_char_next_get(cl->text, pos, &d);
      i++;
   }
   cl->len = i-1;
   cl->cpos = cl->len+1;
   cl->bcpos = evas_string_char_prev_get(cl->text, pos, &d);
}

void cl_history_up(Evas_Object *_o)
{
	char *s, *p;
	cl_text_get(_o, &p);

	if(!curr_h)
	{
		s = DUP(p);
		history = evas_list_prepend(history, s);
		curr_h = history;
	}
	else
	{
		IF_FREE(curr_h->data);
		curr_h->data = DUP(p);
	}

	if(!curr_h->next)
		return;
	curr_h = curr_h->next;
	cl_text_set(_o, curr_h->data);
}

void cl_history_down(Evas_Object *_o)
{
	char *s, *p;
	cl_text_get(_o, &p);

	if(!curr_h)
	{
		s = DUP(p);
		history = evas_list_prepend(history, s);
		curr_h = history;
	}
	else
	{
		IF_FREE(curr_h->data);
		curr_h->data = DUP(p);
	}

	if(!evas_list_prev(curr_h))
		return;
	curr_h = evas_list_prev(curr_h);
	cl_text_set(_o, curr_h->data);
}

void cl_autocomplete(Evas_Object *_o)
{
	Evas_List *l;
	char *s, *s1;

	if(!history)
		return;
	cl_text_get(_o, &s1);
	if(!s1)
		return;

	for(l = history; l; l = l->next)
	{
		s = l->data;
		if(strncpy(s, s1, strlen(s1)))
			continue;

		cl_text_set(_o, s);
		return;
	}
}


void 
cl_hint_set(Evas_Object * _o, const char * str)
{
   CL * cl;
   int pos = 0;
   int len = 0;
   int d = 0;
   
   cl = evas_object_smart_data_get(_o);
   if (cl->hint)
      free(cl->hint);
   cl->hint = NULL;
   if (!str)
      return;

   cl->hint = strdup(str);
   cl->hintlen = strlen(str);

//   len = strlen(str);
/*   while (pos < len)
   {
      pos = evas_string_char_next_get(str, pos, &d);
      cl->hintlen++;
   }
*/
   cl_refresh(_o);	
}



void 
cl_text_get(Evas_Object * _o, char ** _str)
{
   CL * cl;
   
   cl = evas_object_smart_data_get(_o);
   if (!_str)
      return;

   *_str = cl->text;
}

void 
cl_cursor_pos_set(Evas_Object * _o, int _pos)
{
   CL * cl;
   int sl, pos = 0, d = 0, i = 0;
   
   cl = evas_object_smart_data_get(_o);
   if (!cl)
      return;
   if (!cl->text)
      return;

   sl = strlen(cl->text);
   if (0 > _pos)
      _pos = 0;
   
   while ((pos < sl) && (i != _pos))
   {
      pos = evas_string_char_next_get(cl->text, pos, &d);
      i++;
   }
   
   if (pos == sl) /* failed to find _pos */
   {
      cl->cpos = cl->len+1;
      cl->bcpos = evas_string_char_prev_get(cl->text, pos, &d);
   }
   else 
   {
      cl->cpos = _pos;
      cl->bcpos = pos;
   }
}



int  cl_cursor_pos_get(Evas_Object * _o)
{
   CL * cl;
   
   cl = evas_object_smart_data_get(_o);
   if (!cl)
      return 0;
   
   return cl->cpos;      
}    

void 
cl_cursor_pos_fwd(Evas_Object * _o)
{
   CL * cl;
   int i;

   cl = evas_object_smart_data_get(_o);
   if (!cl)
      return;
   
   cl_cursor_pos_set(_o, cl->cpos+1);
}

void 
cl_cursor_pos_bwd(Evas_Object * _o)
{
   CL * cl;
   int i;
   
   cl = evas_object_smart_data_get(_o);
   if (!cl)
      return;
   
   cl_cursor_pos_set(_o, cl->cpos-1); 
}

static void
cl_cursor_append(Evas_Object * _o, const char * _new_text)
{
   CL * cl;
   char buf[4096];
   
   cl = evas_object_smart_data_get(_o);
   if (!cl)
      return;
   memset(buf, 0, 4096);
   snprintf(buf, 4095, "%s%s", cl->text, _new_text);
   
   cl_text_set(_o, buf);
}

static void
cl_cursor_prepend(Evas_Object * _o, const char * _new_text)
{
   CL * cl;
   char buf[4096];
   char bbuf[4096];
   int dummy;
   
   cl = evas_object_smart_data_get(_o);
   if (!cl)
      return;
   memset(buf, 0, 4096);
   memset(bbuf, 0, 4096);
   strncpy(bbuf, cl->text, 4095);
   snprintf(buf, 4095, "%s", _new_text);
   
   cl_text_set(_o, buf);
   dummy = cl_cursor_pos_get(_o);
   
   memset(buf, 0, 4096);
   snprintf(buf, 4095, "%s%s", _new_text, bbuf);

   cl_text_set(_o, buf);
   cl_cursor_pos_set(_o, dummy);
 
}

void 
cl_cursor_insert(Evas_Object * _o, const char * _new_text)
{
   CL * cl;
   char buf[4096];
   char bbuf[4096];
   int dummy;

   cl = evas_object_smart_data_get(_o);
   if (!cl)
      return;

   if (cl->cpos == cl->len+1)
   {
      cl_cursor_append(_o, _new_text);      
      return;
   }

   if (cl->cpos == 0)
   {
      cl_cursor_prepend(_o, _new_text);
      return;
   }

   memset(buf, 0, 4096);
   memset(bbuf, 0, 4096);
   strncpy(bbuf, cl->text+cl->bcpos, 4095);
   
   cl->text[cl->bcpos] = '\0';
   snprintf(buf, 4095,"%s%s",cl->text, _new_text );
   cl_text_set(_o, buf);
   dummy = cl_cursor_pos_get(_o);
   memset(buf, 0, 4096);
   snprintf(buf, 4095, "%s%s", cl->text, bbuf);
   cl_text_set(_o, buf);
   cl_cursor_pos_set(_o, dummy);
}

void 
cl_cursor_del(Evas_Object * _o)
{
   CL * cl;
   int d=0, dummy, pos;
   char buf[4096];
   char bbuf[4096];
   char bbbuf[4096];
   
   cl = evas_object_smart_data_get(_o);
   
   if (!cl)
      return;

   if (cl->len+1 == cl->cpos)
      return;

   if (0 == cl->cpos)
   {	// cutting head
      dummy = evas_string_char_next_get(cl->text, cl->bcpos, &d);
      pos = cl->cpos;
      memset(buf, 0, 4096);
      snprintf(buf, 4095, "%s", cl->text+dummy);
      cl_text_set(_o, buf);
      cl_cursor_pos_set(_o, pos);
      return;
   }

   dummy = evas_string_char_next_get(cl->text, cl->bcpos, &d);
   pos = cl->cpos;
   memset(buf, 0, 4096);
   memset(bbuf, 0, 4096);
   memset(bbbuf, 0, 4096);
  
   snprintf(bbbuf, 4095, "%s", cl->text + dummy);
   
   cl->text[cl->bcpos] = '\0';
   snprintf(bbuf, 4095, "%s", cl->text);

   snprintf(buf, 4095, "%s%s", bbuf, bbbuf);
   cl_text_set(_o, buf);
   cl_cursor_pos_set(_o, pos);
}


void 
cl_cursor_bsp(Evas_Object * _o)
{
   CL * cl;
   int d=0, dummy, pos;
   char buf[4096];
   char bbuf[4096];

   cl = evas_object_smart_data_get(_o);

   if (!cl)
      return;
   
   if (0 == cl->cpos)
      return;

   if (cl->cpos == cl->len + 1)
   {      // cutting tail
      dummy = evas_string_char_prev_get(cl->text, cl->bcpos, &d);
      cl->text[cl->bcpos] = '\0';
      cl->bcpos = dummy;
      cl->len--;
      cl->cpos--;
      return;
   }

   memset(buf, 0, 4096);
   memset(bbuf, 0, 4096);
   strncpy(bbuf, cl->text + cl->bcpos, 4095);
   
   dummy = evas_string_char_prev_get(cl->text, cl->bcpos, &d);
   cl->text[dummy] = '\0';

   pos = cl->cpos;
   
   snprintf(buf, 4096, "%s%s", cl->text, bbuf);
   cl_text_set(_o, buf);

   cl_cursor_pos_set(_o, pos-1);
}


void 
cl_refresh(Evas_Object * _o)
{
   CL * cl;
   Evas_Coord x = 0, y = 0, w = 0, h = 0;
   Evas_Coord d = 0;
   
   cl = evas_object_smart_data_get(_o);
   ENGY_ASSERT(cl);

   char buf[4096];
   snprintf(buf, 4095, "%s%s", cl->hint, cl->text);

   evas_object_text_text_set(cl->otext, buf);
   evas_object_text_char_pos_get(cl->otext, 
		   cl->bcpos + cl->hintlen, &x, &y, &w, &h);


   if(x + 10> cl->w - cl->d)
   {
	   cl->d = cl->w - x - 10;
   }
   
   if(x + cl->d < 0)
	   cl->d = 0;

   if(cl->cpos > cl->len)
   {
	   evas_object_move(cl->ocu, cl->x + x + w + cl->hoffs - 1 + cl->d,  
			   cl->y + cl->voffs);
   }
   else
   {
	   evas_object_move(cl->ocu, cl->x + x + cl->hoffs - 1 + cl->d, 
			   cl->y + cl->voffs);
   }

   evas_object_move(cl->otext, cl->x + cl->d + cl->hoffs, 
		   cl->y + cl->voffs);
  
   evas_object_resize(cl->ocu, 1, h);
}




/*************************************************************/

char *
my_iconv(iconv_t dcd, const char *s)
{
   size_t              a, b, tmp;
   char                buf[4096];
   char               *res;
   char               *p1;
   char               *p2;
   
   if ((long)dcd == -1L)
      return NULL;
   if (!s)
      return NULL;
   
   a = strlen(s);
   
   if (4096 < a)
      return NULL;
   
   b = 4095;
   
   memset(buf, 0, 4096);
   
   p1 = (char*) s;
   p2 = buf;
   tmp = iconv(dcd, &p1, &a, &p2, &b);
   res = (char *)malloc(strlen(buf) + 1);

   if (!res)
      return NULL;
   
   strcpy(res, buf);

   return res;
}


void cl_configure(Evas_Coord w, Evas_Coord h)
{
	evas_object_move(o_cl, 0, h - 20);
	evas_object_resize(o_cl, w, 20);
}


void cl_shutdown(void)
{
	Evas_List *l;
	iconv_close(dcd);

	for(l = history; l; l = l->next)
	{
		IF_FREE(l->data);
	}
	
	evas_list_free(history);
}





