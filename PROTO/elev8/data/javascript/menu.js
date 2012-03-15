#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var small_icon = elm.datadir + "data/images/logo_small.png";

item_1 = {
    label : "item 1",
    disabled: true,
    items : {
        item_1_1 : {
            icon : small_icon,
            label : "item 1_1",
            on_clicked : function (arg) {
                print("Item 1_1 selected\n");
            },
        },
        item_1_2 : {
            icon : small_icon,
            label : "item 1_2",
        },
        item_1_3 : {
            icon : small_icon,
            label : "item 1_3",
            on_clicked : function (arg) {
                print("Item 1_3 selected\n");
            },
        },
    },
};

item_2 = {
    separator : true,
};

item_3 = {
    label : "item 3",
    disabled : true,
    items : {
        item_3_1 : {
            icon : small_icon,
            label : "item 3_1",
        },
        item_3_2 : {
            separator : true,
        },
        item_3_3 : {
            icon : small_icon,
            label : "item 3_3",
            on_clicked : function (arg) {
                print("Item 3_3 selected\n");
            },
        },
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
                the_menu : {
                    type : "menu",
                    items : {
                        1 : item_1,
                        2 : item_2,
                        3 : item_3,
                    },
                },
                but_box : {
                    type : "box",
                    weight : EXPAND_BOTH,
                    horizontal : true,
                    resize : true,
                    elements : {
                        icon_button : {
                            type : "button",
                            label : "Launch Menu",
                            weight : { x : -1.0, y : -1.0 },
                            on_clicked : function() {
                                print("Button Clicked");
                                my_window.elements.the_box.elements.the_menu.x = 30;
                                my_window.elements.the_box.elements.the_menu.y = 30;
                                my_window.elements.the_box.elements.the_menu.visible = true;
                            },
                        },
                    }
                },
            },
       },
    },
});
