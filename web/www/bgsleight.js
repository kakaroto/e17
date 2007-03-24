var bgsleight = function() {
   function addLoadEvent(func) {
      var oldonload = window.onload;
      if (typeof window.onload != 'function') {
	 window.onload = func;
      }
      else {
	 window.onload = function() {
	    if (oldonload) {
	       oldonload();
	    }	
	    func();
	 }	
      }	
   }	
   
   function fnLoadPngs() {
      var rslt = navigator.appVersion.match(/MSIE (\d+\.\d+)/, '');
      var itsAllGood = (rslt != null && Number(rslt[1]) >= 5.5);
      for (var i = document.all.length - 1, obj = null; (obj = document.all[i]); i--) {
	 if (itsAllGood && obj.currentStyle.backgroundImage.match(/\.png/i) != null) {
	    fnFixPng(obj);
	    obj.attachEvent("onpropertychange", fnPropertyChanged);
	 }	
      }	
   }	
   
   function fnPropertyChanged() {
      if (window.event.propertyName == "style.backgroundImage") {
	 var el = window.event.srcElement;
	 if (!el.currentStyle.backgroundImage.match(/_\.gif/i)) {
	    var bg	= el.currentStyle.backgroundImage;
	    var src = bg.substring(5,bg.length-2);
	    el.filters.item(0).src = src;
	    el.style.backgroundImage = "url(_.gif)";
	 }	
      }	
   }	
   
   function fnFixPng(obj) {
      var bg = obj.currentStyle.backgroundImage;
      var src = bg.substring(5,bg.length-2);
      obj.style.filter = "progid:DXImageTransform.Microsoft.AlphaImageLoader(src='" + src + "', sizingMethod='scale')";
      obj.style.backgroundImage = "url(_.gif)";
   }	
   
   return {
      init: function() {
	 if (navigator.platform == "Win32" && navigator.appName == "Microsoft Internet Explorer" && window.attachEvent) {
	    addLoadEvent(fnLoadPngs);
	 }	
      }	
   }	
} ();

//bgsleight.init();
