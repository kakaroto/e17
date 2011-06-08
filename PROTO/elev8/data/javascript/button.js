var elements = {
	the_background : {
		type : "background",
		image : "data/images/rock_01.jpg",
	},
	pack_box : {
		type : "pack",
		elements : {
			ok_button : {
				type : "button",
				label : "OK",
				width : 60,
				height : 30,
				on_clicked : function() {
					print("click!");
				},
				/* on_animate : function() {
					print("animate!");
				} */
			},
			cancel_button : {
				type : "button",
				label : "cancel",
				x : 0,
				y : 40,
				width : 60,
				height : 60,
				on_clicked : function() {
					print("clock!");
				}
			},
			check_button : {
				type : "button",
				label : "check",
				x : 0,
				y : 130,
				width : 60,
				height : 20,
				style : "default",
			},
			radio_on : {
				type : "radio",
				label : "on",
				x : 0,
				y : 160,
				width : 60,
				height : 40,
			},
			radio_off : {
				type : "radio",
				label : "off",
				x : 0,
				y : 210,
				width : 60,
				height : 40,
			}
		}
	},
};
