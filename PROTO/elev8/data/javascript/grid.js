#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

elm.datadir=elm.datadir + "/data/images/";

var yellow = {
    element : elm.Photo ({
        type : "photo",
        size : 80,
        weight : EXPAND_BOTH,
        align : FILL_BOTH,
        image : elm.datadir + "yellow.png",
        fill : true,
    }),
    x : 30,
    y : 10,
    w : 27,
    h : 21,
};

var my_window = elm.realise(elm.Window({
        title : "Grid Demo",
        width : 320,
        height : 480,
        weight : EXPAND_BOTH,
        align : FILL_BOTH,
        elements : {
            the_background : elm.Background ({
                weight : EXPAND_BOTH,
                align : FILL_BOTH,
                resize : true,
            }),
            the_grid : elm.Grid ({
                size : { x : 100, y : 100 },
                weight : EXPAND_BOTH,
                align : FILL_BOTH,
                resize : true,
                elements : {
                    0 : {
                        element : elm.Button ({
                            label : "I am the one.",
                            on_click : function() {
                                my_window.elements.the_grid.elements.yellow = yellow;
                            },
                        }),
                        x : 50,
                        y : 10,
                        w : 40,
                        h : 10,
                    },
                    1 : {
                        element : elm.Button ({
                            label : "Dodge Bullets",
                            on_click : function() {
                                delete my_window.elements.the_grid.elements.yellow;
                            },
                        }),
                        x : 60,
                        y : 20,
                        w : 30,
                        h : 10,
                    },
                    2 : {
                        element : elm.Button ({
                            label : "Kill'em All",
                            on_click : function () {
                                for (var i in my_window.elements.the_grid.elements)
                                    if (my_window.elements.the_grid.elements.hasOwnProperty(i))
                                        delete my_window.elements.the_grid.elements[i];
                            }
                        }),
                        x : 40,
                        y : 40,
                        w : 20,
                        h : 20,
                    },
                    3 : {
                        element : elm.Button ({
                            type : "button",
                            label : "Astala Vista Baby",
                            on_click : function () {
                                my_window.elements.the_grid.elements[3] =
                                        {element: elm.Label({label: "Bye!"})};
                            }
                        }),
                        x : 10,
                        y : 10,
                        w : 40,
                        h : 20,
                    },
                },
            }),
        },
}));

