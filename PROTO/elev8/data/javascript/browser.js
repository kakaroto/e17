var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var HOME_URL = "http://www.enlightenment.org/";

var window = elm.realise(elm.Window({
  width: 800,
  height: 600,
  elements: {
    background: elm.Background({
      weight: EXPAND_BOTH,
      align: FILL_BOTH,
      resize: true,
    }),
    vbox: elm.Box({
      weight: EXPAND_BOTH,
      align: FILL_BOTH,
      resize: true,
      horizontal: false,
      elements: {
        toolbar: elm.Toolbar({
          align: FILL_BOTH,
          select_mode: 'none',
          elements: [
            {
              icon: 'go-previous',
              label: 'Back',
              priority: 500,
              on_select: function() {
                window.elements.vbox.elements.webpage.elements.web.back();
              }
            },
            {
              icon: 'go-next',
              label: 'Forward',
              priority: 100,
              on_select: function() {
                window.elements.vbox.elements.webpage.elements.web.forward();
              }
            },
            {
              icon: 'process-stop',
              label: 'Stop',
              states: {
                'page-finished-loading': {
                  icon: 'reload',
                  label: 'Reload',
                  on_select: function() {
                    window.elements.vbox.elements.webpage.elements.web.reload();
                  }
                },
                'page-is-loading': {
                  icon: 'process-stop',
                  label: 'Stop',
                  on_select: function() {
                    window.elements.vbox.elements.webpage.elements.web.stop();
                  }
                }
              },
              priority: 200
            },
            { separator: true },
            {
              element: elm.Entry({
                align: FILL_BOTH,
                weight: EXPAND_BOTH,
                single_line: true,
                scrollable: true,
                hint_min: {width: 250, height: 30},
                on_activate: function(text) {
                  window.elements.vbox.elements.webpage.elements.web.uri = text;
                }
              }),
              priority: 400
            },
            { separator: true },
            {
              icon: 'go-home',
              label: 'Home',
              priority: 50,
              on_select: function() {
                window.elements.vbox.elements.webpage.elements.web.uri = HOME_URL;
              }
            }
          ]
        }),
        webpage: elm.Box({
          weight: EXPAND_BOTH,
          align: FILL_BOTH,
          resize: true,
          horizontal: true,
          elements: {
            web: elm.Web({
              weight: EXPAND_BOTH,
              align: FILL_BOTH,
              resize: true,
              history_enabled: false,
              uri: HOME_URL,
              on_title_change: function(title) {
                window.title = title;
              },
              on_load_progress: function(load) {
                if (load == 1) {
                  window.elements.vbox.elements.toolbar.item_state_set(window.elements.vbox.elements.toolbar.elements[2], 'page-finished-loading');
                  window.elements.progress.visible = false;
                } else {
                  window.elements.vbox.elements.toolbar.item_state_set(window.elements.vbox.elements.toolbar.elements[2], 'page-is-loading');
                  window.elements.progress.visible = true;
                  window.elements.progress.content.value = load;
                }
              },
              on_uri_change: function(uri) {
                window.elements.vbox.elements.toolbar.elements[4].element.text = uri;
              }
            })
          }
        })
      }
    }),
    progress: elm.Notify({
        content: elm.ProgressBar({ value: 0.0 }),
        visible: false,
        orient: 'bottom-right',
        hint_min: { width: 200, height: 20 },
        align: FILL_BOTH,
        weight: EXPAND_BOTH
    })
  }
}));
