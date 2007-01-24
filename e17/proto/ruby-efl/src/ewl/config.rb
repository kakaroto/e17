{
	:headers => [
	'Ewl.h'
	 ] ,
	
	:gcc_params => '-nostdinc -I.', 
	# :gcc_params => ' -D"__attribute__(x)="  -D"__extension__="  -D"__restrict=restrict"  -D"__inline=inline"  -D"__const=const" ',
	
	:module_base_name => 'Ewl' ,
	 
	:external_types => [ 
 	'Evas', 'Evas_Object', 'Evas_Coord', 
 	'Ecore', 'Ecore_List', 'Ecore_DList', 'Ecore_Hash', 'Ecore_Timer',
 	'off_t', 'mode_t', 'uid_t', 'gid_t', 
 	'short int', 'time_t'
	] ,
	
	:type_aliases => { 
 	'off_t' => 'long', 'mode_t' => 'int',  
 	'uid_t' => 'unsigned int', 'gid_t' => 'unsigned int',
 	'short int' => 'short', 
	'unsigned short int' => 'unsigned short',
	'time_t' => 'unsigned int'
	} ,

	:classes => [
	{ :name => 'EwlBase', :wraps => 'ewl' } ,
        { :name => 'EConfig', :wraps => 'config', :child_of => 'EwlBase' } ,
        { :name => 'Theme' , :child_of => 'EwlBase' } ,
        { :name => 'Model' , :child_of => 'EwlBase' },
        { :name => 'View' , :child_of => 'EwlBase' },
        { :name => 'DND' , :child_of => 'EwlBase' },

        { :name => 'EObject', :wraps => 'object', :child_of => 'EwlBase' } ,
        
            { :name => 'Widget' , :child_of => 'EObject' } ,
                { :name => 'Label' , :child_of => 'Widget' } ,
                { :name => 'TextTrigger' , :wraps => 'text_trigger', :child_of => 'Widget' } ,
                { :name => 'Row' , :child_of => 'Widget' } ,
                { :name => 'Spacer' , :child_of => 'Widget' },
                { :name => 'Image' , :child_of => 'Widget' },
                { :name => 'Check' , :child_of => 'Widget' },
                { :name => 'Media' , :child_of => 'Widget' },

                { :name => 'Separator' , :child_of => 'Widget' },
                    { :name => 'HSeparator' , :child_of => 'Separator' },
                    { :name => 'VSeparator' , :child_of => 'Separator' },

                { :name => 'Container' , :child_of => 'Widget' } ,
                    { :name => 'Cell' , :child_of => 'Container' } ,
                    { :name => 'Grid' , :child_of => 'Container' } ,
                    { :name => 'Table' , :child_of => 'Container' } ,
                    { :name => 'Scrollpane' , :child_of => 'Container' },
                    { :name => 'Tree' , :child_of => 'Container' },
                    { :name => 'Tree2' , :child_of => 'Container' },
                    { :name => 'Paned' , :child_of => 'Container' },
                    { :name => 'PanedGrabber' , :wraps => 'paned_grabber', :child_of => 'Container' },

                    { :name => 'Overlay' , :child_of => 'Container' } ,
                        { :name => 'Spectrum' , :child_of => 'Overlay' },

                    { :name => 'Text' , :child_of => 'Container' } ,
                        { :name => 'DatePicker' , :child_of => 'Text' },
                    	{ :name => 'Entry' , :child_of => 'Text' } ,
                            { :name => 'Password' , :child_of => 'Entry' } ,

                    { :name => 'Range' , :child_of => 'Container' } ,
                        { :name => 'Seeker' , :child_of => 'Range' } ,
                        { :name => 'Spinner' , :child_of => 'Range' },
                        { :name => 'Progressbar' , :child_of => 'Range' },

                    { :name => 'Embed' , :child_of => 'Container' } ,
                        { :name => 'Window' , :child_of => 'Embed' } ,
                            { :name => 'Dialog' , :child_of => 'Window' } ,
                                { :name => 'FileDialog' , :child_of => 'Dialog' } ,
                                { :name => 'ColorDialog' , :child_of => 'Dialog' } ,
    
                    { :name => 'Box' , :child_of => 'Container' } ,
                        { :name => 'VBox' , :child_of => 'Box' } ,
                        { :name => 'HBox' , :child_of => 'Box' } ,
                        { :name => 'FreeBox' , :child_of => 'Box' } ,
                        { :name => 'Border' , :child_of => 'Box' } ,
                        { :name => 'Floater' , :child_of => 'Box' } ,
                        { :name => 'Scrollbar' , :child_of => 'Box' },
                        { :name => 'FilePicker' , :child_of => 'Box' } ,
                        { :name => 'ColorPicker' , :child_of => 'Box' } ,
                        { :name => 'Calendar' , :child_of => 'Box' },
                        { :name => 'Icon' , :child_of => 'Box' },
                        { :name => 'IconBox' , :child_of => 'Box' },
                        { :name => 'IconBoxIcon' , :wraps => 'iconbox_icon', :child_of => 'Box' },
                        { :name => 'Notebook' , :child_of => 'Box' },
                        { :name => 'StatusBar' , :child_of => 'Box' },

                        { :name => 'MenuBar' , :child_of => 'Box' },
                            { :name => 'VMenuBar' , :child_of => 'MenuBar' },
                            { :name => 'HMenuBar' , :child_of => 'MenuBar' },
    
                        { :name => 'Button' , :child_of => 'Box' } ,
                            { :name => 'CheckButton' , :child_of => 'Button' },
                                { :name => 'RadioButton' , :child_of => 'CheckButton' },
                                
                        	{ :name => 'MenuItem' , :wraps => 'menu_item', :child_of => 'Button' },
                                { :name => 'MenuBase' , :child_of => 'MenuItem' },
                                    { :name => 'Menu' , :child_of => 'MenuBase' },
                                    { :name => 'IMenu' , :child_of => 'MenuBase' },
                                    { :name => 'Combo' , :child_of => 'MenuBase' },
                        
                        { :name => 'FileList' , :child_of => 'Box' } ,
                            { :name => 'FileList_List' , :child_of => 'FileList' } ,
                            { :name => 'FileList_Column' , :child_of => 'FileList' } ,
                            { :name => 'FileList_Icon' , :child_of => 'FileList' }
	] ,
	
	:ignore => [
	]
}