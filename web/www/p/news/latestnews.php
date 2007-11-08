<?php
require_once 'class.news.php';

setlocale(LC_ALL, 'en_EN');

$news = new news;
$list = $news->get(2);
foreach($list as $new)
{
   if (($new = $news->read_content($new)) !== NULL)
   {
     echo "<p class='news-latest-title'><a href='p.php?p=news/show&l=".$news->lang."&news_id=$new[id]'>".$new['title']."</a><br />";
     echo "<em class='news-latest-date'>" . $new['date'] . "</em></p>";
     echo "<p class='news-latest-body'>".$new['content']."</p>\n";
   }
}
echo "<p align='right'><a href='p.php?p=news&l=".$news->lang."' title='News'>more ... </a></p>";
?>

