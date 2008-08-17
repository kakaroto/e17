<?php
require_once 'class.news.php';

setlocale(LC_ALL, 'en_EN');

$news = new news;
$list = $news->get(2);
foreach($list as $new)
{
   if (($new = $news->read_content($new)) !== NULL)
   {
     $n = $new['content'];
     $link = "p.php?p=news/show&l=" . $news->lang . "&news_id=$new[id]";

     $pos = strpos($n, '<!-- cut -->');
     if ($pos != FALSE) 
     {
	     $before = substr($n, 0, $pos);
	     $before .= "<div align='right' class='news-latest-date'><a href='$link'>more &raquo;</div>";
     }
     else $before = $n;

     echo "<p class='news-latest-title'><a href='$link'>" . $new['title'] . "</a><br />";
     echo "<em class='news-latest-date'>" . $new['date'] . "</em></p>";
     echo "<p class='news-latest-body'>" . $before . "</p>\n";
   }
}
echo "<p align='right'><a href='p.php?p=news&l=" . $news->lang . "' title='News'>Archives</a></p>";
?>

