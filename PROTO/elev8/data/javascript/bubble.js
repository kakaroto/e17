#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
	type : "main",
	label : "Bubble Demo",
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
				the_bubble_1 : {
					type : "bubble",
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
				   text_part : { item : "info", label : "Hello Bubble" },
					corner : "bottom_right",
				},
				the_bubble_2 : {
					type : "bubble",
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
				   text_part : { item : "info", label : "Hi Bumble" },
					corner : "top_left",
					content : {
						type : "label",
						label : "<b>This is a small label</b>",
						weight : { x : 0.0, y : 0.0 },
						align : { x : -1.0, y : -1.0 },
					},
				},
				the_bubble_3 : {
					type : "bubble",
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
				   text_part : { item : "info", label : "Hi Bumble" },
					corner : "top_left",
					content : {
						type : "icon",
						image : elm.datadir + "data/images/logo_small.png",
					},
				},
			},
		},
	},
});
