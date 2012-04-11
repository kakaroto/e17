var elm = require('elm')

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var entry_text =
            "This is an entry widget in this window that<br>" +
            "uses markup <b>like this</> for styling and<br>" +
            "formatting <em>like this</>, as well as<br>" +
            "<a href=X><link>links in the text</></a>, so enter text<br>" +
            "in here to edit it. By the way, links are<br>" +
            "called <a href=anc-02>Anchors</a> so you will need<br>" +
            "to refer to them this way.<br>" +
            "<br>";


var w = elm.realise(elm.Window({
    title : "Conform example",
    conform : true,
    align : FILL_BOTH,
    weight : EXPAND_BOTH,
    width : 240,
    height : 360,
    elements : {
        the_background : elm.Background ({
            weight : EXPAND_BOTH,
            resize : true,
        }),
        the_conform : elm.Conform ({
            weight : EXPAND_BOTH,
            resize : true,
            content : elm.Box ({
                weight : EXPAND_BOTH,
                align : FILL_BOTH,
                resize : true,
                elements : {
                    the_entry : elm.Entry ({
                        text : entry_text,
                        weight : {x:-1.0, y:0.0},
                        align : FILL_BOTH,
                        line_wrap : 3,
                        editable : true,
                    }),
                    icon_no_scale : elm.Button ({
                        label : "Icon no scale",
                        weight : { x : -1.0, y : -1.0 },
                    }),
                    the_pass : elm.Entry ({
                        text : "Password",
                        weight : {x:-1.0, y:0.0},
                        editable : false,
                        password : true,
                    }),
                },
            }),
        }),
    },
}));
