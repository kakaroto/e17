<?php
#############################################################################
# list of "apps" and "latest versions"
$apps = array
  (
   "enlightenment"        => "0.16.999.65643",
   "eina"                 => "1.1.0",
   "eet"                  => "1.5.0",
   "evas"                 => "1.1.0",
   "ecore"                => "1.1.0"
   "embryo"               => "1.1.0",
   "edje"                 => "1.1.0",
   "eeze"                 => "1.1.0",
   "e_dbus"               => "1.1.0",
   "efreet"               => "1.1.0",
   "expedite"             => "1.1.0",
   "evas_generic_loaders" => "1.1.0",
   "evil"                 => "1.0.0",
   "elementary"           => "0.8.0.65643",
   "eio"                  => "0.1.0.65643",
   "ethumb"               => "0.1.1.65643",
   "emotion"              => "0.2.0.65643"
   );
#############################################################################






#############################################################################
ob_start();
############ limit - 64kb.
$data = file_get_contents('php://input', NULL, NULL, 0, 64 * 1024);
############ parse post data header
$items = explode(" ", $data);
$res = "OK";

#############################################################################
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
}

############ respond!
header("HTTP/1.1 200 OK");
header("Content-Type: text/plain");
header("X-Enlightenment-Service: Pants On");
print $res;
ob_end_flush();
?>
