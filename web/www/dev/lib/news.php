<?php
/**
    @author Andres Blanc
    @license Whatever the Enlightenment Window Manager is using ATM.

    This file deals with the news from the news folder. The planet contents
    are in planet.php.

    @todo Implement filters by date.
*/

/**
    Provides an array of articles in the specified language, when a given
    article was not translated an article from the fallback language is included
    instead.

    The key for the array is the article id and the value is the array provided
    by the `article()` function outlined below.

    The resulting article list can be filtered in many ways usign the $filter
    array in the format.

    ('year' => n, 'month' => n, 'day' => n, 'limit' => n, 'offset' => n )

    All filter parameters are optional and will only be applied if set, by
    default calling `articles()` will display the 10 newest articles in the
    fallback language.

*/
function articles ( $language = null, $filter = array () )
{
    static $articles = array (); // This static array only saves the IDs.
    $fallback = option('fallback_language');

    if ( is_array($language) )
    {
        $filter = $language;
        $language = null;
    }
    $language = language($language);

    if ( empty($articles[$fallback]) )
       $articles[$fallback] = _article_list_init($fallback);

    if ( $language != $fallback )
    {
        if ( empty($articles[$language]) )
            $articles[$language] = _article_list_init($language);
        $article_list = array_merge($articles[$fallback], $articles[$language]);
    }
    else
        $article_list = $articles[$fallback];

    if ( isset($filter['limit']) )
        $limit = $filter['limit'];
    else
        $limit = 10;

    if ( isset($filter['offset']) )
        $offset = $filter['offset'];
    else
        $offset = 0;

    $article_list = array_slice($article_list, $offset, $limit);
    $result = array ();
    foreach ($article_list as $id)
        $result[$id] = article($id);

    return $result;
}

/**
    Returns a given article in the requested language or the fallback language
    in the format of an array:

    ( 'title'   => '', 'author'  => '',
      'mail'    => '', 'date'    => '',
      'summary' => '', 'content' => '' )

*/
function article ( $id, $language = null )
{
    $language = language($language);

    $path = file_path(option('news_dir'), $language, $id);
    if ( !file_exists($path) )
    {
        $path = file_path(option('news_dir'), option('fallback_language'), $id);
        if ( !file_exists($path) )
            trigger_error(message("article_not_found:") . $path, E_USER_WARNING);
        else
            return _article_init($id, option('fallback_language'));
    } else
        return _article_init($id, $language);
}

# Helper for `articles()`
function _article_list_init ( $language )
{
    if ( !$articles = cache("array.articles.$language") )
    {
        $path = file_path(option('news_dir'), $language);
        if ( !file_exists($path) )
        {
            $articles = array ();
        }
        else
        {
            $articles = file_list_dir($path);
            arsort($articles);
        }
        cache("array.articles.$language", $articles);
    }
    return $articles;
}

# Helper for `article()`
function _article_init ( $id, $language )
{
    if ( !$article = cache("array.article.$language.$id") )
    {
        $path = file_path(option('news_dir'), $language, $id);
        $raw = file_get_contents($path);
        $article = _article_parse($raw, $id);
        cache("array.article.$language.$id", $article);
    }
    return $article;
}

# Helper for `_article_init()`
function _article_parse ( $raw, $id )
{
    $article = array ();
    $parts = explode("\n", $raw, 4);

    $summary = explode('<!-- cut -->', $parts[3]);
    if ( count($summary) == 1)
        $summary = strip_tags(substr($parts[3], 0, 350) . "...");
    else
        $summary = strip_tags($summary[0]);

    $article['title']   = $parts[0];
    $article['author']  = $parts[1];
    $article['mail']    = $parts[2];
    $article['date']    = _parse_date($id);
    $article['summary'] = $summary;
    $article['content'] = $parts[3];

    return $article;
}

# Helper for `_article_parse()`
function _parse_date ( $id )
{
    $matches = array ();
    $regex = "/(?P<YYYY>\d{4})"     /* Year     */
             . "(?P<MM>\d{2})"      /* Month    */
             . "(?P<DD>\d{2})-"     /* Day      */
             . "(?P<H>\d{2})"       /* Hour     */
             . "(?P<M>\d{2})"       /* Minutes  */
             . "(?P<S>\d{2})/";     /* Seconds  */
    preg_match ($regex, $id, $matches);

    return "{$matches['YYYY']}-{$matches['MM']}-{$matches['DD']} {$matches['H']}:{$matches['M']}:{$matches['S']}";
}

?>