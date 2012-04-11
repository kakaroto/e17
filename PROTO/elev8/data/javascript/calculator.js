#!/usr/local/bin/elev8

var elm = require('elm');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var stack = new Array;

/* clear the display when the next button is pressed? */
var clear = 1;
var number_string;

function multiply(left, right) {
    return left * right;
}

function divide(left, right) {
    return left / right;
}

function add(left, right) {
    return left + right;
}

function subtract(left, right) {
    return left - right;
}

function push(val) {
    stack.push(val);
}

function push_entry() {
    push(number_string);
}

function set_entry(val) {
    number_string = val;
    calc.elements.vbox.elements.entry.label = "<align=right>" + number_string;
}

function append_entry(val) {
    var cur;

    if (clear)
        cur = "0";
    else
        cur = number_string;
    clear = 0;

    if (val != '.' || -1 == cur.indexOf('.')) {
        if (cur == "0" && val != '.')
            cur = val;
        else
            cur += val;
    }
    set_entry(cur);
}

function append_number(b) {
    append_entry(b.label);
}

function evaluate() {
    right = stack.pop();
    operand = stack.pop();
    left = stack.pop();

    if (typeof operand != 'function')
        return;
    if (typeof left != 'string' && typeof left != 'number')
        return;
    if (typeof right != 'string' && typeof right != 'number')
        return;

    return operand(parseFloat(left), parseFloat(right));
}

function any_button() {
    this.type = "button";
    this.weight = { x : -1, y : -1 };
}

function number_button(num) {
    return elm.Button({
        label : num,
        on_click : append_number
    });
}

number_button.prototype = new any_button;

function op_button(str, op) {
    return elm.Button ({
        label : str,
        on_click : function () {
             push_entry();
             push(op);
             clear = 1;
        }
     });
}

op_button.prototype = new any_button;

var calc = elm.realise(elm.Window({
    title : "Calculator demo",
    elements : {
        the_background : elm.Background ({
            weight : EXPAND_BOTH,
            resize : true,
        }),
        vbox : elm.Box ({
            resize : true,
            elements : {
                entry : elm.Entry ({
                    label : "<align=right>0",
                    align : { x : -1, y : 0 },
                }),
                hbox1 : elm.Box ({
                    horizontal : true,
                    homogeneous : true,
                    elements : {
                        b7 : number_button("7"),
                        b8 : number_button("8"),
                        b9 : number_button("9"),
                        divide : op_button("/", divide),
                    },
                }),
                hbox2 : elm.Box ({
                    horizontal : true,
                    homogeneous : true,
                    elements : {
                        b4 : number_button("4"),
                        b5 : number_button("5"),
                        b6 : number_button("6"),
                        multiply : op_button("*", multiply),
                    },
                }),
                hbox3 : elm.Box ({
                    horizontal : true,
                    homogeneous : true,
                    elements : {
                        b1 : number_button("1"),
                        b2 : number_button("2"),
                        b3 : number_button("3"),
                        subtract : op_button("-", subtract),
                    },
                }),
                hbox4 : elm.Box ({
                    horizontal : true,
                    homogeneous : true,
                    elements : {
                        b0 : number_button("0"),
                        bdot : number_button("."),
                        equals : elm.Button ({
                            type : "button",
                            label : "=",
                            weight : { x : -1.0, y : -1.0 },
                            on_click : function () {
                                push_entry();
                                set_entry("");
                                var answer = evaluate();
                                set_entry(answer);
                                clear = 1;
                            },
                        }),
                        add : op_button("+", add),
                    },
                }),
            },
        }),
    },
}));

