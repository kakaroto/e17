var request = new XMLHttpRequest();
var EXPAND_BOTH = { x : 1.0, y : 1.0 };
var FILL_BOTH = { x : -1.0, y : -1.0 };

var logo_icon = {
	type : "icon",
	image : elm.datadir + "data/images/logo_small.png",
};
var url= "http://7.mshcdn.com/wp-content/uploads/2007/09/obama.png";
//var url= "http://www.google.co.kr";

request.onreadystatechange = function(){
    print("request's onreadystatechange function called " + request.responseText + "\n");
    print("request's onreadystatechange function called " + request.status + "\n");
    print("Get response Header : " + request.getResponseHeader("Content-Type") + "\n");
    my_window.elements.the_box.elements.the_photo.image = request.responseText;
}

var my_window = new elm.window({
		type : "main",
		label : "XML HTTP Request Demo",
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
						image : elm.datadir + "data/images/logo_small.png",
						fill : true,
					},
					icon_button : {
						type : "button",
						label : "Icon sized to button",
						weight : { x : -1.0, y : -1.0 },
						on_clicked : function(arg) {
							print("clicked event on Button: 1");
							request.open("GET", url);
							request.send("");
						},
						on_unpressed : function(arg) {
							print("unpressed event on Button: 1");
						},
						icon : logo_icon,
					},
				},
			},
		},
});


