#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var num_bubbles = 3;

function bubble(n) {
	this.type = "image";
	this.image = "data/images/bubble.png";
	this.width = 64;
	this.height = 64;
	this.on_animate = function (arg) {
		t = elm.loop_time();
		fac = 2.0/num_bubbles;
		PI = 3.14159;
		r = 48;
		zz = (((2 + Math.sin(t*6 + (PI * (n * fac))))/3) * 64 ) * 2;
		xx = (Math.cos(t * 4 + (PI * (n * fac))) * r) * 2;
		yy = (Math.sin(t * 6 + (PI * (n * fac))) * r) * 2;

		/* resize and move the object */
		arg.x = my_window.width / 2 + xx - (arg.width / 2);
		arg.y = my_window.height / 2 + yy - (arg.height / 2);
		arg.width = zz;
		arg.height = zz;
	}
}

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
		bubble1 : new bubble(0),
		bubble2 : new bubble(1),
		bubble3 : new bubble(2),
	},
});
