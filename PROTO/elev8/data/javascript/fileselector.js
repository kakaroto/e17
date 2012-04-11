#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var desc = elm.Window({
    title : "File Selector",
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
                fse : elm.FileSelector ({
                    label : "Choose File",
                    align : FILL_BOTH,
                    weight : EXPAND_BOTH,
                    path : "/var/log/",
                    expandable : true,
                    folder_only : false,
                    is_save : false,
                    mode : 0,
                    on_done : function(arg) {
                        print("File Selected = " + arguments[1]);
                    }
                }),
            },
        }),
    },
});

var win = elm.realise(desc);
