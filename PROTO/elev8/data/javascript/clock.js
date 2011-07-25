#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var EDIT_HOUR = 2; //constants required by elementary - why should I expose this?
var EDIT_MINUTE = 8; // constants required by elementary - why should I expose this?
var EDIT_SECOND = 32; // constants required by elementary - why should I expose this?

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
					hour : 23,
					minute : 45,
					second : 34,
					edit : true,
					digit_edit : EDIT_HOUR | EDIT_MINUTE | EDIT_SECOND,
					on_clicked : function(arg) {
						print("Edit button clicked\n");
					},
				},
			},
		},
	},
});
