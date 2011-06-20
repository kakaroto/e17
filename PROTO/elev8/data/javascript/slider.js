#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };

var win = new elm.main({
	label : "Slider",
	elements : {
		the_background : {
			type : "background",
			resize : true,
			weight : EXPAND_BOTH,
		},
		the_box : {
			type : "pack",
			weight : EXPAND_BOTH,
			resize : true,
			elements : {
				the_slider : {
					type : "slider",
					label : "Horizontal",
					units : "%1.1f units",
					span : 120,
					align : { x : -1.0, y : 0.5 },
					weight : EXPAND_BOTH,
				},
			},
		},
	},
});
