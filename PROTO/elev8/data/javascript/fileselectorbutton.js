#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var desc = elm.Window({
    tile : "File Selector Button",
    width : 320,
    height : 480,
    weight : EXPAND_BOTH,
    align : FILL_BOTH,
    elements : {
        the_background : elm.Background ({
            type : "background",
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
        }),
        the_box : elm.Box ({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            elements : {
                the_entry : elm.Entry ({
                    text : "Selected Path",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    line_wrap : 3,
                    single_line : 1,
                    editable : true,
                }),
                fsb : elm.FileSelectorButton ({
                    label : "Choose Folder",
                    win_title : "File Selector Demo...",
                    win_size : { w : 50, h : 50 },
                    path : "/var/log/",
                    expandable : true,
                    folder_only : true,
                    is_save : true,
                    inwin_mode : true,
                    on_file_choose : function(arg) {
                        print("File Selected = " + arguments[1]);
                        my_window.elements.the_box.elements.the_entry.text = arguments[1];
                    }
                }),
            },
        }),
    },
});

var my_window = elm.realise(desc);
