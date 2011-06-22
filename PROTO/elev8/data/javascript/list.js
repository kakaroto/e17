#!/usr/local/bin/elev8

/*
 * This is a reproduction of the elementary_test Genlist example
 */

var EXPAND_BOTH = { x : 1.0, y : 1.0 };

var win = new elm.main({
	label : "list",
	width : 320,
	height : 300,
	elements : {
		the_background : {
			type : "background",
			resize : true,
			weight : EXPAND_BOTH,
		},
		the_list : {
			type : "list",
			weight : EXPAND_BOTH,
			align : { x: -1, y: -1 },
			elements : {
				0 : {
					label : "Hello",
				},
				1 : {
					label : "world",
				},
				2 : {
					label : ".",
				},
				3 : {
					label : "How",
				},
				4 : {
					label : "are",
				},
				5 : {
					label : "you",
				},
				6 : {
					label : "doing?",
				},
			},
		},
	},
});
