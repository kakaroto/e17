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
	label : "Button demo",
	width : 320,
	height : 480,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			resize : true,
		},
		scroll : {
			type : "scroller",
			bounce : { x : false, y : true },
			policy : { x : "off", y : "auto" },
			weight : EXPAND_BOTH,
			resize : true,
			content : {
				type : "box",
				weight : EXPAND_BOTH,
				elements : {
					icon_button : {
						type : "button",
						label : "Icon sized to button",
						weight : { x : -1.0, y : -1.0 },
						on_clicked : function(arg) {
							print("clicked event on Button: 1");
						},
						on_unpressed : function(arg) {
							print("unpressed event on Button: 1");
						},
						icon : logo_icon,
					},
					icon_no_scale : {
						type : "button",
						label : "Icon no scale",
						weight : { x : -1.0, y : -1.0 },
						icon : logo_icon_unscaled,
					},
					disabled : {
						type : "button",
						disabled : true,
						label : "Disabled Button",
						icon : logo_icon,
					},
					disabled_icon : {
						type : "button",
						icon : logo_icon,
						disabled : true,
					},
					label_only : {
						type : "button",
						label : "Label Only",
					},
					icon_only : {
						type : "button",
						icon : logo_icon_unscaled,
					},
					anchor_style : {
						type : "button",
						label : "Anchor style",
					},
					icon_anchor_style : {
						type : "button",
						disabled : true,
						icon : logo_icon,
					},
					icon_anchor_style_disabled : {
						type : "button",
						disabled : true,
						icon : logo_icon,
					},
				},
			},
		},
	},
});

