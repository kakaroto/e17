var elm = require('elm');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var EXPAND_X = { x : 1.0, y : 0.0 };
var EXPAND_Y = { x : 0.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var FILL_X = { x : -1.0, y : 0.0 };
var FILL_Y = { x : 0.0, y : -1.0 };

var myplane=null;
var num_planes = 4;

var alien = elm.Image ({
    file : elm.datadir + "data/images/alien.png",
    width : 30,
    height : 40,
    x : 10,
    y : 10,
});

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

function choose_plane()
{
}

function make_alien_population()
{
    for(var i = 0; i<50; i++)
    {
        my_win.elements[i] = alien;
        my_win.elements[i].x = (((i/10) + 1) * 10) + (i * 4)
        my_win.elements[i].y = (((i/10) + 1) * 10) + (i * 4);
    }
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

    my_win.elements.plane1.on_animate = null;
    my_win.elements.plane2.on_animate = null;
    my_win.elements.plane3.on_animate = null;
    my_win.elements.plane4.on_animate = null;

    make_alien_population();

}

var my_win = elm.realise(elm.Window({
        title : "Air Show",
        width : 800,
        height : 600,
        weight : EXPAND_BOTH,
        align : FILL_BOTH,
        elements : {
            the_background : elm.Background ({
                weight : EXPAND_BOTH,
                align : FILL_BOTH,
                image : elm.datadir + "data/images/space.png",
                resize : true,
            }),
            plane1 : elm.Image ({
                file : elm.datadir + "data/images/plane1.png",
                width : 50,
                height : 70,
                x : 350,
                y : 100,
                on_click : function() {
                    if(myplane==null)
                    {
                        print(this.height + " " + this.weight);
                        myplane = my_win.elements.plane1 ;
                        init();
                    }
                },
                on_animate : choose_plane,
            }),
            plane2 : elm.Image ({
                file : elm.datadir + "data/images/plane2.png",
                width : 50,
                height : 70,
                x : 200,
                y : 250,
                on_click : function() {
                    if(myplane==null)
                    {
                        print(this.height + " " + this.weight);
                        myplane = my_win.elements.plane2;
                        init();
                    }
                },
                on_animate : choose_plane,
            }),
            plane3 : elm.Image ({
                file : elm.datadir + "data/images/plane3.png",
                width : 50,
                height : 70,
                x : 500,
                y : 250,
                on_click : function() {
                    if(myplane==null)
                    {
                        print(this.height + " " + this.weight);
                        myplane = my_win.elements.plane3;
                        init();
                    }
                },
                on_animate : choose_plane,
            }),
            plane4 : elm.Image ({
                file : elm.datadir + "data/images/plane4.png",
                width : 50,
                height : 70,
                x : 350,
                y : 400,
                on_click : function() {
                    print("On Click Triggered");
                    if(myplane==null)
                    {
                        print(this.height + " " + this.weight);
                        myplane = my_win.elements.plane4;
                        init();
                    }
                },
                on_animate : choose_plane,
            }),
        },
        on_key_down : function () {
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
}));
