<?php
/**
    @author Andres Blanc
    @license Whatever the Enlightenment Window Manager is using ATM.

    Fetches themes from http://exchange.enlightenment.org/ and provides several
    methods for displaying them.

    We should also improve the exchange side to add "sorting by" directly in the
    rss so we can use it with the already implmeneted limit parameter to not
    download the whole theme list

    @todo Implement a cache system (obsolete if exchange is updated)-
    @todo More functions with different focuses?

*/
function themes ( $limit )
{
    static $themes = array ();

    if ( empty ($themes) ){
        $url = option('themes_url');
        /* For some reason requesting the theme list from the server adds a
           couple of characters before and after the xml, when this is resolved
           we should go back to the way it was before */
        $str = file_get_contents($url);
        $srt = str_replace('89fb', '', $str);

        libxml_use_internal_errors(true);
        $xml = simplexml_load_string($str);
        debug($xml);
        debug(libxml_get_errors());
        debug($str);

        $themes = array ();
        foreach ($xml->themes->theme as $theme)
            $themes[(integer) $theme->id] = (float) $theme->rating;
        arsort($themes);
    }

    return array_keys(array_slice($themes, 0, $limit, true));
}

/**
    The resulting array of modules is similar to the array of themes
*/
function modules ( $limit )
{
    static $modules = array ();

    if ( empty ($modules) ){
        $url = option('modules_url');
        $url = $url . "&limit=$limit";
        $xml = simplexml_load_file($url);

        $modules = array ();
        foreach ($xml->modules->module as $module)
            $modules[(integer) $module->id] = (string) $module->name;
    }

    return $modules;
}
?>