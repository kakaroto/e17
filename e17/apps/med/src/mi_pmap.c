#include "menu.h"
#include "config.h"
#include "evaswin.h"

#include "mi_pmap.h"


void
med_drag_image( Window drag_win, E_Menu* m_in, E_Menu_Item *mi_in )
{
  Pixmap pmap /*, mask*/;
  Evas e;
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
#if 0
  ww = m_in->current.w;
  wh = m_in->current.h;
#endif
  ww = mi_in->size.w + m_in->sel_border.l + m_in->sel_border.r 
    + m_in->border.l + m_in->border.r;
  wh = mi_in->size.h  + m_in->sel_border.t + m_in->sel_border.b
    + m_in->border.t + m_in->border.b;

  /* Create image */
  im = imlib_create_image(ww, wh);
  imlib_context_set_image(im);
  /*imlib_image_set_has_alpha(1);
    imlib_context_set_blend(1);
  */
  imlib_image_clear();


  /* Create evas for rendering to image */
  font_dir = e_config_get("fonts");

  e = evas_new();
  evas_set_output_method(e, RENDER_METHOD_IMAGE);
  evas_set_output_image(e, im);
  evas_set_output_colors(e, max_colors);
  if (font_dir) evas_font_add_path(e, font_dir);
  /*evas_set_output(e, display, window, visual, colormap);*/
  evas_set_output_size(e, ww, wh);
  evas_set_output_viewport(e, 0, 0, ww, wh);
  evas_set_font_cache(e, font_cache);
  evas_set_image_cache(e, image_cache);

  /* Create a menu/menuitem duplicate of input m/mi */
  {
    m = NEW(E_Menu, 1);
    ZERO(m, E_Menu, 1);
    OBJ_INIT(m, e_menu_free);

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
	/*sub = e_menu_new();*/
	sub = NEW(E_Menu, 1);
	ZERO(sub, E_Menu, 1);
	OBJ_INIT(sub, e_menu_free);
	e_menu_set_padding_icon(sub, 2);
	e_menu_set_padding_state(sub, 2);
	subi = e_menu_item_new("Empty");
	e_menu_add_item(sub, subi);

	e_menu_item_set_submenu(mi, sub /*menu2*/);
	break;

      case TOOL_NEW_SEP:
	e_menu_item_set_separator(mi, 1);
	mi->size.min.w = 70;
	/*menuitem->size.min.h = 15;*/
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


  /* create output pixmap */
  pmap = e_pixmap_new(drag_win, ww, wh, 0);

  /* render to image */
  imlib_context_set_image(im);

  imlib_context_set_drawable(pmap);
  /*imlib_context_set_mask(mask);
   */
  imlib_context_set_blend(0);
  imlib_render_image_on_drawable(0, 0);

  /* show it */
  e_window_resize(drag_win, 
		  mi->size.w + m->sel_border.l + m->sel_border.r 
		  + m->border.l + m->border.r,
		  mi->size.h  + m->sel_border.t + m->sel_border.b
		  + m->border.t + m->border.b
		  );
  e_window_set_background_pixmap(drag_win, pmap);
  /*e_window_set_shape_mask(ic->view->drag.win, mask);*/
  e_window_ignore(drag_win);
  e_window_raise(drag_win);
  e_window_show(drag_win);
  e_pixmap_free(pmap);

}

/*eof*/
