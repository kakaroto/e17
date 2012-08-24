var fs = require('fs');

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

function dumpTree(tree, indent) {
    if (indent == undefined)
        indent = 0;

    var prefix = "";
    for (var i = 0; i < indent; i++)
        prefix += "  ";

    for (var i = 0; i < tree.length; i++) {
        var n = tree[i];
        if (typeof n != "object")
            win.elements.box.elements.tree.text += prefix + n + "<br>";
        else {
            win.elements.box.elements.tree.text += prefix + n[0] + "<br>";
            dumpTree(n[1], indent + 1);
        }
    }
}

function buildTree(path, patterns, onDone) {
    var waiting = [];
    var tree = [[path, undefined]];

    function internalBuildTree(path) {
        var node = [];
        waiting.push(path);
        fs.listFiles(path, function (files, isDone, isError) {
            for (var i = 0; i < files.length; i++) {
                var f = files[i];
                if (f.isFile)
                    node.push(f.name);
                else
                    node.push([f.name, internalBuildTree(f.path)]);
            }

            if (isDone) {
                var idx = waiting.indexOf(path);
                if (idx >= 0) {
                    waiting.splice(idx, 1);
                    if (waiting.length == 0)
                        onDone(tree);
                }
            }
        }, {
            patterns: patterns
        });
        return node;
    }

    tree[0][1] = internalBuildTree(path);
    return tree;
}

function listFilesCb(files, isDone, isError) {
    win.elements.box.elements.tree.text += "--- GOT FILES! ---<br>";
    win.elements.box.elements.tree.text += "isDone: " + isDone + ", isError: " + isError + ", count: "+ files.length + "<br>";
    for (var i = 0; i < files.length; i++)
        win.elements.box.elements.tree.text += "   " + (files[i].isFile ? "file" : "dir") + ": " + files[i].path + "<br>";
    win.elements.box.elements.tree.text += "--- GOT FILES! ---<br>";
}

var path = ".";
var patterns = ["*.jpg", "*.png"];

var ls = fs.listFiles(path, listFilesCb, {
                      recursive: true,
                      period: 0.5,
                      batch_size: 1024,
                      allow_hidden: false,
                      filters: patterns});

buildTree(path, patterns, function (t) {
    dumpTree(t);
})

var win = elm.realise(elm.Window({
    title : "Fs",
    width : 320,
    height : 480,
    elements : {
        background: elm.Background({
            weight: EXPAND_BOTH,
            align: FILL_BOTH,
            resize: true,
        }),
        box: elm.Box({
            weight: EXPAND_BOTH,
            resize: true,
            elements: {
                tree: elm.Entry ({
                    text : "",
                    weight : EXPAND_BOTH,
                    align : FILL_BOTH,
                    editable : false,
                    scrollable : true,
                }),
            }
        })
    },
}));
