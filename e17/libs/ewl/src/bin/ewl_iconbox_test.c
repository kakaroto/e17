#include "ewl_test.h"

Ewl_Widget* ib;
static Ewl_Widget* ewl_iconbox_button = NULL;

void icon_click_cb(Ewl_Widget *w __UNUSED__, void *ev_data, void *user_data __UNUSED__) {
	Ewl_Event_Mouse_Down *ev = ev_data;
	if (ev->clicks > 1)
		printf("Icon clicked!\n");
}

void add_icons_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__, void *user_data __UNUSED__) {
		Ewl_Iconbox_Icon* icon = ewl_iconbox_icon_add(EWL_ICONBOX(ib), "Draw", PACKAGE_DATA_DIR "/images/Draw.png");
		printf("Loading %s\n", PACKAGE_DATA_DIR "/images/Draw.png");
		ewl_callback_prepend(EWL_WIDGET(icon), EWL_CALLBACK_CLICKED, icon_click_cb, NULL);
		
		ewl_iconbox_icon_arrange(EWL_ICONBOX(ib));

		ewl_iconbox_icon_add(EWL_ICONBOX(ib),"End", PACKAGE_DATA_DIR "/images/End.png");
		ewl_iconbox_icon_add(EWL_ICONBOX(ib),"Card", PACKAGE_DATA_DIR "/images/NewBCard.png");
		ewl_iconbox_icon_add(EWL_ICONBOX(ib),"Open", PACKAGE_DATA_DIR "/images/Open.png");
		ewl_iconbox_icon_add(EWL_ICONBOX(ib),"Package", PACKAGE_DATA_DIR "/images/Package.png");
		ewl_iconbox_icon_add(EWL_ICONBOX(ib),"World", PACKAGE_DATA_DIR "/images/World.png");
		
}

void
 __destroy_iconbox_test_window(Ewl_Widget *main_win, void *ev_data __UNUSED__, void *user_data __UNUSED__)
 {
        ewl_widget_destroy(main_win);
	ewl_callback_append(ewl_iconbox_button, EWL_CALLBACK_CLICKED,
	                                  __create_iconbox_test_window, NULL);


        return;
 }

void
__create_iconbox_test_window(Ewl_Widget * w __UNUSED__, void *ev_data __UNUSED__,
	                                      void *user_data __UNUSED__)
{
		Ewl_Widget* ib_win;
		Ewl_Widget* box;
		Ewl_Widget* button;

		ewl_iconbox_button=w;
		
		ib_win= ewl_window_new();

                if (w) {
                        ewl_callback_del(w, EWL_CALLBACK_CLICKED,
                                        __create_iconbox_test_window);
                        ewl_callback_append(ib_win, EWL_CALLBACK_DELETE_WINDOW,
                                    __destroy_iconbox_test_window, NULL);
                } else
                        ewl_callback_append(ib_win, EWL_CALLBACK_DELETE_WINDOW,
                                        __close_main_window, NULL);

		
		ewl_window_title_set(EWL_WINDOW(ib_win), "Icon Box");
		ewl_window_name_set(EWL_WINDOW(ib_win), "Icon Box");
		ewl_window_class_set(EWL_WINDOW(ib_win), "Icon Box");
		ewl_object_size_request(EWL_OBJECT(ib_win), 500,500);

		
		ib = ewl_iconbox_new();
		//ewl_object_fill_policy_set(EWL_OBJECT(ib), EWL_FLAG_FILL_SHRINK);
		ewl_iconbox_editable_set(EWL_ICONBOX(ib), 1);


		box = ewl_hbox_new();
		button = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(button), "Add Icons");
		ewl_widget_show(button);

		
		ewl_container_child_append(EWL_CONTAINER(ib_win), box);
		ewl_object_fill_policy_set(EWL_OBJECT(ib_win), EWL_FLAG_FILL_ALL);
		ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_ALL);
		ewl_container_child_append(EWL_CONTAINER(box), button);
		ewl_object_maximum_size_set(EWL_OBJECT(button), 50, 50);
		
		ewl_container_child_append(EWL_CONTAINER(box), ib);
		ewl_widget_show(box);
		


		ewl_widget_show(ib_win);
		ewl_widget_show(ib);


		 ewl_callback_append(button, EWL_CALLBACK_MOUSE_DOWN,
				 add_icons_cb, NULL);

}
