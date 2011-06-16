#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.main({
	type : "main",
	label : "Button demo",
	width : 320,
	height : 480,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			image: "data/images/rock_01.jpg",
			resize : true,
		},
		scroll : {
			type : "scroller",
			bounce : { x : false, y : true },
			policy : { x : "off", y : "auto" },
			resize : true,
			content : {
				type : "pack",
				elements : {
					first : {
						type : "label",
						label : "<b>This is a label</b><br>It has two lines",
					},
					counter_button : {
						type : "button",
						label : "Press me!",
						weight : { x : -1.0, y : -1.0 },
						on_clicked : function() {
							if (this.clicked == 1)
								this.n++;
							else
								this.n = 1;
							this.clicked = 1;
							my_window.elements.scroll.content.elements.counter_button.label = "Pressed me " + this.n + " times";
						},
					},
					ok_button : {
						type : "button",
						label : "OK",
						weight : { x : -1.0, y : -1.0 },
						on_clicked : function() {
							if (my_window.elements.scroll.content.elements.ok_button.label == "OK")
								my_window.elements.scroll.content.elements.ok_button.label = "not OK";
							else
								my_window.elements.scroll.content.elements.ok_button.label = "OK";

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
						weight : EXPAND_BOTH,
						on_clicked : function() {
							print("clock!");
						},
					},
					check_button : {
						type : "button",
						label : "check",
						align : { x : -1.0, y : -1.0 },
					},
					hidden_button : {
						type : "button",
						label : "no see me",
						hidden : true,
						align : { x : -1.0, y : -1.0 },
					},
					logo : {
						type : "icon",
						image : "data/images/logo_small.png",
					}
				},
			},
		},
	},
});

print("label = " + my_window.label);
print("type  = " + my_window.type);
print("width = " + my_window.width);
print("width = " + my_window.height);

