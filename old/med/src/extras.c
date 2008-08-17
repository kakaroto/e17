#include "entry.h"
#include "menu.h"
#include "menubuild.h"
#include "config.h"
#include "util.h"
#include "evaswin.h"

#include "extras.h"

static void new_med_entry(Evas * e, E_Entry *entry, Evas_Object * eet, int x, int y, int w, const char *);
static void med_entry_key_down(Ecore_Event * ev);
static void
med_entry_mouse_down(Ecore_Event * ev);
static void 
med_entry_mouse_down_cb( void *_data, Evas * _e, Evas_Object * _o,
			 int _b, int _x, int _y);
static void
med_return_cb(E_Entry *_entry, void *_data);

void
med_renumber_menu(E_DB_File *db, int from, int to);
void
med_del_submenu_tree(E_DB_File *db, int sm, int *num_m);
E_DB_File *
med_db_open();
int
get_menu_count( E_DB_File *db, int mnum );
void
set_menu_count( E_DB_File *db, int mnum, int mc );
void
del_menu_count( E_DB_File *db, int mnum );
int
get_num_menus( E_DB_File *db );
void
set_num_menus( E_DB_File *db, int mc );
E_Menu_Item *
get_mi( E_DB_File *db, int n1, int n2 );
void
set_mi( E_DB_File *db, int n1, int n2, E_Menu_Item *mnew );
void
del_mi( E_DB_File *db, int num, int i2 );
void
free_mi( E_Menu_Item *mi );
char *
med_util_extract_filename(const char *fil);
static void 
med_entry_in_cb(void *_data, Evas * _e, Evas_Object * _o, void *_event);
static void 
med_entry_out_cb(void *_data, Evas * _e, Evas_Object * _o, void *_event);
static void
med_entry_check_dnd_status(E_Entry *entry, int enter);


static Evas * e;
static E_Entry *ee_text, *ee_exe, *ee_icon, *ee_script, *ee_subm, *ee_test;
static E_Menu_Item current_mi;
static Evas_Object *ee_grad_mi, *ee_grad_sm, *e_icon;
E_Entry *med_drop_pending = NULL;


pid_t
e_exec_run(char *exe)
{
  /* stub */

  /* printf( "Exec: %s\n", exe ); */

  return 0;
}


void pop_menu( int x, int y )
{
  /* from view.c:e_bg_up_cb() */

		  static E_Build_Menu *buildmenu = NULL;

		  /* Needs to move, only do once. */
		     e_menu_init();


		  
		  if (!buildmenu)
		    {
		       char *apps_menu_db;
		       
		       apps_menu_db = e_config_get("apps_menu");
		       if (apps_menu_db) buildmenu = e_build_menu_new_from_db(apps_menu_db);
		    }
		  if (buildmenu)
		    {
		       static E_Menu *menu = NULL;
		       menu = buildmenu->menu;
		       if (menu)
			 {
			   
			   buildmenu->x = x;
			   buildmenu->y = y;
			   buildmenu->visible = 1;
			   menu->always_visible = 1;
			   buildmenu->edit_target = 1;
			   menu->edit_target = 1;

			 e_menu_show_at_mouse(menu, x, y, 0);
			 /* e_menu_show_at_mouse(menu, ev->rx, ev->ry, ev->time); */
			 }
		    }

}

static void
new_med_entry(Evas * e, E_Entry *entry, Evas_Object * eet, int x, int y, int w, const char *text)
{
  char *buf;

  e_entry_resize(entry, w, 18 );
  /* Only sets min_size (sometimes...) */
  /* e_entry_set_size(entry, 200, 40); */
  e_entry_move(entry, x, y);
  e_entry_set_evas(entry, e);
  e_entry_set_text(entry, text );
  e_entry_set_layer(entry, 110);
  e_entry_set_enter_callback(entry, med_return_cb, NULL);
  /*  e_entry_show(entry);*/

  evas_object_event_callback_add(entry->event_box, EVAS_CALLBACK_MOUSE_IN, med_entry_in_cb, entry);
  evas_object_event_callback_add(entry->event_box, EVAS_CALLBACK_MOUSE_OUT, med_entry_out_cb, entry);


  buf = strdup(text);
  evas_object_text_text_set(eet, buf);
  evas_object_color_set(eet, 255, 255, 255, 255);
  evas_object_move(eet, x, y-15 );
  evas_object_layer_set(eet, 110);
  evas_object_show(eet);

}

void
med_entry_text( const char *text, const char *exe, const char *icon, const char *script )
{
  e_entry_set_text(ee_text, text ? text : ""  );
  e_entry_set_text(ee_exe, exe ? exe : "" );
  e_entry_set_text(ee_icon, icon ? icon : "" );
  e_entry_set_text(ee_script, script ? script : "" );  
}

void
med_setup_entries(Evas * e_in)
{
  E_Entry *ee;
  Evas_Object * eet;
  int iw, ih;

  e = e_in;

  ee = e_entry_new();
  eet = evas_object_text_add(e);
  evas_object_text_font_set(eet, "borzoib", 8);
  evas_object_text_text_set(eet, "");
  new_med_entry(e, ee, eet, 200, 30, 100, "Text" );
  ee->drop_style = STRIP_PATH;
  ee_text = ee;
  

  ee = e_entry_new();
  eet = evas_object_text_add(e);
  evas_object_text_font_set(eet, "borzoib", 8);
  evas_object_text_text_set(eet, "");
  new_med_entry(e, ee, eet, 200, 70, 200, "Exe" );
  ee->drop_style = DE_URL;
  ee_exe = ee;

  ee = e_entry_new();
  eet = evas_object_text_add(e);
  evas_object_text_font_set(eet, "borzoib", 8);
  evas_object_text_text_set(eet, "");
  new_med_entry(e, ee, eet, 200, 110, 240, "Icon" );
  ee->drop_style = DE_URL;
  ee_icon = ee;

  ee = e_entry_new();
  eet = evas_object_text_add(e);
  evas_object_text_font_set(eet, "borzoib", 8);
  evas_object_text_text_set(eet, "");
  new_med_entry(e, ee, eet, 200, 150, 240, "Script" );
  ee->drop_style = NONE;
  ee_script = ee;

  ee = e_entry_new();
  eet = evas_object_text_add(e);
  evas_object_text_font_set(eet, "borzoib", 8);
  evas_object_text_text_set(eet, "");
  new_med_entry(e, ee, eet, 200, 205 /*172*/, 100, "Submenu" );
  ee->drop_style = NONE;
  ee_subm = ee;

#if 0
  ee = e_entry_new();
  new_med_entry(e, ee, 200, 205, 100, "Testing..." );
  ee_test = ee;
  e_entry_show(ee);
  e_entry_set_focus(ee, 1);
#endif

  ecore_event_filter_handler_add(ECORE_EVENT_KEY_DOWN, med_entry_key_down);
  /*  ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_DOWN, med_entry_mouse_down);*/
  /*  
  evas_callback_add(ee_test->evas, ee_test->event_box, CALLBACK_MOUSE_DOWN,
		    med_entry_mouse_down_cb, ee_test);
  */

  ee_grad_mi = evas_object_gradient_add(e);
  evas_object_gradient_color_add(ee_grad_mi, 0,50,255, 255, 10);
  evas_object_gradient_color_add(ee_grad_mi, 0,0,0, 255, 10);

  evas_object_move(ee_grad_mi, 195, 10);
  evas_object_resize(ee_grad_mi, 250, 115-35+48+40);
  /*  evas_set_angle(e, ee_grad_mi, 0);*/
  evas_object_layer_set(ee_grad_mi, 104);
  evas_object_show(ee_grad_mi);

  ee_grad_sm = evas_object_gradient_add(e);
  evas_object_gradient_color_add(ee_grad_sm, 0,50,255, 255, 10);
  evas_object_gradient_color_add(ee_grad_sm, 0,0,0, 255, 10);

  evas_object_move(ee_grad_sm, 195, 145+40);
  evas_object_resize(ee_grad_sm, 250, 50);
  /*  evas_set_angle(e, ee_grad_sm, 0);*/
  evas_object_layer_set(ee_grad_sm, 104);
  evas_object_show(ee_grad_sm);

  e_icon = evas_object_image_add(e);
  evas_object_move(e_icon, 360, 280);
  iw = ih = 48;
  evas_object_resize(e_icon, iw, ih);
  evas_object_layer_set(e_icon, 102);
  evas_object_image_fill_set(e_icon, 0, 0, iw, ih);
  evas_object_show(e_icon);


}


static void 
med_entry_mouse_down_cb( void *_data, Evas * _e, Evas_Object * _o,
			 int _b, int _x, int _y)
{

  if(_b == 1 && _data)
    {
      e_entry_set_focus(_data, 1);
    }
}


void
med_display( MED_ENUM type, E_Menu_Item *mi)
{

  if( mi->menu->edit_target )
    {
      /* save info we need later */
      current_mi.db_num1 = mi->db_num1;
      current_mi.db_num2 = mi->db_num2;

      e_entry_clear_current_focus();

      switch(type)
	{
	case MED_SUBMENU:
	  e_entry_hide(ee_text);
	  e_entry_hide(ee_exe);
	  e_entry_hide(ee_icon);
	  e_entry_hide(ee_script);
	  e_entry_set_text(ee_subm, mi->str ? mi->str : ""  );
	  e_entry_show(ee_subm);

	  break;

	case MED_MENU_ITEM:
	  e_entry_show(ee_text);
	  e_entry_show(ee_exe);
	  e_entry_show(ee_icon);
	  e_entry_show(ee_script);
	  med_entry_text( mi->str, mi->exe, mi->icon, mi->script );
	  med_display_icon( mi->icon );
	  e_entry_hide(ee_subm);
	  
	  break;
	}
    }
}



static void
med_entry_key_down(Ecore_Event * ev)
{
   Ecore_Event_Key_Down          *e;

   e = ev->event;

   e_entry_handle_keypress( ee_text, e);
   e_entry_handle_keypress( ee_exe, e);
   e_entry_handle_keypress( ee_icon, e);
   e_entry_handle_keypress( ee_script, e);
   /*e_entry_handle_keypress( ee_test, e);*/
   e_entry_handle_keypress( ee_subm, e);
}


static void
med_entry_mouse_down(Ecore_Event * ev)
{
   Ecore_Event_Mouse_Down          *e;

   e = ev->event;

   return;
   UN(e);
}


void
med_commit_entry_changes(E_Entry *_entry)
{
  char key[4096] = "";
  char text[]="text", icon[]="icon", command[]="command", submenu[]="submenu";
  char script[]="script";

  char *apps_menu_db;
		       
  apps_menu_db = e_config_get("apps_menu");
  if (!apps_menu_db)
    {
      printf( "Error: failed to get config path for apps_menu db\n: %s\n", apps_menu_db);
      return;
    }


  if(_entry == ee_text || _entry == ee_subm)
    {
      sprintf(key, "/menu/%i/%i/%s", 
	      current_mi.db_num1, current_mi.db_num2, 
	      text);
    }
  else if(_entry == ee_exe)
    {
      sprintf(key, "/menu/%i/%i/%s", 
	      current_mi.db_num1, current_mi.db_num2, 
	      command);
    }
  else if(_entry == ee_icon)
    {
      sprintf(key, "/menu/%i/%i/%s", 
	      current_mi.db_num1, current_mi.db_num2, 
	      icon);
    }
  else if(_entry == ee_script)
    {
      sprintf(key, "/menu/%i/%i/%s", 
	      current_mi.db_num1, current_mi.db_num2, 
	      script);
    }

  /*  E_DB_DEL(apps_menu_db, key);*/
  E_DB_STR_SET(apps_menu_db, key, 
	       /*e_entry_get_text(_entry)*/
	       _entry->buffer
	       );
  E_DB_FLUSH;
}


static void
med_return_cb(E_Entry *_entry, void *_data)
{
  /* Do nothing - hide cursor */
}


int
med_delete_mi_at(E_Menu_Item *mi, E_Menu_Item *mi_track)
{
  E_DB_File *db;
  int mc, in, num_m, num_m2, renumber_sm = 0;
  int track_out_index = 0;
  E_Menu_Item *mnew;

  db = med_db_open();

  if(mi_track)
    {
      track_out_index = mi_track->db_num2;
    }

  mc = get_menu_count( db, mi->db_num1 );
  num_m = num_m2 = get_num_menus(db);

  if(mi->submenu_num || mi->submenu)
    {
      E_Menu_Item *sm;

      /* In case there aren't any entries. */
      if( mi->submenu->entries && mi->submenu->entries->data )
	{
	  sm = mi->submenu->entries->data;

	  med_del_submenu_tree(db, sm->db_num1 /*mi->submenu_num*/, &num_m);
	  set_num_menus(db, num_m);
	}
    }

  /* Read item+1 and write it to removed item */
  /* Loop down. */
  for(in = mi->db_num2+1; in < mc; in++ )
    {
      mnew = get_mi( db, mi->db_num1, in );
      set_mi( db, mi->db_num1, in-1, mnew );

      if(mi_track && mi_track->db_num2 == in)
	{
	  track_out_index = in-1;
	}
    }

  /* Delete last item & dec. menu item count */
  del_mi( db, mi->db_num1, mc-1);
  set_menu_count( db, mi->db_num1, mc-1 );

  /* close here - flush is done when loading menu again */
  e_db_close( db );

  return track_out_index;
}


void
med_insert_mi_at_dnd(E_Menu_Item *mi, int num_files, char **files)
{
  E_Menu_Item *mnew;
  char *fp, *exe;

  mnew = NEW( E_Menu_Item, 1);
  ZERO( mnew, E_Menu_Item, 1);

  /* do something with files */
  fp = e_util_de_url_and_verify(files[0]);

  if(fp)
    {
      mnew->exe = strdup(fp);
      exe = med_util_extract_filename(fp);
      if(exe)
	{
	  mnew->str = exe;
	}

      med_insert_mi(mi, mnew, -1);
    }
}


void
med_insert_mi_at(E_Menu_Item *mi, med_tool_type tt )
{
  E_Menu_Item *mnew;

  mnew = NEW( E_Menu_Item, 1);
  ZERO( mnew, E_Menu_Item, 1);

  switch( tt )
    {
    case TOOL_NEW_ITEM:
      /* all done */
      break;
    case TOOL_NEW_SUBMENU:
      /* I'm cheating here, outside med this is a E_Menu. */
      mnew->submenu = NEW( E_Menu_Item, 1);
      ZERO( mnew->submenu, E_Menu_Item, 1);
      mnew->new_one = 1;
      break;
    case TOOL_NEW_SEP:
      mnew->separator = 1;
      break;
    }

  med_insert_mi(mi, mnew, -1);
}


void
med_insert_mi(E_Menu_Item *mi, E_Menu_Item *mnew, int move_index )
{
  int mc, in, st_num;
  E_DB_File *db;
  E_Menu_Item *mprev;

  db = med_db_open();
  mc = get_menu_count( db, mi->db_num1 );

  if(move_index == -1)
    st_num = mi->db_num2;
  else
    st_num = move_index;

  /* save the entry we are replacing */
  /* insert mi and roll items down */
  for(in = st_num; in < mc; in++ )
    {
      mprev = get_mi( db, mi->db_num1, in );
      set_mi( db, mi->db_num1, in, mnew );
      mnew = mprev;
    }
  set_mi( db, mi->db_num1, mc, mnew );
  set_menu_count( db, mi->db_num1, mc+1 );

  /* close here - flush is done when loading menu again */
  e_db_close( db );
}


/* Delete a submenu and all it's children

algorithm: (caution, includes recursion)

del_tree
  submenu_num (num1)
    read mc
    loop mc
      read item of submenu
      if(submenu) del_tree
      del item
    del mc
    dec num_menus
    renumber menu at new num_menus to submenu_num
    return

test:
del 2
2 -> 3 (2 contains 3)
tot 6


-- 2,(6)
3,(6)
-3,(5)
5 ==> 3
-2,(4)
4 ==> 2

*/

void
med_del_submenu_tree(E_DB_File *db, int sm, int *num_m)
{
  int mc, in;
  E_Menu_Item *mnew;

  mc = get_menu_count( db, sm );

  for(in = 0; in < mc; in++)
    {
      mnew = get_mi( db, sm, in );
      if(mnew->submenu_num || mnew->submenu)
	{
	  med_del_submenu_tree(db, mnew->submenu_num, num_m);
	}
      del_mi( db, sm, in);
      free_mi(mnew);
    }
  del_menu_count(db, sm);
  (*num_m)--;
  
  if(sm!=*num_m) med_renumber_menu(db, *num_m, sm);
}


void
med_renumber_menu(E_DB_File *db, int from, int to)
{
  int mc, in, num_m, done=0, m_ind;
  E_Menu_Item *mnew;

  mc = get_menu_count( db, from );

  for(in = 0; in < mc; in++ )
    {
      mnew = get_mi( db, from, in );
      set_mi( db, to, in, mnew );
      del_mi( db, from, in);
    }
  set_menu_count(db, to, mc);
  del_menu_count(db, from);

  num_m = get_num_menus(db);

  for(m_ind=0; m_ind < num_m && !done; m_ind++)
    {
      mc = get_menu_count( db, m_ind );

      for(in = 0; in < mc && !done; in++ )
	{
	  mnew = get_mi( db, m_ind, in );

	  if(mnew->submenu_num == from)
	    {
	      mnew->submenu_num = to;
	      set_mi( db, m_ind, in, mnew );
	      done++;
	    }
	  else
	    free_mi(mnew);
	}
    }
}


E_DB_File *
med_db_open()
{
  char *db_f;
  E_DB_File *db;

  db_f = e_config_get("apps_menu");
  db = e_db_open( db_f );
  if (!db)
    {
      printf( "Error: failed to get config path for apps_menu db\n: %s\n", db);
      return;
    }
  return db;
}


int
get_menu_count( E_DB_File *db, int mnum )
{
  char key[4096] = "";
  int mc, ok;

  sprintf(key, "/menu/%i/count", mnum );

  e_db_int_get(db, key, &mc);

  return mc;
}


void
set_menu_count( E_DB_File *db, int mnum, int mc )
{
  char key[4096] = "";

  sprintf(key, "/menu/%i/count", mnum );

  e_db_int_set(db, key, mc);
}


void
del_menu_count( E_DB_File *db, int mnum )
{
  char key[4096] = "";

  sprintf(key, "/menu/%i/count", mnum );

  e_db_data_del(db, key);
}


int
get_num_menus( E_DB_File *db )
{
  char key[4096] = "";
  int mc, ok;

  e_db_int_get(db, "/menu/count", &mc);

  return mc;
}


void
set_num_menus( E_DB_File *db, int mc )
{
  char key[4096] = "";

  e_db_int_set(db, "/menu/count", mc);
}


E_Menu_Item *
get_mi( E_DB_File *db, int num, int i2 )
{
  char    buf[4096];
  int          ok, sub, sep;
  E_Menu_Item *mi;

  mi = NEW( E_Menu_Item, 1);
  if(!mi) printf( "Bad alloc!\n" );
  ZERO( mi, E_Menu_Item, 1);

	sprintf(buf, "/menu/%i/%i/text", num, i2);
	mi->str = e_db_str_get(db, buf);
	sprintf(buf, "/menu/%i/%i/icon", num, i2);
	mi->icon = e_db_str_get(db, buf);
	sprintf(buf, "/menu/%i/%i/command", num, i2);
	mi->exe = e_db_str_get(db, buf);
	sprintf(buf, "/menu/%i/%i/script", num, i2);
	mi->script = e_db_str_get(db, buf);
	sprintf(buf, "/menu/%i/%i/submenu", num, i2);
	ok = e_db_int_get(db, buf, &sub);
	if(ok)
	  mi->submenu_num = sub;
	/*sep = 0;*/
	sprintf(buf, "/menu/%i/%i/separator", num, i2);
	e_db_int_get(db, buf, &(mi->separator) );

	/*printf( "m %d,%d - sep %d\n", num, i2, mi->separator );*/

	return mi;
}


void
set_mi( E_DB_File *db, int num, int i2, E_Menu_Item *mi )
{
  char    buf[4096];
  int          ok, sub, sep;

  /* submenu mi */
  if( mi->submenu_num || mi->submenu )
    {
      int nm;
	  
      nm = get_num_menus(db);
      
      /* write keys */
      sprintf(buf, "/menu/%i/%i/text", num, i2);
      if( mi->str )
	e_db_str_set(db, buf, mi->str);
      else
	e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/submenu", num, i2);
      e_db_int_set(db, buf, (mi->new_one) ? nm : mi->submenu_num);

      /* delete keys if they exist */
      sprintf(buf, "/menu/%i/%i/icon", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/command", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/script", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/separator", num, i2);
      e_db_data_del(db, buf);

      /* If this is an edit/add, then create an empty menu */
      /* item under the new submenu. */
      if( mi->new_one )
	{
	  sprintf(buf, "/menu/%i/%i/text", nm, 0);
	  e_db_str_set(db, buf, "Empty");
	  sprintf(buf, "/menu/%i/count", nm);
	  e_db_int_set(db, buf, 1);

	  set_num_menus(db, nm+1);
	}
    }
  /* separator mi */
  else if( mi->separator )
    {
      /* write keys */
      sprintf(buf, "/menu/%i/%i/separator", num, i2);
      e_db_int_set(db, buf, 1);

      /* delete keys if they exist */
      sprintf(buf, "/menu/%i/%i/text", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/icon", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/command", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/script", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/submenu", num, i2);
      e_db_data_del(db, buf);
    }
  /* regular mi */
  else
    {
      /* write keys */
      sprintf(buf, "/menu/%i/%i/text", num, i2);
      if( mi->str )
	e_db_str_set(db, buf, mi->str);
      else
	e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/icon", num, i2);
      if( mi->icon )
	e_db_str_set(db, buf, mi->icon);
      else
	e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/command", num, i2);
      if( mi->exe )
	e_db_str_set(db, buf, mi->exe);
      else
	e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/script", num, i2);
      if( mi->script )
	e_db_str_set(db, buf, mi->script);
      else
	e_db_data_del(db, buf);

      /* delete keys if they exist */
      sprintf(buf, "/menu/%i/%i/submenu", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/separator", num, i2);
      e_db_data_del(db, buf);
    }


  free_mi( mi );
}


void
del_mi( E_DB_File *db, int num, int i2 )
{
  char    buf[4096];

      /* delete keys if they exist */
      sprintf(buf, "/menu/%i/%i/text", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/icon", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/command", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/script", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/submenu", num, i2);
      e_db_data_del(db, buf);
      sprintf(buf, "/menu/%i/%i/separator", num, i2);
      e_db_data_del(db, buf);

}


void
free_mi( E_Menu_Item *mi )
{
  if( mi )
    {
      IF_FREE(mi->str);
      IF_FREE(mi->icon);
      IF_FREE(mi->exe);
      IF_FREE(mi->script);
      /* I'm cheating here, normally this is a E_Menu, but not here in med. */
      free_mi((E_Menu_Item*)mi->submenu);
      FREE(mi);
    }
}


void
med_move_focus( E_Entry *entry, Ecore_Event_Key_Down *_e)
{
  int b=0, x=0, y=0;
  static Time handled_event_time = 0;

  /* kjb debug */
  /*printf( "Tab.\n" );*/

  /* *ee_text, *ee_exe, *ee_icon, *ee_script */

  if (_e->time == handled_event_time) return;

  if(entry == ee_text)
    {
      if(_e->mods && ECORE_EVENT_KEY_MODIFIER_SHIFT)
	{
	  med_e_entry_down_internal(entry->evas, ee_script, b, x, y, 1);
	}
      else
	{
	  med_e_entry_down_internal(entry->evas, ee_exe, b, x, y, 1);
	}
      handled_event_time = _e->time;
    }
  else if(entry == ee_exe)
    {
      if(_e->mods && ECORE_EVENT_KEY_MODIFIER_SHIFT)
	{
	  med_e_entry_down_internal(entry->evas, ee_text, b, x, y, 1);
	}
      else
	{
	  med_e_entry_down_internal(entry->evas, ee_icon, b, x, y, 1);
	}
      handled_event_time = _e->time;
    }
  else if(entry == ee_icon)
    {
      if(_e->mods && ECORE_EVENT_KEY_MODIFIER_SHIFT)
	{
	  med_e_entry_down_internal(entry->evas, ee_exe, b, x, y, 1);
	}
      else
	{
	  med_e_entry_down_internal(entry->evas, ee_script, b, x, y, 1);
	}
      handled_event_time = _e->time;
    }
  else if(entry == ee_script)
    {
      if(_e->mods && ECORE_EVENT_KEY_MODIFIER_SHIFT)
	{
	  med_e_entry_down_internal(entry->evas, ee_icon, b, x, y, 1);
	}
      else
	{
	  med_e_entry_down_internal(entry->evas, ee_text, b, x, y, 1);
	}
      handled_event_time = _e->time;
    }
}



void
med_display_icon( char *file )
{
  if( file )
    {
      evas_object_image_file_set(e_icon, file, "");
      evas_object_show(e_icon);
    }
  else
    evas_object_hide(e_icon);
}


char *
med_util_extract_filename(const char *fil)
{
  char *wk, *fn;

  wk = strrchr(fil, '/');
  if(wk)
    {
      fn = strdup(wk+1);
    }
  else
    {
      return NULL;
    }
}


static void 
med_entry_in_cb(void *_data, Evas * _e, Evas_Object * _o, void *_event)
{
   E_Entry *entry;

   entry = _data;

   med_entry_check_dnd_status(entry, 1);

   return;
   UN(_e);
   UN(_o);
   UN(_event);
}


static void 
med_entry_out_cb(void *_data, Evas * _e, Evas_Object * _o, void *_event)
{
   E_Entry *entry;

   entry = _data;

   med_entry_check_dnd_status(entry, 0);


   return;
   UN(_e);
   UN(_o);
   UN(_event);
}


Evas *
med_entry_get_evas(void)
{
  return e;
}


static void
med_entry_check_dnd_status(E_Entry *entry, int enter)
{
  if(enter)
    {
      if(med_check_dnd_motion_pend())
	{
	  /* flag drag in progress */
	  med_drop_pending = entry;
	  /* hilight the target */
	  med_entry_indicate_drop(entry, 1);
	}
    }
  else
    {
      med_drop_pending = NULL;
      med_entry_indicate_drop(entry, 0);
    }
}


void
med_entry_handle_drop(int dnd_num_files, char **dnd_files)
{
  char *fp, *exe;

  if(med_drop_pending)
    {
      switch(med_drop_pending->drop_style)
	{
	case NONE:
	  e_entry_set_text(med_drop_pending, dnd_files[0]);
	  break;
	case DE_URL:
	  fp = e_util_de_url_and_verify(dnd_files[0]);
	  e_entry_set_text(med_drop_pending, fp);
	  break;
	case STRIP_PATH:
	  fp = e_util_de_url_and_verify(dnd_files[0]);
	  exe = med_util_extract_filename(fp);
	  e_entry_set_text(med_drop_pending, exe);
	  break;
	}

      med_mark_entry_dirty(med_drop_pending);
      med_drop_pending = NULL;
    }
}


void
med_move_mi(E_Menu_Item *mi, E_Menu_Item *mi_dest)
{

  if(mi && mi_dest)
    {
      E_DB_File *db;
      E_Menu_Item *msav;

      db = med_db_open();
      msav = get_mi( db, mi->db_num1, mi->db_num2 );
      e_db_close( db );

      /* If seperate submenus, then just delete and add */
      if(mi->db_num1 != mi_dest->db_num1)
	{
	  med_delete_mi_at(mi, NULL);
	  med_insert_mi(mi_dest, msav, -1);
	}
      else
	{
	  int mdest_index;

	  mdest_index = med_delete_mi_at(mi, mi_dest);
	  med_insert_mi(mi_dest, msav, mdest_index);

	}
    }
}


/*eof*/
