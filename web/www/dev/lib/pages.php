<?php
/**
    @author Andres Blanc
    @license Whatever the Enlightenment Window Manager is using ATM.

    Manages the contents of the option('pages_dir) directory.
*/

/**
    In charge of locating and validating all the pages for all the languages.
    It uses a multi-dimensional array in the format:

    ( 'language name' => ('page name' => 'localized page name', .. ),
      .. )

    When a language is provided it will return the sub-array belonging to said
    language, if an invalid language is provided a fallback value will be used
    and a warning will be triggered.

    When no language is provided the complete page tree is returned, useful to
    display a site map with pages organized by languages.

    @param string $language The language to use for the page list.
    @return array The resulting pages array
*/
function pages ($language = null)
{
    static $pages = array ();

    if ( is_null($language) ) {
        foreach ( languages() as $language )
            if ( empty($pages[$language]) )
                $pages[$language] = _pages_init($language);
        return $pages;
    }

    $language = language ($language);
    if ( empty($pages[$language]) )
        $pages[$language] = _pages_init($language);

    return $pages[$language];
}

/**
    Returns the path to the page file in the specified language, in the fallback
    language, or false if its not found anywere.

    @param string $page     The name of the page.
    @param string $language Force a language or not (null).
    @return string          The path to the page file or false.
*/
function page ($page, $language = null)
{
    $language = language($language);
    $pages = pages($language);

    if ( !isset($pages[$page]) ) {
        trigger_error(message("not_translated"), E_USER_NOTICE);
        $language = option('fallback_language');
        $pages = pages($language);
    }

    if ( isset($pages[$page]) )
        return $language . "/" . $page . '.html.php';
    else
        return false;
}

/**
    Merges a list of available pages for a given language with the list of a
    fallback language.

    FIXME: Assumes a valid language and valid fallback language.
*/
function page_list_merge ($language, $fallback)
{
    $pages = pages($language);
    $fallback_pages = pages($fallback);
    return array_merge($fallback_pages, $pages);
}

# Helper for `pages()` generates the page tree for a given language.
function _pages_init ($language)
{
    $pages = array ();
    if ( !$pages = cache("array.pages[$language]") ) {
        $path = file_path(option('pages_dir'), $language);
        $files = file_list_dir($path);
        $pages = _validate_pages($files);
        $pages = _init_page_names($pages, $language);
        cache("array.pages[$language]", $pages);
    }
    return $pages;
}

# Helper for `_pages_init()` adds the localized names to the pages list
function _init_page_names ($pages, $language)
{
    $names = array ();
    foreach ( $pages as $page )
        $names[$page] = message($page, $language);
    return $names;
}

# Helper for `_pages_init()` generates a valid page list from a filename array.
function _validate_pages ($files)
{
    $pages = array ();
    foreach ($files as $filename)
        if ( $page = _validate_page($filename) )
            $pages[] = $page;
    return $pages;
}

# Helper for `_validate_pages()` returns the page name from a filename or false.
function _validate_page ($filename)
{
    if ( $pos = strpos($filename, '.html.php') )
        return substr($filename, 0, $pos);
    else
        return false;
}

?>