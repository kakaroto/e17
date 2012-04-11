#!/usr/local/bin/elev8

var desc = elm.Window({
    title : "Label example",
    width : 320,
    height : 200,
    elements : {
        bg : elm.Background ({
            resize : true,
        }),
        box : elm.Box ({
            resize : true,
            elements : {
                small_label : elm.Label ({
                    label : "<b>This is a small label</b>",
                    weight : { x : 0.0, y : 0.0 },
                    align : { x : -1.0, y : -1.0 },
                }),
                larger_label : elm.Label ({
                    label : "This is a larger label with newlines<br>" +
                           "to make it bigger, bit it won't expand or wrap<br>" +
                           "just be a block of text that can't change its<br>" +
                           "formatting as it's fixed based on text<br>",
                    weight : { x : 0.0, y : 0.0 },
                    align : { x : -1.0, y : -1.0 },
                }),
                linewrap_label : elm.Label ({
                    wrap : 1,
                    label : "<b>" +
                           "This is more text designed to line-wrap here as " +
                           "This object is resized horizontally. As it is " +
                           "resized vertically though, nothing should change. " +
                           "The amount of space allocated vertically should " +
                           "change as horizontal size changes." +
                           "</b>",
                    weight : { x : 1.0, y : 0.0 },
                    align : { x : -1.0, y : -1.0 },
                }),
                small_wrap_label : elm.Label ({
                    label : "This small label set to wrap",
                    weight : { x : 0.0, y : 0.0 },
                    align : { x : -1.0, y : -1.0 },
                }),
            },
        }),
    },
});

var win = elm.realise(desc);
