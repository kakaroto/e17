#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
	type : "main",
	label : "D-Feet Demo",
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
				the_entry : {
					type : "entry",
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
					line_wrap : 3,
					editable : false,
				},
				the_bus : {
					type : "entry",
					text : "org.freedesktop.DBus",
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
					line_wrap : 3,
                    singleline : 1,
					editable : true,
				},
                the_button : {
                    type : "button",
                    label : "Get ConnMan State",
                    weight : { x : -1.0, y : -1.0 },
                    on_clicked : function() {
                        print("Button Clicked");
                        var test = new dbus("System");
                        test.introspect("net.connman",
                                        "/org/freedesktop/NetworkManager",
                                        "org.freedesktop.NetworkManager",
                                        "state"); 

                    },
                },
            }
        },
    },
});
