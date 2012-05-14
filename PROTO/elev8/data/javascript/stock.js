#!/usr/local/bin/elev8

var XMLHttpRequest = require('http').XMLHttpRequest;

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var EXPAND_X = { x : 1.0, y : 0.0 };
var EXPAND_Y = { x : 0.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var FILL_X = { x : -1.0, y : 0.0 };
var FILL_Y = { x : 0.0, y : -1.0 };

var ps= "http://download.finance.yahoo.com/d/quotes.csv?s=";
var pe = "&f=l1";
var cs = "http://chart.finance.yahoo.com/t?s=";

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
                req1.open("GET", ps + this + pe);
                print("URL = " +  ps + this + pe);
                req1.send("");

                //get stock chart
                req2 = new XMLHttpRequest();
                req2.onreadystatechange = on_chart_complete;
                req2.open("GET", cs + this);
                print("URL = " + cs + this);
                req2.send("");
                
}

list_items = ["GOOG", "CSCO", "AAPL"];

var my_window = elm.realise(elm.Window({
        title : "Live Stock Quote Demo",
        width : 320,
        height : 480,
        elements : {
            the_background : elm.Background({
                align : FILL_BOTH,
                weight : EXPAND_BOTH,
                resize : true,
            }),
            scroll : elm.Scroller({
                align : FILL_BOTH,
                weight : EXPAND_BOTH,
                bounce : { x : false, y : true },
                policy : { x : "off", y : "auto" },
                resize : true,
                content : elm.Box({
                    align : FILL_BOTH,
                    weight : EXPAND_BOTH,
                    elements : {
                        the_chart : elm.Photo({
                            size : 180,
                            weight : EXPAND_BOTH,
                            align : FILL_BOTH,
                            resize : true,
                            image : elm.datadir + "data/images/logo_small.png",
                            fill : true,
                        }),
                        the_entry : elm.Entry({
                            text : "N/A",
                            line_wrap : 3,
                            single_line : 1,
                            editable : false,
                        }),
                        the_list: elm.Genlist({
                            weight: EXPAND_BOTH,
                            align: FILL_BOTH,
                            classes: {
                                'default': {
                                    text: function(arg) {
                                        return arg.data;
                                    },
                                },
                            }
                        }),
                        the_box : elm.Box({
                            align : FILL_BOTH,
                            weight : EXPAND_BOTH,
                            resize : true,
                            horizontal : true,
                            elements : {
                                refresh : elm.Button({
                                    type : "button",
                                    label : "Refresh",
                                }),
                            },
                        }),
                    },
                }),
            }),
        },
}));

for (var i = 0; i < list_items.length; i++) {
  my_window.elements.scroll.content.elements.the_list.append('default', list_items[i], on_click);
}
