#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
    type : "main",
    label : "File Selector",
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
                fse : {
                    type : "fileselector",
                    label : "Choose File",
                    align : FILL_BOTH,
                    weight : EXPAND_BOTH,
                    path : "/var/log/",
                    expandable : true,
                    folder_only : false,
                    is_save : false,
                    mode : 0,
                    on_clicked : function(arg) {
                        print("File Selected = " + arguments[1]);
                    }
                },
            },
        },
    },
});
