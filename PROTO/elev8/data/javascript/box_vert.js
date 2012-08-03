elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var count = 0;

var win = elm.realise(elm.Window({
    title: "Vertical Box example",
    width: 240,
    height: 480,
    elements: {
        background: elm.Background({
            weight: EXPAND_BOTH,
            resize: true
        }),
        box: elm.Box({
            weight: EXPAND_BOTH,
            horizontal: false,
            resize: true,
            elements: {
                button: elm.Button({
                    label: 'Pack',
                    align: FILL_BOTH,
                    on_click: function() {
                      win.elements.box.elements['logo_' + count++] = elm.Icon({
                        image: elm.datadir + "data/images/logo_small.png",
                        resizable_up: false,
                        resizable_down: false,
                      })
                    }
                }),
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
}));
