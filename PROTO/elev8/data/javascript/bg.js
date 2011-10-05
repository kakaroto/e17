#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var small_icon = elm.datadir + "data/images/logo_small.png";

var my_window = new elm.window({
	type : "main",
	label : "Background Demo",
	width : 320,
	height : 480,
	align : FILL_BOTH,
	weight : EXPAND_BOTH,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			align : FILL_BOTH,
			resize : true,
                        red : 255,
                        green : 0,
                        blue : 0,
		},
	},
});
