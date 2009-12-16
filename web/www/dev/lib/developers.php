<?php
/**
    @author Andres Blanc
    @license Whatever the Enlightenment Window Manager is using ATM.

    This file manages the developer lists, for that the option "developers_dir"
    must be set.

    The inactive developer list is generated manually, I have no idea if this
    can be done programatically but the old site seemed to have done it by
    hand as well.
*/

/**
    Returns a multi-level array representing the developer list if the format:

    ( 'login' => array ( 'name' => 'developer name', .. ),
      ..)

    ex:
        $name = $developers['raster']['name'];
        echo $name;
        >> "Carsten Haitzler"

    The available parameters are set by `_developer_list_init()` and can be
    easely expanded. The developer list can be sort by any of these parameters.

    @param string $sortby The sorting parameter.
*/
function developers ( $sort_by = null )
{
    static $developers = array();
    static $last_sort = '';

    if ( empty($developers) )
        $developers = _developers_init();

    if ( is_null($sort_by) ) {
        ksort($developers);
        $last_sort = null;

    } else {
        if ($last_sort != $sort_by)
           $developers = developers_sort($sort_by, $developers);
        $last_sort = $sort_by;
    }

    return $developers;
}

# Uses a case insensitive "natural" sort version, whatever that means, if by
# is not a valid index this will fail miserably.
function developers_sort ( $by, $developers )
{
    $sorted = array();
    $result = array();

    foreach ($developers as $login => $data)
        $sorted[$login] = $data[$by];
    natcasesort($sorted);

    foreach ($sorted as $login => $value)
        $result[$login] = $developers[$login];

    return $result;
}

/**
    Returns a multi-level array representing the inactive developer list in
    the format.

    ('developer name' => array ('email' => 'email address', ..), ..)

    ex:
        $email = $developers['Mandrake']['email'];
        echo $email;
        >> "mandrake@user.sourceforge.net"

*/
function inactive ()
{
    static $developers = array
    (
//         array (
//             'name' => '',
//             'mail' => '',
//             'managed' => '',
//             'irc' => '',
//             'website' => '',
//         ),
        'Mandrake' => array (
            'name' => 'Mandrake',
            'email' => 'mandrake@user.sourceforge.net',
            'managed' => 'Core Dev Team',
            'irc' => 'mandrake',
            'website' => 'http://www.mandrake.net/'
        ),
        'Christopher Rosendahl' => array (
            'name' => 'Christopher Rosendahl',
            'email' => 'crosendahl@trustlab.com',
            'managed' => 'Ewl, Ewd, Etcher',
            'irc' => 'smugg',
            'website' => ''
        ),
        'Espen Nilsen' => array (
            'name' => 'Espen Nilsen',
            'email' => 'enilsenNO@SPAMportalen.no',
            'managed' => 'EWL Contribs',
            'irc' => 'ymer|SpNlsn',
            'website' => ''
        ),
        'Till Adam' => array (
            'name' => 'Till Adam',
            'email' => 'tillsan@users.sourceforge.net',
            'managed' => 'E17, EVAS DFB',
            'irc' => 'tillsan',
            'website' => ''
        ),
        'Klaas Bosteels' => array (
            'name' => 'Klaas Bosteels',
            'email' => 'redalb@users.sourceforge.net',
            'managed' => 'Etox',
            'irc' => 'redalb',
            'website' => ''
        ),
        'Adam Kisiel' => array (
            'name' => 'Adam Kisiel',
            'email' => 'lofwyrm@users.sourceforge.net',
            'managed' => 'Etox',
            'irc' => 'lofwyrm',
            'website' => ''
        ),
        'Burra' => array (
            'name' => 'Burra',
            'email' => 'burra@users.sourceforge.net',
            'managed' => 'Ecore',
            'irc' => 'burra',
            'website' => ''
        ),
        'Franz Marini' => array (
            'name' => 'Franz Marini',
            'email' => 'marini@pcmenelao.mi.infn.it%3E',
            'managed' => 'Imlib2 Contributions',
            'irc' => '',
            'website' => ''
        ),
        'Horms' => array (
            'name' => 'Horms',
            'email' => 'horms@users.sourceforge.net',
            'managed' => 'Dev Team',
            'irc' => 'horms',
            'website' => ''
        ),
        'Joshua Deere' => array (
            'name' => 'Joshua Deere',
            'email' => 'dphase@users.sourceforge.net',
            'managed' => 'Retina',
            'irc' => 'dphase',
            'website' => ''
        ),
        'Chris Thomas' => array (
            'name' => 'Chris Thomas',
            'email' => 'x5rings@users.sourceforge.net',
            'managed' => 'Elogin',
            'irc' => 'x5rings',
            'website' => ''
        ),
        'Maher Awamy' => array (
            'name' => 'Maher Awamy',
            'email' => 'muhri@users.sourceforge.net',
            'managed' => 'Imlib/Evas perl bindings',
            'irc' => 'muhri',
            'website' => 'http://www.muhri.net/'
        ),
        'Tom Gilbert' => array (
            'name' => 'Tom Gilbert',
            'email' => 'gilbertt@users.sourceforge.net',
            'managed' => 'Epplets API, Giblib, Feh, CamE, Geist, Gozer, Gom, Scrot',
            'irc' => 'gilbertt',
            'website' => 'http://www.linuxbrit.co.uk/'
        ),
        'Kirth' => array (
            'name' => 'Kirth',
            'email' => 'kirth@hole.org',
            'managed' => 'Ports and Package Team',
            'irc' => 'kirth',
            'website' => ''
        ),
        'Laurence J. Lane' => array (
            'name' => 'Laurence J. Lane',
            'email' => 'ljlane@users.sourceforge.net',
            'managed' => 'Ports and Package Team',
            'irc' => 'ljlane',
            'website' => ''
        ),
        'Platon Fomichev' => array (
            'name' => 'Platon Fomichev',
            'email' => 'stauff@users.sourceforge.net',
            'managed' => 'Ports and Package Team',
            'irc' => 'stauff',
            'website' => ''
        ),
        'Stephen Frost' => array (
            'name' => 'Stephen Frost',
            'email' => 'sfrost@users.sourceforge.net',
            'managed' => 'Ports and Package Team',
            'irc' => 'sfrost',
            'website' => ''
        ),
        'Andrew' => array (
            'name' => 'Andrew',
            'email' => 'andrew@neep.com.au',
            'managed' => 'Web and Doc Team ',
            'irc' => 'Andrew ',
            'website' => ''
        ),
        'Christian Kreibich' => array (
            'name' => 'Christian Kreibich',
            'email' => 'cK@whoop.org',
            'managed' => 'EFSD Owner,  Web Team',
            'irc' => 'cK ',
            'website' => 'http://www.whoop.org/'
        ),
        'Hendryx' => array (
            'name' => 'Hendryx',
            'email' => 'winterlight@hendryx.com',
            'managed' => 'Web and Doc Team',
            'irc' => 'Hendryx',
            'website' => 'http://www.hendyrx.com/'
        ),
        'S. J. Black' => array (
            'name' => 'S. J. Black',
            'email' => 'alphafemale@radiant.net',
            'managed' => 'Doc Team',
            'irc' => 'alpha female',
            'website' => ''
        ),
        'Term' => array (
            'name' => 'Term',
            'email' => 'term@NOSPAM-THANKSusers.sourceforge.net',
            'managed' => 'Web Team',
            'irc' => 'term',
            'website' => 'http://www.twistedpath.org/'
        ),
        'Tim Fabiniak' => array (
            'name' => 'Tim Fabiniak',
            'email' => 'trip@ga.prestige.net',
            'managed' => 'Web Team',
            'irc' => 'minion2',
            'website' => ''
        ),
        'Dan Sinclair' => array (
            'name' => 'Dan Sinclair',
            'email' => '',
            'managed' => 'Ewl, Efreet, EwlBook',
            'irc' => 'dj2',
            'website' => 'http://everburning.com/'
        ),
        'Sthithaprajna Garapaty' => array (
            'name' => 'Sthithaprajna Garapaty',
            'email' => '',
            'managed' => 'taskbar module',
            'irc' => 'iamsthitha',
            'website' => ''
        ),
    );
    return $developers;
}

function _developers_init ()
{
    $developers = array ();
    if ( !$developers = cache("array.developers") )
    {
        $path = option('developers_dir');
        $developers = file_list_dir($path);
        $developers = array_filter($developers, '_developer_list_filter');
        $developers = _developer_list_init($developers, $path);
        cache("array.developers", $developers);
    }
    return $developers;
}

function _developer_list_filter ( $node )
{
    static $ignore = array ('.svn', 'ewww');
    $path = option('developers_dir');

    if ( in_array($node, $ignore) )
        return false;
    else {
        $dir = file_path($path,$node);
        if ( is_dir($dir) )
            if ( file_exists(file_path($dir, 'info.txt')) )
                return true;
    }
    return false;
}

//FIXME: This could be smarter, but its not going to happen done so often after
//       its cached anyway.
function _developer_list_init ( $logins, $path )
{
    $developers = array_fill_keys($logins, array());
    foreach ( $logins as $login ) {
        $text = file_get_contents(file_path($path, $login, 'info.txt'));
        $developers[$login]['name']     = _get_info_entry('Name', $text);
        $developers[$login]['email']    = _get_info_entry('E-Mail', $text);
        $developers[$login]['managing'] = _get_info_entry('Managing', $text) .
                                          " ". _get_info_entry('Contributing', $text);
        $developers[$login]['irc']      = _get_info_entry('IRC Nick', $text);
        $developers[$login]['location'] = _get_info_entry('Location', $text);
        $developers[$login]['website']  = _get_info_entry('WWW', $text);
    }
    return $developers;
}

function _get_info_entry ($id, $text)
{
    $info = explode($id, $text);
    if ( isset($info[1]) ) {
        $info = explode("\n", $info[1]);
        $info = trim($info[0], ": ");
        return $info;
    }
    return "";
}

?>
