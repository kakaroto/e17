<?php 
   $hostname = strtolower($_SERVER['HTTP_HOST']);
   if ($hostname == 'www.enlightenment.org')
      header("Location: http://enlightenment.org/pages/main.html");
   else
      header("Location: /pages/main.html");
   exit; 
?>
