#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var entry_text = "This is an entry widget in this window that<br>" ;
var the_entry = {
    type : "entry",
    text : entry_text,
    weight : EXPAND_BOTH,
    align : FILL_BOTH,
    line_wrap : 3,
    single_line : 1,
    editable : true,
};

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
                inwin : {
                    type : "inwin",
                    content : the_entry,
                },
                icon_no_scale : {
                    type : "button",
                    label : "Icon no scale",
                    weight : { x : -1.0, y : -1.0 },
                    on_clicked : function(arg) {
                        print("Activate InWin");
                        my_window.elements.the_box.elements.inwin.activate();
                    },
                },
            },
        },
    },
});
