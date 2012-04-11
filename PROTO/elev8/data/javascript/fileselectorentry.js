#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var desc = elm.Window({
    title : "File Selector Button",
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
                fse : elm.FileSelectorEntry ({
                    label : "Choose File",
                    align : FILL_BOTH,
                    win_title : "File Selector Entry Demo...",
                    win_size : { w : 150, h : 50 },
                    path : "/var/log/",
                    expandable : true,
                    folder_only : false,
                    is_save : false,
                    inwin_mode : true,
                    on_file_choose : function(arg) {
                        print("File Selected = " + arguments[1]);
                    }
                }),
            },
        }),
    },
});

var win = elm.realise(desc);
