#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ewl.h>

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
				    ecore_evas_software_x11_window_get(ee));
	evas_object_show(embobj);
	ewl_widget_show(embed);

	button = ewl_button_new("This button does jack shit");
	ewl_container_append_child(EWL_CONTAINER(embed), button);
	ewl_widget_show(button);

	ecore_main_loop_begin();

	return 0;
}
