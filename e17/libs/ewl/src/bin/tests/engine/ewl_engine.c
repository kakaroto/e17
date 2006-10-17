#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @addtogroup Ewl_Embed
 * @section text_tut Tutorial
 *
 * The embed allows for overriding the engine used for rendering and display
 * so that an application have multiple target engines. This is most commonly
 * used through the window layer as the embed itself does not create windows in
 * the display system. The reason it is not implemented at the window level is
 * that the embed will need the correct engine information in order to interact
 * with menus, tooltips and similar widgets.
 *
 * @code
 * Ewl_Widget *window;
 * window = ewl_window_new();
 * ewl_embed_engine_name_set(EWL_EMBED(window), "evas_buffer");
 * ewl_widget_show(window);
 * @endcode
 *
 * The code above demonstrates how to setup an embed to render to an ARGB
 * data buffer. When the window is realized it will create a data buffer and
 * store that as the window information on the embed.
 *
 */

static int create_test(Ewl_Container *win);
static void cb_configure(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Engine";
	test->tip = "Overrides the engine on a window.";
	test->filename = __FILE__;
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *pointer_win;
	Ewl_Widget *img;
	Ewl_Widget *o;

	img = ewl_image_new();
	ewl_container_child_append(EWL_CONTAINER(box), img);
	ewl_widget_show(img);

	pointer_win = ewl_window_new();
	ewl_embed_engine_name_set(EWL_EMBED(pointer_win), "evas_buffer");
	ewl_callback_append(pointer_win, EWL_CALLBACK_CONFIGURE, cb_configure,
			img);
	ewl_widget_show(pointer_win);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Buffer");
	ewl_container_child_append(EWL_CONTAINER(pointer_win), o);
	ewl_widget_show(o);

	return 1;
}

static void
cb_configure(Ewl_Widget *w, void *ev, void *data)
{
	Evas_Object *eimg;
	Ewl_Embed *emb = EWL_EMBED(w);
	Ewl_Image *img = EWL_IMAGE(data);

	eimg = img->image;
	evas_object_image_size_set(eimg, CURRENT_W(emb), CURRENT_H(emb));
	evas_object_image_data_set(eimg, emb->evas_window);
	ewl_object_preferred_inner_size_set(EWL_OBJECT(img), CURRENT_W(emb),
			CURRENT_H(emb));
}

