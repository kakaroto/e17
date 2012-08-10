#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var item_count = 0;

function logEvent(info) {
  var list = win.elements.box.elements.list;
  list.elements[item_count] = {
    data: info,
    class: default_class,
  };
  item_count++;
}

var default_class = {
   text: function(part) {
      return this.data;
   },
};

var win = elm.realise(elm.Window({
    title : "Timer",
    width : 320,
    height : 530,
    elements : {
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
                }),
            }
        })
    },
}));

es = setInterval(function() {
   logEvent("Every Second");
}, 1000);

e2s = setInterval(function() {
   logEvent("Every 2 Seconds");
}, 2000);

setTimeout(function() {
   logEvent("Stop 'Every Second' timer");
   clearInterval(es);
}, 5000);

setTimeout(function() {
   logEvent("Say Goodbye!");
}, 9000);

setTimeout(function() {
   logEvent("Goodbye!");
   clearInterval(e2s);
}, 11000);
