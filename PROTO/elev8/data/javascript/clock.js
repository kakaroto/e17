#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var EDIT_HOUR = 2;
var EDIT_MINUTE = 8;
var EDIT_SECOND = 32;

var my_window = new elm.window({
	type : "main",
	label : "Clock Demo",
	width : 320,
	height : 480,
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
			resize : true,
			elements : {
				the_clock : {
					type : "clock",
					show_am_pm : 1,
					show_seconds : 1,
					time : { hh : 23, mm : 45, ss : 34 },
					edit : 0,
					digit_edit : EDIT_HOUR | EDIT_MINUTE | EDIT_SECOND,
					on_clicked : function(arg) {
						print("Edit button clicked\n");
					},
				},
			},
		},
	},
});
