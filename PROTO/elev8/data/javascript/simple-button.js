var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var w = elm.Window({
    title: "Simple Button demo",
    width: 320,
    height: 480,
    elements: {
        background: elm.Background({
            weight: EXPAND_BOTH,
            resize: true
        }),
        box: elm.Box({
            weight: EXPAND_BOTH,
            resize: true,
            elements: {
                label_only: elm.Button({
                    label: "Label Only"
                }),
                icon_only: elm.Button({
                    icon: elm.Icon({
                        image: elm.datadir + "data/images/logo_small.png"
                    })
                })
            }
        })
    }
});

var e = elm.realise(w);
