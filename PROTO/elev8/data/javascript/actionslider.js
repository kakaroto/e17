#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.main({
	label : "ActionSlider example",
	width : 320,
	height : 480,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			resize : true,
		},
		pack_box : {
			type : "pack",
			weight : { x : 1.0, y : 0.0 },
			resize : true,
			elements : {
				snooze_right : {
					type : "actionslider",
					weight : { x : 1.0, y : 0.0 },
					align : { x : -1.0, y : 0.0 },
					labels : { left : "Snooze", right : "Stop" },
					magnet : "right",
					slider : "right",
				},
				snooze_middle : {
					type : "actionslider",
					weight : { x : 1.0, y : 0.0 },
					align : { x : -1.0, y : 0.0 },
					labels : { left : "Snooze", right : "Stop" },
					magnet : "center",
					slider : "center",
				},
				accept_reject_middle : {
					type : "actionslider",
					weight : { x : 1.0, y : 0.0 },
					align : { x : -1.0, y : 0.0 },
					labels : { center : "Accept", right : "Reject" },
					magnet : "center",
				},
				left_right_left : {
					type : "actionslider",
					weight : { x : 1.0, y : 0.0 },
					align : { x : -1.0, y : 0.0 },
					label : "Go",
					labels : { center : "Accept", right : "Reject" },
				},
				left_right_center : {
					type : "actionslider",
					weight : { x : 1.0, y : 0.0 },
					align : { x : -1.0, y : 0.0 },
					label : "Go",
					labels : { left: "Left", center : "Center", right : "Right" },
				},
				enable_disable_center : {
					type : "actionslider",
					weight : { x : 1.0, y : 0.0 },
					align : { x : -1.0, y : 0.0 },
					labels : { left: "Left", center : "Center", right : "Right" },
					magnet : "center",
					slider : "center",
				},
			},
		},
	},
});
