<?php require_once 'class.news.php'; setlocale(LC_ALL, 'en_EN'); $news = 
new news; $list = $news->get(2); foreach($list as $new) {
   if (($new = $news->read_content($new)) !== NULL)
   {
     echo "<div class='post'>\n";
     echo "<p class='news-title'><a 
href='p.php?p=news/show&l=".$_GET['l']."&news_id=$new[id]'>".$new['title']."</a></p>";
     echo "<p class='tiny'><a 
href='mailto:".$new['email']."'>".$new['author']."</a> - 
".$new['date']."</p>";
     echo "<p class='news-content'>".$new['content']."</p>\n";
     echo "</div>\n";
   }
echo "<p align='right'><a href='p.php?p=news&l=".$_GET['l']."' 
title='News'>More news ... </a></p>";
}
?>
