function confirmFirst(url) {
	if (confirm("Are you sure?")) {
		document.location = url;
	}
}
function toggle(nr, label) {
	$$(nr).each(function(el) {
		if (el.getStyle('display')=='none') {
			el.setStyle('display', 'block');
			label.getParent().set('class', 'inner active');
		} else {
			el.setStyle('display', 'none');
			label.getParent().set('class', 'inner');
		}
	});
}