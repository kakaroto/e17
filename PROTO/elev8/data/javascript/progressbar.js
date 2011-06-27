#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon = {
	type : "icon",
	image : "data/images/logo_small.png",
};

var logo_icon_unscaled = {
	type : "icon",
	image : "data/images/logo_small.png",
	scale_up : false,
	scale_down : false,
};

var bouncers = new Array(3);

function bouncing(on) {
	for (var i = 0; i < bouncers.length; i++) {
		bouncers[i].pulse(on);
	}
}

var my_window = new elm.main({
	type : "main",
	label : "Progressbar",
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			resize : true,
		},
		the_box : {
			type : "box",
			weight : EXPAND_BOTH,
			resize : true,
			elements : {
				progress_plain : {
					type : "progressbar",
					align : { x : -1.0, y : 0.5 },
					weight : EXPAND_BOTH,
				},
				infinite_bounce : {
					type : "progressbar",
					label : "Infinite bounce",
					align : { x : -1.0, y : 0.5 },
					weight : EXPAND_BOTH,
					pulse : true,
				},
				icon_progress : {
					type : "progressbar",
					label : "Label",
					icon : logo_icon,
					inverted : true,
					units : "%1.1f units",
					span : 200,
					align : { x : -1.0, y : 0.5 },
					weight : EXPAND_BOTH,
				},
				hbox : {
					type : "box",
					align : FILL_BOTH,
					weight : EXPAND_BOTH,
					horizontal : true,
					elements : {
						vertical_pb : {
							type : "progressbar",
							horizontal : false,
							label : "percent",
							align : FILL_BOTH,
							weight : EXPAND_BOTH,
							span : 60,
						},
						vert_infinite : {
							type : "progressbar",
							horizontal : false,
							label : "Infinite bounce",
							align : { x : -1.0, y : 0.5 },
							weight : EXPAND_BOTH,
							units : "",
							span : 80,
							pulse : true,
						},
						vert_icon : {
							type : "progressbar",
							label : "Label",
							horizontal : false,
							icon : logo_icon,
							inverted : true,
							format : "%1.2f%%",
							span : 200,
							align : { x : -1.0, y : 0.5 },
							weight : EXPAND_BOTH,
						},
					},
				},
				wheel : {
					type : "progressbar",
					style : "wheel",
					label : "Style: wheel",
					align : { x : -1.0, y : 0.5 },
					weight : EXPAND_BOTH,
				},
				button_box : {
					type : "box",
					weight : EXPAND_BOTH,
					horizontal : true,
					elements : {
						start : {
							type : "button",
							label : "Start",
							on_clicked : function() {
								bouncing(true);
							},
						},
						stop : {
							type : "button",
							label : "Stop",
							on_clicked : function() {
								bouncing(false);
							},
						},
					},
				},
			},
		},
	},
});

bouncers[0] = my_window.elements.the_box.elements.infinite_bounce;
bouncers[1] = my_window.elements.the_box.elements.hbox.elements.vert_infinite;
bouncers[2] = my_window.elements.the_box.elements.wheel;

