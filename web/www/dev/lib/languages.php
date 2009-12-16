<?php
/**
    @author Andres Blanc
    @license Whatever the Enlightenment Window Manager is using ATM.
*/

/**
    Manages the available languages. The function simply manages an array of
    languages that meet a minimal requirement of validity like having a home
    page file and a message file.

    Even when the data structure is quite simple, generating it is hdd intensive
    so the result is cached as with the messages and pages.

    @return array The available language list.
*/
function languages ()
{
    static $languages = array ();

    if ( empty($languages) )
        $languages = _languages_init();

    return $languages;
}

/**
    Returns a valid language, either the same language provided or a fallback
    value that might be the current param('language'), if set, the session
    language, if any, or the option('fallback_language).

    @param  string  $language The language to validate (null).
    @return string  A valid language, either the same provided or a fallback.
*/
function language ($language = null)
{
    if( is_null($language) )
        return _safe_language();

    if ( in_array($language, languages()) )
        return $language;
    else
        trigger_error(message('invalid_language:') . $language, E_USER_WARNING);

    return _safe_language();
}

/**
    Set the provided $language for the session. If no language is provided the
    current session language (if available) will be returned. If the session
    language is invalid null will be returned.

    @param string $language The language to use for the session (null)
    @return string A valid language or null.
*/
function session_language ( $language = null )
{
    if ( is_null($language) ) {
        if ( isset($_SESSION['language']) and in_array($_SESSION['language'], languages()) )
           return $_SESSION['language'];
        else
           return null;
    }

    $_SESSION['language'] = language($language);
    return $_SESSION['language'];
}

# Helper for `languages()` self-contained assesment of valid languages.
function _languages_init ( )
{
    $languages = array ();
    if ( !$languages = cache('array.languages') ) {
        $nodes = file_list_dir(option('pages_dir'));

        foreach ( $nodes as $i => $node ) {
            $path = file_path(option('pages_dir'), $node);

            if ( is_dir($path) ) {
                if ( !file_exists(file_path($path, 'home.html.php')) )
                    unset($nodes[$i]);
                if ( !file_exists(file_path($path, 'messages.php')) )
                    unset($nodes[$i]);

            } else
                unset($nodes[$i]);
        }

        $languages = $nodes;
        cache('array.languages', $languages);
    }
    return $languages;
}

# Helper for `language` returns either the current, session's or fallback.
function _safe_language ()
{
    if ( $language = params('language') )
        return $language;
    elseif ( $language = session_language() )
        return $language;
    else
        return option('fallback_language');
}

