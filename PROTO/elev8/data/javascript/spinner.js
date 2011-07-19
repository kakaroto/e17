#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var FILL_X = { x : -1.0, y : 0.5 };

var my_window = new elm.window({
		type : "main",
		label : "Spinner Demo",
		width : 320,
		height : 480,
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
				resize : true,
				elements : {
					the_spinner : {
						type : "spinner",
						label_format : "%1.1f units",
						step : 1.3,
						wrap : 1,
						min : -50.0,
						max : 250.0,
						style : "vertical",
						disabled : false,
						editable : true,
						special_value : { value : 1, label : "January" },
						weight : EXPAND_BOTH,
						align : FILL_X,
					},
				},
			},
		},
});
