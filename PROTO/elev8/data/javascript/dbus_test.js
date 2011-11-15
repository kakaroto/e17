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
				},
                the_buttons : {
			            type : "box",
			            weight : EXPAND_BOTH,
			            horizontal : true,
			            resize : true,
			            elements : {
                            fdo : {
                                type : "button",
                                label : "UPower",
                                weight : { x : -1.0, y : -1.0 },
                                on_clicked : function() {
                                    print("Button Clicked");
                                    var test = new dbus("System");
                                    test.on_introspect= function(args){
                                        my_window.elements.the_box.elements.the_bus.text = arguments[0];
                                    },
                                    test.introspect("org.freedesktop.UPower", "/");
                                },
                            },
                            bz : {
                                type : "button",
                                label : "BlueZ",
                                weight : { x : -1.0, y : -1.0 },
                                on_clicked : function() {
                                    print("Button Clicked");
                                    var test = new dbus("Session");
                                    test.on_introspect= function(args){
                                        my_window.elements.the_box.elements.the_bus.text = arguments[0];
                                    },
                                    test.introspect("org.freedesktop.Notifications", "/org/freedesktop/Notifications");
                                },
                            },
                        },
                },
            }
        },
    },
});
