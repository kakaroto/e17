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
                label: "Left",
                weight: { x: -1.0, y: -1.0 },
                on_click: function(arg) {
                    print("clicked event on Button: Left");
                },
                on_unpress: function(arg) {
                    print("unpressed event on Button: Left");
                },
                icon: logo_icon,
            }),
            right: elm.Pane({
                weight: EXPAND_BOTH,
                align: FILL_BOTH,
                resize: true,
                fill: true,
                horizontal: true,
                left: elm.Button({
                    label: "Up",
                    weight: { x: -1.0, y: -1.0 },
                    on_click: function(arg) {
                        print("clicked event on Button: Up");
                    },
                    on_unpress: function(arg) {
                        print("unpressed event on Button: Up");
                    },
                    icon: logo_icon,
                }),
                right: elm.Button({
                    label: "Down",
                    weight: { x: -1.0, y: -1.0 },
                    on_click: function(arg) {
                        print("clicked event on Button: Down");
                    },
                    on_unpress: function(arg) {
                        print("unpressed event on Button: Down");
                    },
                    icon: logo_icon,
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
