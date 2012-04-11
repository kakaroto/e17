#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var entry_text = "This is an entry widget in this window that<br>" ;
var the_entry = elm.Entry ({
    text : entry_text,
    weight : EXPAND_BOTH,
    align : FILL_BOTH,
    line_wrap : 3,
    single_line : 1,
    editable : true,
});

var desc = elm.Window({
    title : "Entry",
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
        the_box : elm.Box ({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            elements : {
                inwin : elm.Inwin ({
                    content : the_entry,
                }),
                icon_no_scale : elm.Button ({
                    label : "Icon no scale",
                    weight : { x : -1.0, y : -1.0 },
                    on_click : function(arg) {
                        print("Activate InWin");
                        win.elements.the_box.elements.inwin.activate();
                    },
                }),
            },
        }),
    },
});

var win = elm.realise(desc);
