#------------------------------------RUBY ETK_TEST --------------------------------------------
if __FILE__ == $0
   puts "This is not the main file, don't run this directly. Instead run etk_test.rb."
   exit
end
#----------------------------------------------------------------------------------------------

class TreeExample < Example
    def initialize
        super()
        self.title = "Etk Tree Test"
        EtkBase.signal_connect("delete-event", self, Window.function(:hide_on_delete), nil)
    end
    
    def create_widgets
        return if !@box.nil? 

        table = Table.new(2, 3, false)
        self << table

        label = Label.new("<h1>Tree:</h1>")
        table.attach(label, 0, 0, 0, 0, 0, 0, Etk::TABLE_FILL)

        tree = Tree.new
        tree.size_request_set(320, 400)
        table.attach_default(tree, 0, 0, 1, 1)

        tree.mode = Etk::TREE_MODE_TREE
        tree.multiple_select = true
	
	col1 = TreeCol.wrap(tree.col_new("Column 1", 90, 0.0))
	col1.model_add(Tree.model_image_new())
        col1.model_add(Tree.model_text_new())
	
	col2 = TreeCol.wrap(tree.col_new("Column 2", 60, 0.0))
	col2.model_add(Tree.model_double_new())
	
	col3 = TreeCol.wrap(tree.col_new("Column 3", 60, 0.0))
	col3.model_add(Tree.model_image_new())
	
	col4 = TreeCol.wrap(tree.col_new("Column 4", 40, 0.0))
	col4.model_add(Tree.model_checkbox_new())		
	
        #col4 = etk_tree_col_new(ETK_TREE(tree), "Column 4", etk_tree_model_checkbox_new(ETK_TREE(tree)), 40);
        
        tree.build
        #etk_signal_connect("cell_value_changed", ETK_OBJECT(col4), ETK_CALLBACK(_etk_test_tree_checkbox_toggled_cb), NULL);

        tree.freeze
        0.upto(1) { |i| 
            row = Tree.wrap(tree.va_call(:row_append, 'P', 
					 nil,
					 col1, Theme.icon_path, "places/user-home_16", "Row1",
					 col2, 10.0, 
					 col3, PACKAGE_DATA_DIR + "/images/1star.png", nil,
					 col4, false, nil))
	    
            # notice that we are calling Tree.va_call and not tree.va_call (in other words,
            # we're NOT calling an instance method)
            
            row = Tree.wrap(tree.va_call(:row_append, 'P', row, 
					 col1, Theme.icon_path, "places/folder_16", "Row2",
					 col2, 20.0, 
					 col3, PACKAGE_DATA_DIR + "/images/2stars.png", nil, 
					 col4, false, nil))
            row = Tree.wrap(tree.va_call(:row_append, 'P', row, 
					 col1, Theme.icon_path, "mimetypes/text-x-generic_16", "Row3",
					 col2, 30.0, 
					 col3, PACKAGE_DATA_DIR + "/images/3stars.png", nil,
					 col4, false, nil))
        }
        tree.thaw
        
        self.show_all
    end
    
    def run
        create_widgets
        self.show_all
    end
end

   #~ /* The list: */
   #~ label = etk_label_new("<h1>List:</h1>");
   #~ etk_table_attach(ETK_TABLE(table), label, 1, 1, 0, 0, 0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL);

   #~ tree = etk_tree_new();
   #~ etk_widget_dnd_source_set(ETK_WIDGET(tree), ETK_TRUE);   
   #~ etk_widget_dnd_dest_set(ETK_WIDGET(tree), ETK_TRUE);
   #~ etk_signal_connect("drag_drop", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree_drag_drop_cb), NULL);
   #~ etk_signal_connect("drag_begin", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree_drag_begin_cb), NULL);
   #~ etk_widget_size_request_set(tree, 320, 400);
   #~ etk_table_attach_defaults(ETK_TABLE(table), tree, 1, 1, 1, 1);

   #~ etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_LIST);
   #~ etk_tree_multiple_select_set(ETK_TREE(tree), ETK_TRUE);
   #~ col1 = etk_tree_col_new(ETK_TREE(tree), "Column 1", etk_tree_model_icon_text_new(ETK_TREE(tree), ETK_TREE_FROM_FILE), 90);
   #~ col2 = etk_tree_col_new(ETK_TREE(tree), "Column 2", etk_tree_model_int_new(ETK_TREE(tree)), 90);
   #~ etk_tree_col_sort_func_set(col2, _etk_test_tree_compare_cb, NULL);
   #~ col3 = etk_tree_col_new(ETK_TREE(tree), "Column 3", etk_tree_model_image_new(ETK_TREE(tree), ETK_TREE_FROM_FILE), 90);
   #~ etk_tree_build(ETK_TREE(tree));

   #~ _etk_test_tree_add_items(ETK_TREE(tree), 5000);
   #~ etk_signal_connect("row_selected", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree_row_selected), NULL);
   #~ etk_signal_connect("row_unselected", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree_row_unselected), NULL);
   #~ etk_signal_connect("row_clicked", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree_row_clicked), NULL);
   #~ etk_signal_connect("row_activated", ETK_OBJECT(tree), ETK_CALLBACK(_etk_test_tree_row_activated), NULL);

   #~ /* Frame */
   #~ frame = etk_frame_new("List Actions");
   #~ etk_table_attach(ETK_TABLE(table), frame, 0, 1, 2, 2, 0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL);
   #~ hbox = etk_hbox_new(ETK_TRUE, 10);
   #~ etk_container_add(ETK_CONTAINER(frame), hbox);

   #~ button = etk_button_new_with_label("Clear");
   #~ etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_clear_list_cb), tree);
   #~ etk_box_pack_start(ETK_BOX(hbox), button, ETK_TRUE, ETK_TRUE, 0);

   #~ button = etk_button_new_with_label("Add 5 rows");
   #~ etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_add_5_cb), tree);
   #~ etk_box_pack_start(ETK_BOX(hbox), button, ETK_TRUE, ETK_TRUE, 0);

   #~ button = etk_button_new_with_label("Add 50 rows");
   #~ etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_add_50_cb), tree);
   #~ etk_box_pack_start(ETK_BOX(hbox), button, ETK_TRUE, ETK_TRUE, 0);

   #~ button = etk_button_new_with_label("Add 500 rows");
   #~ etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_add_500_cb), tree);
   #~ etk_box_pack_start(ETK_BOX(hbox), button, ETK_TRUE, ETK_TRUE, 0);

   #~ button = etk_button_new_with_label("Add 5000 rows");
   #~ etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_add_5000_cb), tree);
   #~ etk_box_pack_start(ETK_BOX(hbox), button, ETK_TRUE, ETK_TRUE, 0);
   
   #~ button = etk_button_new_with_label("Sort");
   #~ etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_tree_sort_cb), tree);
   #~ etk_box_pack_start(ETK_BOX(hbox), button, ETK_TRUE, ETK_TRUE, 0);





#~ /* TODO: doc */
#~ static void _etk_test_tree_drag_drop_cb(Etk_Object *object, void *event, void *data)
#~ {
   #~ Etk_Tree *tree;
   #~ Etk_Tree_Row *row;
   #~ char *col1_string, *col3_path;
   #~ int col2_value;

   #~ tree = ETK_TREE(object);
   #~ row = etk_tree_selected_row_get(tree);
   #~ etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, &col1_string, etk_tree_nth_col_get(tree, 1), &col2_value, etk_tree_nth_col_get(tree, 2), &col3_path, NULL);
   #~ printf("Row dropped on %p: \"%s\" %d %s\n", row, col1_string, col2_value, col3_path);      
#~ }

#~ /* TODO: doc */
#~ static void _etk_test_tree_drag_begin_cb(Etk_Object *object, void *data)
#~ {
   #~ Etk_Tree *tree;
   #~ Etk_Tree_Row *row;
   #~ char *col1_string, *col3_path;
   #~ int col2_value;   
   #~ const char **types;
   #~ unsigned int num_types;
   #~ char *drag_data;
   #~ Etk_Drag *drag;
   #~ Etk_Widget *button;
   
   #~ tree = ETK_TREE(object);
   #~ row = etk_tree_selected_row_get(tree);
   
   #~ drag = ETK_DRAG((ETK_WIDGET(tree))->drag);
   
   #~ etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, &col1_string, etk_tree_nth_col_get(tree, 1), &col2_value, etk_tree_nth_col_get(tree, 2), &col3_path, NULL);   
   
   #~ types = calloc(1, sizeof(char*));
   #~ num_types = 1;
   #~ types[0] = strdup("text/plain");
   #~ drag_data = strdup(col1_string);
   
   #~ etk_drag_types_set(drag, types, num_types);
   #~ etk_drag_data_set(drag, drag_data, strlen(drag_data) + 1);
   #~ button = etk_button_new_with_label(col1_string);
   #~ etk_button_image_set(ETK_BUTTON(button), ETK_IMAGE(etk_image_new_from_file(col3_path)));
   #~ etk_container_add(ETK_CONTAINER(drag), button);
#~ }

#~ /* Adds n items to the tree */
#~ static void _etk_test_tree_add_items(Etk_Tree *tree, int n)
#~ {
   #~ int i;
   #~ Etk_Tree_Col *col1, *col2, *col3;
   #~ char row_name[256];
   #~ char star_path[256];
   #~ int rand_value;

   #~ if (!tree)
      #~ return;

   #~ col1 = etk_tree_nth_col_get(tree, 0);
   #~ col2 = etk_tree_nth_col_get(tree, 1);
   #~ col3 = etk_tree_nth_col_get(tree, 2);

   #~ etk_tree_freeze(tree);
   #~ for (i = 0; i < n; i++)
   #~ {
      #~ snprintf(row_name, 256, "Row%d", i);
      #~ if (i % 3 == 0)
         #~ strncpy(star_path, PACKAGE_DATA_DIR "/images/1star.png", 256);
      #~ else if (i % 3 == 1)
         #~ strncpy(star_path, PACKAGE_DATA_DIR "/images/2stars.png", 256);
      #~ else
         #~ strncpy(star_path, PACKAGE_DATA_DIR "/images/3stars.png", 256);
      #~ rand_value = rand() % 10000;
      #~ etk_tree_append(ETK_TREE(tree), col1, PACKAGE_DATA_DIR "/images/1star.png", row_name, col2, rand_value, col3, star_path, NULL);
   #~ }
   #~ etk_tree_thaw(tree);
#~ }

#~ /* Called when a row is selected */
#~ static void _etk_test_tree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
#~ {
   #~ Etk_Tree *tree;
   #~ char *col1_string, *col3_path;
   #~ int col2_value;

   #~ tree = ETK_TREE(object);
   #~ etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, &col1_string, etk_tree_nth_col_get(tree, 1), &col2_value, etk_tree_nth_col_get(tree, 2), &col3_path, NULL);
   #~ printf("Row selected %p: \"%s\" %d %s\n", row, col1_string, col2_value, col3_path);
#~ }

#~ /* Called when a row is unselected */
#~ static void _etk_test_tree_row_unselected(Etk_Object *object, Etk_Tree_Row *row, void *data)
#~ {
   #~ printf("Row unselected %p\n", row);
#~ }

#~ /* Called when a row is clicked */
#~ static void _etk_test_tree_row_clicked(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up_Down *event, void *data)
#~ {
   #~ printf("Row clicked %p. Button: %d. ", row, event->button);
   #~ if (event->flags & EVAS_BUTTON_TRIPLE_CLICK)
      #~ printf("Triple Click\n");
   #~ else if (event->flags & EVAS_BUTTON_DOUBLE_CLICK)
      #~ printf("Double Click\n");
   #~ else
      #~ printf("Single Click\n");
#~ }

#~ /* Called when a row is activated */
#~ static void _etk_test_tree_row_activated(Etk_Object *object, Etk_Tree_Row *row, void *data)
#~ {
   #~ Etk_Tree *tree;
   #~ char *col1_string, *col3_path;
   #~ int col2_value;

   #~ tree = ETK_TREE(object);
   #~ etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, &col1_string, etk_tree_nth_col_get(tree, 1), &col2_value, etk_tree_nth_col_get(tree, 2), &col3_path, NULL);
   #~ printf("Row activated %p: \"%s\" %d %s\n", row, col1_string, col2_value, col3_path);
#~ }

#~ /* Called when a checkbox of the tree is toggled */
#~ static void _etk_test_tree_checkbox_toggled_cb(Etk_Object *object, Etk_Tree_Row *row, void *data)
#~ {
   #~ Etk_Bool checked;
   #~ Etk_Tree_Col *col;
   
   #~ if (!(col = ETK_TREE_COL(object)) || !row)
      #~ return;
   
   #~ etk_tree_row_fields_get(row, col, &checked, NULL);
   #~ if (checked)
      #~ printf("Checkbox activated\n");
   #~ else
      #~ printf("Checkbox deactivated\n");
#~ }

#~ /* Called when the "Clear" button is clicked */
#~ static void _etk_test_tree_clear_list_cb(Etk_Object *object, void *data)
#~ {
   #~ Etk_Tree *tree;

   #~ if (!(tree = ETK_TREE(data)))
      #~ return;
   #~ etk_tree_clear(tree);
#~ }

#~ /* Called when the "Add 5 items" button is clicked */
#~ static void _etk_test_tree_add_5_cb(Etk_Object *object, void *data)
#~ {
   #~ _etk_test_tree_add_items(ETK_TREE(data), 5);
#~ }

#~ /* Called when the "Add 50 items" button is clicked */
#~ static void _etk_test_tree_add_50_cb(Etk_Object *object, void *data)
#~ {
   #~ _etk_test_tree_add_items(ETK_TREE(data), 50);
#~ }

#~ /* Called when the "Add 500 items" button is clicked */
#~ static void _etk_test_tree_add_500_cb(Etk_Object *object, void *data)
#~ {
   #~ _etk_test_tree_add_items(ETK_TREE(data), 500);
#~ }

#~ /* Called when the "Add 5000 items" button is clicked */
#~ static void _etk_test_tree_add_5000_cb(Etk_Object *object, void *data)
#~ {
   #~ _etk_test_tree_add_items(ETK_TREE(data), 5000);
#~ }

#~ /* Called when the "Sort" button is clicked */
#~ static void _etk_test_tree_sort_cb(Etk_Object *object, void *data)
#~ {
   #~ Etk_Tree *tree;
   
   #~ if (!(tree = ETK_TREE(data)))
      #~ return;
   #~ etk_tree_sort(tree, _etk_test_tree_compare_cb, ascendant, etk_tree_nth_col_get(tree, 1), NULL);
   #~ ascendant = !ascendant;
#~ }

#~ /* Compares two rows of the tree */
#~ static int _etk_test_tree_compare_cb(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data)
#~ {
   #~ int row1_value, row2_value;
   
   #~ if (!tree || !row1 || !row2 || !col)
      #~ return 0;
   
   #~ etk_tree_row_fields_get(row1, col, &row1_value, NULL);
   #~ etk_tree_row_fields_get(row2, col, &row2_value, NULL);
   #~ if (row1_value < row2_value)
      #~ return -1;
   #~ else if (row1_value > row2_value)
      #~ return 1;
   #~ else
      #~ return 0;
#~ }
