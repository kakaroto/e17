var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

function select_cb(widget, label) {
    print(label);
    if (!label) // undefined
        widget.label = "Go";
    else
        widget.label = label;
}

var w = elm.Window({
    title: "ActionSlider example",
    width: 320,
    height: 480,
    elements: {
        the_background: elm.Background({
            weight: EXPAND_BOTH,
            resize: true,
        }),
        the_box: elm.Box({
            weight: { x: 1.0, y: 0.0 },
            resize: true,
            elements: {
                snooze_right: elm.ActionSlider({
                    weight: { x: 1.0, y: 0.0 },
                    align: { x: -1.0, y: 0.0 },
                    labels: { left: "Snooze", right: "Stop" },
                    magnet: "right",
                    slider: "right",
                }),
                snooze_middle: elm.ActionSlider({
                    weight: { x: 1.0, y: 0.0 },
                    align: { x: -1.0, y: 0.0 },
                    labels: { left: "Snooze", right: "Stop" },
                    magnet: "center",
                    slider: "center",
                }),
                accept_reject_middle: elm.ActionSlider({
                    weight: { x: 1.0, y: 0.0 },
                    align: { x: -1.0, y: 0.0 },
                    labels: { center: "Accept", right: "Reject" },
                    magnet: "center",
                }),
                left_right_left: elm.ActionSlider({
                    weight: { x: 1.0, y: 0.0 },
                    align: { x: -1.0, y: 0.0 },
                    label: "Go",
                    labels: { center: "Accept", right: "Reject" },
                    on_select: select_cb,
                }),
                left_right_center: elm.ActionSlider({
                    weight: { x: 1.0, y: 0.0 },
                    align: { x: -1.0, y: 0.0 },
                    label: "Go",
                    labels: { left: "Left", center: "Center", right: "Right" },
                }),
                enable_disable_center: elm.ActionSlider({
                    weight: { x: 1.0, y: 0.0 },
                    align: { x: -1.0, y: 0.0 },
                    labels: { left: "Left", center: "Center", right: "Right" },
                    magnet: "center",
                    slider: "center",
                }),
            },
        }),
    },
});

var e = elm.realise(w);
