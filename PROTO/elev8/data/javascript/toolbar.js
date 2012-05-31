var elm = require('elm');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var win = elm.realise(elm.Window({
    title: "test",
    width: 480,
    height: 800,
    elements: {
        background: elm.Background({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            red : 255,
            green : 0,
            blue : 0,
        }),
        toolbar: elm.Toolbar({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize : true,
            items :
            [
              {label: 'Label'},
              {label: 'Label 2'},
              {icon: 'home'}
            ]
        })
    }
}));

function toolbar_cb(item) {

  print('Click: ', item);

  if (item == "CLOSE")
    elm.exit();
}

win.elements.toolbar.append('home', 'Home', 'HOME', toolbar_cb);
win.elements.toolbar.append('chat', 'Chat', 'CHAT', toolbar_cb);
win.elements.toolbar.append('clock', 'Clock', 'CLOCK', toolbar_cb);
win.elements.toolbar.append('close', 'Close', 'CLOSE', toolbar_cb);
