<?php
/**
    @author Andres Blanc
    @license Whatever the Enlightenment Window Manager is using ATM.

    Helper functions to be used inside the views.

    All these functions ECHO the result so you don't have to do it in your
    views.

    Using these for urls will guarantee a seameless move to mod_rewrite or any
    other chagnes in the directory structure of the website.
*/

/**
    Echoes the absolute url for a given page name. Use it only when refering
    to internal pages, not things like trac or exchange.
*/
function url_page ( $page )
{
    echo url_for($page);
}

/**
    Echoes the absolute url for style image
*/
function url_style_image ( $filename )
{
    echo '/dev/style/images/' . $filename;
}

/**
    Echoes the absolute url for css file.
*/
function url_css($filename)
{
    echo '/dev/style/css/' . $filename;
}

/**
    Echoes the absolute url for an image belonging to a given page, if no page
    is provided the current page name will be used.

    The file must be placed inside a folder with the same name of the page,
    inside the images folder.
*/
function url_image($filename, $page = null)
{
    if ( is_null($page ) )
        echo '/dev//images/' . set('page') . "/" . $filename;
    else
        echo '/dev//images/' . $page . "/" . $filename;
}

/**
    Echoes $content or if content is empty &nbsp; for crappy old browsers.
*/
function td ( $content )
{
    if ( empty($content) or strlen(trim($content)) < 1 )
        echo "&nbsp;";
    else
        echo $content;
}

/**
    Echoes "class='odd'" on every odd call to the function. The function keep
    track of itself. Its useful determining odd rows in a table for prettyfing.
*/
function tr_odd()
{
    static $n = 0;
    if ($n & 1)
        echo "class='odd'";
    $n++;
}

/**
    Returns an array of notices or false if there are none.
*/
function notices ()
{
    $notices = error_notice();
    error_notice(null);

    if ( empty($notices) )
        return false;
    else
        return $notices;
}

