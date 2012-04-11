#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };

var desc = elm.Window({
    title : "Flip test",
    width : 320,
    height : 480,
    elements : {
        the_background : elm.Background ({
            image : elm.datadir + "data/images/rock_01.jpg",
            resize : true,
            weight : EXPAND_BOTH,
        }),
        test_flip : elm.Flip ({
            weight : EXPAND_BOTH,
            resize : true,
            front : elm.Button ({
                label : "Heads",
                weight : EXPAND_BOTH,
                on_click : function() {
                    print("click!");
                    win.elements.test_flip.flip();
                },
            }),
            back : elm.Button ({
                label : "Tails",
                weight : EXPAND_BOTH,
                on_click : function() {
                    print("clock!");
                    win.elements.test_flip.flip();
                },
            }),
        }),
    },
});

var win = elm.realise(desc);

