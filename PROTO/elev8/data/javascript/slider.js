#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };

var logo_icon = {
	type : "icon",
	image : "data/images/logo_small.png",
};

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
				horz_slider : {
					type : "slider",
					label : "Horizontal",
					units : "%1.1f units",
					span : 120,
					align : { x : -1.0, y : 0.5 },
					weight : EXPAND_BOTH,
					icon : logo_icon,
				},
				disabled_slider : {
					type : "slider",
					label : "Disabled",
					units : "%1.1f units",
					span : 120,
					min : 50,
					max : 150,
					value : 80,
					disabled : true,
					align : { x : -1.0, y : 0.5 },
					weight : EXPAND_BOTH,
					icon : logo_icon,
				},
				inverted_slider : {
					type : "slider",
					label : "Horizontal Inverted",
					units : "%3.0f units",
					span : 80,
					min : 50,
					max : 150,
					align : { x: 0.5, y: 0.5 },
					weight : { x: 0, y: 0 },
					inverted : true,
					end : logo_icon,
				},
				disabled_inverted : {
					type : "slider",
					label : "Disabled Inverted",
					units : "%3.0f units",
					span : 80,
					min : 50,
					max : 150,
					disabled : true,
					align : { x: 0.5, y: 0.5 },
					weight : { x: 0, y: 0 },
					inverted : true,
					end : logo_icon,
				},
				scaled_double : {
					type : "slider",
					label : "Scaled Double",
					units : "%3.0f units",
					span : 40,
					min : 50,
					max : 150,
					value : 80,
					scale : 2.0,
					inverted : true,
					align : { x: -1, y: 0.5 },
					weight : EXPAND_BOTH,
				},
				vert_inverted : {
					type : "slider",
					label : "Vertical inverted",
					inverted : true,
					units : "units",
					span : 60,
					align : { x: 0.5, y: -1 },
					weight : { x: 0, y: 1 },
					//indicator : "%1.1f",
					horizontal : false,
					value : 0.2,
					scale : 1.0,
				},
			},
		},
	},
});

print("min   = " + win.elements.the_box.elements.disabled_slider.min);
print("max   = " + win.elements.the_box.elements.disabled_slider.max);
print("value = " + win.elements.the_box.elements.disabled_slider.value);
