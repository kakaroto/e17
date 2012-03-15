#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var EXPAND_X = { x : 1.0, y : 0.0 };
var EXPAND_Y = { x : 0.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var FILL_X = { x : -1.0, y : 0.0 };
var FILL_Y = { x : 0.0, y : -1.0 };

var ps= "http://download.finance.yahoo.com/d/quotes.csv?s=";
var pe = "&f=l1";
var cs = "http://chart.finance.yahoo.com/t?s=";

var list_items = new Array();

var on_price_complete = function(){
    my_window.elements.scroll.content.elements.the_entry.text = "$ " +this.responseText; 
}

var on_chart_complete = function(){
    my_window.elements.scroll.content.elements.the_chart.image = this.responseText ;
}

on_click = function(){
                //get stock price
                var req1 = new XMLHttpRequest();
                req1.onreadystatechange = on_price_complete;
                req1.open("GET", ps + this.label + pe);
                print("URL = " +  ps + this.label + pe);
                req1.send("");

                //get stock chart
                req2 = new XMLHttpRequest();
                req2.onreadystatechange = on_chart_complete;
                req2.open("GET", cs + this.label);
                print("URL = " + cs + this.label);
                req2.send("");
                
}

list_items[0] =  {
            label : "GOOG",
            tooltip : ps + "GOOG" + pe,
            on_clicked : on_click,
};


list_items[1] = {
            label : "CSCO",
            tooltip : ps + "CSCO" + pe,
            on_clicked : on_click,
        }
list_items[2] = {
            label : "AAPL",
            tooltip : ps + "AAPL" + pe,
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
                        the_chart : {
                            type : "photo",
                            size : 180,
                            weight : EXPAND_BOTH,
                            align : FILL_BOTH,
                            resize : true,
                            image : elm.datadir + "data/images/logo_small.png",
                            fill : true,
                        },
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
                                },
                            },
                        },
                    },
                },
            },
        },
});
