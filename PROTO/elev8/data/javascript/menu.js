#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var small_icon = elm.datadir + "data/images/logo_small.png";

item_1 = {
    label : "item 1",
    disabled : true,
    items : {
        item_2 : {
            icon : small_icon,
            label : "item 2",
            on_clicked : function (arg) {
                print("Item 1 selected\n");
            },
        },
        item_3 : {
            icon : small_icon,
            label : "item 3",
        },
        item_4 : {
            icon : small_icon,
            label : "item 4",
            on_clicked : function (arg) {
                print("Item 4 selected\n");
            },
        },
    },
};
item_5 = {
    icon : small_icon,
    label : "item 5",
    separator : true,
};
item_6 = {
    label : "item 6",
    disabled : true,
    items : {
        item_7 : {
            icon : small_icon,
            label : "item 7",
        },
        item_8 : {
            icon : small_icon,
            label : "item 8",
                seperator : true,
        },
        item_9 : {
            icon : small_icon,
            label : "item 9",
            on_clicked : function (arg) {
                print("Item 9 selected\n");
            },
        },
    },
};

var menu_content = {
	type : "menu",
	x : 50, 
	y : 50,
	items : {
        1 : item_1,
        2 : item_5,
        3 : item_6,
	},
};

var my_window = new elm.window({
	type : "main",
	label : "Menu Demo",
	width : 320,
	height : 480,
	align : FILL_BOTH,
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			align : FILL_BOTH,
			resize : true,
		},
		the_box : {
			type : "box",
			weight : EXPAND_BOTH,
			resize : true,
			elements : {
                the_menu : menu_content,
                but_box : {
                    type : "box",
                    weight : EXPAND_BOTH,
                    horizontal : true,
                    resize : true,
                    elements : {
                        icon_button : {
                            type : "button",
                            label : "Icon sized to button",
                            weight : { x : -1.0, y : -1.0 },
                            on_clicked : function() {
                                print("Button Clicked");
                                my_window.elements.the_box.elements.the_menu.x = 30;
                                my_window.elements.the_box.elements.the_menu.y = 30;
                                my_window.elements.the_box.elements.the_menu.visible = true;
                            },
                        },
                        add : {
                            type : "button",
                            label : "Add menu",
                            weight : { x : -1.0, y : -1.0 },
                            on_clicked : function() {
                                print("Adding new Item");
                                my_window.elements.the_box.elements.the_menu.child(item_1);
                            },
                        },
                    }
                },
            },
       },
	},
});
