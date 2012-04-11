#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var desc = elm.Window({
        label : "Photo Demo",
        width : 320,
        height : 480,
        align : FILL_BOTH,
        elements : {
            the_background : elm.Background ({
                weight : EXPAND_BOTH,
                align : FILL_BOTH,
                resize : true,
            }),
            the_box : elm.Box ({
                weight : EXPAND_BOTH,
                resize : true,
                elements : {
                    the_photo : elm.Photo ({
                        size : 80,
                        weight : EXPAND_BOTH,
                        align : FILL_BOTH,
                        resize : true,
                        image : elm.datadir + "data/images/logo_small.png",
                        fill : true,
                    }),
                },
            }),
        },
});

var win = elm.realise(desc);
