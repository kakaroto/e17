#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon = {
	type : "icon",
	image : "data/images/logo_small.png",
};

var logo_icon_unscaled = {
	type : "icon",
	image : "data/images/logo_small.png",
	scale_up : false,
	scale_down : false,
};

var my_window = new elm.main({
	type : "main",
	label : "Check box demo",
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
				icon_sized_check : {
					type : "check",
					label : "Icon sized to check",
					weight : { x : -1.0, y : -1.0 },
					align : { x : -1.0, y : 0.5 },
					icon : logo_icon,
					state : true,
				},
				icon_check : {
					type : "check",
					label : "Icon no scale",
					icon : logo_icon_unscaled,
				},
				label_check : {
					type : "check",
					label : "Label Only",
				},
				disabled_check : {
					type : "check",
					label : "Disabled check",
					weight : { x : -1.0, y : -1.0 },
					align : { x : -1.0, y : 0.5 },
					icon : logo_icon,
					state : true,
					disabled : true,
				},
				disabled_icon_only : {
					type : "check",
					icon : logo_icon_unscaled,
				},
			},
		},
	},
});

