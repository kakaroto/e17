#include "exhibit.h"

static void _ex_comment_save_clicked_cb(Etk_Object *obj, void *data);
static void _ex_comment_revert_clicked_cb(Etk_Object *obj, void *data);

void
_ex_comment_show(Exhibit *e)
{
   if(e->cur_tab->comment.visible)
     return;
   
   e->cur_tab->comment.vbox = etk_vbox_new(ETK_FALSE, 0);
   if(evas_list_count(e->tabs) == 1)
     {
	/* we only have 1 tab, ie, no notebook */
	etk_paned_child2_set(ETK_PANED(e->hpaned), e->cur_tab->comment.vbox, ETK_TRUE);
     }
   else
     {
	/* we have multiple tabs */
	etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->comment.vbox);
     }

   if (e->cur_tab->fit_window)
     etk_box_pack_start(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->alignment, ETK_TRUE, ETK_TRUE, 0);
   else
     etk_box_pack_start(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->scrolled_view, ETK_TRUE, ETK_TRUE, 0);
   
   e->cur_tab->comment.frame = etk_frame_new(_("Image Comments"));
   etk_box_pack_start(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->comment.frame, ETK_FALSE, ETK_FALSE, 3);
   
   e->cur_tab->comment.textview = etk_text_view_new();
   etk_widget_size_request_set(e->cur_tab->comment.textview, -1, 50);
   e->cur_tab->comment.save = etk_button_new_with_label("Save");
   etk_signal_connect("clicked", ETK_OBJECT(e->cur_tab->comment.save), ETK_CALLBACK(_ex_comment_save_clicked_cb), e);
   e->cur_tab->comment.revert = etk_button_new_with_label("Revert");
   etk_signal_connect("clicked", ETK_OBJECT(e->cur_tab->comment.revert), ETK_CALLBACK(_ex_comment_revert_clicked_cb), e);
   
   e->cur_tab->comment.vbox2 = etk_vbox_new(ETK_FALSE, 0);
   e->cur_tab->comment.hbox = etk_hbox_new(ETK_FALSE, 0);
   
   etk_container_add(ETK_CONTAINER(e->cur_tab->comment.frame), e->cur_tab->comment.vbox2);
   
   etk_box_pack_start(ETK_BOX(e->cur_tab->comment.vbox2), e->cur_tab->comment.textview, ETK_TRUE, ETK_TRUE, 0);
   etk_box_pack_start(ETK_BOX(e->cur_tab->comment.vbox2), e->cur_tab->comment.hbox, ETK_FALSE, ETK_FALSE, 0);
   
   etk_box_pack_start(ETK_BOX(e->cur_tab->comment.hbox), e->cur_tab->comment.revert, ETK_FALSE, ETK_FALSE, 0);
   etk_box_pack_start(ETK_BOX(e->cur_tab->comment.hbox), e->cur_tab->comment.save, ETK_FALSE, ETK_FALSE, 0);
   
   e->cur_tab->comment.visible = ETK_TRUE;
   
   etk_widget_show_all(ETK_WIDGET(e->hpaned));
   _ex_comment_load(e);   
}

void
_ex_comment_hide(Exhibit *e)
{
   if(!e->cur_tab->comment.visible)
     return;

   if(evas_list_count(e->tabs) == 1)
     {
	/* we only have 1 tab, ie, no notebook */
	if (e->cur_tab->fit_window)
	  etk_paned_child2_set(ETK_PANED(e->hpaned), e->cur_tab->alignment, ETK_TRUE);
	else
	  etk_paned_child2_set(ETK_PANED(e->hpaned), e->cur_tab->scrolled_view, ETK_TRUE);
     }
   else
     {
	/* we have multiple tabs */
	if (e->cur_tab->fit_window)
	  etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->alignment);
	else
	  etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->scrolled_view);
     }   
   
   etk_object_destroy(ETK_OBJECT(e->cur_tab->comment.textview));
   etk_object_destroy(ETK_OBJECT(e->cur_tab->comment.save));
   etk_object_destroy(ETK_OBJECT(e->cur_tab->comment.revert));
   etk_object_destroy(ETK_OBJECT(e->cur_tab->comment.hbox));
   etk_object_destroy(ETK_OBJECT(e->cur_tab->comment.vbox2));
   etk_object_destroy(ETK_OBJECT(e->cur_tab->comment.frame));
   etk_object_destroy(ETK_OBJECT(e->cur_tab->comment.vbox));   
   
   e->cur_tab->comment.visible = ETK_FALSE;
}

void
_ex_comment_load(Exhibit *e)
{
   char *file;
   char *comment;
   unsigned int len;

   file = ((Ex_Tab *) e->cur_tab)->cur_file;
   if (_ex_file_is_jpg(file))
     if (_ex_comment_jpeg_read(file, &comment, &len) && (len != 0))
       {
	  etk_textblock_text_set(ETK_TEXT_VIEW(e->cur_tab->comment.textview)->textblock,
				 comment, ETK_TRUE);
	  return;
       }
   etk_textblock_text_set(ETK_TEXT_VIEW(e->cur_tab->comment.textview)->textblock,
			  "", ETK_TRUE);   
}

void
_ex_comment_save(Exhibit *e)
{
   char *file;
   const char *comment;
   int len;
   
   file = ((Ex_Tab *) e->cur_tab)->cur_file;
   comment = etk_string_get(etk_textblock_text_get(ETK_TEXT_VIEW(e->cur_tab->comment.textview)->textblock, ETK_FALSE));
   if (comment)
     len = strlen(comment);
   else
     len = 0;
   
   if (_ex_file_is_jpg(file))
     _ex_comment_jpeg_write(file, comment, len);
}

void
_ex_comment_revert(Exhibit *e)
{
   _ex_comment_load(e);
}

static void _ex_comment_save_clicked_cb(Etk_Object *obj, void *data)
{
   _ex_comment_save((Exhibit *) data);
}

static void _ex_comment_revert_clicked_cb(Etk_Object *obj, void *data)
{
   _ex_comment_revert((Exhibit *) data);
}
