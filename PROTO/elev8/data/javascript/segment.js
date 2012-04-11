#!/usr/local/bin/elev8

elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var logo_icon = elm.Icon({
    image: elm.datadir + "data/images/logo_small.png",
});

var logo_icon_unscaled = elm.Icon({
    image: elm.datadir + "data/images/logo_small.png",
    resizable_up: false,
    resizable_down: false,
});

var window = elm.Window({
    label: "Segment Control demo",
    elements: {
        background: elm.Background({
            weight: EXPAND_BOTH,
            resize: true,
        }),
        segment: elm.Segment({
            weight: { x: 1, y: 0 },
            resize: true,
            items: {
                foo: {
                    label: "Only Text",
                },
                1: {
                    icon: logo_icon,
                },
                2: {
                    label: "Text_Icon_test",
                    icon: logo_icon_unscaled,
                },
                3: {
                    label: "Seg4",
                },
                4: {
                    label: "Seg5",
                }
            }
        })
    }
});

elm.realise(window);
