{
	:headers => [
            'Enhance.h'
	 ] ,
	
	:gcc_params => '-nostdinc ', 
	# :gcc_params => ' -D"__attribute__(x)="  -D"__extension__="  -D"__restrict=restrict"  -D"__inline=inline"  -D"__const=const" ',
	
	:module_base_name => 'Enhance' ,
	 
	:external_types => [ 
            'EXML',
            'Evas_Hash', 'Evas_List',
            'Etk_Widget'
	] ,
	
	:type_aliases => { 
        'Etk_Widget' => 'void',
        'Enhance_Signals_Enumerator' => 'void*',
        'Enhance_Widgets_Enumerator' => 'void*'
 	#'off_t' => 'long', 'mode_t' => 'int',  
 	#'uid_t' => 'unsigned int', 'gid_t' => 'unsigned int',
 	#'short int' => 'short', 
	#'unsigned short int' => 'unsigned short',
	#'time_t' => 'unsigned int'
	} ,

	:classes => [
            { :name => 'EnhanceBase', :wraps => 'enhance' } 
        ] ,
	
	:ignore => [
        'enhance_callback_data_set', 'enhance_callback_data_get'
	]
}