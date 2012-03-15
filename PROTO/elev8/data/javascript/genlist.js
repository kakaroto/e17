#!/usr/local/bin/elev8

/*
 * This is a reproduction of the elementary_test Genlist example
 */

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

elm.datadir=elm.datadir + "/data/images/";

var bubble = {
    type : "icon",
    image : elm.datadir + "bubble.png",
    size_hint_aspect : {
                a : 3,
                w : 1,
                h : 1,
                },  
};

var logo = {
    type : "icon",
    image : elm.datadir + "logo_small.png",
    size_hint_aspect : {
                a : 3,
                w : 1,
                h : 1,
                },  
};

var violet =  {
    type : "icon",
    prescale : 1,
    image : elm.datadir + "violet.png",
};

function button_prepend() {
    print("button_prepend");
}
function button_append() {
    for (var i = 0; i<2000; i++)
            win.elements.box.elements.the_list.append(new ItemType1(i));
}

var ItemType1 = function(data)
{
    this.data = data;
    this.type = "default";
};

ItemType1.prototype = {
        select : function() {
            print("The member is " + this.type);
        },
        text : function(arg) {
            return "Item # " + this.data.toString();
        },
        content : function(arg) {
            if (arguments[0].part == "elm.swallow.icon")
                return logo;
            if (arguments[0].part == "elm.swallow.end")
                return bubble;
        },
        state : function(arg) {
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
                            the_list : {
                                type : "genlist",
                                weight : EXPAND_BOTH,
                                align : FILL_BOTH,
                            },
                but_box : {
                    type : "box",
                    //weight : EXPAND_BOTH,
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

