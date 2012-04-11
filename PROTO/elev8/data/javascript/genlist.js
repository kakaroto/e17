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
    for (var i = 0; i<2000; i++)
        win.elements.box.elements.list.append(new ItemType1(i));
}

var ItemType1 = function(data)
{
    this.data = data;
    this.type = "default";
};

ItemType1.prototype = {
        select: function() {
            print("Selected item #", this.data);
        },
        text: function(arg) {
            return "Item # " + this.data.toString();
        },
        content: function(arg) {
            if (arguments[0].part == "elm.swallow.icon")
                return logo;
            if (arguments[0].part == "elm.swallow.end")
                return bubble;
        },
        state: function(arg) {
            print("Calling state get function.");
        }
};

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
                    align: FILL_BOTH
                }),
                but_box: elm.Box({
                    //weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    resize: true,
                    horizontal: true,
                    elements: {
                        prepend: elm.Button({
                            label: "Prepend",
                            weight: EXPAND_BOTH,
                            icon: violet,
                        }),
                        append: elm.Button({
                            label: "Append",
                            weight: EXPAND_BOTH,
                            icon: violet,
                            on_click: button_append,
                        })
                    }
                })
            }
        })
    }
}));

