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

function button_pop() {
    print("button_pop");
    win.elements.box.elements.naviframe.pop();
}
function button_push() {
    print("button_push");
    var naviframe = win.elements.box.elements.naviframe;
    
    naviframe.push({
        "Orange": orange,
        "Yellow": yellow,
        "Green": green
    }[this.label] || orange, this.label);
}

var win = new elm.window({
    type : "main",
    label : "Naviframe Example",
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
                naviframe : {
                    type : "naviframe",
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
