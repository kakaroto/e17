#!/usr/local/bin/elev8

elm.datadir=elm.datadir + "/data/images/";

var small_icon = {
	type : "icon",
	prescale : 20,
	image : elm.datadir + "logo_small.png",
};
var green = {
	type : "icon",
	prescale : 20,
	image : elm.datadir + "green.png",
};
var yellow = {
	type : "icon",
	prescale : 20,
	image : elm.datadir + "yellow.png",
};
var orange = {
	type : "icon",
	image : elm.datadir + "orange.png",
};
var maroon = {
	type : "icon",
	prescale : 20,
	image : elm.datadir + "maroon.png",
};
var violet = {
	type : "icon",
	prescale : 20,
	image : elm.datadir + "violet.png",
};

var logo_icon = {
	type : "icon",
	prescale : 20,
	image : elm.datadir + "/data/images/logo_small.png",
};

var logo_end = {
	type : "icon",
	prescale : 20,
	image : elm.datadir + "bubble.png",
};

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var examples = new Array(
                            [["actionslider"],["Action Slider Example"]],
                            [["airshow"],["A simple game."]], 
                            [["anim"],["Animator"]],
                            [["background"],["Background Example"]],
                            [["box_horiz"],["Box Horizontal Example"]],
                            [["box_vert"],["Box Vertical Example"]],
                            [["bubble"],["Bubble Example"]],
                            [["button"],["Button Example"]],
                            [["calculator"],["Calculator Example"]],
                            [["calendar"],["Calendar Example"]],
                            [["check"],["Check Example"]],
                            [["clock"],["Clock Example"]],
                            [["color"],["Color Example"]],
                            [["dilbert"],["Dilbert Example"]],
                            [["entry"],["Entry Example"]],
                            [["flip"],["Flip Example"]],
                            [["http_test"],["Simple HTTP Example"]],
                            [["label"],["Label Example"]],
                            [["list"],["List Example"]],
                            [["menu"],["Menu Example"]],
                            [["panes"],["Panes Example"]],
                            [["photo"],["Photo Example"]],
                            [["progressbar"],["Progress Bar Example"]],
                            [["radio"],["Radio Example"]],
                            [["segment"],["Segment Example"]],
                            [["slider"],["Slider Example"]],
                            [["spinner"],["Spinner Example"]],
                            [["table"],["Table Example"]],
                            [["twitter"],["Twitter Example"]]
                            );

var list_items = new Array();

for (i=0; i<examples.length; i++)
{
    print("Checking : " + examples[i][0] + " " +  examples[i][1]);
    list_items[i] = {
            label : examples[i][0],
            icon : small_icon,
            end : logo_end,
            tooltip : examples[i][1],
            on_clicked : function (arg) {
                print("List callback" + this.label);
            },
    };
}

var win = new elm.window({
	label : "list",
	width : 320,
	height : 480,
	elements : {
		the_background : {
			type : "background",
			resize : true,
			weight : EXPAND_BOTH,
		},
		//scroll : {
		//	type : "scroller",
		//	bounce : { x : false, y : true },
		//	policy : { x : "off", y : "auto" },
		//	weight : EXPAND_BOTH,
		//	resize : true,
		content : {
				type : "box",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
				elements : {
                    the_list : {
                        type : "list",
                        resize : true,
                        weight : EXPAND_BOTH,
                        align : { x: -1, y: -1 },
                        items : list_items,
                    },
                    the_box : {
                        type : "box",
                        weight : EXPAND_BOTH,
                        resize : true,
                        elements : {
                            the_entry : {
                                type : "entry",
                                text : "http://165.213.220.149:8080/http_menu.js",
                                weight : EXPAND_BOTH,
                                align : FILL_BOTH,
                                line_wrap : 3,
                                editable : true,
                            },
                            click : {
                                type : "button",
                                label : "Download and Run",
                            },
                        },
                    },
                },
            },
        //},
	},
});
