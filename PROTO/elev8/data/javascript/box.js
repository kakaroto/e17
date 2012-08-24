elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var icon_count = 0;
var selected = null;

function icon(key, before) {
  return elm.Button({
    key: key,
    style: 'anchor',
    before: before,
    label: '#' + icon_count,
    on_click: function() {
      for (var i in items.elements)
        items.elements[i].style = 'anchor';
      this.style = '';
      selected = this;
      print('Item ' + selected.label + ' selected');
    }
  });
}

var win = elm.realise(elm.Window({
  title: "Box example",
  width: 480,
  height: 240,
  elements: {
    background: elm.Background({
      weight: EXPAND_BOTH,
      resize: true
    }),
    box: elm.Box({
      elements: {
        control: elm.Box({
          horizontal: true,
          elements: {
            append: elm.Button({
              label: "Append",
              on_click: function() {
                items.elements[icon_count] = icon(icon_count);
                icon_count++;
              }
            }),
            pack_before: elm.Button({
              label: "Pack Before",
              on_click: function() {
                if (selected) {
                  items.elements[icon_count] = icon(icon_count, selected.key);
                  icon_count++;
                }
              }
            }),
            replace: elm.Button({
              label: "Replace",
              on_click: function() {
                if (selected) {
                  items.elements[selected.key] = icon(selected.key);
                  icon_count++;
                }
              }
            })
          }
        }),
        items: elm.Box({
          align: FILL_BOTH,
          weight: EXPAND_BOTH,
          resize: true,
          horizontal: true,
          elements: {}
        })
      }
    })
  }
}));

var items = win.elements.box.elements.items;
