/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"

static void _ex_comment_save_clicked_cb(Etk_Object *obj, void *data);
static void _ex_comment_revert_clicked_cb(Etk_Object *obj, void *data);

void
_ex_comment_show(Exhibit *e)
{
   e->cur_tab->comment.vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->comment.vbox);
   
   if (e->cur_tab->fit_window)
     etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->alignment, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   else
     etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->scrolled_view, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   e->cur_tab->comment.frame = etk_frame_new(_("Image Comments"));
   etk_box_append(ETK_BOX(e->cur_tab->comment.vbox), e->cur_tab->comment.frame, ETK_BOX_START, ETK_BOX_NONE, 3);
   
   e->cur_tab->comment.textview = etk_text_view_new();
   etk_widget_size_request_set(e->cur_tab->comment.textview, -1, 50);
   e->cur_tab->comment.save = etk_button_new_with_label("Save");
   etk_signal_connect("clicked", ETK_OBJECT(e->cur_tab->comment.save), ETK_CALLBACK(_ex_comment_save_clicked_cb), e);
   e->cur_tab->comment.revert = etk_button_new_with_label("Revert");
   etk_signal_connect("clicked", ETK_OBJECT(e->cur_tab->comment.revert), ETK_CALLBACK(_ex_comment_revert_clicked_cb), e);
   
   e->cur_tab->comment.vbox2 = etk_vbox_new(ETK_FALSE, 0);
   e->cur_tab->comment.hbox = etk_hbox_new(ETK_FALSE, 0);
   
   etk_container_add(ETK_CONTAINER(e->cur_tab->comment.frame), e->cur_tab->comment.vbox2);
   
   etk_box_append(ETK_BOX(e->cur_tab->comment.vbox2), e->cur_tab->comment.textview, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_box_append(ETK_BOX(e->cur_tab->comment.vbox2), e->cur_tab->comment.hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   etk_box_append(ETK_BOX(e->cur_tab->comment.hbox), e->cur_tab->comment.revert, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(e->cur_tab->comment.hbox), e->cur_tab->comment.save, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   e->cur_tab->comment.visible = ETK_TRUE;
   
   etk_widget_show_all(ETK_WIDGET(e->hpaned));
   _ex_comment_load(e);   
}

void
_ex_comment_hide(Exhibit *e)
{
   if(!e->cur_tab->comment.visible)
     return;

   if (e->cur_tab->fit_window)
     etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->alignment);
   else
     etk_notebook_page_child_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, e->cur_tab->scrolled_view);
   
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
   char *comment = NULL;
   unsigned int len = 0;

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
