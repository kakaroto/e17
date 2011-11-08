<?php

/**
    @return Multi-level array of developer data in the format:

    ( 'Name'      => array ( 'name' => 'developer name', .. ), ..)

    @example

        $website = $developers['Carsten Haitzler']['WWW'];
        echo $website;
        >> "http://www.rasterman.com/"

    @todo Cache either this or the generated table.

*/
function developers_active ()
{
    $path = '/var/www/web/devs/*';
    $paths = glob($path, GLOB_ONLYDIR);
    $paths = array_filter($paths, '_developer_path_filter');

    $developers = _developer_list($paths);
    ksort($developers);
    return $developers;
}

function _developer_path_filter ( $path )
{
    if ($path == '/var/www/web/devs/ewww')
        return false;

    if ( !file_exists("$path/info.txt") )
        return false;

    return true;
}

function _developer_list ( $paths )
{
    $developers = array();

    foreach ($paths as $path)
    {
        $file = file("$path/info.txt", FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        $entry = _developer_info($file);
        $entry['Photo'] = file_exists("$path/icon-sml.png");
        $developers[$entry['Name']] = $entry;
    }

    return $developers;
}


function _developer_info ( $file )
{
    $entry = array();

    foreach ($file as $line)
    {
        $info = explode(':', $line, 2);
        $info[0] = trim($info[0]);
        $info[1] = trim($info[1]);
        $entry[$info[0]] = $info[1];
    }

    return $entry;
}

/**
    @return Multi-level array of inactive developer data (with less fields).

    @todo Create a "inactive" folder in SVN to move the data of inactive
          developers there instead of deleting them. Parse that.
*/
function developers_inactive ()
{
    $developers = array
    (
//         'Name' => array (
//             'Name'       => '',
//             'mail'       => '',
//             'Managing'   => '',
//             'IRC Nick'   => '',
//             'WWW'        => '',
//         ),
        'Mandrake' => array (
            'Name'      => 'Mandrake',
            'E-Mail'    => 'mandrake@user.sourceforge.net',
            'Managing'  => 'Core Dev Team',
            'IRC Nick'  => 'mandrake',
            'WWW'       => 'http://www.mandrake.net/'
        ),
        'Christopher Rosendahl' => array (
            'Name'      => 'Christopher Rosendahl',
            'E-Mail'    => 'crosendahl@trustlab.com',
            'Managing'  => 'Ewl, Ewd, Etcher',
            'IRC Nick'  => 'smugg',
            'WWW'       => ''
        ),
        'Espen Nilsen' => array (
            'Name'      => 'Espen Nilsen',
            'E-Mail'    => 'enilsenNO@SPAMportalen.no',
            'Managing'  => 'EWL Contribs',
            'IRC Nick'  => 'ymer|SpNlsn',
            'WWW'       => ''
        ),
        'Till Adam' => array (
            'Name'      => 'Till Adam',
            'E-Mail'    => 'tillsan@users.sourceforge.net',
            'Managing'  => 'E17, EVAS DFB',
            'IRC Nick'  => 'tillsan',
            'WWW'       => ''
        ),
        'Klaas Bosteels' => array (
            'Name'      => 'Klaas Bosteels',
            'E-Mail'    => 'redalb@users.sourceforge.net',
            'Managing'  => 'Etox',
            'IRC Nick'  => 'redalb',
            'WWW'       => ''
        ),
        'Adam Kisiel' => array (
            'Name'      => 'Adam Kisiel',
            'E-Mail'    => 'lofwyrm@users.sourceforge.net',
            'Managing'  => 'Etox',
            'IRC Nick'  => 'lofwyrm',
            'WWW'       => ''
        ),
        'Burra' => array (
            'Name'      => 'Burra',
            'E-Mail'    => 'burra@users.sourceforge.net',
            'Managing'  => 'Ecore',
            'IRC Nick'  => 'burra',
            'WWW'       => ''
        ),
        'Franz Marini' => array (
            'Name'      => 'Franz Marini',
            'E-Mail'    => 'marini@pcmenelao.mi.infn.it',
            'Managing'  => 'Imlib2 Contributions',
            'IRC Nick'  => '',
            'WWW'       => ''
        ),
        'Horms' => array (
            'Name'      => 'Horms',
            'E-Mail'    => 'horms@users.sourceforge.net',
            'Managing'  => 'Dev Team',
            'IRC Nick'  => 'horms',
            'WWW'       => ''
        ),
        'Joshua Deere' => array (
            'Name'      => 'Joshua Deere',
            'E-Mail'    => 'dphase@users.sourceforge.net',
            'Managing'  => 'Retina',
            'IRC Nick'  => 'dphase',
            'WWW'       => ''
        ),
        'Chris Thomas' => array (
            'Name'      => 'Chris Thomas',
            'E-Mail'    => 'x5rings@users.sourceforge.net',
            'Managing'  => 'Elogin',
            'IRC Nick'  => 'x5rings',
            'WWW'       => ''
        ),
        'Maher Awamy' => array (
            'Name'      => 'Maher Awamy',
            'E-Mail'    => 'muhri@users.sourceforge.net',
            'Managing'  => 'Imlib/Evas perl bindings',
            'IRC Nick'  => 'muhri',
            'WWW'       => 'http://www.muhri.net/'
        ),
        'Tom Gilbert' => array (
            'Name'      => 'Tom Gilbert',
            'E-Mail'    => 'gilbertt@users.sourceforge.net',
            'Managing'  => 'Epplets API, Giblib, Feh, CamE, Geist, Gozer, Gom, Scrot',
            'IRC Nick'  => 'gilbertt',
            'WWW'       => 'http://www.linuxbrit.co.uk/'
        ),
        'Kirth' => array (
            'Name'      => 'Kirth',
            'E-Mail'    => 'kirth@hole.org',
            'Managing'  => 'Ports and Package Team',
            'IRC Nick'  => 'kirth',
            'WWW'       => ''
        ),
        'Laurence J. Lane' => array (
            'Name'      => 'Laurence J. Lane',
            'E-Mail'    => 'ljlane@users.sourceforge.net',
            'Managing'  => 'Ports and Package Team',
            'IRC Nick'  => 'ljlane',
            'WWW'       => ''
        ),
        'Platon Fomichev' => array (
            'Name'      => 'Platon Fomichev',
            'E-Mail'    => 'stauff@users.sourceforge.net',
            'Managing'  => 'Ports and Package Team',
            'IRC Nick'  => 'stauff',
            'WWW'       => ''
        ),
        'Stephen Frost' => array (
            'Name'      => 'Stephen Frost',
            'E-Mail'    => 'sfrost@users.sourceforge.net',
            'Managing'  => 'Ports and Package Team',
            'IRC Nick'  => 'sfrost',
            'WWW'       => ''
        ),
        'Andrew' => array (
            'Name'      => 'Andrew',
            'E-Mail'    => 'andrew@neep.com.au',
            'Managing'  => 'Web and Doc Team ',
            'IRC Nick'  => 'Andrew ',
            'WWW'       => ''
        ),
        'Christian Kreibich' => array (
            'Name'      => 'Christian Kreibich',
            'E-Mail'    => 'cK@whoop.org',
            'Managing'  => 'EFSD Owner,  Web Team',
            'IRC Nick'  => 'cK ',
            'WWW'       => 'http://www.whoop.org/'
        ),
        'Hendryx' => array (
            'Name'      => 'Hendryx',
            'E-Mail'    => 'winterlight@hendryx.com',
            'Managing'  => 'Web and Doc Team',
            'IRC Nick'  => 'Hendryx',
            'WWW'       => 'http://www.hendyrx.com/'
        ),
        'S. J. Black' => array (
            'Name'      => 'S. J. Black',
            'E-Mail'    => 'alphafemale@radiant.net',
            'Managing'  => 'Doc Team',
            'IRC Nick'  => 'alpha female',
            'WWW'       => ''
        ),
        'Term' => array (
            'Name'      => 'Term',
            'E-Mail'    => 'term@NOSPAM-THANKSusers.sourceforge.net',
            'Managing'  => 'Web Team',
            'IRC Nick'  => 'term',
            'WWW'       => 'http://www.twistedpath.org/'
        ),
        'Tim Fabiniak' => array (
            'Name'      => 'Tim Fabiniak',
            'E-Mail'    => 'trip@ga.prestige.net',
            'Managing'  => 'Web Team',
            'IRC Nick'  => 'minion2',
            'WWW'       => ''
        ),
        'Dan Sinclair' => array (
            'Name'      => 'Dan Sinclair',
            'E-Mail'    => '',
            'Managing'  => 'Ewl, Efreet, EwlBook',
            'IRC Nick'  => 'dj2',
            'WWW'       => 'http://everburning.com/'
        ),
        'Sthithaprajna Garapaty' => array (
            'Name'      => 'Sthithaprajna Garapaty',
            'E-Mail'    => '',
            'Managing'  => 'taskbar module',
            'IRC Nick'  => 'iamsthitha',
            'WWW'       => ''
        ),
     'Marco Trevisan' => array(
                               'Name' => 'Marco Trevisan',
                               'E-Mail' => 'mail@3v1n0.net',
                               'Managing' => 'nothing',
                               'IRC Nick' => 'Trevinho',
                               'WWW' => '',
                               ),
     'Adam Flott' => array(
                           'Name' => 'Adam Flott',
                           'E-Mail' => 'adam@npjh.com',
                           'Managing' => '',
                           'IRC Nick' => 'amf',
                           'WWW' => 'http://www.npjh.com',
                           ),
     'Caio Marcelo de Oliveira Filho' => array(
                                               'Name' => 'Caio Marcelo de Oliveira Filho',
                                               'E-Mail' => '-cmarcelo@gmail.com',
                                               'Managing' => '',
                                               'IRC Nick' => 'cmarcelo',
                                               'WWW' => '',
                                               ),
     'Andre Dieb Martins' => array(
                                   'Name' => 'Andre Dieb Martins',
                                   'E-Mail' => 'andre.dieb@gmail.com',
                                   'Managing' => 'eupnp',
                                   'IRC Nick' => 'andredieb',
                                   'WWW' => 'http://genuinepulse.blogspot.com',
                                   ),
     'Mikhail Gusarov' => array(
                                'Name' => 'Mikhail Gusarov',
                                'E-Mail' => 'dottedmag@dottedmag.net',
                                'Managing' => '',
                                'IRC Nick' => 'dottedmag',
                                'WWW' => 'http://dottedmag.net/',
                                ),
     'Emfox Zhou' => array(
                           'Name' => 'Emfox Zhou',
                           'E-Mail' => 'emfoxzhou@gmail.com',
                           'Managing' => 'zh_CN translation, Debian packaging',
                           'IRC Nick' => 'emfox',
                           'WWW' => 'http://emfoxzhou.googlepages.com',
                           ),
     'Helen Mae Koike Fornazier' => array(
                                          'Name' => 'Helen Mae Koike Fornazier',
                                          'E-Mail' => 'helen.fornazier@profusion.mobi',
                                          'Managing' => '',
                                          'IRC Nick' => 'koike',
                                          'WWW' => '',
                                          ),
     'Jaime Thomas' => array(
                             'Name' => 'Jaime Thomas',
                             'E-Mail' => '-avi.thomas@gmail.com',
                             'Managing' => '',
                             'IRC Nick' => 'jethomas',
                             'WWW' => '',
                             ),
     'Johannes Schauer' => array(
                                 'Name' => 'Johannes Schauer',
                                 'E-Mail' => 'j.schauer@email.de',
                                 'Managing' => '',
                                 'IRC Nick' => 'josch',
                                 'WWW' => '',
                                 ),
     'Falko Schmidt' => array(
                              'Name' => 'Falko Schmidt',
                              'E-Mail' => 'kaethorn@gmail.com',
                              'Managing' => '',
                              'IRC Nick' => 'kaeth0rn',
                              'WWW' => 'http://www.alphagemini.org',
                              ),
     'Laurent Ghigonis' => array(
                                 'Name' => 'Laurent Ghigonis',
                                 'E-Mail' => 'ooookiwi@gmail.com',
                                 'Managing' => 'news, photo, alarm',
                                 'IRC Nick' => 'kiwi_',
                                 'WWW' => 'http://oooo.zapto.org/',
                                 ),
     'Lars Munch' => array(
                           'Name' => 'Lars Munch',
                           'E-Mail' => '-lars@segv.dk',
                           'Managing' => '',
                           'IRC Nick' => 'lmunch',
                           'WWW' => 'http://segv.dk/',
                           ),
     'Luchezar P. Petkov' => array(
                                   'Name' => 'Luchezar P. Petkov',
                                   'E-Mail' => 'luchezar.petkov@gmail.com',
                                   'Managing' => 'Icons',
                                   'IRC Nick' => 'ManowarrioR',
                                   'WWW' => 'http://lucho.sagehall.com',
                                   ),
     'Massimiliano Calamelli' => array(
                                       'Name' => 'Massimiliano Calamelli',
                                       'E-Mail' => 'mcalamelli@gmail.com',
                                       'Managing' => 'exchange (library)',
                                       'IRC Nick' => 'mcalamelli',
                                       'WWW' => 'http://www.mcalamelli.net',
                                       ),
     'Simon Busch' => array(
                            'Name' => 'Simon Busch',
                            'E-Mail' => 'morphis@gravedo.de',
                            'Managing' => 'python-elementary',
                            'IRC Nick' => 'morphis',
                            'WWW' => 'http://mm.gravedo.de/',
                            ),
     'Nathan Ingersoll' => array(
                                 'Name' => 'Nathan Ingersoll',
                                 'E-Mail' => 'ningerso@gmail.com',
                                 'Managing' => 'EWL, ecore_data',
                                 'IRC Nick' => 'RbdPngn',
                                 'WWW' => 'http://ningerso.edevelop.org/',
                                 ),
     'Otávio Pontes' => array(
                              'Name' => 'Otávio Pontes',
                              'E-Mail' => '-otavio@profusion.mobi',
                              'Managing' => '',
                              'IRC Nick' => 'otaviobp',
                              'WWW' => '',
                              ),
     'Peter Wehrfritz' => array(
                                'Name' => 'Peter Wehrfritz',
                                'E-Mail' => 'peter.wehrfritz@web.de',
                                'Managing' => 'EWL, Elitaire',
                                'IRC Nick' => 'pfritz',
                                'WWW' => '',
                                ),
     'Rodrigo Cesar Lopes Belem' => array(
                                          'Name' => 'Rodrigo Cesar Lopes Belem',
                                          'E-Mail' => 'rclbelem@gmail.com',
                                          'Managing' => '',
                                          'IRC Nick' => 'rbelem',
                                          'WWW' => 'http://rbelem.blogspot.com',
                                          ),
     'Sergey Pavlovich Semernin' => array(
                                          'Name' => 'Sergey Pavlovich Semernin',
                                          'E-Mail' => 'sergey.semernin@gmail.com',
                                          'Managing' => '',
                                          'IRC Nick' => 'aviator',
                                          'WWW' => '',
                                          ),
     'Sevcsik András' => array(
                               'Name' => 'Sevcsik András',
                               'E-Mail' => '',
                               'Managing' => 'eyesight',
                               'IRC Nick' => 'sevcsik',
                               'WWW' => 'http://sevcsik.blogspot.com',
                               ),
     'Tom Haste' => array(
                          'Name' => 'Tom Haste',
                          'E-Mail' => 'tomhaste@gmail.com',
                          'Managing' => 'exchange',
                          'IRC Nick' => 'Toma-',
                          'WWW' => 'http://edjy.wordpress.com/',
                          ),
     'Ulisses Furquim' => array(
                                'Name' => 'Ulisses Furquim',
                                'E-Mail' => '-ulissesf@gmail.com',
                                'Managing' => 'Python-efl',
                                'IRC Nick' => 'Ryback_',
                                'WWW' => 'http://ulissesf.wordpress.com/',
                                ),
     'Ibukun Olumuyiwa' => array(
                                 'Name' => 'Ibukun Olumuyiwa',
                                 'E-Mail' => 'ibukun@computer.org',
                                 'Managing' => 'Entrance, code contributions (Ecore, E17)',
                                 'IRC Nick' => 'xcomp',
                                 'WWW' => 'http://xcomputerman.com',
                                 ),
     );
    
    ksort($developers);
    return $developers;
}


/* Don't add the ?>  */
