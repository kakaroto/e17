#!/usr/local/bin/elev8

var elm = require('elm');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var entry_text =
            "Start typing...";

var my_window = elm.realise(elm.Window({
    title : "Notepad",
    width : 320,
    height : 480,
    weight : EXPAND_BOTH,
    align : FILL_BOTH,
    elements : {
        the_background : elm.Background ({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
        }),
        the_box1 : elm.Box ({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            elements : {
                the_entry : elm.Entry ({
                    text : entry_text,
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    line_wrap : 3,
                    editable : true,
                }),
                icon_no_scale : elm.Button ({
                    label : "Clear",
                    weight : { x : -1.0, y : -1.0 },
                    on_click : function(arg) {
                        my_window.elements.the_box1.elements.the_entry.text = "";
                    }
                }),
            },
        }),
    },
}));
