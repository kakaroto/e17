#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var elements = {
	the_background : {
		type : "background",
		weight : EXPAND_BOTH,
	},
	pack_box : {
		type : "pack",
		weight : { x : 1.0, y : 0.0 },
		elements : {
			snooze_right : {
				type : "actionslider",
				weight : { x : 1.0, y : 0.0 },
				align : { x : -1.0, y : 0.0 },
				label : { left : "Snooze", right : "Stop" },
				magnet : "right",
				slider : "right",
			},
			snooze_middle : {
				type : "actionslider",
				weight : { x : 1.0, y : 0.0 },
				align : { x : -1.0, y : 0.0 },
				label : { left : "Snooze", right : "Stop" },
				magnet : "center",
				slider : "center",
			},
			accept_reject_middle : {
				type : "actionslider",
				weight : { x : 1.0, y : 0.0 },
				align : { x : -1.0, y : 0.0 },
				label : { center : "Accept", right : "Reject" },
				magnet : "center",
			},
			left_right_left : {
				type : "actionslider",
				weight : { x : 1.0, y : 0.0 },
				align : { x : -1.0, y : 0.0 },
				label : { center : "Accept", right : "Reject" },
			},
			left_right_center : {
				type : "actionslider",
				weight : { x : 1.0, y : 0.0 },
				align : { x : -1.0, y : 0.0 },
				label : { left: "Left", center : "Center", right : "Right" },
			},
			left_right_center : {
				type : "actionslider",
				weight : { x : 1.0, y : 0.0 },
				align : { x : -1.0, y : 0.0 },
				label : { left: "Left", center : "Center", right : "Right" },
				magnet : "center",
				slider : "center",
			},
		},
	},
};
