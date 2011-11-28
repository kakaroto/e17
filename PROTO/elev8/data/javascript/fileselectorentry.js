#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
	type : "main",
	label : "File Selector Button",
	width : 320,
	height : 480,
	weight : EXPAND_BOTH,
	align : FILL_BOTH,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			align : FILL_BOTH,
			resize : true,
		},
		the_box : {
			type : "box",
			weight : EXPAND_BOTH,
			align : FILL_BOTH,
			resize : true,
			elements : {
				fse : {
					type : "fileselectorentry",
					label : "Choose File",
                    align : FILL_BOTH,
                    win_title : "File Selector Entry Demo...",
                    win_size : { w : 150, h : 50 },
                    path : "/var/log/",
                    expandable : true,
                    folder_only : false,
                    is_save : false,
                    inwin_mode : true,
                    on_clicked : function(arg) {
                        print("File Selected = " + arguments[1]);
                    }
				},
			},
		},
	},
});
