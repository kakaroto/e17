#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.main({
	label : "Horizontal Box example",
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			resize : true,
		},
		the_box : {
			type : "box",
			weight : EXPAND_BOTH,
			horizontal : true,
			resize : true,
			elements : {
				logo_top : {
					type : "icon",
					image : "data/images/logo_small.png",
					scale : { x : false, y : false },
					align : { x : 0.5, y : 0.5 },
				},
				logo_middle : {
					type : "icon",
					image : "data/images/logo_small.png",
					scale : { x : false, y : false },
					align : { x : 0.5, y : 0.0 },
				},
				logo_bottom : {
					type : "icon",
					image : "data/images/logo_small.png",
					scale : { x : false, y : false },
					align : { x : 0.0, y : 1.0 },
				},
			},
		},
	},
});
