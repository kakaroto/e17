var animate_called = 0;

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
				on_clicked : function() {
					print("click!");
				},
				on_animate : function() {
					if (!animate_called) {
						print("animate!");
						animate_called = 1;
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
