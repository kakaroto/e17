var elm = require('elm');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var w = elm.Window({
    title: "test",
    x: 100,
    y: 200,
    width: 800,
    height: 600,
    elements: {
        background: elm.Background({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            red : 255,
            green : 0,
            blue : 0,
        }),
    },
});

var e = elm.realise(w);
