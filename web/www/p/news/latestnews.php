<?php
require_once 'class.news.php';

setlocale(LC_ALL, 'en_EN');

$news = new news;
$list = $news->get(3);
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
	     $before .= "<div align='right' class='news-more'><a href='$link'>more &raquo;</a></div>";
     }
     else $before = $n;

     echo "<p class='news-latest-title'><a href='$link'>" . $new['title'] . "</a><br />";
     echo "<em class='news-latest-date'>" . $new['date'] . "</em></p>";
     echo "<div class='news-latest-body'>" . $before . "</div>\n";
   }
}
echo '<p align="right"><a href="/p.php?p=news&amp;l='.$news->lang.'" title="News">Archives</a></p>';
echo '<p align="right"><a href="/rss.php?p=news&amp;l='.$news->lang.'">RSS 2.0 updates&nbsp;<img src="/i/feed-icon-14x14.png" alt="rss" border="0" /></a></p>';
?>

