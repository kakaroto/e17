#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };

var win = new elm.window({
	label : "Flip test",
	width : 320,
	height : 480,
	elements : {
		the_background : {
			type : "background",
			image : elm.datadir + "data/images/rock_01.jpg",
			resize : true,
			weight : EXPAND_BOTH,
		},
		test_flip : {
			type : "flip",
			weight : EXPAND_BOTH,
			resize : true,
			front : {
				type : "button",
				label : "Heads",
				weight : EXPAND_BOTH,
				on_clicked : function() {
					print("click!");
					win.elements.test_flip.flip();
				},
			},
			back : {
				type : "button",
				label : "Tails",
				weight : EXPAND_BOTH,
				on_clicked : function() {
					print("clock!");
					win.elements.test_flip.flip();
				},
			},
		},
	},
});
