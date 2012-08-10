elm = require('elm');

/*
 * This is a reproduction of the elementary_test Genlist example
 */

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

elm.datadir += "/data/images/";
var item_selected = null;
var bulk_size = 10000;
var item_count = 0;

var bubble = elm.Icon({
    image: elm.datadir + "bubble.png",
    size_hint_aspect: { a: 3, w: 1, h: 1 }
});

var logo = elm.Icon({
    image: elm.datadir + "logo_small.png",
    size_hint_aspect: { a: 3, w: 1, h : 1 }
});

var violet =  elm.Icon({
    prescale: 1,
    image: elm.datadir + "violet.png"
});

function createItem(key, before) {
  var list = win.elements.box.elements.list;
  item_count++;
  key = key || item_count;
  list.elements[key] = {
    key: key,
    data: item_count,
    before: before,
    class: default_class,
    on_select: function(item) {
      print('Selected item with data: ' + item.data);
      item_selected = item;
    }
  };
}

function append(n_items) {
   for (; n_items; n_items--)
    createItem();
}

var default_class = {
   text: function(part) {
      return 'Item #' + this.data;
   },
   content: function(part) {
      if (part == 'elm.swallow.icon')
         return logo;
      if (part == 'elm.swallow.end')
         return bubble;
   },
   state: function(part) {
      return false;
   }
};

var other_class = {
   style: 'double_label',
   text: function(part) {
      return 'Other #' + this.data;
   },
   state: function(part) {
      return true;
   }
};

var win = elm.realise(elm.Window({
    title: "GenList Example",
    width: 320,
    height: 480,
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
                list: elm.Genlist({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    elements: {},
                    on_longpress: function(item) {
                        item.long_pressed = true;
                    },
                    classes: {
                        'default': {
                            text: function(part) {
                                if (this.long_pressed)
                                  return 'Item #' + this.data + ' (long pressed)'
                                return 'Item #' + this.data;
                            },
                            content: function(part) {
                                if (part == 'elm.swallow.icon')
                                    return logo;
                                if (part == 'elm.swallow.end')
                                    return bubble;
                            },
                            state: function(part) {
                                return false;
                            }
                        },
                        'other': {
                            style: 'double_label',
                            text: function(part) {
                                return 'Other #' + this.data.toString();
                            },
                            state: function(part) {
                                return true;
                            }
                        }
                    }
                }),
                but_box: elm.Box({
                    horizontal: true,
                    elements: {
                        appendBulk: elm.Button({
                            icon: violet,
                            label: "+ " + bulk_size,
                            weight: EXPAND_BOTH,
                            on_click: function() { append(bulk_size); }
                        }),
                        appendOne: elm.Button({
                            icon: violet,
                            label: "+ 1",
                            weight: EXPAND_BOTH,
                            on_click: function() { append(1); }
                        }),
                        replace: elm.Button({
                            icon: violet,
                            label: 'Replace',
                            weight: EXPAND_BOTH,
                            on_click: function() { createItem(item_selected && item_selected.key); }
                        }),
                        appendBefore: elm.Button({
                            icon: violet,
                            label: 'Before',
                            weight: EXPAND_BOTH,
                            on_click: function() { createItem(null, item_selected.key); }
                        }),
                        incData: elm.Button({
                            icon: violet,
                            label: "Inc Data",
                            weight: EXPAND_BOTH,
                            on_click: function() {
                              var list = win.elements.box.elements.list;
                              if (item_selected && list.elements[item_selected.key])
                                 item_selected.data += 1;
                            }
                        }),
                        setclass: elm.Button({
                            icon: violet,
                            label: "Class",
                            weight: EXPAND_BOTH,
                            on_click: function(item) {
                              var list = win.elements.box.elements.list;
                              if (item_selected && list.elements[item_selected.key])
                                 list.elements[item_selected.key].class = other_class;
                            },
                        }),
                        del: elm.Button({
                            icon: violet,
                            label: "Delete",
                            weight: EXPAND_BOTH,
                            on_click: function() {
                              var list = win.elements.box.elements.list;
                              if(item_selected && list.elements[item_selected.key])
                                 delete list.elements[item_selected.key];
                            },
                        }),
                        clear: elm.Button({
                            icon: violet,
                            label: "Clear",
                            weight: EXPAND_BOTH,
                            on_click: function() {
                              var list  = win.elements.box.elements.list.elements;
                              for (var i in list)
                                 if (list.hasOwnProperty(i))
                                    delete list[i];
                            },
                        })
                    }
                })
            }
        })
    }
}));

