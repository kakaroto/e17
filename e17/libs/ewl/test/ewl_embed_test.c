#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ewl.h>

void print_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Widget *entry = user_data;
	printf("%s\n", ewl_entry_get_text(EWL_ENTRY(entry)));
	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void move_embed_contents_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	ewl_object_geometry_request(EWL_OBJECT(user_data), CURRENT_X(w),
				    CURRENT_Y(w), CURRENT_W(w), CURRENT_H(w));
	return;
	ev_data = NULL;
}

int main(int argc, char **argv)
{
	Ecore_Evas *ee;
	Evas_Object *embobj;
	Ewl_Widget *embed;
	Ewl_Widget *entry;
	Ewl_Widget *box;
	Ewl_Widget *button;

	ewl_init(&argc, argv);
	ecore_evas_init();

	if (!(ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 320, 240))) {
		printf("Failed to open display, exiting\n");
		exit(1);
	}
	ecore_evas_title_set(ee, "EWL Embed Test App");
	ecore_evas_name_class_set(ee, "EWL TEST APP", "EWL TEST APP");
	ecore_evas_show(ee);

	embed = ewl_embed_new();
	embobj = ewl_embed_evas_set(EWL_EMBED(embed), ecore_evas_get(ee),
				    EWL_EMBED_EVAS_WINDOW(ecore_evas_software_x11_window_get(ee)));
	ewl_embed_focus_set(EWL_EMBED(embed), TRUE);
	evas_object_show(embobj);
	ewl_widget_show(embed);

	box = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(embed), box);
	ewl_widget_show(box);

	entry = ewl_entry_new("Type stuff here");
	ewl_container_child_append(EWL_CONTAINER(box), entry);
	ewl_widget_show(entry);

	button = ewl_button_new("Print");
	ewl_container_child_append(EWL_CONTAINER(box), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, print_cb, entry);
	ewl_widget_show(button);

	ewl_callback_append(embed, EWL_CALLBACK_CONFIGURE,
			    move_embed_contents_cb, box);

	ecore_main_loop_begin();

	return 0;
}
