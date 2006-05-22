#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
	char *name;
	void (*cb)(Ewl_Widget *w, void *ev, void *data);
} Control;

static char *txt = "I'm not a lumberjack, or a fur trader,\n"
		"I don't live in an igloo,\n"
		"Or eat blubber,\n"
		"Or own a dogsled,\n"
		"And I don't know Jimmy, Sally, or Suzie from Canada, although I'm certain they're really really nice,\n"
		"I have a Prime Minister, not a President,\n"
		"I speak English and French, not American,\n"
		"And I pronounce it about, not aboot,\n"
		"I can proudly sew my country's flag on my backpack,\n"
		"I believe in peacekeeping, not policing,\n"
		"diversity, not assimilation,\n"
		"and that the Beaver is a truly proud and nobel animal,\n"
		"a touque is a hat,\n"
		"a chesterfield is a couch,\n"
		"and it is pronounced Zed, not Zee, Zed,\n"
		"Canada is the 2nd largest land mass,\n"
		"the first nation of hockey,\n"
		"and the best part of North America,\n"
		"My name is Joe,\n"
		"and I AM CANADIAN!!!!!";

static int create_test(Ewl_Container *box);

static void ete_style_apply(Ewl_Text *t, Ewl_Text_Style s);
static void ete_cb_underline(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_double_underline(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_strikethrough(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_shadow(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_soft_shadow(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_far_shadow(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_glow(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_outline(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_none(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_font_size(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_fetch(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_set(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_load(Ewl_Widget *w, void *ev, void *data);
static void ete_cb_clear(Ewl_Widget *w, void *ev, void *data);

void 
test_info(Ewl_Test *test)
{
	test->name = "Text Editor";
	test->tip = "Defines a class for multi-line text layout\n"
			"and formatting.";
	test->filename = "ewl_text_editor.c";
	test->func = create_test;
	test->type = EWL_TEST_TYPE_ADVANCED;
}

static int
create_test(Ewl_Container *box)
{
	Ewl_Widget *hbox, *scroll, *o;

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
	ewl_widget_show(hbox);

	{
		int i;
		Control btns [] = {
				{"U",  ete_cb_underline},
				{"DU", ete_cb_double_underline},
				{"S",  ete_cb_strikethrough},
				{"SH", ete_cb_shadow},
				{"SS", ete_cb_soft_shadow},
				{"FS", ete_cb_far_shadow},
				{"G",  ete_cb_glow},
				{"O",  ete_cb_outline},
				{"N",  ete_cb_none},
				{NULL, NULL}
			};

		for (i = 0; btns[i].name != NULL; i++) 
		{
			o = ewl_button_new();
			ewl_button_label_set(EWL_BUTTON(o), btns[i].name);
			ewl_container_child_append(EWL_CONTAINER(hbox), o);
			ewl_object_fill_policy_set(EWL_OBJECT(o),
					EWL_FLAG_FILL_HSHRINK|EWL_FLAG_FILL_VSHRINK);
			ewl_callback_append(o, EWL_CALLBACK_CLICKED, btns[i].cb, NULL);
			ewl_widget_show(o);
		}

		o = ewl_spacer_new();
		ewl_container_child_append(EWL_CONTAINER(hbox), o);
		ewl_widget_show(o);

		o = ewl_label_new();
		ewl_container_child_append(EWL_CONTAINER(hbox), o);
		ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
		ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
		ewl_label_text_set(EWL_LABEL(o), "Font size");
		ewl_widget_show(o);

		o = ewl_spinner_new();
		ewl_container_child_append(EWL_CONTAINER(hbox), o);
		ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_NONE);
		ewl_spinner_value_set(EWL_SPINNER(o), 12);
		ewl_spinner_min_val_set(EWL_SPINNER(o), 8);
		ewl_spinner_max_val_set(EWL_SPINNER(o), 72);
		ewl_spinner_step_set(EWL_SPINNER(o), 1);
		ewl_spinner_digits_set(EWL_SPINNER(o), 0);
		ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED,
					ete_cb_font_size, NULL);
		ewl_widget_show(o);
	}

	scroll = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(box), scroll);
	ewl_widget_show(scroll);

	o = ewl_entry_new();
	ewl_container_child_append(EWL_CONTAINER(scroll), o);
	ewl_widget_name_set(o, "entry");
	ewl_entry_multiline_set(EWL_ENTRY(o), TRUE);
	ewl_text_wrap_set(EWL_TEXT(o), TRUE);
	ewl_text_font_size_set(EWL_TEXT(o), 12);

	ewl_text_underline_color_set(EWL_TEXT(o), 0, 0, 0, 255);
	ewl_text_double_underline_color_set(EWL_TEXT(o), 50, 50, 50, 255);
	ewl_text_shadow_color_set(EWL_TEXT(o), 128, 128, 128, 128);
	ewl_text_outline_color_set(EWL_TEXT(o), 200, 200, 200, 200);
	ewl_text_strikethrough_color_set(EWL_TEXT(o), 0, 0, 0, 255);
	ewl_text_glow_color_set(EWL_TEXT(o), 0, 255, 0, 128);

	ewl_object_fill_policy_set(EWL_OBJECT(o), 
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VFILL);
	ewl_widget_show(o);

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_NONE);
	ewl_widget_show(hbox);

	{
		int i;
		Control btns [] = {
				{"Fetch", ete_cb_fetch},
				{"Set",   ete_cb_set},
				{"Load",  ete_cb_load},
				{"Clear", ete_cb_clear},
				{NULL, NULL}
			};

		for (i = 0; btns[i].name != NULL; i++) 
		{
			o = ewl_button_new();
			ewl_button_label_set(EWL_BUTTON(o), btns[i].name);
			ewl_container_child_append(EWL_CONTAINER(hbox), o);
			ewl_object_fill_policy_set(EWL_OBJECT(o),
					EWL_FLAG_FILL_HSHRINK|EWL_FLAG_FILL_VSHRINK);
			ewl_callback_append(o, EWL_CALLBACK_CLICKED, btns[i].cb, NULL);
			ewl_widget_show(o);
		}

	}

	return 1;
}

static void
ete_style_apply(Ewl_Text *t, Ewl_Text_Style s)
{
	unsigned int cursor_pos;
	unsigned int styles;

	cursor_pos = ewl_text_cursor_position_get(t);
	styles = ewl_text_styles_get(t, cursor_pos);

	if (styles & s)
		styles &= ~s;
	else
		styles |= s;

	if (ewl_text_has_selection(t))
	{
		Ewl_Text_Trigger *selection;

		selection = ewl_text_selection_get(t);
		ewl_text_cursor_position_set(t,
				ewl_text_trigger_start_pos_get(selection));

		ewl_text_styles_apply(t, styles,
				ewl_text_trigger_length_get(selection));

		ewl_text_cursor_position_set(t, cursor_pos);
	}
	else
		ewl_text_styles_set(t, styles);
}

static void
ete_cb_underline(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_UNDERLINE);
}

static void
ete_cb_double_underline(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
						void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_DOUBLE_UNDERLINE);
}

static void
ete_cb_strikethrough(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_STRIKETHROUGH);
}

static void
ete_cb_shadow(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
					void *data __UNUSED__)
{ 
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_SHADOW);
}

static void
ete_cb_soft_shadow(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
						void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_SOFT_SHADOW);
}

static void
ete_cb_far_shadow(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
						void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_FAR_SHADOW);
}

static void
ete_cb_glow(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_GLOW);
}

static void
ete_cb_none(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	if (ewl_text_has_selection(EWL_TEXT(entry)))
	{
		unsigned int cursor_pos;
		Ewl_Text_Trigger *selection;

		cursor_pos = ewl_text_cursor_position_get(EWL_TEXT(entry));
		selection = ewl_text_selection_get(EWL_TEXT(entry));

		ewl_text_cursor_position_set(EWL_TEXT(entry),
					ewl_text_trigger_start_pos_get(selection));
		ewl_text_styles_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_NONE,
					ewl_text_trigger_length_get(selection));
		ewl_text_cursor_position_set(EWL_TEXT(entry), cursor_pos);
	}
	else
		ewl_text_styles_set(EWL_TEXT(entry), EWL_TEXT_STYLE_NONE);
}

static void
ete_cb_outline(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
						void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ete_style_apply(EWL_TEXT(entry), EWL_TEXT_STYLE_OUTLINE);
}

static void
ete_cb_font_size(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
						void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	if (ewl_text_has_selection(EWL_TEXT(entry)))
	{
		unsigned int cursor_pos;
		Ewl_Text_Trigger *selection;

		cursor_pos = ewl_text_cursor_position_get(EWL_TEXT(entry));
		selection = ewl_text_selection_get(EWL_TEXT(entry));
		ewl_text_cursor_position_set(EWL_TEXT(entry),
					ewl_text_trigger_start_pos_get(selection));
		ewl_text_font_size_apply(EWL_TEXT(entry),
					ewl_spinner_value_get(EWL_SPINNER(w)),
					ewl_text_trigger_length_get(selection));
		ewl_text_cursor_position_set(EWL_TEXT(entry), cursor_pos);
	}
	else
		ewl_text_font_size_set(EWL_TEXT(entry), 
					ewl_spinner_value_get(EWL_SPINNER(w)));
}

static void
ete_cb_fetch(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	Ewl_Widget *entry;
	char *txt;

	entry = ewl_widget_name_find("entry");
	txt = ewl_text_text_get(EWL_TEXT(entry));

	if (txt) {
		printf("%s\n", txt);
		free(txt);
	}
}

static void
ete_cb_set(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ewl_text_text_set(EWL_TEXT(entry), txt);
}

static void
ete_cb_load(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	printf("I do nothing yet ...\n");
}

static void
ete_cb_clear(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	Ewl_Widget *entry;

	entry = ewl_widget_name_find("entry");
	ewl_text_clear(EWL_TEXT(entry));
}


