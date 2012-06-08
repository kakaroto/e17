var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var outlier = {
    row: 2, col: 1,
    element: elm.Label({
        label: "I was outside"
    })
};

var win = elm.realise(elm.Window({
        title: "Table Demo",
        width: 320,
        height: 480,
        align: FILL_BOTH,
        elements: {
            bg: elm.Background({
                type: "background",
                weight: EXPAND_BOTH,
                align: FILL_BOTH,
                resize: true
            }),
            the_table: elm.Table({
                weight: EXPAND_BOTH,
                align: FILL_BOTH,
                elements: {
                    etitle: {
                        row: 0, col: 0, rowspan: 1, colspan: 2,
                        element: elm.Label({
                            label: "<b>This is a small table</b>",
                            weight: EXPAND_BOTH,
                            align : FILL_BOTH,
                        })
                    },
                    e1: {
                        row: 1, col: 0,
                        element: elm.Button({
                            label: "I am the one.",
                            on_click: function(me) {
                                print("click: " + me.label);
                                if (win.elements.the_table.elements.e1.element == this) {
                                    var temp = win.elements.the_table.elements.e1.element;
                                    win.elements.the_table.elements.e1 =
                                        {element: elm.Label({label: "I was the one"})};
                                    temp.visible = false;
                                    delete temp;
                                }
                            }
                        })
                    },
                    e2: {
                        row: 1, col: 1,
                        element: elm.Button({
                            label: "Dodge Bullets",
                            on_click: function(me) {
                                print("click: " + me.label);
                            },
                        }),
                    },
                    e3: {
                        row: 2, col: 0,
                        element: elm.Button({
                            label: "Pack outlier",
                            on_click: function(me) {
                                var temp = win.elements.the_table.elements['outlier'];
                                win.elements.the_table.elements['outlier'] = outlier;
                                if (temp) {
                                   temp.element.visible = false;
                                   delete temp;
                                }
                            }
                        }),
                    },
                    e4: {
                        row: 3, col: 0, rowspan: 2, colspan: 2,
                        element: elm.Button({
                            label: "Big button",
                            weight: EXPAND_BOTH,
                            align : FILL_BOTH,
                            on_click: function() {
                                delete win.elements.the_table.elements['outlier'];
                            }
                        })
                    }
                }
            })
        }
}));
