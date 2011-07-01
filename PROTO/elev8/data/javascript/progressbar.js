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

var bars = new Array(4);
var bouncers = new Array(3);
var start_time;

function set_animator(on) {
	start_time = elm.loop_time();
	for (var i = 0; i < bars.length; i++) {
		if (on) {
			bars[i].on_animate = function(what) {
				t = (elm.loop_time() - start_time);
				what.value = t/10.0;
			};
		}
		else {
			bars[i].on_animate = null;
		}
	}
}

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
								set_animator(true);
							},
						},
						stop : {
							type : "button",
							label : "Stop",
							on_clicked : function() {
								bouncing(false);
								set_animator(false);
							},
						},
					},
				},
			},
		},
	},
});

var el = my_window.elements.the_box.elements;

bars[0] = el.progress_plain;
bars[1] = el.icon_progress;
bars[2] = el.hbox.elements.vertical_pb;
bars[3] = el.hbox.elements.vert_icon;

bouncers[0] = el.infinite_bounce;
bouncers[1] = el.hbox.elements.vert_infinite;
bouncers[2] = el.wheel;

