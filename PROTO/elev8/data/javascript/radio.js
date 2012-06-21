#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon = elm.Icon ({
    image : elm.datadir + "data/images/logo_small.png",
});

var logo_icon_unscaled = elm.Icon ({
    image : elm.datadir + "data/images/logo_small.png",
    resizable_up : false,
    resizable_down : false,
});

var desc = elm.Window({
    title : "Radios demo",
    elements : {
        the_background : elm.Background ({
            weight : EXPAND_BOTH,
            resize : true,
        }),
        radio_box : elm.Box ({
            weight : EXPAND_BOTH,
            resize : true,
            elements : {
                sized_radio_icon : elm.Radio ({
                    icon : logo_icon,
                    label : "Icon sized to radio",
                    weight : EXPAND_BOTH,
                    align : { x : 1.0, y : 0.5 },
                    value : 0,
                    group : "rdg",
                }),
                unscaled_radio_icon : elm.Radio ({
                    icon : logo_icon_unscaled,
                    label : "Icon no scale",
                    weight : EXPAND_BOTH,
                    align : { x : 1.0, y : 0.5 },
                    value : 1,
                    group : "rdg",
                }),
                label_only_radio : elm.Radio ({
                    label : "Label Only",
                    value : 2,
                    group : "rdg",
                }),
                disabled_radio : elm.Radio ({
                    label : "Disabled",
                    enabled : false,
                    value : 3,
                    group : "rdg",
                }),
                icon_radio : elm.Radio ({
                    icon : logo_icon_unscaled,
                    value : 4,
                    group : "rdg",
                }),
                disabled_icon_radio : elm.Radio ({
                    enabled : false,
                    icon : logo_icon_unscaled,
                    value : 5,
                    group : "rdg",
                }),
            },
        }),
    },
});

var win = elm.realise(desc);
