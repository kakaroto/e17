#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var small_icon = elm.datadir + "data/images/logo_small.png";

var menu_content = {
	type : "menu",
	move : { x : 50, y : 50 },
	items : {
		item_1 : {
			label : "item 1",
			disabled : true,
			items : {
				item_2 : {
					icon : small_icon,
					label : "item 2",
				},
				item_3 : {
					icon : small_icon,
					label : "item 3",
				},
				item_4 : {
					icon : small_icon,
					label : "item 4",
				},
			},
		},
		item_5 : {
			icon : small_icon,
			label : "item 5",
		},
		item_6 : {
			label : "item 6",
			disabled : true,
		},
		item_7 : {
			icon : small_icon,
			label : "item 7",
		},
		item_8 : {
			icon : small_icon,
			label : "item 8",
		},
		item_9 : {
			icon : small_icon,
			label : "item 9",
		},
	},
};

var my_window = new elm.window({
	type : "main",
	label : "Menu Demo",
	width : 320,
	height : 480,
	align : FILL_BOTH,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			align : FILL_BOTH,
			resize : true,
		},
		the_menu : menu_content,
	},
});
