#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var entry_text =
            "Start typing...";

var my_window = new elm.window({
	type : "main",
	label : "Entry",
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
		the_box1 : {
			type : "box",
			weight : EXPAND_BOTH,
			align : FILL_BOTH,
			resize : true,
			elements : {
				the_entry : {
					type : "entry",
					text : entry_text,
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
					line_wrap : 3,
					editable : true,
				},
				icon_no_scale : {
					type : "button",
					label : "Clear",
					weight : { x : -1.0, y : -1.0 },
                    on_clicked : function(arg) {
                        my_window.elements.the_box1.elements.the_entry.text = "";
                    }
				},
            },
		},
    },
});
