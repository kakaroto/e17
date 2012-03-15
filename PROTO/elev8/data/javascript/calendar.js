#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_window = new elm.window({
        type : "main",
        label : "Calendar Demo",
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
            the_calendar : {
                type : "calendar",
                weight : EXPAND_BOTH,
                align : FILL_BOTH,
                resize : true,
                fill : true,
                weekday_names : {
                    0 : "Mo",
                    1 : "Tu",
                    2 : "We",
                    3 : "Th",
                    4 : "Fr",
                    5 : "Sa",
                    6 : "Su",
                },
                min_year : 1900,
                max_year : 2100,
                day_selection_enabled : true,
                selected_year : 2011,
                selected_month : 1,
                selected_date : 21,
                calendar_interval : 0.5,
                marks : {
                    0 : {
                        type : "holiday",
                        date : 20,
                        mon  : 7,
                        year : 2011,
                        repeat : "unique",
                    },
                    1 : {
                        type : "checked",
                        date : 21,
                        mon  : 8,
                        year : 0,
                        repeat : "daily",
                    },
                    2 : {
                        type : "checked",
                        date : 22,
                        mon  : 1,
                        year : 0,
                        repeat : "weekly",
                    },
                    3 : {
                        type : "clicked",
                        date : 23,
                        mon  : 7,
                        year : 0,
                        repeat : "monthly",
                    },
                    4 : {
                        type : "checked",
                        date : 24,
                        mon  : 8,
                        year : 2011,
                        repeat : "annualy",
                    },
                },
                on_changed : function (args) {
                             print("Callback Invoked\n");
                         }
            },
        },
});
