#include "menu.h"
#include "menubuild.h"
#include "config.h"
#include "evaswin.h"

#include "mi_pmap.h"
#include "Evas.h"
#include "Evas_Engine_Software_X11.h"

void
med_drag_image( Window drag_win, E_Menu* m_in, E_Menu_Item *mi_in )
{
  Pixmap pmap, mask;
  Evas * e;
  int max_colors = 216;
  int font_cache = 1024 * 1024;
  int image_cache = 8192 * 1024;
  char *font_dir;
  Imlib_Image im;
  E_Menu *m, *sub;
  E_Menu_Item *mi, *subi;
  int ww,wh;
  int tt;

  /* Get output size */
  ww = mi_in->size.w + m_in->sel_border.l + m_in->sel_border.r 
    + m_in->border.l + m_in->border.r;
  wh = mi_in->size.h  + m_in->sel_border.t + m_in->sel_border.b
    + m_in->border.t + m_in->border.b;

  font_dir = e_config_get("fonts");
  e = evas_new();
  evas_output_method_set(e, 
			 evas_render_method_lookup("software_x11"));
  evas_font_path_append(e, font_dir);
  evas_output_size_set(e, ww, wh);
  evas_output_viewport_set(e, 0, 0, ww, wh);
  evas_font_cache_set(e, font_cache);
  evas_image_cache_set(e, image_cache);
  {
    Evas_Engine_Info_Software_X11 *einfo;
    XSetWindowAttributes att;

    pmap = ecore_pixmap_new(drag_win, ww, wh, 0);
    mask = ecore_pixmap_new(drag_win, ww, wh, 1);

    einfo = (Evas_Engine_Info_Software_X11 *) evas_engine_info_get(e);

    /* the following is specific to the engine */
    einfo->info.display = ecore_display_get();
    einfo->info.visual = DefaultVisual(einfo->info.display, DefaultScreen(einfo->info.display));
    einfo->info.colormap = DefaultColormap(einfo->info.display, DefaultScreen(einfo->info.display));
    
    att.background_pixmap = None;
    att.colormap = DefaultColormap(einfo->info.display, DefaultScreen(einfo->info.display));
    att.border_pixel = 0;
    att.event_mask = 0;
    
    einfo->info.drawable = pmap;
    einfo->info.mask = mask;

    einfo->info.depth = DefaultDepth(einfo->info.display, DefaultScreen(einfo->info.display));
    einfo->info.rotation = 0;
    einfo->info.debug = 0;
    evas_engine_info_set(e, (Evas_Engine_Info *) einfo);
  }

  /* Create a menu/menuitem duplicate of input m/mi */
  {
    m = NEW(E_Menu, 1);
    ZERO(m, E_Menu, 1);

    e_object_init(E_OBJECT(m), (E_Cleanup_Func) e_build_menu_cleanup);

    m->evas = e;
    m->pmap_render = 1;
    e_menu_set_background(m);
    e_menu_set_padding_icon(m, 2);
    e_menu_set_padding_state(m, 2);

    mi = e_menu_item_new(mi_in->str);

    /* if menu edit tool, just grab the type, otherwise */
    /* determine type from input menu */
    if( m_in->edit_tool )
      tt = m_in->edit_tool;
    else if( mi_in->submenu )
      tt = TOOL_NEW_SUBMENU;
    else if( mi_in->sep )
      tt = TOOL_NEW_SEP;

    switch( tt /*m_in->edit_tool*/ )
      {
      case TOOL_NEW_ITEM:
	break;

      case TOOL_NEW_SUBMENU:
	sub = NEW(E_Menu, 1);
	ZERO(sub, E_Menu, 1);
	e_object_init(E_OBJECT(sub), (E_Cleanup_Func) e_build_menu_cleanup);
	e_menu_set_padding_icon(sub, 2);
	e_menu_set_padding_state(sub, 2);
	subi = e_menu_item_new("Empty");
	e_menu_add_item(sub, subi);

	e_menu_item_set_submenu(mi, sub);
	break;

      case TOOL_NEW_SEP:
	e_menu_item_set_separator(mi, 1);
	mi->size.min.w = 70;
	break;
      }/*switch*/

    e_menu_add_item(m, mi);

    m->current.x = 0;
    m->current.y = 0;



    /* Render it on the image */
    e_menu_update_base(m);
    /* ? update_hides ? */
    e_menu_update_finish(m);

    evas_render(m->evas);

  }

  /* show it */
  ecore_window_resize(drag_win, 
		  mi->size.w + m->sel_border.l + m->sel_border.r 
		  + m->border.l + m->border.r,
		  mi->size.h  + m->sel_border.t + m->sel_border.b
		  + m->border.t + m->border.b
		  );
  ecore_window_set_background_pixmap(drag_win, pmap);
  /*e_window_set_shape_mask(ic->view->drag.win, mask);*/
  ecore_window_ignore(drag_win);
  ecore_window_raise(drag_win);
  ecore_window_show(drag_win);
  ecore_pixmap_free(pmap);

  ecore_pixmap_free(mask);
  evas_free(e);

}

/*eof*/
