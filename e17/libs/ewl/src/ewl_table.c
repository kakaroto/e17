
#include <Ewl.h>

static void ewl_table_init(Ewl_Table * table);



static void ewl_table_realize(Ewl_Widget * widget, void * func_data);
static void ewl_table_show(Ewl_Widget * widget, void * func_data);
static void ewl_table_hide(Ewl_Widget * widget, void * func_data);
static void ewl_table_destroy(Ewl_Widget * widget, void * func_data);
static void ewl_table_configure(Ewl_Widget * widget, void * func_data);



Ewl_Widget *
ewl_table_new()
{
	Ewl_Table * table = NULL;

	table = NEW(Ewl_Table, 1);

	ewl_table_init(table);

	return EWL_WIDGET(table);
}

Ewl_Widget *
ewl_table_new_all(unsigned int homogeneous,
				  unsigned int columns,
				  unsigned int rows,
				  unsigned int col_spacing,
				  unsigned int row_spacing)
{
	Ewl_Table * table = NULL;

	table = EWL_TABLE(ewl_table_new());

	table->columns = columns;
	table->rows = rows;
	table->col_spacing = col_spacing;
	table->row_spacing = row_spacing;

	return EWL_WIDGET(table);
}

static void
ewl_table_init(Ewl_Table * table)
{
	CHECK_PARAM_POINTER("table", table);

	memset(table, 0, sizeof(Ewl_Table));

	table->columns = 0;
	table->rows = 0;
	table->homogeneous = 0;
	table->col_spacing = 2;
	table->row_spacing = 2;

    ewl_callback_append(EWL_WIDGET(table), Ewl_Callback_Realize,
                            ewl_table_realize, NULL);
    ewl_callback_append(EWL_WIDGET(table), Ewl_Callback_Show,
                            ewl_table_show, NULL);
    ewl_callback_append(EWL_WIDGET(table), Ewl_Callback_Hide,
                            ewl_table_hide, NULL);
    ewl_callback_append(EWL_WIDGET(table), Ewl_Callback_Destroy,
                            ewl_table_destroy, NULL);
    ewl_callback_append(EWL_WIDGET(table), Ewl_Callback_Configure,
                            ewl_table_configure, NULL);

	EWL_WIDGET(table)->type = Ewl_Widget_Table;
	EWL_WIDGET(table)->container.recursive = TRUE;

    EWL_OBJECT(table)->current.w = 100;
    EWL_OBJECT(table)->current.h = 50;
    EWL_OBJECT(table)->maximum.w = 200;
    EWL_OBJECT(table)->maximum.h = 75;
    EWL_OBJECT(table)->minimum.w = 200;
    EWL_OBJECT(table)->minimum.h = 75;
    EWL_OBJECT(table)->request.w = 200;
    EWL_OBJECT(table)->request.h = 75;
}

void
ewl_table_resize(Ewl_Widget * widget,
				 unsigned int rows,
				 unsigned int columns)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_TABLE(widget)->rows = rows;
	EWL_TABLE(widget)->columns = columns;
}

void
ewl_table_attach(Ewl_Widget * table,
				 Ewl_Widget * child,
				 unsigned int start_col,
				 unsigned int end_col,
				 unsigned int start_row,
				 unsigned int end_row)
{
	Ewl_Table_Child * tb_child = NULL;

	CHECK_PARAM_POINTER("table", table);
	CHECK_PARAM_POINTER("child", child);

	tb_child = NEW(Ewl_Table_Child, 1);
	memset(tb_child, 0, sizeof(Ewl_Table_Child));

	tb_child->child = child;
	child->evas = table->evas;
	child->evas_window = table->evas_window;
	child->parent = EWL_WIDGET(table);
	child->object.layer = table->object.layer +1;
	tb_child->start_col = start_col;
	tb_child->end_col = end_col;
	tb_child->start_row = start_row;
	tb_child->end_row = end_row;

	if (ewd_list_is_empty(table->container.children))
		ewl_container_show_clip(table);

	ewd_list_goto_index(table->container.children, start_col);
	ewd_list_insert(table->container.children, tb_child);
}

void
ewl_table_set_homogeneous(Ewl_Widget * widget,
						  unsigned int homogeneous)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_TABLE(widget)->homogeneous = homogeneous;

	ewl_widget_configure(widget);
	ewl_widget_configure(widget->parent);
}

void
ewl_table_set_col_spacing(Ewl_Widget * widget,
						  unsigned int col_spacing)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_TABLE(widget)->col_spacing = col_spacing;

	ewl_widget_configure(widget);
	ewl_widget_configure(widget->parent);
}

void
ewl_table_set_row_spacing(Ewl_Widget * widget,
						  unsigned int row_spacing)
{
	CHECK_PARAM_POINTER("widget", widget);

	EWL_TABLE(widget)->row_spacing = row_spacing;

	ewl_widget_configure(widget);
	ewl_widget_configure(widget->parent);
}





/* Internal callback's */
static void
ewl_table_realize(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_widget_set_ebit(widget, ewl_theme_ebit_get("table", "default", "base"));

	ewl_container_new(widget);
}

static void
ewl_table_show(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_show(widget->ebits_object);

	ewl_container_set_clip(widget);
}

static void
ewl_table_hide(Ewl_Widget * widget, void * func_data)
{

}

static void
ewl_table_destroy(Ewl_Widget * widget, void * func_data)
{

}

static void
ewl_table_configure(Ewl_Widget * widget, void * func_data)
{
	Ewl_Table_Child * child;
	int col, row;
	int l = 0, r = 0, t = 0, b = 0;
	int max_col_w[EWL_TABLE(widget)->columns];
	int max_row_h[EWL_TABLE(widget)->rows];
	int x = 0, y = 0;
	int total_w = 0, total_h = 0, total_w2 = 0;

	CHECK_PARAM_POINTER("widget", widget);

	if (widget->ebits_object)
		ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

	ewd_list_goto_first(widget->container.children);

	x = EWL_OBJECT(widget)->request.x + l;
	y = EWL_OBJECT(widget)->request.y + t;

	for (col=0;col<EWL_TABLE(widget)->columns;col++)
	 {
		max_col_w[col] = 0;
	 }

	for (row=0;row<EWL_TABLE(widget)->rows;row++)
	 {
		max_row_h[row] = 0;
	 }

	/* Really really incomplete atm */
	for (row=0;row<EWL_TABLE(widget)->rows;row++)
	 {
	  for (col=0;col<EWL_TABLE(widget)->columns;col++)
	   {
		while ((child = ewd_list_next(widget->container.children)) != NULL)
		 {
		  if (child->start_col == col && child->start_row == row)
		   {
			if (EWL_OBJECT(child->child)->current.w > max_col_w[col]) {
			 max_col_w[col] = EWL_OBJECT(child->child)->current.w;
			 row = 0;
			 col = 0;
			 EWL_OBJECT(child->child)->request.x = x -
			 	(EWL_OBJECT(child->child)->current.w / 2) +
				(max_col_w[col] / 2);
			 EWL_OBJECT(child->child)->request.y = y;
			 x = EWL_OBJECT(widget)->request.x + l;
			 y = EWL_OBJECT(widget)->request.y + t;
			 total_h = 0;
			 total_w = 0;
			 total_w2 = 0;
			}
			if (EWL_OBJECT(child->child)->current.h > max_row_h[row])
			 max_row_h[row] = EWL_OBJECT(child->child)->current.h;

			 EWL_OBJECT(child->child)->request.x = x -
			 	(EWL_OBJECT(child->child)->current.w / 2) +
				(max_col_w[col] / 2);
			 EWL_OBJECT(child->child)->request.y = y;
		   }
		 }
		 ewd_list_goto_first(widget->container.children);
		 x += max_col_w[col] + EWL_TABLE(widget)->col_spacing;
		 total_w2 += max_col_w[col] + EWL_TABLE(widget)->col_spacing;
	   }
	   x = EWL_OBJECT(widget)->request.x + l;
	   y += max_row_h[row] + EWL_TABLE(widget)->row_spacing;
	   total_h += max_row_h[row] + EWL_TABLE(widget)->row_spacing;
	   if (total_w2 > total_w)
	   	total_w = total_w2;
		total_w2 = 0;
	 }


	 ewd_list_goto_first(widget->container.children);

	 while ((child = ewd_list_next(widget->container.children)) != NULL)
	  {
		ewl_widget_configure(EWL_WIDGET(child->child));
	  }

	/* Resize and move table bg (if any) */
    if (EWL_OBJECT(widget)->realized) {
		if (total_w)
			EWL_OBJECT(widget)->request.w = (total_w -
									EWL_TABLE(widget)->col_spacing) + l + r;
		if (total_h)
			EWL_OBJECT(widget)->request.h = (total_h -
									EWL_TABLE(widget)->row_spacing) + t + b;

        ebits_move(widget->ebits_object, EWL_OBJECT(widget)->request.x,
                                         EWL_OBJECT(widget)->request.y);
        ebits_resize(widget->ebits_object, EWL_OBJECT(widget)->request.w,
                                           EWL_OBJECT(widget)->request.h);
        ewl_container_clip_box_resize(widget);
    }

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
	EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->request.h;
	EWL_OBJECT(widget)->maximum.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->maximum.h = EWL_OBJECT(widget)->request.h;
	EWL_OBJECT(widget)->minimum.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->minimum.h = EWL_OBJECT(widget)->request.h;
}
