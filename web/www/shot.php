<?php
ob_start();
############ limit - 6 mb.
$data = file_get_contents('php://input', NULL, NULL, 0, 6 * 1024 * 1024);
############ magic jpeg signature
$jpeg_match = "\xff\xd8\xff\xe0";
$jpeg_magic = substr($data, 0, 4);
############ magic png signature
$png_match = "\x89\x50\x4e\x47";
$png_magic = substr($data, 0, 4);

############ base on signaure, add file extension
$ext = ".unknown";
if ($jpeg_match == $jpeg_magic) $ext = ".jpg";
else if ($png_match == $png_magic) $ext = ".png";
############ not a correct matching file - abort
else {
        echo "bzzt";
        ob_end_flush();
        die();
}

############ get a unique name
$dest = uniqid("e-", true) . $ext;
############ store the file
$fh = fopen("/var/www/download/shots/".$dest, 'wb');
fwrite($fh, $data);
fclose($fh);
############ prepare url to get file from
$loc = "http://www.enlightenment.org/ss/" . $dest;

############ respond!
header("HTTP/1.1 200 OK");
header("Content-Type: text/plain");
header("X-Enlightenment-Service: Pants On");
print $loc;
ob_end_flush();
?>
