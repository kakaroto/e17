#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var entry_text =
            "This is an entry widget in this window that<br>" +
            "uses markup <b>like this</> for styling and<br>" +
            "formatting <em>like this</>, as well as<br>" +
            "<a href=X><link>links in the text</></a>, so enter text<br>" +
            "in here to edit it. By the way, links are<br>" +
            "called <a href=anc-02>Anchors</a> so you will need<br>" +
            "to refer to them this way.<br>" +
            "<br>" +

            "Also you can stick in items with (relsize + ascent): " +
            "<item relsize=16x16 vsize=ascent href=emoticon/evil-laugh></item>" +
            " (full) " +
            "<item relsize=16x16 vsize=full href=emoticon/guilty-smile></item>" +
            " (to the left)<br>" +

            "Also (size + ascent): " +
            "<item size=16x16 vsize=ascent href=emoticon/haha></item>" +
            " (full) " +
            "<item size=16x16 vsize=full href=emoticon/happy-panting></item>" +
            " (before this)<br>" +

            "And as well (absize + ascent): " +
            "<item absize=64x64 vsize=ascent href=emoticon/knowing-grin></item>" +
            " (full) " +
            "<item absize=64x64 vsize=full href=emoticon/not-impressed></item>" +
            " or even paths to image files on disk too like: " +
            "<item absize=96x128 vsize=full href=file://data/images/sky_01.jpg></item>" +
            " ... end.";

var my_window = new elm.window({
	type : "main",
	label : "Entry",
	width : 320,
	height : 480,
	weight : EXPAND_BOTH,
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
				the_entry : {
					type : "entry",
					text : entry_text,
					weight : EXPAND_BOTH,
					align : FILL_BOTH,
					line_wrap : 3,
					editable : true,
				},
				the_pass : {
					type : "entry",
					text : "Password",
					weight : EXPAND_BOTH,
					editable : false,
					password : true,
				},
				icon_no_scale : {
					type : "button",
					label : "Icon no scale",
					weight : { x : -1.0, y : -1.0 },
				},
			},
		},
	},
});
