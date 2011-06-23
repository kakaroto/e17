#!/usr/local/bin/elev8

/*
 * This is a reproduction of the elementary_test Slider example
 */

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
			type : "box",
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
					on_changed : function(me) {
						win.elements.the_box.elements.box2.elements.vert_inverted.value = me.value;
					},
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
					span : 80,
					min : 50,
					max : 150,
					value : 80,
					indicator : "%3.0f",
					align : { x: 0.5, y: 0.5 },
					weight : { x: 0, y: 0 },
					inverted : true,
					end : logo_icon,
				},
				disabled_inverted : {
					type : "slider",
					label : "Disabled Inverted",
					span : 80,
					min : 50,
					max : 150,
					value : 80,
					disabled : true,
					indicator : "%3.0f",
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
					indicator : "%3.0f",
					inverted : true,
					align : { x: -1, y: 0.5 },
					weight : EXPAND_BOTH,
				},
				box2 : {
					type : "box",
					horizontal : true,
					weight : EXPAND_BOTH,
					elements : {
						vert_inverted : {
							type : "slider",
							label : "Vertical inverted",
							inverted : true,
							units : "units",
							span : 60,
							align : { x: 0.5, y: -1 },
							weight : { x: 0, y: 1 },
							indicator : "%1.1f",
							horizontal : false,
							value : 0.2,
							scale : 1.0,
							icon : logo_icon,
						},
						vertical : {
							type : "slider",
							label : "Vertical",
							units : "units",
							span : 60,
							align : { x: 0.5, y: -1 },
							weight : { x: 0, y: 1 },
							indicator : "%1.1f",
							horizontal : false,
							value : 0.2,
						},
						vert_disabled : {
							type : "slider",
							label : "Disabled vertical",
							units : "units",
							inverted : true,
							horizontal : false,
							disabled : true,
							span : 100,
							value : 0.2,
							align : { x: 0.5, y: -1 },
							weight : { x: 0, y: 1 },
							icon : logo_icon,
						},
					},
				},
			},
		},
	},
});
