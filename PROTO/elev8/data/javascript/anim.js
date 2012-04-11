var elm = require('elm');

var EXPAND_BOTH = { x: 1.0, y: 1.0 };
var FILL_BOTH = { x: -1.0, y: -1.0 };

var num_bubbles = 3;

function animator(arg, n, is_shadow) {
    t = elm.loop_time();
    fac = 2.0/num_bubbles;
    r = 48;
    zz = (((2 + Math.sin(t*6 + (Math.PI * (n * fac))))/3) * 64 ) * 2;
    xx = (Math.cos(t * 4 + (Math.PI * (n * fac))) * r) * 2;
    yy = (Math.sin(t * 6 + (Math.PI * (n * fac))) * r) * 2;

    x = win.width / 2 + xx - (arg.width / 2);
    y = win.height / 2 + yy - (arg.height / 2);
    w = zz;
    h = zz;

    /* get pointer position */
    lx = arg.pointer.x
    ly = arg.pointer.y;
    if (is_shadow) {
        x -= ((lx - (x + w / 2)) / 4);
        y -= ((ly - (y + h / 2)) / 4);
    }

    /* resize and move the object */
    arg.x = x;
    arg.y = y;
    arg.width = w;
    arg.height = h;
}

function sprite(n, type) {
    return elm.Image({
        width: 64,
        height: 64,
        file: elm.datadir + 'data/images/bubble_' + type + '.png',
        on_animate: function(arg) {
            animator(arg, n, type == 'sh');
        }
    });
}

function bubble(n) {
    return sprite(n, 'bb');
}

function shadow(n) {
    return sprite(n, 'sh');
}

var win = elm.realise(elm.Window({
    label: "Animation demo",
    width: 480,
    height: 800,
    elements: {
        the_background: elm.Background({
            weight: EXPAND_BOTH,
            image: elm.datadir + "data/images/rock_01.jpg",
            resize: true
        }),
        shadow1: shadow(0),
        shadow2: shadow(1),
        shadow3: shadow(2),
        bubble1: bubble(0),
        bubble2: bubble(1),
        bubble3: bubble(2)
    },
    on_keydown: function () {
        print(this.label);
        elm.exit();
    }
}));
