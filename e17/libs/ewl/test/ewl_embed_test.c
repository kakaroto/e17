#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ewl.h>

void print_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	printf("jack shit\n");
}

void move_embed_contents_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	ewl_object_geometry_request(EWL_OBJECT(user_data), CURRENT_X(w),
				    CURRENT_Y(w), CURRENT_W(w), CURRENT_H(w));
}

int main(int argc, char **argv)
{
	Ecore_Evas *ee;
	Evas_Object *embobj;
	Ewl_Widget *embed;
	Ewl_Widget *button;

	ewl_init(&argc, argv);
	ecore_evas_init();

	if (!(ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 320, 240))) {
		printf("Failed to open display, exiting\n");
		exit(1);
	}
	ecore_evas_show(ee);

	embed = ewl_embed_new();
	embobj = ewl_embed_set_evas(EWL_EMBED(embed), ecore_evas_get(ee),
			EWL_EMBED_EVAS_WINDOW(ecore_evas_software_x11_window_get(ee)));
	evas_object_show(embobj);
	ewl_widget_show(embed);

	button = ewl_button_new("This button does jack shit");
	ewl_container_child_append(EWL_CONTAINER(embed), button);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, print_cb, button);
	ewl_widget_show(button);

	ewl_callback_append(embed, EWL_CALLBACK_CONFIGURE,
			    move_embed_contents_cb, button);

	ecore_main_loop_begin();

	return 0;
}
