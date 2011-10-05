#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
	label : "Vertical Box example",
	align : FILL_BOTH,
	weight : EXPAND_BOTH,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			resize : true,
		},
		the_box : {
			type : "box",
			weight : EXPAND_BOTH,
			resize : true,
			elements : {
				logo_top : {
					type : "icon",
					image : elm.datadir + "data/images/logo_small.png",
					scale_up : false,
					scale_down : false,
					align : { x : 0.5, y : 0.5 },
				},
				logo_middle : {
					type : "icon",
					image : elm.datadir + "data/images/logo_small.png",
					scale_up : false,
					scale_down : false,
					align : { x : 0.0, y : 0.5 },
				},
				logo_bottom : {
					type : "icon",
					image : elm.datadir + "data/images/logo_small.png",
					scale_up : false,
					scale_down : false,
					align : { x : 1.0, y : 0.5 },
				},
			},
		},
	},
});
