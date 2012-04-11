/*
 * This is a reproduction of the elementary_test Slider example
 */

var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };

var logo_icon = elm.Icon({
    image: elm.datadir + "data/images/logo_small.png",
});

var w = elm.Window({
    title: "Slider",
    width: 320,
    height: 480,
    elements: {
        the_background: elm.Background({
            resize: true,
            weight: EXPAND_BOTH
        }),
        the_box: elm.Box({
            weight: EXPAND_BOTH,
            resize: true,
            elements: {
                horz_slider: elm.Slider({
                    label: "Horizontal",
                    units: "%1.1f units",
                    span: 120,
                    align: { x: -1.0, y: 0.5 },
                    weight: EXPAND_BOTH,
                    icon: logo_icon,
                    on_change: function(me) {
                        e.elements.the_box.elements.box2.elements.vert_inverted.value = me.value
                    }
                }),
                disabled_slider: elm.Slider({
                    label: "Disabled",
                    units: "%1.1f units",
                    span: 120,
                    min: 50,
                    max: 150,
                    value: 80,
                    disabled: true,
                    align: { x: -1.0, y: 0.5 },
                    weight: EXPAND_BOTH,
                    icon: logo_icon
                }),
                inverted_slider: elm.Slider({
                    label: "Horizontal Inverted",
                    span: 80,
                    min: 50,
                    max: 150,
                    value: 80,
                    indicator: "%3.0f",
                    align: { x: 0.5, y: 0.5 },
                    weight: { x: 0, y: 0 },
                    inverted: true,
                    end: logo_icon
                }),
                disabled_inverted: elm.Slider({
                    label: "Disabled Inverted",
                    span: 80,
                    min: 50,
                    max: 150,
                    value: 80,
                    disabled: true,
                    indicator: "%3.0f",
                    align: { x: 0.5, y: 0.5 },
                    weight: { x: 0, y: 0 },
                    inverted: true,
                    end: logo_icon
                }),
                scaled_double: elm.Slider({
                    label: "Scaled Double",
                    units: "%3.0f units",
                    span: 40,
                    min: 50,
                    max: 150,
                    value: 80,
                    scale: 2.0,
                    indicator: "%3.0f",
                    inverted: true,
                    align: { x: -1, y: 0.5 },
                    weight: EXPAND_BOTH
                }),
                box2: elm.Box({
                    horizontal: true,
                    weight: EXPAND_BOTH,
                    elements: {
                        vert_inverted: elm.Slider({
                            label: "Vertical inverted",
                            inverted: true,
                            units: "units",
                            span: 60,
                            align: { x: 0.5, y: -1 },
                            weight: { x: 0, y: 1 },
                            indicator: "%1.1f",
                            horizontal: false,
                            value: 0.2,
                            scale: 1.0,
                            icon: logo_icon
                        }),
                        vertical: elm.Slider({
                            label: "Vertical",
                            units: "units",
                            span: 60,
                            align: { x: 0.5, y: -1 },
                            weight: { x: 0, y: 1 },
                            indicator: "%1.1f",
                            horizontal: false,
                            value: 0.2
                        }),
                        vert_disabled: elm.Slider({
                            label: "Disabled vertical",
                            units: "units",
                            inverted: true,
                            horizontal: false,
                            disabled: true,
                            span: 100,
                            value: 0.2,
                            align: { x: 0.5, y: -1 },
                            weight: { x: 0, y: 1 },
                            icon: logo_icon
                        })
                    }
                })
            }
        })
    }
});

var e = elm.realise(w);
