#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };

var my_window = new elm.window({
    type: "main",
    label: "Elev8 demo",
    width: 1024,
    height: 768,
    elements : {
	background : {
	    type: "background",
	    weight: EXPAND_BOTH,
	    image: "fond.jpg",
	    resize: true,
	},
	bubble : {
	    type: "image",
	    width: 64,
	    height: 64,
	    file: "bubble.png",
	    dx: +2,
	    dy: -3,
	    on_animate : function (obj) {
		var x, y;

		x = obj.x + obj.dx;
		y = obj.y + obj.dy;

		if (x + obj.width > my_window.width || x < 0)
		{
		    obj.dx = -obj.dx;
		    x += 2 * obj.dx;
		}
		if (y + obj.height > my_window.height || y < 0)
		{
		    obj.dy = -obj.dy;
		    y += 2 * obj.dy;
		}

		obj.x = x;
		obj.y = y;
	    }
	}
    },
    on_keydown : function(obj, keyname) {
	switch (keyname)
	{
	case "Escape":
	    elm.exit();
	    break;
	default:
	    print(keyname);
	    break;
	}
    }
});
