#include "etk_test.h"
#include "config.h"

int _etk_test_progress_bar_update(void *data)
{
   Etk_Widget *pbar;
   double fraction;
   char text[32];
   
   pbar = data;
   
   fraction = etk_progress_bar_fraction_get(ETK_PROGRESS_BAR(pbar));   
   fraction += 0.01;
   
   if(fraction > 1.0)
     fraction = 0.0;  
   
   snprintf(text, sizeof(text), "%.0f%% done", fraction * 100.0);
   etk_progress_bar_text_set(ETK_PROGRESS_BAR(pbar), text);
   etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(pbar), fraction);   
   
   return 1;
}

int _etk_test_progress_bar_update2(void *data)
{
   Etk_Widget *pbar;
   
   pbar = data;   
   etk_progress_bar_pulse(ETK_PROGRESS_BAR(pbar));
   
   return 1;
}

void etk_test_progress_bar_window_create(void *data)  
{
   Etk_Widget *win;
   Etk_Widget *vbox;   
   Etk_Widget *pbar;
   Etk_Widget *pbar2;   
   
   etk_init();   
   
   win = etk_window_new();
   
   vbox = etk_vbox_new(ETK_TRUE, 5);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   pbar = etk_progress_bar_new_with_text(_("1% done"));
   etk_box_pack_start(ETK_BOX(vbox), pbar, ETK_TRUE, ETK_TRUE, 0);
   
   pbar2 = etk_progress_bar_new_with_text(_("Loading..."));
   etk_progress_bar_pulse_step_set(ETK_PROGRESS_BAR(pbar2), 0.015);
   etk_box_pack_start(ETK_BOX(vbox), pbar2, ETK_TRUE, ETK_TRUE, 0);
   
   etk_window_title_set(ETK_WINDOW(win), _("Etk Progress Bar Test"));
   etk_widget_show_all(win);
   
   ecore_timer_add(0.05, _etk_test_progress_bar_update, pbar);
   ecore_timer_add(0.025, _etk_test_progress_bar_update2, pbar2);
}
