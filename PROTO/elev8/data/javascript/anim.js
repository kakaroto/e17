#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.main({
	type : "main",
	label : "Button demo",
	width : 480,
	height : 800,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			image : "data/images/rock_01.jpg",
			resize : true,
		},
		bubble1 : {
			type : "image",
			image : "data/images/bubble.png",
			x : 100,
			y : 100,
			width : 64,
			height : 64,
		},
		shadow1 : {
			type : "image",
			image : "data/images/bubble_sh.png",
			x : 200,
			y : 200,
			width : 64,
			height : 64,
		},
	},
});

print("label = " + my_window.label);
print("type  = " + my_window.type);
print("width = " + my_window.width);
print("height = " + my_window.height);

