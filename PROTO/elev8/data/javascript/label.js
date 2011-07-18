var my_window = new elm.window({
	label : "Label example",
	width : 320,
	height : 200,
	elements : {
		bg : {
			type : "background",
			resize : true,
		},
		box : {
			type : "box",
			resize : true,
			elements : {
				small_label : {
					type : "label",
					label : "<b>This is a small label</b>",
					weight : { x : 0.0, y : 0.0 },
					align : { x : -1.0, y : -1.0 },
				},
				larger_label : {
					type : "label",
					label : "This is a larger label with newlines<br>" +
					       "to make it bigger, bit it won't expand or wrap<br>" +
					       "just be a block of text that can't change its<br>" +
					       "formatting as it's fixed based on text<br>",
					weight : { x : 0.0, y : 0.0 },
					align : { x : -1.0, y : -1.0 },
				},
				linewrap_label : {
					type : "label",
					wrap : 1,
					label : "<b>" +
					       "This is more text designed to line-wrap here as " +
					       "This object is resized horizontally. As it is " +
					       "resized vertically though, nothing should change. " +
					       "The amount of space allocated vertically should " +
					       "change as horizontal size changes." +
					       "</b>",
					weight : { x : 1.0, y : 0.0 },
					align : { x : -1.0, y : -1.0 },
				},
				small_wrap_label : {
					type : "label",
					label : "This small label set to wrap",
					weight : { x : 0.0, y : 0.0 },
					align : { x : -1.0, y : -1.0 },
				},
			},
		},
	},
});
