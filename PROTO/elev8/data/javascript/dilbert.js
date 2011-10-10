#!/usr/local/bin/elev8

//ELEV8_IMAGE:logo_icon.png:http://127.0.0.1:8080/logo_small.png

var request = new XMLHttpRequest();
var url= "http://www.dotnetpowered.com/dailydilbertservice/dailydilbert.axd"

var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon_unscaled = {
	type : "icon",
	image : elm.datadir + "logo_small.png",
	scale_up : false,
	scale_down : false,
};

request.onreadystatechange = function(){
    my_window.elements.the_box.elements.the_photo.size = 200;
    my_window.elements.the_box.elements.the_photo.image = request.responseText;
}


var my_window = new elm.window({
		type : "main",
		label : "Dilbert Demo",
		width : 320,
		height : 480,
		align : FILL_BOTH,
		elements : {
			the_background : {
				type : "background",
				weight : EXPAND_BOTH,
				align : FILL_BOTH,
				resize : true,
			},
			the_box : {
				type : "box",
				weight : EXPAND_BOTH,
				resize : true,
				elements : {
					the_photo : {
						type : "photo",
						size : 80,
						weight : EXPAND_BOTH,
						align : FILL_BOTH,
						resize : true,
						image : elm.datadir + "logo_small.png",
						fill : true,
					},
					icon_no_scale : {
						type : "button",
						label : "Fetch My Dilbert"
						weight : { x : -1.0, y : -1.0 },
						icon : logo_icon_unscaled,
						on_clicked : function() {
							request.open("GET", url);
							request.send("");
						},
					},
				},
			},
		},
});
