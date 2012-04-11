#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon = elm.Icon({
    image : elm.datadir + "data/images/logo_small.png",
});

var logo_icon_unscaled = elm.Icon ({
    image : elm.datadir + "data/images/logo_small.png",
    resizable_up : false,
    resizable_down : false,
});

var bars = new Array(4);
var bouncers = new Array(3);
var start_time;

function set_animator(on) {
    start_time = elm.loop_time();
    for (var i = 0; i < bars.length; i++) {
        if (on) {
            bars[i].on_animate = function(what) {
                t = (elm.loop_time() - start_time);
                what.value = t/10.0;
            };
        }
        else {
            bars[i].on_animate = null;
        }
    }
}

function bouncing(on) {
    for (var i = 0; i < bouncers.length; i++) {
        bouncers[i].pulse(on);
    }
}

var desc = elm.Window({
    title : "Progressbar",
    elements : {
        the_background : elm.Background ({
            weight : EXPAND_BOTH,
            resize : true,
        }),
        the_box : elm.Box ({
            weight : EXPAND_BOTH,
            resize : true,
            elements : {
                progress_plain : elm.ProgressBar ({
                    align : { x : -1.0, y : 0.5 },
                    weight : EXPAND_BOTH,
                }),
                infinite_bounce : elm.ProgressBar ({
                    label : "Infinite bounce",
                    align : { x : -1.0, y : 0.5 },
                    weight : EXPAND_BOTH,
                    pulser : true,
                }),
                icon_progress : elm.ProgressBar ({
                    label : "Label",
                    icon : logo_icon,
                    inverted : true,
                    units : "%1.1f units",
                    span : 200,
                    align : { x : -1.0, y : 0.5 },
                    weight : EXPAND_BOTH,
                }),
                hbox : elm.Box ({
                    align : FILL_BOTH,
                    weight : EXPAND_BOTH,
                    horizontal : true,
                    elements : {
                        vertical_pb : elm.ProgressBar ({
                            horizontal : false,
                            label : "percent",
                            align : FILL_BOTH,
                            weight : EXPAND_BOTH,
                            span : 60,
                        }),
                        vert_infinite : elm.ProgressBar ({
                            horizontal : false,
                            label : "Infinite bounce",
                            align : { x : -1.0, y : 0.5 },
                            weight : EXPAND_BOTH,
                            units : "",
                            span : 80,
                            pulser : true,
                        }),
                        vert_icon : elm.ProgressBar ({
                            label : "Label",
                            horizontal : false,
                            icon : logo_icon,
                            inverted : true,
                            format : "%1.2f%%",
                            span : 200,
                            align : { x : -1.0, y : 0.5 },
                            weight : EXPAND_BOTH,
                        }),
                    },
                }),
                wheel : elm.ProgressBar ({
                    style : "wheel",
                    label : "Style: wheel",
                    align : { x : -1.0, y : 0.5 },
                    weight : EXPAND_BOTH,
                }),
                button_box : elm.Box ({
                    weight : EXPAND_BOTH,
                    horizontal : true,
                    elements : {
                        start : elm.Button ({
                            label : "Start",
                            on_click : function() {
                                bouncing(true);
                                set_animator(true);
                            },
                        }),
                        stop : elm.Button ({
                            label : "Stop",
                            on_click : function() {
                                bouncing(false);
                                set_animator(false);
                            },
                        }),
                        exit : elm.Button ({
                            label : "Exit",
                            on_click : function() {
                                elm.exit();
                            },
                        })
                    },
                }),
            },
        }),
    },
});

var win = elm.realise(desc);
var el = win.elements.the_box.elements;

bars[0] = el.progress_plain;
bars[1] = el.icon_progress;
bars[2] = el.hbox.elements.vertical_pb;
bars[3] = el.hbox.elements.vert_icon;

bouncers[0] = el.infinite_bounce;
bouncers[1] = el.hbox.elements.vert_infinite;
bouncers[2] = el.wheel;

