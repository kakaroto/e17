#!/usr/local/bin/elev8

//ELEV8_IMAGE:logo_small.jpg:http://farm7.static.flickr.com/6187/6072313250_a5b8d354b5_s.jpg
//ELEV8_IMAGE:green.jpg:http://farm7.static.flickr.com/6184/6072313786_8b0eb09826_s.jpg
//ELEV8_IMAGE:violet.jpg:http://farm7.static.flickr.com/6209/6072313750_50874d9bfd_s.jpg
//ELEV8_IMAGE:maroon.jpg: http://farm7.static.flickr.com/6084/6071770879_252f85bf70_s.jpg
//ELEV8_IMAGE:orange.jpg:http://farm7.static.flickr.com/6089/6072313680_85795c6ea2_s.jpg
//ELEV8_IMAGE:yellow.jpg:http://farm7.static.flickr.com/6188/6072313656_476f7fdf2d_s.jpg

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var small_icon = elm.datadir + "logo_small.jpg";
var maroon = elm.datadir + "maroon.jpg";
var yellow = elm.datadir + "yellow.jpg";
var orange = elm.datadir + "orange.jpg";
var violet = elm.datadir + "violet.jpg";

var menu_content = {
	type : "menu",
	x : 50, 
	y : 50,
	items : {
		item_1 : {
			label : "item 1",
			disabled : true,
			items : {
				item_2 : {
					icon : small_icon,
					label : "item 2",
					on_clicked : function (arg) {
						print("Item 1 selected\n");
					},
				},
				item_3 : {
					icon : yellow,
					label : "item 3",
				},
				item_4 : {
					icon : violet,
					label : "item 4",
					on_clicked : function (arg) {
						print("Item 4 selected\n");
					},
				},
			},
		},
		item_5 : {
			icon : orange,
			label : "item 5",
			separator : true,
		},
		item_6 : {
			label : "item 6",
			disabled : true,
			icon : maroon,
			items : {
				item_7 : {
					icon : yellow,
					label : "item 7",
				},
				item_8 : {
					icon : orange,
					label : "item 8",
				        seperator : true,
				},
				item_9 : {
					icon : violet,
					label : "item 9",
					on_clicked : function (arg) {
						print("Item 9 selected\n");
					},
				},
			},
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
