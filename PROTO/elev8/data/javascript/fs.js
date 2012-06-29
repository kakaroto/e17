var fs = require('fs');

function dumpTree(tree, indent) {
    if (indent == undefined)
        indent = 0;

    var prefix = "";
    for (var i = 0; i < indent; i++)
        prefix += "  ";

    for (var i = 0; i < tree.length; i++) {
        var n = tree[i];
        if (typeof n != "object")
            print(prefix + n);
        else {
            print(prefix + n[0]);
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
    print("--- GOT FILES! ---");
    print("isDone: " + isDone + ", isError: " + isError + ", count: "+ files.length);
    for (var i = 0; i < files.length; i++)
        print("   " + (files[i].isFile ? "file" : "dir") + ": " + files[i].path);
    print("--- GOT FILES! ---");
}


var path = ".";
var patterns = ["*.jpg", "*.png"];

var ls = fs.listFiles(path, listFilesCb, patterns, true, 0.5, false)

buildTree(path, patterns, function (t) {
    dumpTree(t);
})
