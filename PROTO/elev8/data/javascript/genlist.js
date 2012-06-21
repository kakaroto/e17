elm = require('elm');

/*
 * This is a reproduction of the elementary_test Genlist example
 */

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

elm.datadir += "/data/images/";

var bubble = elm.Icon({
    image: elm.datadir + "bubble.png",
    size_hint_aspect: { a: 3, w: 1, h: 1 }
});

var logo = elm.Icon({
    image: elm.datadir + "logo_small.png",
    size_hint_aspect: { a: 3, w: 1, h : 1 }
});

var violet =  elm.Icon({
    prescale: 1,
    image: elm.datadir + "violet.png"
});

function button_prepend() {
    print("button_prepend");
}
function button_append() {
    for (var i = 0; i < 2000; i++)
        win.elements.box.elements.list.append((i % 5 == 0) ? 'default' : 'other', i,
                function(data) {
                    print('Selected item with data: ' + data.toString());
                });
}

var win = elm.realise(elm.Window({
    title: "GenList Example",
    width: 320,
    height: 480,
    elements: {
        background: elm.Background({
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            resize: true,
        }),
        box: elm.Box({
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            resize: true,
            elements: {
        
                list: elm.Genlist({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    classes: {
                        'default': {
                            text: function(arg) {
                                return 'Item #' + arg.data.toString();
                            },
                            content: function(arg) {
                                if (arg.part == 'elm.swallow.icon')
                                    return logo;
                                if (arg.part == 'elm.swallow.end')
                                    return bubble;
                            },
                            state: function(arg) {
                                return false;
                            }
                        },
                        'other': {
                            style: 'double_label',
                            text: function(arg) {
                                return 'Other #' + arg.data.toString();
                            },
                            state: function(arg) {
                                return true;
                            }
                        }
                    }
                }),
                but_box: elm.Box({
                    horizontal: true,
                    elements: {
                        prepend: elm.Button({
                            icon: violet,
                            label: "Prepend",
                            weight: EXPAND_BOTH,
                        }),
                        append: elm.Button({
                            icon: violet,
                            label: "Append",
                            weight: EXPAND_BOTH,
                            on_click: button_append,
                        })
                    }
                })
            }
        })
    }
}));

