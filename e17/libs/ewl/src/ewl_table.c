
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

	table->homogeneous = homogeneous;
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

	EWL_WIDGET(table)->container.recursive = TRUE;
	EWL_WIDGET(table)->type = EWL_WIDGET_TABLE;

	table->columns = 0;
	table->rows = 0;
	table->homogeneous = 0;
	table->col_spacing = 2;
	table->row_spacing = 2;

    ewl_callback_append(EWL_WIDGET(table), EWL_CALLBACK_REALIZE,
                            ewl_table_realize, NULL);
    ewl_callback_append(EWL_WIDGET(table), EWL_CALLBACK_SHOW,
                            ewl_table_show, NULL);
    ewl_callback_append(EWL_WIDGET(table), EWL_CALLBACK_HIDE,
                            ewl_table_hide, NULL);
    ewl_callback_append(EWL_WIDGET(table), EWL_CALLBACK_DESTROY,
                            ewl_table_destroy, NULL);
    ewl_callback_append(EWL_WIDGET(table), EWL_CALLBACK_CONFIGURE,
                            ewl_table_configure, NULL);

    EWL_OBJECT(table)->current.w = 100;
    EWL_OBJECT(table)->current.h = 50;
    EWL_OBJECT(table)->maximum.w = 2048;
    EWL_OBJECT(table)->maximum.h = 2048;
    EWL_OBJECT(table)->minimum.w = 1;
    EWL_OBJECT(table)->minimum.h = 1;
    EWL_OBJECT(table)->request.w = 100;
    EWL_OBJECT(table)->request.h = 50;
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
	tb_child->start_col = start_col -1;
	tb_child->end_col = end_col -1;
	tb_child->start_row = start_row -1;
	tb_child->end_row = end_row -1;

	if (!table->container.children)
		ewl_container_new(table);

	if (ewd_list_is_empty(table->container.children)) {
		ewl_container_show_clip(table);
		ewl_container_set_clip(table);
	}

	ewd_list_goto_index(table->container.children, start_row + start_col);
	ewd_list_insert(table->container.children, tb_child);

	ewl_widget_configure(table->parent);
}

void
ewl_table_detach(Ewl_Widget * table,
				 unsigned int start_row,
				 unsigned int start_col)
{
	CHECK_PARAM_POINTER("table", table);
	
	
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
	char * image = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	image = ewl_theme_ebit_get("table", "default", "base");

	EWL_TABLE(widget)->ebits_object = ebits_load(image);
	IF_FREE(image);
	ebits_add_to_evas(EWL_TABLE(widget)->ebits_object, widget->evas);
}

static void
ewl_table_show(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

    ewl_fx_clip_box_create(widget);

    ebits_show(EWL_TABLE(widget)->ebits_object);

    if (widget->parent && widget->parent->container.clip_box)
      {
        evas_set_clip(widget->evas, widget->fx_clip_box,
                    widget->parent->container.clip_box);
        ebits_set_clip(EWL_TABLE(widget)->ebits_object,
                    widget->fx_clip_box);
        evas_set_clip(widget->evas, widget->container.clip_box,
                    widget->fx_clip_box);
      }

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, 255);
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
	int first = 1;
	int x = 0, y = 0;
	int total_w = 0, total_h = 0, total_w2 = 0;
	int top_row_h = 0, top_col_w = 0;

	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_TABLE(widget)->ebits_object)
		ebits_get_insets(EWL_TABLE(widget)->ebits_object, &l, &r, &t, &b);

	if (widget->container.children)
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

	first = FALSE;
	/* Really really incomplete atm */
	if (widget->container.children)
	for (row=0;row<EWL_TABLE(widget)->rows;row++)
	  {
		for (col=0;col<EWL_TABLE(widget)->columns;col++)
		  {
			while ((child = ewd_list_next(widget->container.children)) != NULL)
			  {
				if (child->start_col == col && child->start_row == row)
				  {
					if (EWL_OBJECT(child->child)->current.w > max_col_w[col])
					  {
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
					if (max_row_h[row] > top_row_h)
						top_row_h = max_row_h[row];
					if (max_col_w[col] > top_col_w)
						top_col_w = max_col_w[col];
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

	if (widget->container.children)
	ewd_list_goto_first(widget->container.children);

	if (widget->container.children)
	if (EWL_TABLE(widget)->homogeneous)
	  {
	y = EWL_OBJECT(widget)->request.y + t;
	for (row=0;row<EWL_TABLE(widget)->rows;row++)
	  {
		x = EWL_OBJECT(widget)->request.x + l;
		for(col=0;col<EWL_TABLE(widget)->columns;col++)
		  {
		  	while((child = ewd_list_next(widget->container.children)) != NULL)
			  {
				if (child->start_col == col && child->start_row == row)
				  {
					EWL_OBJECT(child->child)->request.x = x + (top_col_w / 2) +
									(EWL_OBJECT(child->child)->current.w / 2);
					EWL_OBJECT(child->child)->request.y = y + (top_row_h / 2) +
									(EWL_OBJECT(child->child)->current.h / 2);
				  }
			  }
			ewd_list_goto_first(widget->container.children);
			x += top_col_w;
		  }
		y += max_row_h[row];
	  }
	  }

	if (widget->container.children)
		ewd_list_goto_first(widget->container.children);

	if (widget->container.children)
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

        ebits_move(EWL_TABLE(widget)->ebits_object,
										 EWL_OBJECT(widget)->request.x,
                                         EWL_OBJECT(widget)->request.y);
        ebits_resize(EWL_TABLE(widget)->ebits_object,
										EWL_OBJECT(widget)->request.w,
										EWL_OBJECT(widget)->request.h);
        ewl_container_clip_box_resize(widget);
		ewl_fx_clip_box_resize(widget);
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
