
/**************************************************
 **               E  -  N O T E S                **
 **                                              **
 **  The contents of this file are released to   **
 **  the public under the General Public Licence **
 **  Version 2.                                  **
 **                                              **
 **  By  Thomas Fletcher (www.fletch.vze.com)    **
 **                                              **
 **************************************************/


#include "msgbox.h"

/* Making a Message Box */
void
msgbox(char *title, char *content, int x, int y, int width, int height)
{
	MsgBox          msgbox;
	MsgBox         *mb = &msgbox;

	/* Setup the Window */
	mb->win = ecore_evas_software_x11_new(NULL, 0, x, y, width, height);
	ecore_evas_title_set(mb->win, title);
	ecore_evas_show(mb->win);

	/* Setup the Canvas, Render-Method */
	mb->evas = ecore_evas_get(mb->win);
	evas_output_method_set(mb->evas,
			       evas_render_method_lookup(main_config->
							 render_method));

	/* Setup the EWL Widgets */
	mb->emb = ewl_embed_new();
	ewl_object_set_fill_policy((Ewl_Object *) mb->emb, EWL_FLAG_FILL_FILL);
	ewl_widget_set_appearance(mb->emb, "window");
	ewl_widget_show(mb->emb);

	mb->eo = ewl_embed_set_evas((Ewl_Embed *) mb->emb, mb->evas,
				    ecore_evas_software_x11_window_get(mb->
								       win));
	evas_object_name_set(mb->eo, "eo");
	evas_object_layer_set(mb->eo, 0);
	evas_object_move(mb->eo, 0, 0);
	evas_object_resize(mb->eo, width, height);
	evas_object_show(mb->eo);

	mb->vbox = ewl_vbox_new();
	ewl_container_append_child((Ewl_Container *) mb->emb, mb->vbox);
	ewl_object_set_fill_policy((Ewl_Object *) mb->vbox, EWL_FLAG_FILL_FILL);
	ewl_widget_show(mb->vbox);

	mb->msg = ewl_text_new(content);
	ewl_container_append_child((Ewl_Container *) mb->vbox, mb->msg);
	ewl_widget_show(mb->msg);

	mb->hbox = ewl_hbox_new();
	ewl_container_append_child((Ewl_Container *) mb->vbox, mb->hbox);
	ewl_object_set_fill_policy((Ewl_Object *) mb->hbox,
				   EWL_FLAG_FILL_HFILL);
	ewl_widget_show(mb->hbox);

	mb->okbtn = ewl_button_new("Ok.");
	ewl_container_append_child((Ewl_Container *) mb->hbox, mb->okbtn);
	ewl_widget_show(mb->okbtn);

	/* Ecore Callbacks */
	ecore_evas_callback_resize_set(mb->win, msgbox_resize);
	ecore_evas_callback_delete_request_set(mb->win, msgbox_close);
	ecore_evas_callback_destroy_set(mb->win, msgbox_close);

	/* EWL Callbacks */
	ewl_callback_append(mb->okbtn, EWL_CALLBACK_MOUSE_DOWN,
			    (void *) msgbox_okbtn_clicked, mb->win);

	return;
}


/* Callbacks */
void
msgbox_resize(Ecore_Evas * ee)
{
	int             x, y, w, h;

	ecore_evas_geometry_get(ee, &x, &y, &w, &h);
	evas_object_resize(evas_object_name_find(ecore_evas_get(ee), "eo"),
			   w, h);
	return;
}

void
msgbox_close(Ecore_Evas * ee)
{
	ecore_evas_free(ee);
	return;
}

void
msgbox_okbtn_clicked(Ewl_Widget * widget, void *ev_data, void *data)
{
	msgbox_close((Ecore_Evas *) data);
	return;
}
