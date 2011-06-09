#!/usr/local/bin/elev8

var elements = {
	the_background : {
		type : "background",
		image : "data/images/rock_01.jpg",
	},
	pack_box : {
		type : "pack",
		elements : {
			first : {
				type : "label",
				label : "<b>This is a label</b><br>It has two lines",
			},
			ok_button : {
				type : "button",
				label : "OK",
				weight_x : -1.0,
				weight_y : -1.0,
				on_clicked : function() {
					print("click!");
				},
				animate_called : 0,
				on_animate : function() {
					if (!this.animate_called) {
						print("animate!");
						this.animate_called = 1;
					}
				},
			},
			cancel_button : {
				type : "button",
				label : "cancel",
				on_clicked : function() {
					print("clock!");
				}
			},
			check_button : {
				type : "button",
				label : "check",
			},
		},
	},
};
