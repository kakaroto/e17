#!/usr/bin/env elev8

var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var entry_text = "This is an entry widget in this window.";

var entry = elm.Entry({
    text: entry_text,
    weight: EXPAND_BOTH,
    align: FILL_BOTH,
    line_wrap: 3,
    single_line: 1,
    editable: true
});

var win = elm.realise(elm.Window({
    title: "Notify Example",
    width: 320,
    height: 480,
    weight: EXPAND_BOTH,
    align: FILL_BOTH,
    elements: {
        background: elm.Background({
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            resize: true
        }),
        box: elm.Box({
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            resize: true,
            elements: {
                notify: elm.Notify({
                    content: entry,
                    timeout: 5,
                    orient: 0
                }),
                icon_no_scale: elm.Button({
                    label: "Icon no scale",
                    weight: { x: -1.0, y: -1.0 },
                    on_click: function(me) {
                        print("notify text: " + win.elements.box.elements.notify.content.text);
                    }
                })
            }
        })
    }
}));
