<?php 
   if (strtolower($_SERVER['HTTP_HOST']) == 'www.enlightenment.org')
      header("Location: http://enlightenment.org/pages/main.html");
   else
      header("Location: /pages/main.html");
   exit; 
?>
