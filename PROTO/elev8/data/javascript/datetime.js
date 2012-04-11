var elm = require('elm');

var EXPAND_BOTH = {x: 1.0, y: 1.0};
var FILL_BOTH = {x: -1.0, y: -1.0};

var win = elm.realise(elm.Window({
    label: "Date Time Demo",
    width: 320,
    height: 480,
    align: FILL_BOTH,
    elements: {
        background: elm.Background({
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            resize: true
        }),
        box: elm.Box({
            weight: EXPAND_BOTH,
            resize: true,
            elements: {
                dubai: elm.DateTime({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    value_min: {
                        sec: 12,
                        min: 59,
                        hour: 11,
                        dayofmonth: 3,
                        month: 1,
                        year: 1978,
                        dayofweek: 0,
                        daysinyear: 12
                    },
                    value_max: {
                        sec: 59,
                        min: 59,
                        hour: 23,
                        dayofmonth: 30,
                        month: 12,
                        year: 2015,
                        dayofweek: 6,
                        daysinyear: 366,
                        dst: 1
                    },
                    field_visible: {
                        date: true,
                        year: true,
                        mon: true,
                        min: true,
                        hour: true,
                        ampm: true
                    },
                    field_limit: {
                        date: {
                            min: 12,
                            max: 23
                        },
                        year: {
                            min: 1900,
                            max: 3000
                        },
                        mon: {
                            min: 1,
                            max: 12
                        },
                        minute: {
                            min: 9,
                            max: 59
                        },
                        hour: {
                            min: 0,
                            max: 23
                        }
                    },
                    on_change: function(arg) {
                        print("Date Time changed");
                    }
                })
            }
        })
    }
}));
