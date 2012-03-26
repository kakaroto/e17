#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH   = { x : -1.0, y : -1.0 };

var logo_icon = {
	type : "icon",
	image : elm.datadir + "data/images/logo_small.png",
};

var logo_icon_unscaled = {
	type : "icon",
	image : elm.datadir + "data/images/logo_small.png",
	resizable_up : false,
	resizable_down : false,
};

var obj1 = {
    label : "Test 1",
    icon : logo_icon,
    on_select : function(arg) {
       print("ctx popup obj selected");
    },
};
var ctxpopup =  {
    type : "ctxpopup",
    weight : EXPAND_BOTH,
    align : FILL_BOTH,
    visible : false,
    on_dismissed : function(arg) {
        print("Ctxpopup is dismissed");
        this.clear();
        delete this;
    }
};

var mywin = new elm.window({
	type : "main",
	label : "CtxPopup demo",
    weight : EXPAND_BOTH,
    align : FILL_BOTH,
	width : 320,
	height : 480,
	elements : {
        the_background : {
            type : "background",
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
        },
        box : {
            type : "box",
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            elements : {
                bt : {
                    type : "button",
                    label : "Click to Launch Popup",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    on_clicked : function(arg) {
                        var popup = mywin.add(ctxpopup);
                        popup.hover_parent = this;
                        popup.append(obj1);
                        popup.append(obj1);
                        popup.append(obj1);
                        popup.append(obj1);
                        popup.x  = 160;
                        popup.y  = 240;
                        popup.visible  = true;
                    },
                },
            },
        },
    }, 
});
