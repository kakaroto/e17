#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

function on_click(arg)
{
    print("Selected '" + arg + "'\n");
}

function Append(label)
{
    if (my_window.elements.box.elements.right.elements.entry.label == "0" && !isNaN(Number(label)))
	my_window.elements.box.elements.right.elements.entry.label = label;
    else
	my_window.elements.box.elements.right.elements.entry.label += label;
}

function Calc()
{
    my_window.elements.box.elements.left.elements.results.append('default', my_window.elements.box.elements.right.elements.entry.label, on_click);
    my_window.elements.box.elements.right.elements.entry.label = eval(my_window.elements.box.elements.right.elements.entry.label);
}

function Button(Label, On_Click) {
    return elm.Button({
	weight : { x : -1, y : -1 },
	label: Label,
	on_click: On_Click
    });
}

function ButtonLogic(num) {
    return Button(num, function(me) { Append(me.label); });  
}

function ButtonEqual() {
    return Button("=", function () { Calc(); });
}

function ButtonClear() {
    return Button("Clear", function () {
	my_window.elements.box.elements.left.elements.results.clear();
	my_window.elements.box.elements.right.elements.entry.label = "0";
    });
}

function GenerateLine(start, operation) {
    var tmp = new Object();

    for (i = 0; i < 3; i++)
	tmp["b"+i] = ButtonLogic(i + start);
    tmp["bop"] = ButtonLogic(operation);

    return tmp;
}

function Row(start, operation) {
    return box = elm.Box({
	horizontal: true,
	homogeneous: true,
	elements: GenerateLine(start, operation)
    });
}

var my_window = new elm.realise(elm.Window({
    label: "Elev8 demo",
    width: 400,
    height: 200,
    elements : {
	background : elm.Background({
	    weight: EXPAND_BOTH,
            resize: true
	}),
	box : elm.Box({
	    weight : EXPAND_BOTH,
	    horizontal : true,
	    resize : true,
	    elements : {
		left : elm.Box({
		    weight : EXPAND_BOTH,
		    align : FILL_BOTH,
		    elements : {
			clear : ButtonClear(),
			results : elm.Genlist({
			    weight : EXPAND_BOTH,
			    align : FILL_BOTH,
			    classes : {
				'default': {
				    text: function(arg) {
					return "'" + arg.data + "' = '" + eval(arg.data) +"'";
				    }
				}
			    }
			})
		    }
		}),
		right : elm.Box({
		    elements : {
			entry : elm.Label({
			    label : "0",
			    align : { x : -1, y : 0 },
			}),
			row1 : Row(7, "/"),
			row2 : Row(4, "*"),
			row3 : Row(1, "-"),
			row4 : elm.Box({
			    horizontal : true,
			    homogeneous : true,
			    elements : {
				b0 : ButtonLogic(0),
				bdot : ButtonLogic("."),
				equals : ButtonEqual(),
				badd : ButtonLogic("+")
			    }
			})
		    }
		})
	    }
	})
    }
}));


