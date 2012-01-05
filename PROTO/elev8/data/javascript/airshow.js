#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var EXPAND_X = { x : 1.0, y : 0.0 };
var EXPAND_Y = { x : 0.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var FILL_X = { x : -1.0, y : 0.0 };
var FILL_Y = { x : 0.0, y : -1.0 };

var myplane=null;

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
}

function move_up()
{

	myplane.y-=5;
	if(myplane.y<10)
		myplane.y=500;
}
function move_down()
{
	myplane.y+=5;
	if(myplane.y>590)
		myplane.y=10;
}
function move_right()
{
	myplane.x+=5;
	if(myplane.x>790)
		myplane.x=10;
}
function move_left()
{
	myplane.x-=5;
	if(myplane.x<10)
		myplane.x=790;
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
