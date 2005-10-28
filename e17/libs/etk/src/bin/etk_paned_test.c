#include "etk_test.h"
#include "../../config.h"

static Etk_Bool _etk_test_paned_window_deleted_cb(void *data)
{
   Etk_Window *win = data;
   etk_widget_hide(ETK_WIDGET(win));
   return 1;
}

void etk_test_paned_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vpaned, *hpaned;
   Etk_Widget *label;

	if (win)
	{
		etk_widget_show_all(ETK_WIDGET(win));
		return;
	}
	
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Paned Test"));
   etk_widget_size_request_set(win, 300, 200);

   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_paned_window_deleted_cb), win);
	
   vpaned = etk_vpaned_new();
   etk_container_add(ETK_CONTAINER(win), vpaned);

   hpaned = etk_hpaned_new();
   etk_paned_add1(ETK_PANED(vpaned), hpaned);

   label = etk_label_new(_("Frame 1"));
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_paned_add1(ETK_PANED(hpaned), label);
   label = etk_label_new(_("Frame 2"));
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_paned_add2(ETK_PANED(hpaned), label);
   label = etk_label_new(_("Frame 3"));
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_paned_add2(ETK_PANED(vpaned), label);
   

   etk_widget_show_all(win);
}
