<?php
/* PUT HTML RELATED FUNCTIONS HERE */

/* Not yet... */
// function button ( $url, $title )
// {
//     echo <<<EOD
//
//     <p class="button">
//         <a href="$url"><span>$title</span></a>
//     </p>
//
// EOD;
// }

function td ( $content )
{
    if ( empty($content) )
        echo '&nbsp;';
    else
        echo $content;
}

function tr_odd ( $reset = false )
{
    static $n = 0;

    if ( $reset )
        $n = 0;
    else if ($n++ & 1)
        echo 'class="odd"';
}

/*
    Functions for developers lists.
*/

function dev_data_photo( $data )
{
    if ( $data['Photo'] )
        return "<img src='http://download.enlightenment.org/devs/{$data['Login']}/icon-sml.png' width='40' height='54' border='0'>";
    else
        return "<img src='i/logo-s.png' width='18' height='24' border='0'>";
}

function dev_data_www ( $data )
{
    $url = @$data["WWW"];

    if ( strstr($url, 'http://') )
    {
        $domain = str_replace(array('http://', 'www.'),'', $url);
        $domain = rtrim($domain, '/');
        return "<a href='$url'>$domain</a>";
    }

    return null;
}

function dev_data_contact ( $data )
{
    $name = @$data['Name'];
    $mail = @$data['E-Mail'];

    if ( !empty($mail) )
        return "<a href='mailto:$mail'>$name</a>";

    return $name;
}

function dev_data_irc ( $data )
{
    $nick = @$data['IRC Nick'];

    if ( $nick )
    {
        $nick = str_replace(' ', '', $nick);
        return str_replace('/', ', ', $nick);
    }

    return null;
}

/* Don't add the ?> */
