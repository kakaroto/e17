#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var the_entry = elm.Entry ({
    text : "This is an entry widget in this window that",
    weight : EXPAND_BOTH,
    align : FILL_BOTH,
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
                    visible: false,
                }),
                button : elm.Button ({
                    label : "Activate InWin",
                    weight : { x : -1.0, y : -1.0 },
                    on_click : function(arg) {
                        if (!win.elements.the_box.elements.inwin.visible) {
                          win.elements.the_box.elements.inwin.activate();
                          this.label = "Hide InWin";
                        } else {
                          win.elements.the_box.elements.inwin.visible = false;
                          this.label = "Activate InWin";
                        }
                    },
                }),
            },
        }),
    },
});

var win = elm.realise(desc);
