var XMLHttpRequest = require('http').XMLHttpRequest;

var request = new XMLHttpRequest();
var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon = elm.Icon({
    image : elm.datadir + "data/images/logo_small.png",
});
var url= "http://7.mshcdn.com/wp-content/uploads/2007/09/obama.png";
//var url= "http://www.google.co.kr";

request.onreadystatechange = function(){
    print("request's onreadystatechange function called " + request.responseText + "\n");
    print("request's onreadystatechange function called " + request.status + "\n");
    print("Get response Header : " + request.getResponseHeader("Content-Type") + "\n");
    my_window.elements.the_box.elements.the_photo.image = request.responseText;
};

var my_window = elm.realise(elm.Window({
        title : "XML HTTP Request Demo",
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
                        size : 80,
                        weight : EXPAND_BOTH,
                        align : FILL_BOTH,
                        resize : true,
                        image : elm.datadir + "data/images/logo_small.png",
                        fill : true,
                    }),
                    icon_button : elm.Button({
                        icon : logo_icon,
                        label : "Icon sized to button",
                        weight : { x : -1.0, y : -1.0 },
                        on_click : function(arg) {
                            print("clicked event on Button: 1");
                            request.open("GET", url);
                            request.send("");
                        },
                        on_unpressed : function(arg) {
                            print("unpressed event on Button: 1");
                        },
                    }),
                },
            }),
        },
}));


