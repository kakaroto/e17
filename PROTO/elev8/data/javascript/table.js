#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
		type : "main",
		label : "Table Demo",
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
			the_table : {
				type : "table",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
				subobjects : {
					0 : {
						subobject : {
							type : "button",
							label : "I am the one.",
						},
						x : 0,
						y : 0,
						w : 1,
						h : 1,
					},
					1 : {
						subobject : {
							type : "button",
							label : "Dodge Bullets",
						},
						x : 1,
						y : 0,
						w : 1,
						h : 1,
					},
					2 : {
						subobject : {
							type : "button",
							label : "Kill'em All",
						},
						x : 2,
						y : 0,
						w : 1,
						h : 1,
					},
					3 : {
						subobject : {
							type : "button",
							label : "Astala Vista Baby",
						},
						x : 0,
						y : 1,
						w : 2,
						h : 1,
					},
				},
			},
		},
});

