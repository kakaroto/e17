#!/usr/local/bin/elev8

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
	this.label = num;
	this.on_clicked = append_number;
}

number_button.prototype = new any_button;

function op_button(str, op) {
	this.label = str;
	this.on_clicked = function () {
		push_entry();
		push(op);
		clear = 1;
	};
}

op_button.prototype = new any_button;

var calc = new elm.window({
	type : "main",
	label : "Calculator demo",
	elements : {
		the_background : {
			type : "background",
			weight : EXPAND_BOTH,
			resize : true,
		},
		vbox : {
			type : "box",
			resize : true,
			elements : {
				entry : {
					type : "entry",
					label : "<align=right>0",
					align : { x : -1, y : 0 },
				},
				hbox1 : {
					type : "box",
					horizontal : true,
					homogeneous : true,
					elements : {
						b7 : new number_button("7"),
						b8 : new number_button("8"),
						b9 : new number_button("9"),
						divide : new op_button("/", divide),
					},
				},
				hbox2 : {
					type : "box",
					horizontal : true,
					homogeneous : true,
					elements : {
						b4 : new number_button("4"),
						b5 : new number_button("5"),
						b6 : new number_button("6"),
						multiply : new op_button("*", multiply),
					},
				},
				hbox3 : {
					type : "box",
					horizontal : true,
					homogeneous : true,
					elements : {
						b1 : new number_button("1"),
						b2 : new number_button("2"),
						b3 : new number_button("3"),
						subtract : new op_button("-", subtract),
					},
				},
				hbox4 : {
					type : "box",
					horizontal : true,
					homogeneous : true,
					elements : {
						b0 : new number_button("0"),
						bdot : new number_button("."),
						equals : {
							type : "button",
							label : "=",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : function () {
								push_entry();
								set_entry("");
								var answer = evaluate();
								set_entry(answer);
								clear = 1;
							},
						},
						add : new op_button("+", add),
					},
				},
			},
		},
	},
});

