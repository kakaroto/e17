var elm = require('elm');
var fs = require('fs');
require('http');

environment['ELEV8_MODPATH'] = elm.datadir + 'data/javascript/';

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var default_class = {
   text: function(part) {
      return this.data;
   },
};

function listFilesCb(files, isDone, isError) {
  for (i = 0; i < (files.length-1); i++)
    {
       var appName = files[i].path.substring(path.length,
                                              files[i].path.length -3);
       var list = win.elements.box.elements.list;
       list.elements[i] = {
         data: appName,
         class: default_class,
         on_select: function(item) {
             item_selected = item;
             require(item.data + ".js");
         }
       };
    }
}

var path = environment['ELEV8_MODPATH'];
var patterns = ["*.js"];
var ls = fs.listFiles(path, listFilesCb,
                      {recursive: false,
                       period: 2,
                       batch_size: -1,
                       allow_hidden: false,
                       filters: patterns});

var win = elm.realise(elm.Window({
    title : "Elev8 Demo",
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
            align: FILL_BOTH,
            resize: true,
            elements: {
                list: elm.Genlist({
                    weight: EXPAND_BOTH,
                    align: FILL_BOTH,
                    elements: {},
                }),
            }
        })
    },
}));
