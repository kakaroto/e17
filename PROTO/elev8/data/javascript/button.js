var list = {
	ok_button : {
		label : "OK",
		width : 60,
		height : 30,
		on_clicked : function() {
			print("click!");
		}
	},
	cancel_button : {
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
		label : "check",
		x : 0,
		y : 130,
		width : 60,
		height : 20,
		style : "default",
		on_clicked : function() {
			print("clack!");
		}
	}
};

realize(list.ok_button);
realize(list.cancel_button);
realize(list.check_button);
