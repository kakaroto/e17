elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var logo_icon = elm.Icon({
    image: elm.datadir + "data/images/logo_small.png"
});

elm.realise(elm.Window({
    title: "Panes Demo",
    width: 320,
    height: 480,
    align: FILL_BOTH,
    elements: {
        background: elm.Background({
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            resize: true,
        }),
        pane_1: elm.Pane({
            weight: EXPAND_BOTH,
            resize: true,
            fill: true,
            left: elm.Button({
                icon: logo_icon,
                label: "Left",
                weight: { x: -1.0, y: -1.0 },
                on_click: function(arg) {
                    print("clicked event on Button: Left");
                },
                on_unpress: function(arg) {
                    print("unpressed event on Button: Left");
                },
            }),
            right: elm.Pane({
                weight: EXPAND_BOTH,
                align: FILL_BOTH,
                resize: true,
                fill: true,
                horizontal: true,
                left: elm.Button({
                    icon: logo_icon,
                    label: "Up",
                    weight: { x: -1.0, y: -1.0 },
                    on_click: function(arg) {
                        print("clicked event on Button: Up");
                    },
                    on_unpress: function(arg) {
                        print("unpressed event on Button: Up");
                    },
                }),
                right: elm.Button({
                    icon: logo_icon,
                    label: "Down",
                    weight: { x: -1.0, y: -1.0 },
                    on_click: function(arg) {
                        print("clicked event on Button: Down");
                    },
                    on_unpress: function(arg) {
                        print("unpressed event on Button: Down");
                    },
                }),
                on_press: function(arg) {
                    print("clicked event on Pane 2");
                },
                on_unpress: function(arg) {
                    print("unpressed event on Pane 2");
                },
            }),
            on_press: function(arg) {
                print("clicked event on Pane 1");
            },
            on_unpress: function(arg) {
                print("unpressed event on Pane 1");
            }
        })
    }
}));
