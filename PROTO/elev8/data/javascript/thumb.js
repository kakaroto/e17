#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

function generateerr_cb(widget)
{
        print("Generation error");
}

function loaderr_cb(widget)
{
        print("Load error");
}

function generate_cb(widget)
{
        print("Generation started");
}

function generated_cb(widget)
{
        print("Generation finished");
}

function clicked_cb(widget)
{
        print("Clicked");
        widget.file = elm.datadir + "data/images/green.png";
}

win = elm.realise(elm.Window({
        title : "Thumb Demo",
        width : 320,
        height : 480,
        align : FILL_BOTH,
        elements : {
            the_background : elm.Background ({
                weight : EXPAND_BOTH,
                align : FILL_BOTH,
                resize : true,
            }),
            the_box : elm.Box ({
                weight : EXPAND_BOTH,
                resize : true,
                elements : {
                    the_thumb : elm.Thumb ({
                        weight : EXPAND_BOTH,
                        align : FILL_BOTH,
                        editable : false,
                        on_generate_start: generate_cb,
                        on_generate_stop: generated_cb,
                        on_generate_error: generateerr_cb,
                        on_load_error: loaderr_cb,
                        on_click: clicked_cb,
                    }),
                },
            })
        },
}));

var thumb = win.elements.the_box.elements.the_thumb;
thumb.file = elm.datadir + "data/images/sky_01.jpg";
thumb.reload();
