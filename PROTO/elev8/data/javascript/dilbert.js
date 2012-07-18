#!/usr/local/bin/elev8

var XMLHttpRequest = require('http').XMLHttpRequest;

//ELEV8_IMAGE:logo_icon.png:http://127.0.0.1:8080/logo_small.png

var request = new XMLHttpRequest();
var url= "http://www.dotnetpowered.com/dailydilbertservice/dailydilbert.axd"

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon_unscaled = elm.Icon({
    type : "icon",
    image : elm.datadir + "data/images/logo_small.png",
    resizable_up : false,
    resizable_down : false,
});

request.onreadystatechange = function(){
    my_window.elements.the_box.elements.the_photo.image = request.responseText;
}


var my_window = elm.realise(elm.Window({
        title : "Dilbert Demo",
        width : 320,
        height : 480,
        align : FILL_BOTH,
        elements : {
            the_background : elm.Background({
                weight : EXPAND_BOTH,
                align : FILL_BOTH,
                resize : true,
            }),
            the_box : elm.Box({
                weight : EXPAND_BOTH,
                resize : true,
                elements : {
                    the_photo : elm.Photo({
                        weight : EXPAND_BOTH,
                        align : FILL_BOTH,
                        resize : true,
                        image : elm.datadir + "data/images/logo_small.png",
                        fill : true,
                    }),
                    icon_no_scale : elm.Button({
                        icon : logo_icon_unscaled,
                        label : "Fetch My Dilbert",
                        on_click : function() {
                            request.open("GET", url);
                            request.send("");
                        },
                    }),
                },
            }),
        },
}));
