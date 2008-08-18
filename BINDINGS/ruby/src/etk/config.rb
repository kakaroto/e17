{
	:headers => [
                'Etk.h'
	 ] ,
	
	:gcc_params => '-nostdinc ', 
	# :gcc_params => ' -D"__attribute__(x)="  -D"__extension__="  -D"__restrict=restrict"  -D"__inline=inline"  -D"__const=const" ',
	
	:module_base_name => 'Etk' ,
	 
	:external_types => [ 
 	'Evas', 'Evas_Object', 'Evas_Hash', 'Evas_List', 'Evas_Modifier', 'Evas_Lock', 'Evas_Button_Flags',
        'Ecore_Timer', 'Ecore_Job', 'Ecore_Evas', 'Ecore_X_Window',
        'va_list',
        'Evas_Event_Mouse_In', 'Evas_Event_Mouse_Out', 'Evas_Event_Mouse_Move', 
        'Evas_Event_Mouse_Up', 'Evas_Event_Mouse_Down', 'Evas_Event_Mouse_Wheel',
        'Evas_Event_Key_Up', 'Evas_Event_Key_Down'
 	#'Ecore', 'Ecore_List', 'Ecore_DList', 'Ecore_Hash', 'Ecore_Timer',
 	#'off_t', 'mode_t', 'uid_t', 'gid_t', 
 	#'short int', 'time_t'
	] ,
	
	:type_aliases => { 
        'Etk_Bool' => 'unsigned char'
 	#'off_t' => 'long', 'mode_t' => 'int',  
 	#'uid_t' => 'unsigned int', 'gid_t' => 'unsigned int',
 	#'short int' => 'short', 
	#'unsigned short int' => 'unsigned short',
	#'time_t' => 'unsigned int'
	} ,

	:classes => [
	{ :name => 'EtkBase', :wraps => 'etk' } ,
            { :name => 'Type', :child_of => 'EtkBase' } ,
            { :name => 'Signal',  :child_of => 'EtkBase' } ,
            { :name => 'EObject', :wraps => 'object', :child_of => 'EtkBase' } ,
                { :name => 'Widget', :child_of => 'EObject' } ,
                    { :name => 'Canvas', :child_of => 'Widget' } ,
                    { :name => 'Colorpicker', :child_of => 'Widget' } ,
                    { :name => 'Combobox', :child_of => 'Widget' } ,
                    { :name => 'ComboboxItem', :wraps => 'combobox_item', :child_of => 'Widget' } ,

                    { :name => 'Container', :child_of => 'Widget' } ,
                        { :name => 'Bin', :child_of => 'Container' } ,
                            { :name => 'Alignement', :child_of => 'Bin' } ,
                            { :name => 'Button', :child_of => 'Bin' } ,
                                { :name => 'ToggleButton', :wraps => 'toggle_button', :child_of => 'Button' } ,
                                    { :name => 'CheckButton', :wraps => 'check_button', :child_of => 'ToggleButton' } ,
                                        { :name => 'RadioButton', :wraps => 'radio_button', :child_of => 'CheckButton' } ,
                            { :name => 'Frame', :child_of => 'Bin' } ,
                            { :name => 'ScrolledView', :wraps => 'scrolled_view', :child_of => 'Bin' } ,

                            { :name => 'ToplevelWidget', :wraps => 'toplevel_widget', :child_of => 'Bin' } ,
                                { :name => 'Embed', :child_of => 'ToplevelWidget' } ,
                                { :name => 'Window', :child_of => 'ToplevelWidget' } ,
                                    { :name => 'Dialog', :child_of => 'Window' } ,
                                        { :name => 'MessageDialog', :wraps => 'message_dialog', :child_of => 'Dialog' } ,
                                    { :name => 'PopupWindow', :wraps => 'popup_window', :child_of => 'Window' } ,
                            { :name => 'Viewport', :child_of => 'Bin' } ,
                            
                        { :name => 'Box', :child_of => 'Container' } ,
                            { :name => 'HBox', :child_of => 'Box' } ,
                            { :name => 'VBox', :child_of => 'Box' } ,
                        
                        { :name => 'Notebook', :child_of => 'Container' } ,
                        { :name => 'Paned', :child_of => 'Container' } ,
                            { :name => 'HPaned', :child_of => 'Paned' } ,
                            { :name => 'BPaned', :child_of => 'Paned' } ,
                        
                        { :name => 'Table', :child_of => 'Container' } ,
                    
                    { :name => 'Entry', :child_of => 'Widget' } ,
                    { :name => 'FilechooserWidget', :wraps => 'filechooser_widget', :child_of => 'Widget' } ,
                    { :name => 'IconBox', :child_of => 'Widget' } ,
                    { :name => 'Image', :child_of => 'Widget' } ,
                    { :name => 'Label', :child_of => 'Widget' } ,
                    
                    { :name => 'MenuShell', :wraps => 'menu_shell', :child_of => 'Widget' } ,
                        { :name => 'Menu', :child_of => 'MenuShell' } ,
                        { :name => 'MenuBarShell', :wraps => 'menu_bar', :child_of => 'MenuShell' } ,
                    
                    { :name => 'MenuItem', :wraps => 'menu_item', :child_of => 'Widget' } ,
                    { :name => 'ProgressBar', :wraps => 'progress_bar', :child_of => 'Widget' } ,
                    
                    { :name => 'Range', :child_of => 'Widget' } ,
                        { :name => 'ScrollBar', :child_of => 'Range' } ,
                        { :name => 'Slider', :child_of => 'Range' } ,
                        
                    { :name => 'Separator', :child_of => 'Widget' } ,
                        { :name => 'HSeparator', :child_of => 'Separator' } ,
                        { :name => 'VSeparator', :child_of => 'Separator' } ,
                    
                    { :name => 'StatusBar', :child_of => 'Widget' } ,
                    { :name => 'TextView', :child_of => 'Widget' } ,
                    
                    { :name => 'Tree', :child_of => 'Widget' } ,
                                                
                { :name => 'TreeCol', :child_of => 'EObject', :wraps => 'tree_col' } ,
            { :name => 'TreeRow', :child_of => 'EtkBase', :wraps => 'tree_row' } ,
            { :name => 'TreeModel', :child_of => 'EtkBase' } ,
            
            { :name => 'ToolTips', :child_of => 'EtkBase' } ,
            { :name => 'Theme', :child_of => 'EtkBase' } 
	] ,
	
	:ignore => [
                'etk_object_new_valist'
	]
}
