
#include <Ewl.h>

static void __ewl_list_init(Ewl_List * list);
static void __ewl_list_realize(Ewl_Widget * widget, void * func_data);
static void __ewl_list_show(Ewl_Widget * widget, void * func_data);
static void __ewl_list_hide(Ewl_Widget * widget, void * func_data);
static void __ewl_list_destroy(Ewl_Widget * widget, void * func_data);
static void __ewl_list_configure(Ewl_Widget * widget, void * func_data);




Ewl_Widget *
ewl_list_new(int columns)
{
	Ewl_List * list;
	Ewl_Widget * table;

	list = NEW(Ewl_List, 1);

	__ewl_list_init(list);

	list->columns = columns;

	table = ewl_table_new_all(FALSE, columns, 1, 0, 2);
	ewl_container_append_child(EWL_WIDGET(list), table);

	return EWL_WIDGET(list);
}

Ewl_Widget *
ewl_list_new_all(int columns, char * titles[])
{
	Ewl_Widget * widget;

	widget = ewl_list_new(columns);

	EWL_LIST(widget)->titles = titles;

	return widget;
}

void
ewl_list_append_text(Ewl_Widget * widget, char * text[])
{
	Ewl_Widget * table;
	int i;

	CHECK_PARAM_POINTER("widget", widget);

	table = widget->container.children->first->data;

	ewl_table_resize(table, EWL_TABLE(table)->rows +1,
							EWL_TABLE(table)->columns);

	{
		Ewl_Widget * text_widgets[EWL_LIST(widget)->columns];

		for (i=0;i<EWL_LIST(widget)->columns;i++)
		  {
			text_widgets[i] = ewl_text_new();
			ewl_text_set_text(text_widgets[i], text[i]);
			ewl_text_set_font_size(text_widgets[i], 8);
			ewl_table_attach(table, text_widgets[i], i+1, i+1,
						EWL_TABLE(table)->rows-1, EWL_TABLE(table)->rows-1);
			ewl_widget_show(text_widgets[i]);
		  }
	}
}

void
ewl_list_prepend_text(Ewl_Widget * widget, char * text[])
{

}

void
ewl_list_insert_text(Ewl_Widget * widget, char * text[], int row)
{

}

void
ewl_list_append_widgets(Ewl_Widget * widget, Ewl_Widget * widgets[])
{

}

void
ewl_list_preppend_widgets(Ewl_Widget * widget, Ewl_Widget * widgets[])
{

}

void
ewl_list_insert_widgets(Ewl_Widget * widget, Ewl_Widget * widgets[], int row)
{

}

static void
__ewl_list_init(Ewl_List * list)
{
	CHECK_PARAM_POINTER("list", list);

	memset(list, 0, sizeof(Ewl_List));

	ewl_callback_append(EWL_WIDGET(list),
			EWL_CALLBACK_REALIZE, __ewl_list_realize, NULL);
	ewl_callback_append(EWL_WIDGET(list),
			EWL_CALLBACK_SHOW, __ewl_list_show, NULL);
	ewl_callback_append(EWL_WIDGET(list),
			EWL_CALLBACK_HIDE, __ewl_list_hide, NULL);
	ewl_callback_append(EWL_WIDGET(list),
			EWL_CALLBACK_DESTROY, __ewl_list_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(list),
			EWL_CALLBACK_CONFIGURE, __ewl_list_configure, NULL);

	EWL_WIDGET(list)->container.recursive = FALSE;

	EWL_OBJECT(list)->current.w = 200;
	EWL_OBJECT(list)->current.h = 200;
	EWL_OBJECT(list)->maximum.w = 2048;
	EWL_OBJECT(list)->maximum.h = 2048;
	EWL_OBJECT(list)->minimum.w = 200;
	EWL_OBJECT(list)->minimum.h = 100;
	EWL_OBJECT(list)->request.w = 200;
	EWL_OBJECT(list)->request.h = 200;
}

static void
__ewl_list_realize(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
__ewl_list_show(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_fx_clip_box_create(widget);

	evas_set_clip(widget->evas, widget->fx_clip_box,
					widget->parent->container.clip_box);

	widget->container.clip_box = widget->fx_clip_box;

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, 255);
	ewl_widget_show(widget->container.children->first->data);
}

static void
__ewl_list_hide(Ewl_Widget * widget, void * func_data)
{

}

static void
__ewl_list_destroy(Ewl_Widget * widget, void * func_data)
{

}

static void
__ewl_list_configure(Ewl_Widget * widget, void * func_data)
{
	Ewl_Widget * table;

	CHECK_PARAM_POINTER("widget", widget);

	ewl_fx_clip_box_resize(widget);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
	EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->request.h;

	table = widget->container.children->first->data;

	EWL_OBJECT(table)->request.x = EWL_OBJECT(widget)->current.x;
	EWL_OBJECT(table)->request.y = EWL_OBJECT(widget)->current.y;
	EWL_OBJECT(table)->request.w = EWL_OBJECT(widget)->current.w;
	EWL_OBJECT(table)->request.h = EWL_OBJECT(widget)->current.h;

	ewl_widget_configure(table);
}
