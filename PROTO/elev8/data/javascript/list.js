#!/usr/local/bin/elev8

//ELEV8_IMAGE:logo_small.png:http://farm7.static.flickr.com/6187/6072313250_a5b8d354b5_s.jpg
//ELEV8_IMAGE:green.png:http://farm7.static.flickr.com/6184/6072313786_8b0eb09826_s.jpg
//ELEV8_IMAGE:violet.png:http://farm7.static.flickr.com/6209/6072313750_50874d9bfd_s.jpg
//ELEV8_IMAGE:maroon.png: http://farm7.static.flickr.com/6084/6071770879_252f85bf70_s.jpg
//ELEV8_IMAGE:orange.png:http://farm7.static.flickr.com/6089/6072313680_85795c6ea2_s.jpg
//ELEV8_IMAGE:yellow.png:http://farm7.static.flickr.com/6188/6072313656_476f7fdf2d_s.jpg

//elm.theme="default";
//
//elm.datadir=elm.datadir + "/data/images/";

var small_icon = {
	type : "icon",
	prescale : 1,
	image : elm.datadir + "logo_small.png",
};
var green = {
	type : "icon",
	prescale : 1,
	image : elm.datadir + "green.png",
};
var yellow = {
	type : "icon",
	prescale : 1,
	image : elm.datadir + "yellow.png",
};
var orange = {
	type : "icon",
	prescale : 1,
	image : elm.datadir + "orange.png",
};
var maroon = {
	type : "icon",
	prescale : 1,
	image : elm.datadir + "maroon.png",
};
var violet = {
	type : "icon",
	prescale : 1,
	image : elm.datadir + "violet.png",
};
/*
 * This is a reproduction of the elementary_test list example
 */

var logo_icon = {
	type : "icon",
	prescale : 20,
	image : elm.datadir + "/data/images/logo_small.png",
};

var logo_end = {
	type : "icon",
	prescale : 20,
	image : elm.datadir + "bubble.png",
};

var EXPAND_BOTH = { x : 1.0, y : 1.0 };

var list_items = new Array();

list_items[0] =  {
			label : "Hello",
			icon : small_icon,
			end : violet,
			tooltip : "This is a toolip",
		};
list_items[1] = {
			label : "world",
			icon : yellow,
			end : maroon,
};
list_items[2] = {
			label : ".",
			icon : orange,
			end : green,
			on_clicked : function (arg) {
			print("List callback = " + win.elements.the_list.items[3].label);
			win.elements.the_list.items[3].icon = logo_icon;
			},
		};
list_items[3] = {
			label : "How",
			icon : maroon,
			end : violet,
			on_clicked : function (arg) {
				print("List callback for 3");
			},
		};
list_items[4] = {
			label : "are",
			icon : violet,
			end : orange,
		};
list_items[5] = {
			label : "you",
			icon : small_icon,
			end : maroon,
		};
list_items[6] =	{
			label : "doing?",
			icon : green,
			end : small_icon,
            tooltip : "I have a green icon",
		};

function button_prepend() {
    print("button_prepend");
    win.elements.scroll.content.elements.the_list.prepend(list_items[6]);
}
function button_append() {
    print("button_append");
    win.elements.scroll.content.elements.the_list.append(list_items[4]);
}
function button_del() {
    print("button_del");
    win.elements.scroll.content.elements.the_list.del(0);
}
function button_set() {
    print("button_set");
    win.elements.scroll.content.elements.the_list.set_label(4, "Changing labels");
    win.elements.scroll.content.elements.the_list.set_tooltip(4, "Howdy Partner");
    win.elements.scroll.content.elements.the_list.set_icon(4, small_icon);
}
function button_get() {
    var label = win.elements.scroll.content.elements.the_list.get_label(4);
    var tooltip = win.elements.scroll.content.elements.the_list.get_tooltip(4);
    print("JS Get Callback : " + label + " " + tooltip);
}

var win = new elm.window({
	label : "list",
	width : 320,
	height : 480,
	elements : {
		the_background : {
			type : "background",
			resize : true,
			weight : EXPAND_BOTH,
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
                    the_list : {
                        type : "list",
                        resize : true,
                        weight : EXPAND_BOTH,
                        align : { x: -1, y: -1 },
                        //items : list_items,
                    },
                    the_box : {
                        type : "box",
                        weight : EXPAND_BOTH,
                        resize : true,
                        elements : {
                            prepend : {
                                type : "button",
                                label : "Prepend",
                                weight : { x : -1.0, y : -1.0 },
                                on_clicked : button_prepend,
                                icon : violet,
                            },
                            append : {
                                type : "button",
                                label : "Append",
                                weight : { x : -1.0, y : -1.0 },
                                on_clicked : button_append,
                                icon : violet,
                            },
                            del : {
                                type : "button",
                                label : "Del",
                                weight : { x : -1.0, y : -1.0 },
                                on_clicked : button_del,
                                icon : violet,
                            },
                            set : {
                                type : "button",
                                label : "Set",
                                weight : { x : -1.0, y : -1.0 },
                                on_clicked : button_set,
                                icon : violet,
                            },
                            get : {
                                type : "button",
                                label : "Get",
                                weight : { x : -1.0, y : -1.0 },
                                on_clicked : button_get,
                                icon : violet,
                            },
                        },
                    },
                },
            },
        },
	},
});
