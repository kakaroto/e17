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
			slider_right : {
				type : "actionslider",
				weight : { x : 1.0, y : 0.0 },
				align : { x : -1.0, y : 0.0 },
			},
			slider_middle : {
				type : "actionslider",
				weight : { x : 1.0, y : 0.0 },
				align : { x : -1.0, y : 0.0 },
			},
			slider_left : {
				type : "actionslider",
				weight : { x : 1.0, y : 0.0 },
				align : { x : -1.0, y : 0.0 },
			},
		},
	},
};
