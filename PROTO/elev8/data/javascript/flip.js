#!/usr/local/bin/elev8

var elements = {
	the_background : {
		type : "background",
		image : "data/images/rock_01.jpg",
	},
	test_flip : {
		type : "flip",
		weight : EXPAND_BOTH,
		top_elements : {
			ok_button : {
				type : "button",
				label : "OK",
				weight : EXPAND_BOTH,
				on_clicked : function() {
					print("click!");
				},
			},
		},
		bottom_elements : {
			cancel_button : {
				type : "button",
				label : "cancel",
				weight : EXPAND_BOTH,
				on_clicked : function() {
					print("clock!");
				},
			},
		},
	},
};
