#!/usr/local/bin/elev8

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


var my_window = new elm.window({
    label : "Conform example",
    conformant : true,
    align : FILL_BOTH,
    weight : EXPAND_BOTH,
    x : 240,
    y : 240,
    elements : {
        the_background : {
            type : "background",
            weight : EXPAND_BOTH,
            resize : true,
        },
        the_conformant : {
            type : "conform",
            weight : EXPAND_BOTH,
            content : {
                type : "box",
                weight : EXPAND_BOTH,
                align : FILL_BOTH,
                resize : true,
                elements : {
                    the_entry : {
                        type : "entry",
                        text : entry_text,
                        weight : {x:-1.0, y:0.0},
                        align : FILL_BOTH,
                        line_wrap : 3,
                        single_line : 1,
                        editable : true,
                    },
                    icon_no_scale : {
                        type : "button",
                        label : "Icon no scale",
                        weight : { x : -1.0, y : -1.0 },
                    },
                    the_pass : {
                        type : "entry",
                        text : "Password",
                        weight : {x:-1.0, y:0.0},
                        editable : false,
                        password : true,
                    },
                },
            },
        },
    },
});
