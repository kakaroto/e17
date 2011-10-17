#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
		type : "main",
		label : "Photocam Demo",
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
				align : FILL_BOTH,
                resize : true,
				elements : {
                    the_photocam : {
                            type : "photocam",
                            weight : EXPAND_BOTH,
                            align : FILL_BOTH,
                            file : elm.datadir + "data/images/sky_01.jpg",
					},
				    /*show : {
							type : "button",
							label : "Show",
					},
					drag : {
							type : "button",
							label : "Drag",
					},
                    pinch : {
							type : "button",
							label : "Pinch",
					},
                    zoom : {
							type : "button",
							label : "Zoom",
					},*/
				},
			},
		},
});
