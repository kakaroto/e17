<?php
  $handle = fopen("/tmp/www-up", "w");
  fclose($handle);
  system("chmod a+r+w /tmp/www-up");
?>
<html><body><p>OK</p></body></html>
