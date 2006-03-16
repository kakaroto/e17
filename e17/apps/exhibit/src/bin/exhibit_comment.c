#include "exhibit.h"

static void _ex_comment_save_clicked_cb(Etk_Object *obj, void *data);
static void _ex_comment_revert_clicked_cb(Etk_Object *obj, void *data);

void
_ex_comment_show(Exhibit *e)
{
   if(e->comment.visible)
     return;
   
   e->comment.vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_paned_add2(ETK_PANED(e->hpaned), e->comment.vbox, ETK_TRUE);
   
   etk_box_pack_start(ETK_BOX(e->comment.vbox), e->cur_tab->scrolled_view, ETK_TRUE, ETK_TRUE, 0);
   
   e->comment.frame = etk_frame_new(_("Image Comments"));
   etk_box_pack_start(ETK_BOX(e->comment.vbox), e->comment.frame, ETK_FALSE, ETK_FALSE, 3);
   
   e->comment.entry = etk_entry_new();
   e->comment.save = etk_button_new_with_label("Save");
   etk_signal_connect("clicked", ETK_OBJECT(e->comment.save), ETK_CALLBACK(_ex_comment_save_clicked_cb), e);
   e->comment.revert = etk_button_new_with_label("Revert");
   etk_signal_connect("clicked", ETK_OBJECT(e->comment.revert), ETK_CALLBACK(_ex_comment_revert_clicked_cb), e);
   
   e->comment.vbox2 = etk_vbox_new(ETK_FALSE, 0);
   e->comment.hbox = etk_hbox_new(ETK_FALSE, 0);
   
   etk_container_add(ETK_CONTAINER(e->comment.frame), e->comment.vbox2);
   
   etk_box_pack_start(ETK_BOX(e->comment.vbox2), e->comment.entry, ETK_TRUE, ETK_TRUE, 0);
   etk_box_pack_start(ETK_BOX(e->comment.vbox2), e->comment.hbox, ETK_FALSE, ETK_FALSE, 0);
   
   etk_box_pack_start(ETK_BOX(e->comment.hbox), e->comment.revert, ETK_FALSE, ETK_FALSE, 0);
   etk_box_pack_start(ETK_BOX(e->comment.hbox), e->comment.save, ETK_FALSE, ETK_FALSE, 0);
   
   e->comment.visible = ETK_TRUE;
   
   etk_widget_show_all(ETK_WIDGET(e->hpaned));   
}

void
_ex_comment_hide(Exhibit *e)
{
   if(!e->comment.visible)
     return;
   
   etk_paned_add2(ETK_PANED(e->hpaned), e->cur_tab->scrolled_view, ETK_TRUE);
   etk_object_destroy(ETK_OBJECT(e->comment.entry));
   etk_object_destroy(ETK_OBJECT(e->comment.save));
   etk_object_destroy(ETK_OBJECT(e->comment.revert));
   etk_object_destroy(ETK_OBJECT(e->comment.hbox));
   etk_object_destroy(ETK_OBJECT(e->comment.vbox2));
   etk_object_destroy(ETK_OBJECT(e->comment.frame));
   etk_object_destroy(ETK_OBJECT(e->comment.vbox));   
   
   e->comment.visible = ETK_FALSE;
}

void
_ex_comment_load(Exhibit *e)
{
   char *file, *comment;
   unsigned int len;

   file = ((Ex_Tab *) e->cur_tab)->cur_file;
   if (_ex_file_is_jpg(file))
     if (_ex_comment_jpeg_read(file, &comment, &len) && (len != 0))
       {
	  etk_entry_text_set(ETK_ENTRY(e->comment.entry), comment);
	  return;
       }
   etk_entry_text_set(ETK_ENTRY(e->comment.entry), "");
}

void
_ex_comment_save(Exhibit *e)
{
   char *file, *comment;
   int len;

   file = ((Ex_Tab *) e->cur_tab)->cur_file;
   comment = etk_entry_text_get(ETK_ENTRY(e->comment.entry));
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
