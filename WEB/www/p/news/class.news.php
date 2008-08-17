<?php

class news {

  var $list = array();
  var $dirnews;
  var $limit = 0;
  var $lang = "en";

  function __construct() {
    $i = 0;
    if (isset($_GET['l']) && (strlen($_GET['l']) == 2)) {
      $this->lang = $_GET['l'];
    }
    $this->dirnews = "p/news/" . $this->lang;

    if(!($dp = opendir($this->dirnews)))
        return NULL;

    while (($file = readdir($dp)) !== false)
    {
        if (!strcasecmp($file, ".") || !strcasecmp($file, ".."))
            continue;

        $news = array();
        $news['id']   = $i;
        $news['path'] = $this->dirnews."/".$file;
        $news['timestamp'] = $this->_get_timestamp($news['path']);
        $news['date'] = $this->_get_localtime($news['timestamp']);
        array_push($this->list, $news);
        $i++;
    }
    closedir($dp);
    $this->_sort($this->list, 'timestamp');
  }

  function get($limit_news) {
    $i = 1;
    $limited = array();

    if ($limit_news <= 0)
        return $this->list;

    foreach($this->list as $val)
    {
        if ($i <= $limit_news)
            array_push($limited, $val);
        $i++;
    }
    $this->limit = $limit_news;
    return $limited;
  }

  function get_one($news_id) {
    if ($news_id < 0)
        return NULL;

    foreach ($this->list as $new)
    {
        if ($new['id'] == $news_id)
        {
            $new = $this->read_content($new);
            return $new;
        }
    }
    return NULL;
  }

  function read_content($new) {
    if (!file_exists($new['path']))
        return NULL;

    if (!($fp = fopen($new['path'], "r")))
        return NULL;

    $new['content'] = NULL;
    while (!feof($fp))
    {
        $buffer = fgets($fp, 4096);
        if ($i == 0)
            $new['title'] = $buffer;
        else if ($i == 1)
            $new['author'] = $buffer;
        else if ($i == 2)
            $new['email'] = $buffer;
        else
            $new['content'] .= $buffer;
        $i++;
    }
    fclose($fp);
    return $new;
  }

  function has_archives() {
    if (count($this->list) > $this->limit)
        return true;
    return false;
  }

  function _sort_cb($a, $b) {
    return ($a['timestamp'] < $b['timestamp']) ? 1 : 0;
  }

  function _sort(&$news) {
    uasort($news, array("news", "_sort_cb"));
  }

  private function _get_timestamp($path) {
    if (!file_exists($path))
       return NULL;

    $name = strrchr($path, "/");
    for($i=1; $name[$i] != NULL; $i++)
    {
        if ($i <= 4)
            $date['year'] .= $name[$i];
        if ($i > 4 && $i <= 6)
            $date['month'] .= $name[$i];
        if ($i > 6 && $i <= 8)
            $date['day'] .= $name[$i];
        if ($i > 9 && $i <= 11)
            $date['hour'] .= $name[$i];
        if ($i > 11 && $i <= 13)
            $date['minute'] .= $name[$i];
        if ($i > 13 && $i <= 15)
            $date['second'] .= $name[$i];
    }
    return mktime($date['hour'], $date['minute'], $date['second'], $date['month'], $date['day'], $date['year'] + 0);
  }

  private function _get_localtime($timestamp) {
    return strftime('%b %e, %G at %I:%M %p', $timestamp);
  }
}

?>
