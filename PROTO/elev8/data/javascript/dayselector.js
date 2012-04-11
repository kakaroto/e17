var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var w = elm.realise(elm.Window({
    label: "Day Selector Demo",
    width: 320,
    height: 480,
    align: FILL_BOTH,
    elements: {
        the_background: elm.Background({
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            resize: true
        }),
        the_box: elm.Box({
            weight: EXPAND_BOTH,
            resize: true,
            elements: {
                dubai: elm.DaySelector({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    week_start: 6,
                    monday: true,
                    tuesday: true,
                    wednesday: true,
                    thursday: true,
                    friday: true,
                    sunday: true,
                    on_change: function(arg) {
                        print("Day changed");
                    }
                }),
                australia: elm.DaySelector({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    week_start: 1,
                    saturday: true,
                    tuesday: true,
                    wednesday: true,
                    thursday: true,
                    friday: true,
                    sunday: true,
                    on_change: function(arg) {
                        print("Day changed");
                    }
                }),
                usa: elm.DaySelector({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    week_start: 0,
                    monday: true,
                    tuesday: true,
                    wednesday: true,
                    thursday: true,
                    friday: true,
                    saturday: true,
                    on_change: function(arg) {
                        print("Day changed");
                    }
                })
            }
        })
    },
}));
