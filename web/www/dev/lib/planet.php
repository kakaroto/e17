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
    $url = option('planet_url');
    $xml = simplexml_load_file($url);

    $entries = array ();
    for ($i = 0; $i < $limit; $i++)
        $entries[] = $xml->entry[$i];

    $result = array ();
    foreach ($entries as $i => $entry) {
        $result[$i]['title'] = (string) $entry->title;
        $result[$i]['link'] = (string) $entry->id;
        $result[$i]['author'] = (string) $entry->author->name;
        $result[$i]['date'] = substr(str_replace('T', ' ', (string) $entry->updated), 0, -6);

        $summary = trim((string) $entry->content);
        $summary = strip_tags($summary);
        $summary = substr($summary, 0, 250) . '...';

        $result[$i]['summary'] = $summary;
    }

    return $result;
}

?>