<?php
/**
    @author Andres Blanc
    @license Whatever the Enlightenment Window Manager is using ATM.

    Fetches http://planet.enlightenment.org/ articles and provides different
    methods for diplaying them.

    This slowdowns the page loading, a cache system should be made to load the
    atom.xml file locally.

    @todo Implement cache
    @todo More functions with different focuses?
*/

function planet ( $limit )
{
    static $articles = array ();

    if ( empty ($articles) )
        $articles = _planet_init(25);

    return array_slice($articles, 0, $limit, true);
}

function _planet_init( $limit )
{
    $articles = array ();
    if ( !$articles = cache("array.planet.articles") )
    {
        $url = option('planet_url');
        $xml = simplexml_load_file($url);

        $entries = array ();
        for ($i = 0; $i < $limit; $i++)
            $entries[] = $xml->entry[$i];

        $articles = array ();
        foreach ($entries as $i => $entry) {
            $articles[$i]['title'] = (string) $entry->title;
            $articles[$i]['link'] = (string) $entry->id;
            $articles[$i]['author'] = (string) $entry->author->name;
            $articles[$i]['date'] = substr(str_replace('T', ' ', (string) $entry->updated), 0, -6);

            $summary = trim((string) $entry->content);
            $summary = strip_tags($summary);
            $summary = substr($summary, 0, 250) . '...';

            $articles[$i]['summary'] = $summary;
        }
        cache("array.planet.articles", $articles);
    }

    return $articles;
}

?>