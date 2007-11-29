/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @addtogroup Ewl_Widget
 * @section widget_tut Tutorial
 *
 * Small as small can be
 * (originally at http://everburning.com/news/small-as-small-can-be)
 *
 * Whats the minimum amount of work you need to do to create your own EWL
 * widget? Just want something you can build on but dont know where to start?
 *
 * Well, hopefully this should give you the base for starting your widget.
 * Assuming you're creating a widget called My_Widget, the EWL convention is to
 * have a my_widget.c and my_widget.h files. There are only a couple things you
 * need to implement to get a working widget.
 *
 * First, my_widget.h.
 *
 * @code
 *     #ifndef MY_WIDGET_H
 *     #define MY_WIDGET_H
 *
 *     #include <Ewl.h>
 *
 *     #define MY_WIDGET(w) ((My_Widget *)w)
 *     #define MY_WIDGET_TYPE "my_widget"
 *
 *     typedef struct My_Widget My_Widget;
 *     struct My_Widget
 *     {
 *         Ewl_Widget widget;
 *     };
 *
 *     Ewl_Widget *my_widget_new(void);
 *     int my_widget_init(My_Widget *w);
 *
 *     #endif
 * @endcode
 *
 * That wasn't so bad. What have we got? Well, the MY_WIDGET(w) define gives us
 * a simple macro to cast other widgets to our widget. The second define,
 * MY_WIDGET_TYPE, is a simple macro containing the type name of the widget.
 * Well use that a bit later (and in any type checking we add to our widget.)
 *
 * We then create the widget structure. In this case were inheriting from
 * Ewl_Widget so its the first item in our struct (and not a pointer, thats
 * important). This is how EWLs inhertiance works. The widget you're inheriting
 * from is the first item in the struct and not a pointer. You will now be able
 * to call any of the methods of the inherited class on the new class.
 *
 * We then declare two methods. The convention in EWL is that the _new()
 * function always takes no parameters (void). There is also always a _init()
 * function that takes the widget as its only parameter and returns an int, if
 * the initialization succeeded or failed.
 *
 * With that out of the way, lets take a look at my_widget.c.
 *
 * @code
 *     #include "my_widget.h"
 *
 *     Ewl_Widget *
 *     my_widget_new(void)
 *     {
 *         Ewl_Widget *w;
 *
 *         w = calloc(1, sizeof(My_Widget)));
 *         if (!w) return NULL;
 *
 *         if (!my_widget_init(MY_WIDGET(w)))
 *         {
 *                 free(w);
 *                 return NULL;
 *         }
 *         return w;
 *     }
 *
 *     int
 *     my_widget_init(My_Widget *w)
 *     {
 *          if (!ewl_widget_init(EWL_WIDGET(w)))
 *                 return 0;
 *
 *         ewl_widget_appearance_set(EWL_WIDGET(w), MY_WIDGET_TYPE);
 *         ewl_widget_inherit(EWL_WIDGET(w), MY_WIDGET_TYPE);
 *
 *         return 1;
 *     }
 * @endcode
 *
 * Thats pretty simple. We create a new widget, initialize it and thats about
 * it. In my_widget_init() we make sure we call ewl_widget_init() as thats the
 * widget we are inheriting from and we then set our inheritance and appearance
 * strings (notice the use of our type define from earlier).
 *
 * With that you've got a simple widget. It doesn't do much, but it exists.
 * Build on as you will.
 *
 */

static int create_test(Ewl_Container *box);
static void ewl_widget_cb_toggle(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_first_click(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_second_click(Ewl_Widget *w, void *ev, void *data);
static void ewl_widget_cb_toggle_fullscreen(Ewl_Widget *w, void *ev, void *data);

static int widget_is_test(char *buf, int len);
static int name_test_set_get(char *buf, int len);
static int name_test_nul_set_get(char *buf, int len);
static int name_find_test(char *buf, int len);
static int name_find_missing_test(char *buf, int len);
static int name_find_null_test(char *buf, int len);
static int widget_type_is_test(char *buf, int len);
static int widget_type_is_non_type_test(char *buf, int len);
static int widget_enable_test(char *buf, int len);
static int widget_disable_test(char *buf, int len);
static int widget_colour_test_set_get(char *buf, int len);
static int widget_colour_test_get_null(char *buf, int len);

static int appearance_test_set_get(char *buf, int len);
static int inheritance_test_set_get(char *buf, int len);
static int internal_test_set_get(char *buf, int len);
static int clipped_test_set_get(char *buf, int len);
static int data_test_set_get(char *buf, int len);
static int data_test_set_remove(char *buf, int len);

static int widget_new(char *buf, int len);
static int init(char *buf, int len);
static int show(char *buf, int len);
static int realize(char *buf, int len);
static int realize_unrealize(char *buf, int len);
static int parent_set(char *buf, int len);
static int parent_set_show(char *buf, int len);
static int reparent_unrealized(char *buf, int len);
static int reparent_realized(char *buf, int len);
static int realize_reveal(char *buf, int len);
static int realize_reveal_obscure(char *buf, int len);
static int realize_reveal_unrealize(char *buf, int len);

static Ewl_Unit_Test widget_unit_tests[] = {
		{"EWL_WIDGET_IS", widget_is_test, NULL, -1, 0},
		{"Widget name set/get", name_test_set_get, NULL, -1, 0},
		{"Widget NULL name set/get", name_test_nul_set_get, NULL, -1, 0},
		{"Widget name find", name_find_test, NULL, -1, 0},
		{"Widget missing name find", name_find_missing_test, NULL, -1, 0},
		{"Widget find NULL", name_find_null_test, NULL, -1, 1},
		{"Widget type is test", widget_type_is_test, NULL, -1, 0},
		{"Widget type is without type test", widget_type_is_non_type_test, NULL, -1, 0},
		{"Widget enable", widget_enable_test, NULL, -1, 0},
		{"Widget disable", widget_disable_test, NULL, -1, 0},
		{"Widget colour set/get", widget_colour_test_set_get, NULL, -1, 0},
		{"Widget colour get NULL", widget_colour_test_get_null, NULL, -1, 0},
		{"widget appearance set/get", appearance_test_set_get, NULL, -1, 0},
		{"widget inheritance set/get", inheritance_test_set_get, NULL, -1, 0},
		{"widget internal set/get", internal_test_set_get, NULL, -1, 0},
		{"widget clipped set/get", clipped_test_set_get, NULL, -1, 0},
		{"widget data set/get", data_test_set_get, NULL, -1, 0},
		{"widget data set/remove", data_test_set_remove, NULL, -1, 0},
		{"widget_new", widget_new, NULL, -1, 0},
		{"widget_init state", init, NULL, -1, 0},
		{"widget_show state", show, NULL, -1, 0},
		{"widget_realize state", realize, NULL, -1, 0},
		{"widget realize unrealize state", realize_unrealize, NULL, -1, 0},
		{"widget_parent_set state", parent_set, NULL, -1, 0},
		{"widget parent set while shown state", parent_set_show, NULL, -1, 0},
		{"widget Reparent unrealized state", reparent_unrealized, NULL, -1, 0},
		{"widget reparent realized state", reparent_realized, NULL, -1, 0},
		{"widget realize then reveal state", realize_reveal, NULL, -1, 0},
		{"widget realize reveal obscure state", realize_reveal_obscure, NULL, -1, 0},
		{"widget realize reveal unrealize state", realize_reveal_unrealize, NULL, -1, 0},
		{NULL, NULL, NULL, -1, 0}
	};

void
test_info(Ewl_Test *test)
{
	test->name = "Widget";
	test->tip = "The base widget type.";
	test->filename = __FILE__;
	test->type = EWL_TEST_TYPE_MISC;
	test->func = create_test;
	test->unit_tests = widget_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *vbox, *b2, *o;

	vbox = ewl_vbox_new();
	ewl_container_child_append(box, vbox);
	ewl_widget_show(vbox);

	b2 = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), b2);
	ewl_widget_show(b2);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "First button");
	ewl_widget_name_set(o, "first_widget");
	ewl_container_child_append(EWL_CONTAINER(b2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_first_click, NULL);
	ewl_widget_show(o);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Second button");
	ewl_widget_name_set(o, "second_widget");
	ewl_container_child_append(EWL_CONTAINER(b2), o);
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_second_click, NULL);
	ewl_widget_disable(o);
	ewl_widget_show(o);

	b2 = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), b2);
	ewl_widget_show(b2);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Toggle");
	ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_widget_cb_toggle, NULL);
	ewl_container_child_append(EWL_CONTAINER(b2), o);
	ewl_widget_show(o);

	b2 = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), b2);
	ewl_widget_show(b2);

	o = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(o), "Change fullscreen setting");
	ewl_callback_append(o, EWL_CALLBACK_CLICKED,
				ewl_widget_cb_toggle_fullscreen, NULL);
	ewl_container_child_append(EWL_CONTAINER(b2), o);
	ewl_widget_show(o);

	return 1;
}

static void
ewl_widget_cb_toggle(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
						void *data __UNUSED__)
{
	Ewl_Widget *o, *o2;

	o = ewl_widget_name_find("first_widget");
	o2 = ewl_widget_name_find("second_widget");

	if (DISABLED(o))
	{
		ewl_widget_enable(o);
		ewl_widget_disable(o2);
	}
	else
	{
		ewl_widget_disable(o);
		ewl_widget_enable(o2);
	}
}

static void
ewl_widget_cb_first_click(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
							void *data __UNUSED__)
{
	printf("First clicked\n");
}

static void
ewl_widget_cb_second_click(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
							void *data __UNUSED__)
{
	printf("Second clicked\n");
}

static void
ewl_widget_cb_toggle_fullscreen(Ewl_Widget *w, void *ev __UNUSED__,
						void *data __UNUSED__)
{
	Ewl_Embed *win;

	win = ewl_embed_widget_find(w);
	ewl_window_fullscreen_set(EWL_WINDOW(win),
		!ewl_window_fullscreen_get(EWL_WINDOW(win)));
}

static int
appearance_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	ewl_widget_appearance_set(w, "my_appearance");
	if (strcmp("my_appearance", ewl_widget_appearance_get(w)))
		snprintf(buf, len, "appearance_get dosen't match appearance_set");
	else
		ret = 1;

	return ret;
}

static int
inheritance_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;
	const char *my_class = "myclass";

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	ewl_widget_inherit(w, my_class);
	if (!ewl_widget_type_is(w, my_class))
		snprintf(buf, len, "inheritance dosen't contain correct type");
	else
		ret = 1;

	return ret;
}

static int
internal_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	if (!ewl_widget_internal_is(w)) {
		ewl_widget_internal_set(w, TRUE);
		if (ewl_widget_internal_is(w)) {
			ewl_widget_internal_set(w, FALSE);
			if (ewl_widget_internal_is(w))
				snprintf(buf, len, "internal flag not FALSE");
			else
				ret = 1;
		}
		else
			snprintf(buf, len, "internal flag not TRUE");
	}
	else
		snprintf(buf, len, "internal set after widget_init");

	return ret;
}

static int
clipped_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	if (ewl_widget_clipped_is(w)) {
		ewl_widget_clipped_set(w, FALSE);
		if (!ewl_widget_clipped_is(w)) {
			ewl_widget_clipped_set(w, TRUE);
			if (!ewl_widget_clipped_is(w))
				snprintf(buf, len, "clipped flag not TRUE");
			else
				ret = 1;
		}
		else
			snprintf(buf, len, "clipped flag not FALSE");
	}
	else
		snprintf(buf, len, "clipped not set after widget_init");

	return ret;
}

static int
data_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;
	char *key, *value, *found;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	key = strdup("Data key");
	value = strdup("Data value");

	ewl_widget_data_set(w, key, value);
	found = ewl_widget_data_get(w, key);

	if (!found)
		snprintf(buf, len, "could not find set data");
	else if (found != value)
		snprintf(buf, len, "found value does not match set data");
	else
		ret = 1;

	return ret;
}

static int
data_test_set_remove(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;
	char *key, *value, *found;

	w = calloc(1, sizeof(Ewl_Widget));
	ewl_widget_init(w);

	key = strdup("Data key");
	value = strdup("Data value");

	ewl_widget_data_set(w, key, value);
	found = ewl_widget_data_del(w, key);

	if (!found)
		snprintf(buf, len, "could not find set data");
	else if (found != value)
		snprintf(buf, len, "removed value does not match set data");
	else if (ewl_widget_data_get(w, key))
		snprintf(buf, len, "data value present after remove");
	else
		ret = 1;

	return ret;
}

static int
widget_new(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	if (!w)
		snprintf(buf, len, "Failed to create widget");
	else
	{
		ewl_widget_destroy(w);
		ret = 1;
	}

	return ret;
}

static int
init(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	if (VISIBLE(w))
		snprintf(buf, len, "Widget VISIBLE after _init");
	else if (REALIZED(w))
		snprintf(buf, len, "Widget REALIZED after _init");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after _init");
	else
		ret = 1;

	ewl_widget_destroy(w);

	return ret;
}

static int
show(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_show(w);

	if (!VISIBLE(w))
		snprintf(buf, len, "Widget !VISIBLE after show");
	else if (REALIZED(w))
		snprintf(buf, len, "Widget REALIZED after show");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after show");
	else
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

static int
realize(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_realize(w);
	if (VISIBLE(w))
		snprintf(buf, len, "Widget VISIBLE after realize");
	else if (!REALIZED(w))
		snprintf(buf, len, "Widget !REALIZED after realize");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after realize");
	else
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

static int
realize_unrealize(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_realize(w);
	ewl_widget_unrealize(w);

	if (VISIBLE(w))
		/* This is the currently expected behavior, but we're discussing
		 * if this is really the behavior we want */
		snprintf(buf, len, "Widget VISIBLE after realize/unrealize");
	else if (REALIZED(w))
		snprintf(buf, len, "Widget REALIZED after realize/unrealize");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after realize/unrealize");
	else 
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

static int
parent_set(char *buf, int len)
{
	Ewl_Widget *w, *b;
	int ret = 0;

	b = ewl_box_new();
	ewl_widget_show(b);

	w = ewl_widget_new();
	ewl_widget_parent_set(w, b);
	if (!w->parent)
		snprintf(buf, len, "Widget parent NULL after parent set");
	else if (VISIBLE(w))
		snprintf(buf, len, "Widget VISIBLE after parent set");
	else if (REALIZED(w))
		snprintf(buf, len, "Widget REALIZED after parent set");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after parent set");
	else
		ret = 1;

	ewl_widget_destroy(w);
	ewl_widget_destroy(b);
	return ret;
}

static int
parent_set_show(char *buf, int len)
{
	Ewl_Widget *w, *b;
	int ret = 0;

	b = ewl_box_new();
	ewl_widget_show(b);

	w = ewl_widget_new();
	ewl_widget_show(w);
	ewl_widget_parent_set(w, b);

	if (!w->parent)
		snprintf(buf, len, "Parent NULL after parent_set");
	else if (!VISIBLE(w))
		snprintf(buf, len, "Widget !VISIBLE after parent_set");
	else if (REALIZED(w))
		/* The widget has not been realized yet as that happens in the
		 * idle loop, so this should test that it is still not realized
		 * after changing parents. */
		snprintf(buf, len, "Widget REALIZED after parent_set");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after parent_set");
	else
		ret = 1;

	ewl_widget_destroy(w);
	ewl_widget_destroy(b);
	return ret;
}

static int
reparent_unrealized(char *buf, int len)
{
	Ewl_Widget *w, *b1, *b2;
	int ret = 0;

	b1 = ewl_box_new();
	ewl_widget_show(b1);

	b2 = ewl_box_new();
	ewl_widget_show(b2);

	w = ewl_widget_new();
	ewl_widget_parent_set(w, b1);
	ewl_widget_parent_set(w, b2);

	if (!w->parent)
		snprintf(buf, len, "Widget parent NULL after reparent");
	else if (w->parent != b2)
		snprintf(buf, len, "Widget parent != b2 after reparent");
	else if (VISIBLE(w))
		snprintf(buf, len, "Widget VISIBLE after reparent");
	else if (REALIZED(w))
		snprintf(buf, len, "Widget REALIZED after reparent");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after reparent");
	else 
		ret = 1;

	ewl_widget_destroy(w);
	ewl_widget_destroy(b1);
	ewl_widget_destroy(b2);
	return ret;
}

static int
reparent_realized(char *buf, int len)
{
	Ewl_Widget *w, *b1, *b2;
	int ret = 0;

	b1 = ewl_box_new();
	ewl_widget_show(b1);

	b2 = ewl_box_new();
	ewl_widget_show(b2);

	w = ewl_widget_new();
	ewl_widget_show(w);
	ewl_widget_parent_set(w, b1);
	ewl_widget_parent_set(w, b2);

	if (!w->parent)
		snprintf(buf, len, "Widget parent NULL after reparent");
	else if (w->parent != b2)
		snprintf(buf, len, "Widget parent != b2 after reparent");
	else if (!VISIBLE(w))
		snprintf(buf, len, "Widget !VISIBLE after reparent");
	else if (!REALIZED(w))
		/* FIXME: This can't possibly be REALIZED, since there is no
		 * top level parent. */
		snprintf(buf, len, "Widget !REALIZED after reparent");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after reparent");
	else 
		ret = 1;

	ewl_widget_destroy(w);
	ewl_widget_destroy(b1);
	ewl_widget_destroy(b2);

	return ret;
}

static int
realize_reveal(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_realize(w);
	ewl_widget_reveal(w);

	if (VISIBLE(w))
		snprintf(buf, len, "Widget VISIBLE after realize/reveal");
	else if (!REALIZED(w))
		snprintf(buf, len, "Widget !REALIZED after realize/reveal");
	else if (!REVEALED(w))
		snprintf(buf, len, "Widget !REVEALED after realize/reveal");
	else 
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

static int
realize_reveal_obscure(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_realize(w);
	ewl_widget_reveal(w);

	if (VISIBLE(w))
		/* Realize presently triggers a show, so this is presently the
		 * correct behavior but it is up for discussion right now. */
		snprintf(buf, len, "Widget VISIBLE after realize/reveal/obscure");
	else if (!REALIZED(w))
		snprintf(buf, len, "Widget !REALIZED after realize/reveal/obscure");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after realize/reveal/obscure");
	else 
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

static int
realize_reveal_unrealize(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_realize(w);
	ewl_widget_reveal(w);
	ewl_widget_unrealize(w);

	if (VISIBLE(w))
		/* This is another case where the realize has caused the show.
		 * Since that occurs, being VISIBLE is correct. */
		snprintf(buf, len, "Widget VISIBLE after realize/reveal/unrealize");
	else if (REALIZED(w))
		snprintf(buf, len, "Widget REALIZED after realize/reveal/unrealize");
	else if (REVEALED(w))
		snprintf(buf, len, "Widget REVEALED after realize/reveal/unrealize");
	else 
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

/*
 * Test the EWL_WIDGET_IS macro (and the widget setting its type correctly)
 */
static int
widget_is_test(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	if (!EWL_WIDGET_IS(w))
		snprintf(buf, len, "Widget is not Widget type");
	else
		ret = 1;
	
	ewl_widget_destroy(w);
	return ret;
}

/*
 * Test the name set/get functions
 */
static int
name_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;
	const char *name;

	w = ewl_widget_new();
	ewl_widget_name_set(w, "test widget");
	name = ewl_widget_name_get(w);
	if (strcmp("test widget", name))
		snprintf(buf, len, "Returned name '%s' not 'test widget'", name);
	else
		ret = 1;
	
	ewl_widget_destroy(w);
	return ret;
}

/*
 * Set the widgets name to NULL
 */
static int
name_test_nul_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_name_set(w, "test name");
	ewl_widget_name_set(w, NULL);
	if (NULL != ewl_widget_name_get(w))
		snprintf(buf, len, "Widget name not NULL");
	else
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

/*
 * Test the name find function
 */
static int
name_find_test(char *buf, int len)
{
	Ewl_Widget *w1, *w2;
	int ret = 0;

	w1 = ewl_widget_new();
	ewl_widget_name_set(w1, "test widget");
	w2 = ewl_widget_name_find("test widget");
	if (w1 != w2)
		snprintf(buf, len, "widget found not equal to widget set");
	else
		ret = 1;

	ewl_widget_destroy(w1);
	return ret;
}

/*
 * Search for a name that won't be in the hash
 */
static int
name_find_missing_test(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_name_find("Missing widget name");
	if (w != NULL)
		snprintf(buf, len, "Found widget when we shouldn't have");
	else
		ret = 1;

	return ret;
}

/*
 * Find with a NULL name passed in
 */
static int
name_find_null_test(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_name_find(NULL);
	if (w != NULL)
		snprintf(buf, len, "Found widget when searching for NULL");
	else
		ret = 1;
	return ret;
}

/*
 * test the ewl_widget_type_is function for a type on the widget
 */
static int
widget_type_is_test(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_inherit(w, "my type");
	if (!ewl_widget_type_is(w, "my type"))
		snprintf(buf, len, "Failed to match 'my type' on widget");
	else
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

/* 
 * test the ewl_widget_type_is for a non existant type on the widget
 */
static int
widget_type_is_non_type_test(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	if (ewl_widget_type_is(w, "my missing type"))
		snprintf(buf, len, "Matchined 'my missing type' on widget without type set");
	else
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

/*
 * Test the widget enable function
 */
static int
widget_enable_test(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_disable(w);
	ewl_widget_enable(w);

	if (DISABLED(w))
		snprintf(buf, len, "Widget DISABLED after calling enable");
	else
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

/*
 * Test the ewl_widget_disable function
 */
static int
widget_disable_test(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;

	w = ewl_widget_new();
	ewl_widget_disable(w);
	if (!DISABLED(w))
		snprintf(buf, len, "Widget not disabled aftering calling disable");
	else
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

/*
 * Test the colour set/get functions
 */
static int
widget_colour_test_set_get(char *buf, int len)
{
	Ewl_Widget *w;
	int ret = 0;
	unsigned int r = 0, g = 0, b = 0, a = 0;

	w = ewl_widget_new();
	ewl_widget_color_set(w, 248, 148, 48, 255);
	ewl_widget_color_get(w, &r, &g, &b, &a);
	if (r != 248)
		snprintf(buf, len, "Red colour not retrieved correctly");
	else if (g != 148)
		snprintf(buf, len, "Green colour not retrived correctly");
	else if (b != 48)
		snprintf(buf, len, "Blue colour not retrived correctly");
	else if (a != 255)
		snprintf(buf, len, "Alpha colour not retrieved correctly");
	else
		ret = 1;

	ewl_widget_destroy(w);
	return ret;
}

/*
 * Call color_get with the destination pointers as NULL. This will only
 * really fail if it crashes or throws warnings. Nothing to check.
 */
static int
widget_colour_test_get_null(char *buf, int len)
{
	Ewl_Widget *w;

	w = ewl_widget_new();
	ewl_widget_color_get(w, NULL, NULL, NULL, NULL);
	return 1;
}

