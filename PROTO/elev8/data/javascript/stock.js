#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var EXPAND_X = { x : 1.0, y : 0.0 };
var EXPAND_Y = { x : 0.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var FILL_X = { x : -1.0, y : 0.0 };
var FILL_Y = { x : 0.0, y : -1.0 };

var url_start= "http://download.finance.yahoo.com/d/quotes.csv?s=";
var url_end = "&f=l1";

var list_items = new Array();

var on_complete = function(){
    var label = "$ " + arguments[1];
    my_window.elements.scroll.content.elements.the_entry.text = label;
}

on_click = function(){
                var request = new XMLHttpRequest();
                request.onreadystatechange = on_complete;
                request.open("GET", this.tooltip);
                print("URL = " + this.tooltip);
                request.send("");
}

list_items[0] =  {
			label : "GOOG",
            tooltip : url_start + "GOOG" + url_end,
            on_clicked : on_click,
};


list_items[1] = {
            label : "CSCO",
            tooltip : url_start + "CSCO" + url_end,
            on_clicked : on_click,
        }
list_items[2] = {
			label : "AAPL",
            tooltip : url_start + "AAPL" + url_end,
            on_clicked : on_click,
		}

var my_window = new elm.window({
		type : "main",
		label : "Live Stock Quote Demo",
		width : 320,
		height : 480,
		elements : {
			the_background : {
				type : "background",
                align : FILL_BOTH,
                weight : EXPAND_BOTH,
                resize : true,
			},
            scroll : {
                type : "scroller",
                align : FILL_BOTH,
                weight : EXPAND_BOTH,
                bounce : { x : false, y : true },
                policy : { x : "off", y : "auto" },
                resize : true,
                content : {
                    type : "box",
                    align : FILL_BOTH,
                    weight : EXPAND_BOTH,
                    elements : {
                        the_entry : {
                            type : "entry",
                            text : "N/A",
                            line_wrap : 3,
                            single_line : 1,
                            editable : false,
                        },
                        the_list : {
                            type : "list",
                            align : FILL_BOTH,
                            weight : EXPAND_BOTH,
                            resize : true,
                            items : list_items,
                        },
                        the_box : {
                            type : "box",
                            align : FILL_BOTH,
                            weight : EXPAND_BOTH,
                            resize : true,
                            horizontal : true,
                            elements : {
                                refresh : {
                                    type : "button",
                                    label : "Refresh",
                                    align : FILL_BOTH,
                                    weight : EXPAND_BOTH,
                                },
                            },
                        },
                    },
                },
            },
	    },
});
