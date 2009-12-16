<?php

/**
    Manages the messages available for all languages. Works with an array in the
    format:

    ('language name' => array ('message name' => 'localized message', .. ),
      .. )

    When no language is provided the complete message tree is returned. If a
    language is provided only the message list belonging to the language is
    returned. The language is validated using language() and the result is used.

    @param string $language The language to use for the messages.
    @return array The resulting message array
*/
function messages ( $language = null )
{
    static $messages = array ();

    if ( is_null($language) ) {
        foreach ( languages() as $language )
            if ( empty($messages[$language]) )
                $messages[$language] = _messages_init($language);
        return $messages;
    }

    $language = language($language);
    if ( empty($messages[$language]) )
        $messages[$language] = _messages_init($language);

    return $messages[$language];
}

/**
     Returns a localized message, the fallback version or false.

    @param string $name The message name.
    @param string $language Force a given language (null)
    @return string The localized message or (boolean) false.

*/
function message ($name, $language = null)
{
    $language = language($language);
    $messages = messages($language);

    if ( isset($messages[$name]) )
        return $messages[$name];
    else {
        $messages = messages(option('fallback_language'));
        if ( isset($messages[$name]) )
            return $messages[$name];
    }

    return false;
}


# Helper for `messages()` generates the message tree, deals with the cache, etc.
function _messages_init ($language)
{
    $messages = array ();
    if ( !$messages = cache("array.messages[$language]") )
    {
        $path = file_path(option('pages_dir'), $language, 'messages.php');
        $fall = file_path(option('pages_dir'), option('fallback_language'), 'messages.php');

        if ( !file_exists($fall) )
            halt("Required message file '$fall' does not exist");

        include $fall;
        if ( !isset($messages) )
            halt("Required message array not found in file '$path'.");

        if ($fall != $path )
        {
            $fallback = $messages;
            unset($messages);

            if ( !file_exists($path) )
                halt("Required message file '$path' does not exist");

            include $path;
            if ( !isset($messages) )
                halt("Required message array not found in file '$path'.");

            $messages = array_merge($fallback, $messages);
        }
        cache("array.messages[$language]", $messages);
    }
    return $messages;
}

?>
