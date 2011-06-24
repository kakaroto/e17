#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var num_bubbles = 3;

function animator(arg, n, is_shadow) {
	t = elm.loop_time();
	fac = 2.0/num_bubbles;
	PI = 3.14159;
	r = 48;
	zz = (((2 + Math.sin(t*6 + (PI * (n * fac))))/3) * 64 ) * 2;
	xx = (Math.cos(t * 4 + (PI * (n * fac))) * r) * 2;
	yy = (Math.sin(t * 6 + (PI * (n * fac))) * r) * 2;

	x = my_window.width / 2 + xx - (arg.width / 2);
	y = my_window.height / 2 + yy - (arg.height / 2);
	w = zz;
	h = zz;

	/* FIXME: get pointer position */
	lx = 0;
	ly = 0;
	if (is_shadow) {
		x -= ((lx - (x + w / 2)) / 4);
		y -= ((ly - (y + h / 2)) / 4);
	}

	/* resize and move the object */
	arg.x = x;
	arg.y = y;
	arg.width = w;
	arg.height = h;
}

function bubble(n) {
	this.type = "image";
	this.image = "data/images/bubble.png";
	this.width = 64;
	this.height = 64;
	this.on_animate = function (arg) {
		animator(arg, n, false);
	}
}

function shadow(n) {
	this.type = "image";
	this.image = "data/images/bubble_sh.png";
	this.width = 64;
	this.height = 64;
	this.on_animate = function (arg) {
		animator(arg, n, true);
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
		shadow1 : new shadow(0),
		shadow2 : new shadow(1),
		shadow3 : new shadow(2),
	},
});
