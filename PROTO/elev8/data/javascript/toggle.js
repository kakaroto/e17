#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon = {
	type : "icon",
	image : elm.datadir + "data/images/logo_small.png",
};

var logo_icon_unscaled = {
	type : "icon",
	image : elm.datadir + "data/images/logo_small.png",
	scale_up : false,
	scale_down : false,
};

var my_window = new elm.window({
	type : "main",
	label : "Toggle demo",
	width : 320,
	height : 480,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			resize : true,
		},
		scroll : {
			type : "scroller",
			bounce : { x : false, y : true },
			policy : { x : "off", y : "auto" },
			weight : EXPAND_BOTH,
			resize : true,
			content : {
				type : "box",
				weight : EXPAND_BOTH,
				elements : {
					toggle1 : {
						type : "toggle",
						label : "Spades",
                        onlabel : "Numbers",
                        offlabel : "Alphabets",
                        icon : logo_icon,
						weight : { x : -1.0, y : -1.0 },
                        on_changed : function(arg) {
                            print("Numbers or Alphabets\n" + elm.datadir);
                        },
					},
				},
			},
		},
	},
});

