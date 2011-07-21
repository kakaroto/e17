#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon = {
	type : "icon",
	image : elm.datadir + "data/images/logo_small.png",
};

var logo_icon_unscaled = {
	type : "icon",
	image : elm.datadir + "data/images/logo_small.png",
	scale_up : false,
	scale_down : false,
};

var my_window = new elm.window({
	type : "main",
	label : "Segment Control demo",
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			resize : true,
		},
		the_segment : {
			type : "segment",
			weight : { x: 1, y: 0 },
			resize : true,
			items : {
				foo: {
					label : "Only Text",
				},
				1: {
					icon : logo_icon,
				},
				2: {
					label : "Text_Icon_test",
					icon: logo_icon_unscaled,
				},
				3: {
					label : "Seg4",
				},
				4: {
					label : "Seg5",
				},
			}
		},
	},
});

