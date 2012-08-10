var elm = require('elm');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var URL = "http://www.enlightenment.org/";

var window = elm.realise(elm.Window({
  width: 800,
  height: 600,
  elements: {
    background: elm.Background({
      weight : EXPAND_BOTH,
      align : FILL_BOTH,
      resize : true,
    }),
    vbox: elm.Box({
      weight: EXPAND_BOTH,
      align: FILL_BOTH,
      resize: true,
      horizontal: false,
      elements: {
        address: elm.Entry({
          align: {x: -1, y: 0},
          single_line: true,
        }),
        webpage: elm.Box({
          weight: EXPAND_BOTH,
          align: FILL_BOTH,
          resize: true,
          horizontal: true,
          elements: {
            web: elm.Web({
              weight : EXPAND_BOTH,
              align : FILL_BOTH,
              resize : true,
              history_enabled : false,
              uri : URL
            })
          }
        })
      }
    })
  }
}));

var address = window.elements.vbox.elements.address;

var web = window.elements.vbox.elements.webpage.elements.web;

web.on_load_progress = function(load)
{
  var p = (load == 1) ? "100.00%" : " " + (100*load).toFixed(2) + "%";

  print("Loading " + this.uri + ": " + p);
}

web.on_title_change = function(title)
{
  window.title = title;
}

address.on_activate = function(text)
{
  web.uri = text;
}
