#!/usr/local/bin/elev8

/*
 * This is a reproduction of the elementary_test Genlist example
 */

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

/*var button = {
	type : "button",
	label : "GenList Item",
	//weight : { x : -1.0, y : -1.0 },
	on_clicked : function(arg) {
		print("clicked event on Button: 1 " + this.label);
	},
};

var text_get = function(arg) {
	print("Calling text get function.");
	return "Clone";
};

var content_get = function(arg) {
	print("Calling content get function.");
	return button;
};

var state_get = function(arg) {
	print("Calling state get function.");
};

var delete_item = function(arg) {
	print("Calling delete item function.");
};*/

var win = new elm.window({
	type : "main",		
	label : "Genlist Demo",
	width : 480,
	height : 800,
	weight : EXPAND_BOTH,
	align : FILL_BOTH,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			align : FILL_BOTH,
			resize : true,
		},
		the_box : {
			type : "box",
			weight : EXPAND_BOTH,
			//resize : true,
			elements : {
			    scroll : {
					type : "scroller",
					weight : EXPAND_BOTH,
					bounce : { x : false, y : true },
					policy : { x : "off", y : "on" },
					weight : EXPAND_BOTH,
					resize : true,
					content : {
						type : "box",
						weight : EXPAND_BOTH,
						elements : {
							the_list : {
								type : "genlist",
								weight : EXPAND_BOTH,
							},
						},
						but_box : {
							type : "box",
							weight : EXPAND_BOTH,
							horizontal : true,
							resize : true,
							elements : {
								prepend : {
									type : "button",
									label : "Prepend",
									weight : EXPAND_BOTH,
								},
								append : {
									type : "button",
									label : "Append",
									weight : EXPAND_BOTH,
								},
							},
						},
					},
				},
			},
		},
	},
});
