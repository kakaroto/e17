elm = require('elm');

w = elm.realise(elm.Window({
  title: 'Popup test',
  elements: {
    bg: elm.Background({
      resize: true
    }),
    popup: elm.Popup({
      on_button_press: function(label) {
        w.elements.popup.toast("Pressed button: " + label);
        if (label === 'Content') {
          w.elements.popup.message({
            content: elm.Button({
              label: 'Button in content area',
              icon: elm.Icon({ image: 'starred' })
            })
          });
        } else if (label === 'Items') {
          w.elements.popup.message({
            items: [
              { text: 'Available', icon: elm.Icon({ image: 'user-available'}) },
              { text: 'Away', icon: elm.Icon({ image: 'user-away'}) },
              { text: 'Busy', icon: elm.Icon({ image: 'user-busy'}) },
              { text: 'Idle', icon: elm.Icon({ image: 'user-idle'}) },
              { text: 'Invisible', icon: elm.Icon({ image: 'user-invisible'}) },
              { text: 'Offline', icon: elm.Icon({ image: 'user-offline'}) },
              'Only text',
              { text: 'Only text in an object' }
            ]
          });
        }
      },
      on_dismiss: function() {
        w.elements.popup.toast("Popup dismissed");
      },
      on_item_select: function(item) {
        if (typeof(item) === 'string')
          w.elements.popup.toast("Selected item: " + item + " [text only]");
        else
          w.elements.popup.toast("Selected item: " + item.text);
      },
      on_timeout: function() {
        print("Popup closed by timeout! New popup in one second.");
        setTimeout(newPopup, 1000);
      }
    })
  }
}));

function newPopup() {
  w.elements.popup.message({
    icon: 'dialog-information',
    buttons: [ 'Content', 'Items' ],
    text: 'Press either "Content" or "Items" below to pop up another dialog with either some Elm widget, or a list with items and icons. Pressing outside this dialog dismiss it.',
    title: 'Popup test'
  });
}

newPopup();


