#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };

function flip() {
	print("flip");
}

var elements = {
	the_background : {
		type : "background",
		image : "data/images/rock_01.jpg",
		resize : true,
	},
	test_flip : {
		type : "flip",
		weight : EXPAND_BOTH,
		resize : true,
		front : {
			type : "button",
			label : "OK",
			weight : EXPAND_BOTH,
			on_clicked : function() {
				print("click!");
				flip();
			},
		},
		back : {
			type : "button",
			label : "cancel",
			weight : EXPAND_BOTH,
			on_clicked : function() {
				print("clock!");
				flip();
			},
		},
	},
};
