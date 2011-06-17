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
			width : 50,
			height : 50,
			on_animate : function (arg) {
				t = elm.loop_time();
				radius_x = my_window.width/2 - 50;
				radius_y = my_window.height/2 - 50;
				PI = 3.14159;
				angle = t*PI;
				arg.x = Math.sin(angle) * radius_x + my_window.width/2 - arg.width/2;
				arg.y = Math.cos(angle) * radius_y + my_window.height/2 - arg.height/2;
			},
		},
	},
});
