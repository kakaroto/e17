#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var Item = function(calc)
{
    this.type = "default";
    this.on_select = function () { print("Selected '" + calc +"'\n"); };
    this.on_text = function (arg) { return "'" + calc + "' = '" + eval(calc) +"'"; };
    this.on_content = function (arg) { return undefined; };
};

function Append(label)
{
    if (my_window.elements.box.elements.right.elements.entry.label == "0" && !isNaN(Number(label)))
	my_window.elements.box.elements.right.elements.entry.label = label;
    else
	my_window.elements.box.elements.right.elements.entry.label += label;
}

function Calc()
{
    my_window.elements.box.elements.left.elements.results.append(new Item(my_window.elements.box.elements.right.elements.entry.label));
    my_window.elements.box.elements.right.elements.entry.label = eval(my_window.elements.box.elements.right.elements.entry.label);
}

function Button() {
    this.type = "button";
    this.weight = { x : -1, y : -1 };
}

function ButtonLogic(num) {
    this.label = num;
    this.on_clicked = function(obj) { Append(obj.label); }
}

ButtonLogic.prototype = new Button;

function ButtonEqual() {
    this.label = "=";
    this.on_clicked = function () { Calc(); }
}

ButtonEqual.prototype = new Button;

function ButtonClear() {
    this.label = "Clear";
    this.on_clicked = function () {
	my_window.elements.box.elements.left.elements.results.clear();
	my_window.elements.box.elements.right.elements.entry.label = "0";
    }
}

ButtonClear.prototype = new Button;

function GenerateLine(start, operation) {
    var tmp = new Object();

    for (i = 0; i < 3; i++)
	tmp["b"+i] = new ButtonLogic(i + start);
    tmp["bop"] = new ButtonLogic(operation);

    return tmp;
}

function Row(start, operation) {
    this.type = "box";
    this.horizontal = true;
    this.homogeneous = true;
    this.elements = GenerateLine(start, operation);
}

var my_window = new elm.window({
    type: "main",
    label: "Elev8 demo",
    width: 400,
    height: 200,
    elements : {
	background : {
	    type: "background",
	    weight: EXPAND_BOTH,
            resize: true
	},
	box : {
	    type : "box",
	    weight : EXPAND_BOTH,
	    horizontal : true,
	    resize : true,
	    elements : {
		left : {
		    type : "box",
		    weight : EXPAND_BOTH,
		    align : FILL_BOTH,
		    elements : {
			clear : new ButtonClear(),
			results : {
			    type : "genlist",
			    weight : EXPAND_BOTH,
			    align : FILL_BOTH
			}
		    }
		},
		right : {
		    type : "box",
		    elements : {
			entry : {
			    type : "label",
			    label : "0",
			    align : { x : -1, y : 0 },
			},
			row1 : new Row(7, "/"),
			row2 : new Row(4, "*"),
			row3 : new Row(1, "-"),
			row4 : {
			    type : "box",
			    horizontal : true,
			    homogeneous : true,
			    elements : {
				b0 : new ButtonLogic(0),
				bdot : new ButtonLogic("."),
				equals : new ButtonEqual(),
				badd : new ButtonLogic("+")
			    }
			}
		    }
		}
	    }
	}
    }
});


