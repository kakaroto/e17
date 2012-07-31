<?php
#############################################################################
# list of "apps" and "latest versions"
$apps = array
  (
   "enlightenment"        => "0.16.999.74598",
   "eina"                 => "1.2.0",
   "eet"                  => "1.6.0",
   "evas"                 => "1.2.0",
   "ecore"                => "1.2.0",
   "embryo"               => "1.2.0",
   "edje"                 => "1.2.0",
   "eeze"                 => "1.2.0",
   "e_dbus"               => "1.2.0",
   "efreet"               => "1.2.0",
   "expedite"             => "1.2.0",
   "evas_generic_loaders" => "1.2.0",
   "evil"                 => "1.0.0",
   "elementary"           => "1.0.0",
   "eio"                  => "1.0.0",
   "ethumb"               => "1.0.0",
   "emotion"              => "1.0.0"
   );
#############################################################################

function get_ip()
{
    if (getenv("REMOTE_ADDR")) $ip = getenv("REMOTE_ADDR");
    else $ip = "UNKNOWN";
    return $ip;
}

#############################################################################
ob_start();
############ limit - 64kb.
$data = file_get_contents('php://input', NULL, NULL, 0, 64 * 1024);
############ parse post data header
$lines = explode("\n", $data);
$linecount = count($lines);

for ($l = 0; $l <= $linecount; $l++)
{
    $items = explode(" ", $lines[$l]);
    $res = "OK";

    ########################################################################
    ############ update check request
    ############ input:
    ############   UPDATE appname version
    ############ e.g.:
    ############   UPDATE enlightenment 0.16.999.65347
    ############ response:
    ############   OK
    ############   ERROR <error string>
    ############   OLD <latest version>
    if ($items[0] == "UPDATE")
    {
	$app     = $items[1];
	$version = $items[2];
	
	$vcl = explode(".", $version);
	$vsv = explode(".", $apps[$app]);
	
	$ncl = count($vcl);
	$nsv = count($vsv);
	$num = $ncl;
	if ($nsv < $num) $num = $nsv;
	for ($i = 0; $i <= $num; $i++)
	{
	    if (intval($vsv[$i]) > intval($vcl[$i]))
	    {
		$res = "OLD " . $apps[$app];
		break;
	    }
	}

	$fh = fopen("/var/www/web/web/www/e17-updates", "a");
	if ($fh)
	{
	    fwrite($fh, date("Y/m/d-H:i:s"));
	    fwrite($fh, " ");
	    fwrite($fh, get_ip());
	    fwrite($fh, " ");
	    fwrite($fh, $app);
	    fwrite($fh, " ");
	    fwrite($fh, $version);
	    fwrite($fh, "\n");
	    fclose($fh);
	}	    
    }
    ########################################################################
    ############ update check request
    ############ input:
    ############   CLIENT uuid
    ############ e.g.:
    ############   CLIENT 422d5ed527567ef489e8b7fe00000007
    if ($items[0] == "CLIENT")
    {
	$id = $items[1];
	$fh = fopen("/var/www/web/web/www/e17-clients", "a");
	if ($fh)
	{
	    fwrite($fh, date("Y/m/d-H:i:s"));
	    fwrite($fh, " ");
	    fwrite($fh, get_ip());
	    fwrite($fh, " ");
	    fwrite($fh, $id);
	    fwrite($fh, "\n");
	    fclose($fh);
	}
    }
}

############ respond!
header("HTTP/1.1 200 OK");
header("Content-Type: text/plain");
header("X-Enlightenment-Service: Pants On");
print $res;
ob_end_flush();
?>
