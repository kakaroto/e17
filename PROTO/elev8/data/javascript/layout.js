

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
	type : "main",
	label : "Layout Demo",
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
		layout : {
		        type : "layout",
		        weight : EXPAND_BOTH,
		        align : FILL_BOTH,
		        resize : true,
		        file : {
		          name : 'layout.edj',
		          group : 'demo'
                        }
		}
        }
});
