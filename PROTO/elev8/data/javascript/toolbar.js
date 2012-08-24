var elm = require('elm');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var win = elm.realise(elm.Window({
    title: "test",
    elements: {
        background: elm.Background({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true
        }),
        box: elm.Box({
            resize: true,
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            elements: {
                toolbar: elm.Toolbar({
                    homogeneous: false,
                    shrink_mode: 'expand',
                    standard_priority: 0,
                    align: FILL_BOTH,
                    elements: [
                        {
                            label: 'Label',
                            on_select: function() {
                                print("Label only clicked")
                            },
                            priority: 100
                        },
                        {
                            element: elm.Slider({
                                hint_min: {width: 100, height: 50},
                                align: FILL_BOTH,
                                weight: EXPAND_BOTH,
                                on_change: function(me) {
                                    print(me.value);
                                }
                            }),
                            priority: 100
                        },
                        {
                            separator: true,
                            priority: 100
                        },
                        {
                            icon: 'apps',
                            on_select: function() {
                                print("Icon only clicked")
                            },
                            priority: 100
                        },
                        {
                            icon: 'preferences-desktop-display',
                            label: 'Display',
                            priority: -500
                        },
                        {
                            icon: 'preferences-desktop-font',
                            label: 'Font',
                            priority: -500
                        },
                        {
                            icon: 'preferences-desktop-theme',
                            label: 'Theme',
                            priority: -500
                        },
                        {
                            icon: 'preferences-desktop-screensaver',
                            label: 'Screensaver',
                            priority: -500
                        }
                    ]
                }),
                l: elm.Label({
                    label: 'Hello world',
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH
                })
            }
        }),
    }
}));

function toolbar_cb(item) {

    print('Click', item, ' ', item.label);

    if (item.label == "Close")
        elm.exit();
}

win.elements.box.elements.toolbar.elements.home = {icon: 'home', label: 'Home', data: 'HOME', on_select: toolbar_cb, priority: 100};
win.elements.box.elements.toolbar.elements.sep = {separator: true, priority: 100};
win.elements.box.elements.toolbar.elements.chat = {icon: 'chat', label: 'Chat', data: 'CHAT', on_select: toolbar_cb, priority: 100};
win.elements.box.elements.toolbar.elements.multi = {
    icon: 'accessories-calculator',
    label: 'Calculator',
    priority: -200,
    on_select: function() {
        print('Changing state to text');
        win.elements.box.elements.toolbar.elements.multi.state = 'text';
    },
    states: {
        'text': {
            icon: 'accessories-text-editor',
            label: 'Text Editor',
            on_select: function() {
                print('Changing state to filemanager');
                win.elements.box.elements.toolbar.elements.multi.state = 'filemanager';
                print('Current state: ', win.elements.box.elements.toolbar.elements.multi.state);
            }
        },
        'filemanager': {
            icon: 'file-manager',
            label: 'File Manager',
            on_select: function() {
                print('Changing state to default');
                win.elements.box.elements.toolbar.elements.multi.state = null;
            }
        }
    }
}

win.elements.box.elements.toolbar.elements.clock = {
    label : 'Clock',
    icon : 'clock',
    data : 'CLOCK',
    on_select : function() {
       win.elements.box.elements.toolbar.elements.clock =  {
          label : 'Blah',
          icon : 'apps',
          data : 'BLAH',
       }
    },
    priority: 100
};

win.elements.box.elements.toolbar.elements.close = {
    label : 'Close',
    icon : 'close',
    data : 'CLOSE',
    on_select : toolbar_cb,
    priority: 100
};

