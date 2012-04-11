elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

elm.realise(elm.Window({
        title: "Photocam Demo",
        width: 320,
        height: 480,
        align: FILL_BOTH,
        elements: {
            background: elm.Background({
                weight: EXPAND_BOTH,
                align: FILL_BOTH,
                resize: true,
            }),
            box: elm.Box({
                weight: EXPAND_BOTH,
                align: FILL_BOTH,
                resize: true,
                elements: {
                    photocam: elm.Photocam({
                            weight: EXPAND_BOTH,
                            align: FILL_BOTH,
                            file: elm.datadir + "data/images/sky_01.jpg",
                            zoom_mode: "auto-fill",
                            zoom: 5.0
                    })
                }
            })
        }
}));
