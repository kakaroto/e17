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
						on_animate : function(me) {
							print("animate!");
							me.on_animate = null;
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
					once_button : {
						type : "button",
						label : "Once",
						on_clicked : function(me) {
							me.on_clicked = null;
							print("clicked once button");
						},
					},
					check_button : {
						type : "button",
						label : "test",
						weight : { x : 0 , y : 0 },
						align : { x : 1, y : 0 },
						on_clicked : function(me) {
							my_window.label = "hello world";
							print("weight.x = " + me.weight.x + " weight.y = " + me.weight.y);
						},
					},
					wider_button : {
						type : "button",
						label : "Wider",
						align : { x : -1.0, y : -1.0 },
						on_clicked : function(me) {
							my_window.width += 50;
						},
					},
					narrower_button : {
						type : "button",
						label : "Narrower",
						align : { x : -1.0, y : -1.0 },
						on_clicked : function(me) {
							my_window.width -= 50;
							print(my_window.width);
						},
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

print("weight.x = " + my_window.elements.scroll.content.elements.check_button.weight.x);
print("scroller.policy.x = " + my_window.elements.scroll.policy.x);
print("scroller.policy.y = " + my_window.elements.scroll.policy.y);
