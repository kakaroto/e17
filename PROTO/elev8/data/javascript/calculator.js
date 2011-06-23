#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var stack = new Array;

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
	push(calc.elements.vbox.elements.entry.label);
}

function set_entry(val) {
	calc.elements.vbox.elements.entry.label = val;
}

function append_entry(val) {
	var cur = calc.elements.vbox.elements.entry.label;
	if (cur == "0")
		cur = val;
        else
		cur += val;
	calc.elements.vbox.elements.entry.label = cur;
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

var calc = new elm.main({
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
					type : "label",
					label : "0",
					align : { x : 1, y : 0 },
				},
				hbox1 : {
					type : "box",
					horizontal : true,
					homogeneous : true,
					elements : {
						b7 : {
							type : "button",
							label : "7",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						b8 : {
							type : "button",
							label : "8",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						b9 : {
							type : "button",
							label : "9",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						divide : {
							type : "button",
							label : "/",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : function () {
								push_entry();
								set_entry("");
								push(divide);
							},
						},
					},
				},
				hbox2 : {
					type : "box",
					horizontal : true,
					homogeneous : true,
					elements : {
						b4 : {
							type : "button",
							label : "4",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						b5 : {
							type : "button",
							label : "5",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						b6 : {
							type : "button",
							label : "6",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						multiply : {
							type : "button",
							label : "*",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : function () {
								push_entry();
								set_entry("");
								push(multiply);
							},
						},
					},
				},
				hbox3 : {
					type : "box",
					horizontal : true,
					homogeneous : true,
					elements : {
						b1 : {
							type : "button",
							label : "1",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						b2 : {
							type : "button",
							label : "2",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						b3 : {
							type : "button",
							label : "3",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						minus : {
							type : "button",
							label : "-",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : function () {
								push_entry();
								set_entry("");
								push(subtract);
							},
						},
					},
				},
				hbox4 : {
					type : "box",
					horizontal : true,
					homogeneous : true,
					elements : {
						b0 : {
							type : "button",
							label : "0",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						bdot : {
							type : "button",
							label : ".",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : append_number,
						},
						equals : {
							type : "button",
							label : "=",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : function () {
								push_entry();
								set_entry("");
								var answer = evaluate();
								set_entry(answer);
							}
						},
						plus : {
							type : "button",
							label : "+",
							weight : { x : -1.0, y : -1.0 },
							on_clicked : function () {
								push_entry();
								set_entry("");
								push(add);
							},
						},
					},
				},
			},
		},
	},
});

