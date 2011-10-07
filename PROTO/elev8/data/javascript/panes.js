#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon = {
	type : "icon",
	image : elm.datadir + "data/images/logo_small.png",
};

var my_window = new elm.window({
	type : "main",
	label : "Panes Demo",
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
		the_pane_1 : {
			type : "pane",
			weight : EXPAND_BOTH,
			resize : true,
                        fill : true,
                        content_left : {
				type : "button",
				label : "Left",
				weight : { x : -1.0, y : -1.0 },
				on_clicked : function(arg) {
					print("clicked event on Button: Left");
				},
				on_unpressed : function(arg) {
					print("unpressed event on Button: Left");
				},
				icon : logo_icon,
			},
                        content_right : {
				type : "pane",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
				resize : true,
				fill : true,
				horizontal : true,
				content_left : {
					type : "button",
					label : "Up",
					weight : { x : -1.0, y : -1.0 },
					on_clicked : function(arg) {
						print("clicked event on Button: Up");
					},
					on_unpressed : function(arg) {
						print("unpressed event on Button: Up");
					},
					icon : logo_icon,
				},
				content_right : {
					type : "button",
					label : "Down",
					weight : { x : -1.0, y : -1.0 },
					on_clicked : function(arg) {
						print("clicked event on Button: Down");
					},
					on_unpressed : function(arg) {
						print("unpressed event on Button: Down");
					},
					icon : logo_icon,
				},
				on_press : function(arg) {
					print("clicked event on Pane 2");
				},
				on_unpressed : function(arg) {
					print("unpressed event on Pane 2");
				},
			},
			on_press : function(arg) {
				print("clicked event on Pane 1");
			},
			on_unpressed : function(arg) {
				print("unpressed event on Pane 1");
			},
		},
	},
});
