var elm = require('elm');
//var dump = require('dump').dump;

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var element_count = 0;

var element = function(group) {
    element_count++;
    return elm.Check({
        label: 'Element ' + element_count,
    });
}

var box = function(horizontal) {
    return elm.Box({
        horizontal: horizontal,
        weight: EXPAND_BOTH,
        align : FILL_BOTH,
        resize: true,
        elements: {}
    });
}

var button = function(side) {
    return {
        icon: 'arrow_' + side,
        label: 'Insert ' + side,
        on_select: function() {
            view[side].elements[element_count] = element(side);
        }
    };
}

var win = elm.realise(elm.Window({
    title: 'Packing example',
    width: 600,
    height: 480,
    elements: {
        background: elm.Background({
            weight: EXPAND_BOTH,
            align : FILL_BOTH,
            resize: true
        }),
    }
}));

win.elements.box = box();
win.elements.box.elements.toolbar = elm.Toolbar({
    weight: EXPAND_BOTH,
    align : FILL_BOTH,
    resize : true,
    elements: [
        button('left'),
        button('right'),
        {
            label: 'Delete',
            icon: 'delete',
            on_select: function() {
                for (var side in {left: 0, right: 0}) {
                    for (var item in view[side].elements) {
                        if (view[side].elements[item].state) {
                            delete view[side].elements[item];
                        }
                    }
                }
            }
        },
        {
            label: 'Move',
            icon: 'refresh',
            on_select: function() {
                var move = {};
                for (var side in {left: 0, right: 0}) {
                    move[side] = [];
                    for (var item in view[side].elements) {
                        if (view[side].elements[item].state) {
                            move[side].push(view[side].elements[item]);
                            view[side].elements[item] = null;
                        }
                    }
                }
                for (var side in move) {
                    var dest = (side == 'left') ? 'right' : 'left';
                    for (var item in move[side]) {
                        if (typeof(move[side][item]) == 'object') {
                            view[dest].elements[move[side][item].label] = move[side][item];
                        }
                    }
                }
            }
        }
    ]
});

win.elements.box.elements.view = box(true);
view = win.elements.box.elements.view.elements;

view.left = box();
view.right = box();

view.left.elements.label = elm.Label({label: 'Left'});
view.right.elements.label = elm.Label({label: 'Right'});
