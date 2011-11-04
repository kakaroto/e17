#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

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
		the_box : {
			type : "box",
			weight : EXPAND_BOTH,
			resize : true,
			elements : {
                icon_button : {
                    type : "button",
                    label : "Icon sized to button",
                    weight : { x : -1.0, y : -1.0 },
                    on_clicked : function() {
                        print("Button Clicked");
                        var test = new dbus("Session");
                    },
                },
            }
        },
    },
});
