var elm = require('elm');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var text = "This is a larger label with newlines<br>" +
           "to make it bigger, bit it won't expand or wrap<br>" +
           "just be a block of text that can't change its<br>" +
           "formatting as it's fixed based on text<br>"

var w = elm.Window({
    title: "test",
    x: 100,
    y: 200,
    width: 200,
    height: 300,
    elements: {
        background: elm.Background({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
        }),
        scroll : elm.Scroller ({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            bounce : { x : true, y : true },
            content : elm.Label ({
                label : text,
                weight : { x : 1.0, y : 0.0 },
                align : { x : -1.0, y : -1.0 },
             }),
        }),
    },
});

var e = elm.realise(w);
