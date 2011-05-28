<?php
$data = file_get_contents('php://input');
#ff d8 ff e0
$jpeg_match = "\xff\xd8\xff\xe0";
$jpeg_magic = substr($data, 0, 4);
#89 50 4e 47
$png_match = "\x89\x50\x4e\x47";
$png_magic = substr($data, 0, 4);
$ext = ".unknown";
if ($jpeg_match == $jpeg_magic) $ext = ".jpg";
else if ($png_match == $png_magic) $ext = ".png";
$dest = uniqid("e-", true) . $ext;
$fh = fopen("/var/www/download/shots/".$dest, 'wb');
fwrite($fh, $data);
fclose($fh);
?>
<?php
  echo "http://www.enlightenment.org/ss/" . $dest;
?>
