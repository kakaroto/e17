var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var w = elm.Window({
    title: "Colour Selector Demo",
    width: 320,
    height: 480,
    align: FILL_BOTH,
    elements: {
        the_background: elm.Background({
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            resize: true
        }),
        the_box: elm.Box({
            weight: EXPAND_BOTH,
            resize: true,
            elements: {
                the_colorselector: elm.ColorSelector({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    red: 30,
                    green: 130,
                    blue: 200,
                    alpha: 30,
                    on_changed: function(arg) {
                        print("Colour changed");
                    }
                })
            }
        })
    }
});

var e = elm.realise(w);
