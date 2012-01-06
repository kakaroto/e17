#!/usr/local/bin/elev8

/*
 * This is a reproduction of the elementary_test Genlist example
 */

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

elm.datadir=elm.datadir + "/data/images/";

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
var violet = {
	type : "icon",
	prescale : 1,
	image : elm.datadir + "violet.png",
};

function button_prepend() {
    print("button_prepend");
}
function button_append() {
    print("button_append");
	for (var i = 0; i<10; i++)
    	win.elements.box.elements.the_list.append(new ItemType1(i));
}

var ItemType1 = function(data)
{
	this.data = data;
	this.type = "default";
};

ItemType1.prototype = {
		on_selected : function() {
			print("The member is " + this.type);
		},
		on_text : function(arg) {
			print("Calling text get function." + this.data);
			return this.data.toString();
		},
		on_content : function(arg) {
			print("Calling content get function.");
		},
		on_state : function(arg) {
			print("Calling state get function.");
		},
};

var win = new elm.window({
    type : "main",
	label : "GenList Example",
	width : 320,
	height : 480,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
		},
        box : {
            type : "box",
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            elements : {
                /*scroll : {
                    type : "scroller",
                    bounce : { x : false, y : true },
                    policy : { x : "off", y : "on" },
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    resize : true,
                    content : {
                        type : "box",
                        weight : EXPAND_BOTH,
                        align : FILL_BOTH,
                        elements : {*/
                            the_list : {
                                type : "genlist",
                                weight : EXPAND_BOTH,
                                align : FILL_BOTH,
                            },
                        /*},
                    },
                },*/
                but_box : {
                    type : "box",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    resize : true,
                    horizontal : true,
                    elements : {
                        prepend : {
                            type : "button",
                            label : "Prepend",
                            weight : EXPAND_BOTH,
                            icon : violet,
                        },
                        append : {
                            type : "button",
                            label : "Append",
                            weight : EXPAND_BOTH,
                            icon : violet,
							on_clicked : button_append,
                        },
                    },
                },
            },
        },
    },
});

