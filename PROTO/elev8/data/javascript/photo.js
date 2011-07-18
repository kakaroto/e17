#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
		type : "main",
		label : "Photo Demo",
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
					the_photo : {
						type : "photo",
						size : 80,
						weight : EXPAND_BOTH,
						align : FILL_BOTH,
						resize : true,
						image : elm.datadir + "data/images/logo_small.png",
						fill : true,
					},
				},
			},
		},
});
