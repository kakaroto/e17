#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var EXPAND_X = { x : 1.0, y : 0.0 };
var EXPAND_Y = { x : 0.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var FILL_X = { x : -1.0, y : 0.0 };
var FILL_Y = { x : 0.0, y : -1.0 };

var myplane=null;
var num_planes = 4;

function move_up()
{
	myplane.y-=10;
	if(myplane.y<10)
		myplane.y=500;
}
function move_down()
{
	myplane.y+=10;
	if(myplane.y>590)
		myplane.y=10;
}
function move_right()
{
	myplane.x+=10;
	if(myplane.x>790)
		myplane.x=10;
}
function move_left()
{
	myplane.x-=10;
	if(myplane.x<10)
		myplane.x=790;
}

function animator(arg, n, is_shadow) {
	t = elm.loop_time();
	fac = 3.0/num_planes;
	r = 120;
	zz = (((2 + Math.sin(t*6 + (Math.PI * (n * fac))))/3) * 64 ) * 2;
	xx = (Math.cos(t * 4 + (Math.PI * (n * fac))) * r) * 2;
	yy = (Math.sin(t * 6 + (Math.PI * (n * fac))) * r) * 2;

	x = my_win.width / 2 + xx - (arg.width / 2);
	y = my_win.height / 2 + yy - (arg.height / 2);
	w = zz;
	h = zz;

	/* get pointer position */
	lx = arg.pointer.x
	ly = arg.pointer.y;
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

function init()
{
	my_win.elements.plane1.x = 1;
	my_win.elements.plane1.y = 1;
	my_win.elements.plane2.x = 1;
	my_win.elements.plane2.y = 150;
	my_win.elements.plane3.x = 1;
	my_win.elements.plane3.y = 300;
	my_win.elements.plane4.x = 1;
	my_win.elements.plane4.y = 450;
	myplane.on_animate = null;
}

var my_win = new elm.window({
		type : "main",
		label : "Air Show",
		width : 800,
		height : 600,
		weight : EXPAND_BOTH,
		align : FILL_BOTH,
		elements : {
			the_background : {
				type : "background",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
				image : elm.datadir + "data/images/space.png",
				resize : true,
			},
			plane1 : {
				type : "image",
				file : elm.datadir + "data/images/plane1.png",
                width : 50,
                height : 70,
                x : 280,
                y : 80,
				on_clicked : function() {
				    if(myplane==null)
				    {
                        print(this.height + " " + this.weight);
				        myplane = my_win.elements.plane1 ;
					    init();
				    }
				},
				on_animate : function(arg) {
						animator(arg, 0, false);
				},
            },
			plane2 : {
				type : "image",
				file : elm.datadir + "data/images/plane2.png",
                width : 50,
                height : 70,
                x : 280,
                y : 200,
				on_clicked : function() {
				    if(myplane==null)
					{
                        print(this.height + " " + this.weight);
					    myplane = my_win.elements.plane2;
						init();
				    }
			    },
				on_animate : function(arg) {
						animator(arg, 1, false);
				},
		    },
		    plane3 : {
			    type : "image",
				file : elm.datadir + "data/images/plane3.png",
                width : 50,
                height : 70,
                x : 280,
                y : 320,
				on_clicked : function() {
					if(myplane==null)
					{
                        print(this.height + " " + this.weight);
					    myplane = my_win.elements.plane3;
						init();
				    }
				},
				on_animate : function(arg) {
						animator(arg, 2, false);
				},
		    },
			plane4 : {
			    type : "image",
				file : elm.datadir + "data/images/plane4.png",
                width : 50,
                height : 70,
                x : 280,
                y : 440,
				on_clicked : function() {
                    print("On Click Triggered");
				    if(myplane==null)
					{
                        print(this.height + " " + this.weight);
					    myplane = my_win.elements.plane4;
						init();
				    }
			    },
				on_animate : function(arg) {
						animator(arg, 3, false);
				},
		    },
		},
		on_keydown : function () {
			print("Argument = " + arguments[1] + "\n");
			print("title = " + this.label + "\n");

			if (arguments[1] == "Up")
				move_up();
			if (arguments[1] == "Down")
				move_down();
			if (arguments[1] == "Left")
				move_left();
			if (arguments[1] == "Right")
				move_right();
		},
});
