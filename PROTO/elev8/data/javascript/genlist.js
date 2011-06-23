#!/usr/local/bin/elev8

/*
 * This is a reproduction of the elementary_test Genlist example
 */

var EXPAND_BOTH = { x : 1.0, y : 1.0 };

var win = new elm.main({
	label : "Genlist",
	width : 480,
	height : 800,
	elements : {
		the_background : {
			type : "background",
			resize : true,
			weight : EXPAND_BOTH,
		},
		the_box : {
			type : "box",
			weight : EXPAND_BOTH,
			resize : true,
			elements : {
				the_list : {
					type : "genlist",
					weight : EXPAND_BOTH,
					align : { x: -1, y: -1 },
				},
			},
		},
	},
});
