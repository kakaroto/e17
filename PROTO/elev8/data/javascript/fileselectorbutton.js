#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
    type : "main",
    label : "File Selector Button",
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
                the_entry : {
                    type : "entry",
                    text : "Selected Path",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    line_wrap : 3,
                    single_line : 1,
                    editable : true,
                },
                fsb : {
                    type : "fileselectorbutton",
                    label : "Choose File",
                    win_title : "File Selector Demo...",
                    win_size : { w : 50, h : 50 },
                    path : "/var/log/",
                    expandable : true,
                    folder_only : true,
                    is_save : true,
                    inwin_mode : true,
                    on_clicked : function(arg) {
                        print("File Selected = " + arguments[1]);
                        my_window.elements.the_box.elements.the_entry.text = arguments[1];
                    }
                },
            },
        },
    },
});
