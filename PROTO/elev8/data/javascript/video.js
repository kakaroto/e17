var elm = require("elm");

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var my_win = elm.realise(elm.Window({
        title : "Image",
        width : 600,
        height : 600,
        weight : EXPAND_BOTH,
        align : FILL_BOTH,
        elements : {
          background: elm.Background({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            resize: true,
          }),
          box: elm.Box({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            horizontal: false,
            elements: {
               buttons: elm.Box({
                 weight : EXPAND_BOTH,
                 align : FILL_BOTH,
                 horizontal: true,
                 elements: {
                   fsbFile: elm.FileSelectorButton({
                     label: 'Choose Video',
                     win_title: 'Choose a Video',
                     width: 60,
                     height: 30,
                     on_file_choose: function(_, path){
                       my_win.elements.video.file = path;
                     }
                   }),
                   buttonPlus: elm.Button({
                     label: '+',
                     width: 20,
                     height: 30,
                     on_click: function(){
                       my_win.elements.video.audio_level += 0.1;
                     }
                   }),
                   buttonMinus: elm.Button({
                     label: '-',
                     width: 20,
                     height: 30,
                     on_click: function(){
                       my_win.elements.video.audio_level -= 0.1;
                     }
                   }),
                   buttonMute: elm.Button({
                     label: 'mute',
                     width: 60,
                     height: 30,
                     on_click: function(){
                       my_win.elements.video.audio_mute =
                        !my_win.elements.video.audio_mute;
                     }
                   }),
                   controllers: elm.VideoControllers({}),
                 }
               }),
            }
          }),
          video: elm.Video({
            weight : EXPAND_BOTH,
            align : FILL_BOTH,
            y: 120,
            width: 600,
            height: 480,
          }),
       }
}));

my_win.elements.box.elements.buttons.elements.controllers.video = my_win.elements.video;
