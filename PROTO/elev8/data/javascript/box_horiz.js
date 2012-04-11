elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var w = elm.Window({
    title: "Horizontal Box example",
    width: 320,
    height: 480,
    elements: {
        background: elm.Background({
            weight: EXPAND_BOTH,
            resize: true
        }),
        box: elm.Box({
            weight: EXPAND_BOTH,
            horizontal: true,
            resize: true,
            elements: {
                logo_top: elm.Icon({
                    image: elm.datadir + "data/images/logo_small.png",
                    resizable_up: false,
                    resizable_down: false,
                    align: { x: 0.5, y: 0.5 }
                }),
                logo_middle: elm.Icon({
                    image: elm.datadir + "data/images/logo_small.png",
                    resizable_up: false,
                    resizable_down: false,
                    align: { x: 0.5, y: 0.0 }
                }),
                logo_bottom: elm.Icon({
                    image: elm.datadir + "data/images/logo_small.png",
                    resizable_up: false,
                    resizable_down: false,
                    align: { x: 0.0, y: 1.0 }
                })
            }
        })
    }
});

var e = elm.realise(w);
