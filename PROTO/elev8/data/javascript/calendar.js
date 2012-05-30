#!/usr/local/bin/elev8

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var my_window = elm.Window({
        title: "Calendar Demo",
        width: 320,
        height: 480,
        align: FILL_BOTH,
        elements: {
            background: elm.Background({
                weight: EXPAND_BOTH,
                align: FILL_BOTH,
                resize: true
            }),
            calendar: elm.Calendar({
                weight: EXPAND_BOTH,
                align: FILL_BOTH,
                resize: true,
                fill: true,
                weekday_names: ['Mo', 'Tu', 'We', 'Th', 'Fr', 'Sa', 'Su'],
                min_year: 1900,
                max_year: 2100,
                select_mode: 0,
                year: 2011,
                month: 1,
                date: 21,
                interval: 0.5,
                marks: {
                    0: {
                        type: "holiday",
                        day: 20,
                        month : 7,
                        year: 0,
                        repeat: "unique",
                    },
                    1: {
                        type: "checked",
                        day: 21,
                        month : 8,
                        year: 0,
                        repeat: "daily",
                    },
                    2: {
                        type: "checked",
                        day: 22,
                        month : 1,
                        year: 0,
                        repeat: "weekly",
                    },
                    3: {
                        type: "clicked",
                        day: 23,
                        month : 7,
                        year: 0,
                        repeat: "monthly",
                    },
                    4: {
                        type: "checked",
                        day: 24,
                        month : 8,
                        year: 2011,
                        repeat: "annualy",
                    }
                },
                on_change: function (args) {
                             print("Callback Invoked\n");
                         }
            })
        }
});

elm.realise(my_window);
