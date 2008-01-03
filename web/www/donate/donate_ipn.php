<?PHP
require_once "donate_settings.php";

$loginParams = mysql_connect("$dbhost", "$dbusername", "$dbpass");
mysql_select_db("$dbname",$loginParams);
			 
$req = 'cmd=_notify-validate';

foreach ($_POST as $key => $value) {
	$value = urlencode(stripslashes($value));
	$req .= "&$key=$value";
}

// post back to PayPal system to validate
$header .= "POST /cgi-bin/webscr HTTP/1.0\r\n".
			"Content-Type: application/x-www-form-urlencoded\r\n".
			"Content-Length: " . strlen($req) . "\r\n\r\n";

$fp = fsockopen ('www.paypal.com', 80, $errno, $errstr, 30);

// assign posted variables to local variables
$invoice = $_POST['invoice'];
$receiver_email = $_POST['receiver_email'];
$item_name = $_POST['item_name'];
$item_number = $_POST['item_number'];
$quantity = $_POST['quantity'];
$payment_status = $_POST['payment_status'];
$pending_reason = $_POST['pending_reason'];
$payment_date = $_POST['payment_date'];
$payment_gross = $_POST['mc_gross'];
$payment_fee = $_POST['payment_fee'];
$trans_id = $_POST['txn_id'];
$txn_type = $_POST['txn_type'];
$first_name = $_POST['first_name'];
$last_name = $_POST['last_name'];
$to = $_POST['payer_email'];
$payer_status = $_POST['payer_status'];
$payment_type = $_POST['payment_type'];
$notify_version = $_POST['notify_version'];
$verify_sign = $_POST['verify_sign'];
$payer_email = $_POST['payer_email'];

$ip =  $_SERVER['REMOTE_ADDR'];

if ( ! $fp ) {
	// HTTP ERROR.
} else {
	fputs ($fp, $header . $req);
	
	while ( !feof($fp) ) {
		$res = fgets ($fp, 1024);
		
		if (strcmp ($res, "VERIFIED") == 0) {
			if (strcmp ($payment_status, "Completed") == 0) {
				$result = mysql_query("select txn_id from donations where txn_id LIKE '%$trans_id%'");
				
				if  (!mysql_num_rows($result) && $receiver_email=="ningerso@gmail.com" ) {					
					// Email the person who donated, thanking them
					$message = "Dear $first_name $last_name,\nThank you for your donation to the Enlightenment project";
					mail($to, "Donation to Enlightenment", $message, "From: " . $receiver_email . "");
					
					$result = mysql_query("select donation,user_email from donations where user_email='$payer_email'");
					
					if(!mysql_num_rows($result))
					{
						$result = mysql_query(
							"INSERT into donations ".
							"(ip, txn_id, name, donation,user_email,date) VALUES ".
							"(\"$ip\", \"$trans_id\", \"$first_name $last_name\", \"$payment_gross\", '$payer_email', NOW())"
						);							
					}
					else
					{
						$result = mysql_query(
							"UPDATE donations 
							SET txn_id=CONCAT(txn_id,',$trans_id') 
							donation=" . $result['donation']+$payment_gross . 
							"WHERE user_email='$payer_email'"
						);
					}
				}
			/*} else if (strcmp ($payment_status, "Pending") == 0) {
				
			} else if (strcmp ($payment_status, "Failed") == 0) {
				
			} else if (strcmp ($payment_status, "Refunded") == 0) {*/
				
			}
		}
		else if (strcmp ($res, "INVALID") == 0) {
			//echo "Sorry, you are not authorized to access this script! Your IP Address: $REMOTE_ADDR , is being E-Mailed to $CompanyName for investigation. \n\n If we find that you have been repeatidly attempting to access this script we will be forced to notify your ISP of this activity";
			$message .= "FYI - There has been an attempted hack from someone trying to access the PayPal IPN script directly. Their IP address is: $REMOTE_ADDR";
			mail($CompanyEmail, "APC PayPal IPN Hack Attempt", $message, "From: " . $CompanyEmail . "");
		}
	}
	
	fclose ($fp);
}

header('Location: http://www.enlightenment.org/donation');

?>
