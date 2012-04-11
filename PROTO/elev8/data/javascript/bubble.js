#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var desc = elm.Window({
    title : "Bubble Demo",
    width : 320,
    height : 480,
    align : FILL_BOTH,
    elements : {
        the_background : elm.Background ({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
        }),
        the_box : elm.Box({
            weight : EXPAND_BOTH,
            resize : true,
            elements : {
                the_bubble_1 : elm.Bubble ({
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    text_part : { item : "info", text : "Hello Bubble" },
                    corner : "bottom_right",
                }),
                the_bubble_2 : elm.Bubble ({
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    text_part : { item : "info", text : "Hi Bumble" },
                    corner : "top_left",
                    content : elm.Label ({
                        label : "<b>This is a small label</b>",
                        weight : { x : 0.0, y : 0.0 },
                        align : { x : -1.0, y : -1.0 },
                    }),
                }),
                the_bubble_3 : elm.Bubble ({
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    text_part : { item : "info", text : "Hi Bumble" },
                    corner : "top_right",
                    content : elm.Icon ({
                        image : elm.datadir + "data/images/logo_small.png",
                    }),
                }),
            },
        }),
    },
});

var win = elm.realise(desc);
