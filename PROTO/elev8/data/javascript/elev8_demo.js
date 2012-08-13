#!/usr/local/bin/elev8

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };
var item_count = 0;

function createItem() {
  var list = win.elements.box.elements.list;
  list.elements[item_count] = {
    data: examples[item_count],
    class: default_class,
    on_select: function(item) {
      item_selected = item;
      require("./" + item.data[0] + ".js");
    }
  };
  item_count++;
}

var default_class = {
   text: function(part) {
      return this.data[1];
   },
};

var examples = new Array(
    ["actionslider","Action Slider Example"],
    ["airshow","A simple game."],
    ["anim","Animator"],
    ["bg","Background Example"],
    ["box_horiz","Box Horizontal Example"],
    ["box","Box Example"],
    ["box_vert","Box Vertical Example"],
    ["bubble","Bubble Example"],
    ["button","Button Example"],
    ["calculator","Calculator Example"],
    ["calendar","Calendar Example"],
    ["check","Check Example"],
    ["clock","Clock Example"],
    ["colorselector","Color Selector Example"],
    ["conform","Conform Example"],
    ["datetime","Datetime Example"],
    ["dayselector","Day Selector Example"],
    ["dilbert","Dilbert Example"],
    ["entry","Entry Example"],
    ["fileselectorbutton","File Selector button Example"],
    ["fileselectorentry","File Selector Entry Example"],
    ["fileselector","File Selector Example"],
    ["flip","Flip Example"],
    ["fs","Fs Example"],
    ["genlist","Genlist Example"],
    ["grid","Grid Example"],
    ["http_test","Simple HTTP Example"],
    ["inwin","Inwin Example"],
    ["label","Label Example"],
    ["naviframe","Naviframe Example"],
    ["notepad","Notepad Example"],
    ["notify","Notify Example"],
    ["packing","Packing Example"],
    ["panes","Panes Example"],
    ["photocam","PhotoCam Example"],
    ["photo","Photo Example"],
    ["popup","Popup Example"],
    ["progressbar","Progress Bar Example"],
    ["radio","Radio Example"],
    ["segment","Segment Example"],
    ["simple-button","Simple  Button Example"],
    ["slider","Slider Example"],
    ["spinner","Spinner Example"],
    ["stock","Stock Example"],
    ["storage","Storage Example"],
    ["table","Table Example"],
    ["thumb","Thumb Example"],
    ["timer","Timer Example"],
    ["toolbar","Toolbar Example"]
    );

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

for (i = 0; i < (examples.length-1); i++)
    createItem();
