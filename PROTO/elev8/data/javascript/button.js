var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var logo_icon = elm.Icon({
    image: elm.datadir + "data/images/logo_small.png",
});

var logo_icon_unscaled = elm.Icon({
    image: elm.datadir + "data/images/logo_small.png",
    resizable_up: false,
    resizable_down: false,
});

var w = elm.Window({
    label: "Button demo",
    width: 320,
    height: 480,
    elements: {
        the_background: elm.Background({
            weight: EXPAND_BOTH,
            resize: true
        }),
        scroll: elm.Scroller({
            bounce: { x: false, y: true },
            policy: { x: "off", y: "auto" },
            weight: EXPAND_BOTH,
            resize: true,
            content: elm.Box({
                weight: EXPAND_BOTH,
                elements: {
                    icon_button: elm.Button({
                        icon: logo_icon,
                        label: "Icon sized to button",
                        weight: { x: -1.0, y: -1.0 },
                        on_click: function(arg) {
                            print("clicked event on Button: 1 " + this.label);
                        },
                        on_unpressed: function(arg) {
                            print("unpressed event on Button: 1" + this.label);
                        },
                    }),
                    icon_no_scale: elm.Button({
                        icon: logo_icon_unscaled,
                        label: "Icon no scale",
                        weight: { x: -1.0, y: -1.0 },
                    }),
                    disabled: elm.Button({
                        icon: logo_icon,
                        label: "Disabled Button",
                        enabled: false
                    }),
                    disabled_icon: elm.Button({
                        icon: logo_icon,
                        enabled: false
                    }),
                    label_only: elm.Button({
                        label: "Label Only"
                    }),
                    icon_only: elm.Button({
                        icon: logo_icon_unscaled
                    }),
                    anchor_style: elm.Button({
                        style: "anchor",
                        label: "Anchor style"
                    }),
                    icon_anchor_style: elm.Button({
                        style: "anchor",
                        icon: logo_icon
                    }),
                    icon_anchor_style_disabled: elm.Button({
                        style: "anchor",
                        enabled: false,
                        icon: logo_icon
                    })
                }
            })
        })
    }
});

var e = elm.realise(w);

