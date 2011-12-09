#!/usr/local/bin/elev8

elm.datadir=elm.datadir + "/data/images/";

var green = {
    type : "photo",
    size : 80,
	weight : EXPAND_BOTH,
	align : FILL_BOTH,
	image : elm.datadir + "green.png",
	fill : true,
};
var yellow = {
    type : "photo",
    size : 80,
	weight : EXPAND_BOTH,
	align : FILL_BOTH,
	image : elm.datadir + "yellow.png",
	fill : true,
};
var orange = {
    type : "photo",
    size : 80,
	weight : EXPAND_BOTH,
	align : FILL_BOTH,
	image : elm.datadir + "orange.png",
	fill : true,
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

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

function button_promote() {
    print("button_promote");
    win.elements.box.elements.pager.promote();
}
function button_pop() {
    print("button_pop");
    win.elements.box.elements.pager.pop();
}
function button_push() {
    print("button_push");
    if(this.label=="Orange")
        win.elements.box.elements.pager.push(orange);
    if(this.label=="Yellow")
        win.elements.box.elements.pager.push(yellow);
    if(this.label=="Green")
        win.elements.box.elements.pager.push(green);
}

var win = new elm.window({
    type : "main",
	label : "Pager Example",
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
                pager : {
                    type : "pager",
                    style : "fade",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    resize : true,
                },
                but_box : {
                    type : "box",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    resize : true,
                    horizontal : true,
                    elements : {
                        promote : {
                            type : "button",
                            label : "Promote",
                            weight : EXPAND_BOTH,
                            on_clicked : button_promote,
                            icon : violet,
                        },
                        pop : {
                            type : "button",
                            label : "Pop",
                            weight : EXPAND_BOTH,
                            on_clicked : button_pop,
                            icon : maroon,
                        },
                    },
                },
                add_box : {
                    type : "box",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    resize : true,
                    horizontal : true,
                    elements : {
                        push_orange : {
                            type : "button",
                            label : "Orange",
                            weight : EXPAND_BOTH,
                            on_clicked : button_push,
                        },
                        push_violet : {
                            type : "button",
                            label : "Yellow",
                            weight : EXPAND_BOTH,
                            on_clicked : button_push,
                        },
                        push_green : {
                            type : "button",
                            label : "Green",
                            weight : EXPAND_BOTH,
                            on_clicked : button_push,
                        },
                    },
                },
            },
        },
    },
});
