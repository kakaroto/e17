elm = require('elm');

elm.datadir += "/data/images/";
var count = 0;

var green = elm.Photo({
    size: 80,
    weight: EXPAND_BOTH,
    align: FILL_BOTH,
    image: elm.datadir + "green.png",
    fill: true
});

var yellow = elm.Photo({
    size: 80,
    weight: EXPAND_BOTH,
    align: FILL_BOTH,
    image: elm.datadir + "yellow.png",
    fill: true
});

var orange = elm.Photo({
    size: 80,
    weight: EXPAND_BOTH,
    align: FILL_BOTH,
    image: elm.datadir + "orange.png",
    fill: true
});

var maroon = elm.Photo({
    prescale: 1,
    image: elm.datadir + "maroon.png"
});

var violet = elm.Photo({
    prescale: 1,
    image: elm.datadir + "violet.png"
});

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

function button_pop() {
    print("button_pop");
    win.elements.box.elements.naviframe.pop();
}
function button_push() {
    print("button_push");
    var naviframe = win.elements.box.elements.naviframe;
    naviframe.elements[count++] = {
        on_delete: function() { print('Deleting ', this.title) },
        content: {
            "Orange": orange,
            "Yellow": yellow,
            "Green": green
        }[this.label] || orange,
        title: this.label
    };
}

win = elm.realise(elm.Window({
    title: "Naviframe Example",
    width: 320,
    height: 480,
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
                naviframe: elm.Naviframe({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    elements: {},
                    resize: true
                }),
                but_box: elm.Box({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    resize: true,
                    horizontal: true,
                    elements: {
                        pop: elm.Button({
                            icon: maroon,
                            label: "Pop",
                            weight: EXPAND_BOTH,
                            on_click: button_pop,
                        })
                    }
                }),
                add_box: elm.Box({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    resize: true,
                    horizontal: true,
                    elements: {
                        push_orange: elm.Button({
                            label: "Orange",
                            weight: EXPAND_BOTH,
                            on_click: button_push,
                        }),
                        push_violet: elm.Button({
                            label: "Yellow",
                            weight: EXPAND_BOTH,
                            on_click: button_push,
                        }),
                        push_green: elm.Button({
                            label: "Green",
                            weight: EXPAND_BOTH,
                            on_click: button_push,
                        }),
                        check: elm.Check({
                            label: "Show title",
                            weight: EXPAND_BOTH,
                            state: true,
                            on_change: function() {
                                win.elements.box.elements.naviframe.title_visible = this.state;
                            }
                        })
                    }
                })
            }
        })
    }
}));
