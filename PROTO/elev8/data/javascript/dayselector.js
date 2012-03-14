#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
	type : "main",
	label : "Day Selector Demo",
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
				dubai : {
					type : "dayselector",
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
                    week_start : 6,
                    monday : true,
                    tuesday : true,
                    wednesday : true,
                    thursday : true,
                    friday : true,
                    sunday : true,
					on_changed : function(arg) {
						print("Day changed");
					},
				},
				australia : {
					type : "dayselector",
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
                    week_start : 1,
                    saturday : true,
                    tuesday : true,
                    wednesday : true,
                    thursday : true,
                    friday : true,
                    sunday : true,
					on_changed : function(arg) {
						print("Day changed");
					},
				},
				usa : {
					type : "dayselector",
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
                    week_start : 0,
                    monday : true,
                    tuesday : true,
                    wednesday : true,
                    thursday : true,
                    friday : true,
                    saturday : true,
					on_changed : function(arg) {
						print("Day changed");
					},
				},
			},
		},
	},
});
