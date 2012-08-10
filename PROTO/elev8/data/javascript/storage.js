elm = require('elm');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

function saveItems(){
    var elements = win.elements.box.elements;
    localStorage.setItem("first", elements.first_name.text);
    localStorage.setItem("last", elements.last_name.text);
    localStorage.setItem("phone", elements.phone_number.text);
}

function loadItems(){
    var elements = win.elements.box.elements;
    elements.first_name.text = localStorage.getItem("first") || '';
    elements.last_name.text = localStorage.getItem("last") || '';
    elements.phone_number.text = localStorage.getItem("phone") || '';
}

function clearItems(){
    var elements = win.elements.box.elements;
    localStorage.clear();
    loadItems();
}

var win = elm.realise(elm.Window({
    title : "Storage",
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
                first_name : elm.Entry ({
                    text : "First Name",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    line_wrap : 3,
                    editable : true,
                }),
                last_name : elm.Entry ({
                    text : "Last Name",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    line_wrap : 3,
                    editable : true,
                }),
                phone_number : elm.Entry ({
                    text : "Phone Number",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    line_wrap : 3,
                    editable : true,
                }),
                but_box: elm.Box({
                    horizontal: true,
                    elements: {
                        save: elm.Button({
                            label: "Save ",
                            weight: EXPAND_BOTH,
                            on_click: function() { saveItems(); }
                        }),
                        load: elm.Button({
                            label: "Load",
                            weight: EXPAND_BOTH,
                            on_click: function() { loadItems(); }
                        }),
                        clear: elm.Button({
                            label: 'Clear',
                            weight: EXPAND_BOTH,
                            on_click: function() { clearItems(); }
                        }),
                    }
                })
            }
        })
    },
}));
