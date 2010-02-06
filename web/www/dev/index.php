<?php
/**
    Welcome to the new code for e.org. I tried to keep the code simple and
    the languages in the content for pages and news articles compartmentalized.

    Some operations can be filesystem-intensive and I don't know how the server
    is setup to handle that so I created a primitive cache mechanism that
    serializes selected data structures into files which are refreshed every
    time there is a change in the SVN.

    I decided to use a library to save some work called "limonade" this library
    allows you to use the normal ulrs or move to mod_rewrite seameslly among
    other conveniences.Take a look at http://limonade.sofa-design.net/README.htm
    if you are interested.

    Also, be careful about not deleting the call to "run()", after the dispatch
    calls.

    Execution flow
    ==============

    1.  index.php::require_once('lib/limonade.php')

        *   Defines some constants.
        *   Unregisters globals.
        *   Removes magic quotes.
        *   Disables error display (temporarly).
        *   Sets sine internal routes via "dispatch()", more on this later.

    2   Back to index.php

        *   Define some local functions to serve as handlers or controllers for
            each page.
        *   Defines configure() and before() which will be called by the
            library.
        *   Does various calls to limonade.php::dispatch() to set call backs to
            the local functions defined earlier on different paths.

    3.  Limonade.php::dispatch()

        *   Basically cleans up the input and adds the callback and path to
            an internal array to be used later.

    4.  Back to index.php

        *   limonade.php::run()

    5.  Limonade.php:run()

        *   Sets default configuration options.
        *   Enables error display back on.
        *   Sets the default callback for the error dispatcher.
        *   Calls index.php::configure().
        *   Requires_once every php file in the /lib/ directory.
        *   Setups the session.
        *   Defines other functions like after() and route_missing() if they
            werent defined by any library or index.
        *   Calls index.php::before() if no fatal errors have been triggered so
            far.
        *   FINALLY parses the request URI and calls the corresponding callback
            function, if no callback matches the not_found() function (defined
            in this file) will be called.
        *   The retured string and parameters are passed to after() which can
            be defined here if we want.

    Using errors and notices
    ========================
    The native php function trigger_error() can be used to either pass non
    fatal messages (USER_ERROR_NOTICE and USER_ERROR_WARNING) that will be
    displayed in the page along with the requested content. Using
    USER_ERROR_WARNING is fatal and will print debug information.

    Normally non-fatal notices and warnings would be plugged atop the page
    before passing the resulting html to after, but the lib/html.php
    function `notices()` called in the layout.php cleans up the stack before
    it gets there.

    I want to use these "user" warnings and notices to pass messages that the
    visitor SHOULD see.

    There is also a more powerful error function, halt() which will
    basically kill everything, including not calling before() or after(),
    but it will call configure(). This error shoudl't be used ligthly, only
    in libraries.

    @todo Add a generic page catcher for those pages that do not need special
          code to work.

*/

require_once 'lib/limonade.php';

//DELETEME: Temporary function, delete on production
#. Move contact back to about?n.
function debug ($a)
{
    echo "<h3>var_dump</h3>";
    echo "<pre>";
    var_dump($a);
    echo "</pre>";
    echo "<hr>";
}

function configure ()
{
    $root = dirname(__FILE__);

    /* Enlightenment.org options */
    option('pages_dir',          $root . '/pages');
    option('news_dir',           $root . '/news');
    option('cache_dir',          '/tmp/e.org.cache.d/');
    option('cache_reference',    $root . '/.svn');
    option('planet_url',         'http://planet.enlightenment.org/atom.xml');
    option('themes_url',         'http://exchange.enlightenment.org/api/list?object=theme&theme_group_id=1674');
    option('modules_url',        'http://exchange.enlightenment.org/api/list?object=module');
    option('developers_dir',     '/var/www/web/devs');
    option('fallback_language',  'english');

    /*  Limonade options */
    option('root_dir',           $root);
    option('env',                ENV_DEVELOPMENT);
    option('views_dir',          option('pages_dir'));
    option('error_views_dir',    option('pages_dir'));
}

function before ()
{
    layout('layout.php');
    error_layout('error_layout.php');

    # Forces cache_reset() to delete everything every page load when localhost
    $localhost = preg_match('/^localhost(\:\d+)?/', $_SERVER['HTTP_HOST']);
    if ( ($localhost) and is_dir(option('cache_dir')) ) {
        touch(option('cache_dir'));
        cache_reset();
    }

    # Reset the cache every 30 minutes
    cache_reset_every(1800);

    # Guarantee that params('language') will always be valid.
    $language = params('language');
    params('language', null);
    if ( !is_string($language) or strlen($language) < 1 )
        $language = null;
    else
        $language = strtolower(urldecode($language));
    params('language', language($language));

    # Default variables avialable to all views as "$language" for example.
    set('language', params('language'));
    set('motto',    message('motto'));
    set('header',   array ('about', 'news', 'libraries', 'desktop', 'community') );
    set('footer',   array ('about', 'news', 'libraries', 'desktop', 'community') );
    set('pages',    page_list_merge(params('language'), option('fallback_language')));
}


/* Setup he index */
dispatch('/', 'home');
dispatch('/home/:language', 'home');
dispatch('/libraries/:language', 'libraries');
dispatch('/community/:language', 'community');
dispatch('/artists/:language', 'artists');
dispatch('/about/:language', 'about');
dispatch('/development/:language', 'development');
dispatch('/desktop/:language', 'desktop');
dispatch('/news/:language', 'news');
dispatch('/article/:id/:language', 'render_article');
dispatch ('set_language/:language/**', 'set_language');
dispatch('/:page/:language', 'generic');
error(E_USER_ERROR, 'render_user_error');

/* Executes the request */
run();

/* "controllers" */
function home ()
{
    if ( !$file = page('home') ) halt(NOT_FOUND);

    set('page', 'home');
    set('name', message('home'));
    set('themes', themes(4));
    set('articles', articles( array('limit' => 4) ));
    set('planet', planet(6));

    return html($file);
}

function libraries ()
{
    if ( !$file = page('libraries') ) halt(NOT_FOUND);

    set('page', 'libraries');
    set('name', message('libraries'));

    return html($file);
}

function generic ()
{
    if ( !$file = page(params('page'))) halt(NOT_FOUND);

    set('page', params('page'));
    set('name', message(params('page')));

    return html($file);
}

function community ()
{
    if ( !$file = page('community') ) halt(NOT_FOUND);

    set('page', 'community');
    set('name', message('community'));

    return html($file);
}


function artists ()
{
    if ( !$file = page('artists') ) halt(NOT_FOUND);

    set('page', 'artists');
    set('name', message('artists'));

    return html($file);
}


function about ()
{
    if ( !$file = page('about') ) halt(NOT_FOUND);

    set('page', 'about');
    set('name', message('about'));
    set('developers', developers('name'));
    set('inactive_devs', inactive());

    return html($file);
}

function development ()
{
    if ( !$file = page('development') ) halt(NOT_FOUND);

    set('page', 'development');
    set('name', message('development'));

     set('lists', array (
        array ( 'name'         => 'Enlightenment Announce',
                'link'         => 'http://lists.sourceforge.net/lists/listinfo/enlightenment-announce',
                'archive'      => 'SourceForge',
                'archive link' => 'http://sourceforge.net/mailarchive/forum.php?forum_name=enlightenment-announce',
              ),

        array ( 'name'         => 'Enlightenment Development',
                'link'         => 'http://lists.sourceforge.net/lists/listinfo/enlightenment-devel',
                'archive'      => 'SourceForge',
                'archive link' => 'http://sourceforge.net/mailarchive/forum.php?forum_name=enlightenment-devel',
              ),

        array ( 'name'         => 'Enlightenment SVN Commit',
                'link'         => 'http://lists.sourceforge.net/lists/listinfo/enlightenment-svn',
                'archive'      => 'SourceForge',
                'archive link' => 'http://sourceforge.net/mailarchive/forum.php?forum_name=enlightenment-svn',
              ),

        array ( 'name'         => 'Enlightenment Internationalisation',
                'link'         => 'http://lists.sourceforge.net/lists/listinfo/enlightenment-intl',
                'archive'      => 'SourceForge',
                'archive link' => 'http://sourceforge.net/mailarchive/forum.php?forum_name=enlightenment-intl',
              ),

        array ( 'name'         => 'Enlightenment Bug Traker Notification',
                'link'         => 'http://lists.sourceforge.net/lists/listinfo/enlightenment-bugs',
                'archive'      => 'SourceForge',
                'archive link' => 'http://sourceforge.net/mailarchive/forum.php?forum_name=enlightenment-bugs',
              )
    ));

    return html($file);
}

function desktop ()
{
    if ( !$file = page('desktop') ) halt(NOT_FOUND);

    set('page', 'desktop');
    set('name', message('desktop'));
    set('themes', themes(6));
    set('modules', modules(6));

    set('lists', array (
        array ( 'name'         => 'Enlightenment Announce',
                'link'         => 'http://lists.sourceforge.net/lists/listinfo/enlightenment-announce',
                'archive'      => 'SourceForge',
                'archive link' => 'http://sourceforge.net/mailarchive/forum.php?forum_name=enlightenment-announce',
              ),

        array ( 'name'         => 'Enlightenment Users',
                'link'         => 'http://lists.sourceforge.net/lists/listinfo/enlightenment-users',
                'archive'      => 'SourceForge',
                'archive link' => 'http://sourceforge.net/mailarchive/forum.php?forum_name=enlightenment-users',
              ),

        array ( 'name'         => 'Enlightenment Internationalisation',
                'link'         => 'http://lists.sourceforge.net/lists/listinfo/enlightenment-intl',
                'archive'      => 'SourceForge',
                'archive link' => 'http://sourceforge.net/mailarchive/forum.php?forum_name=enlightenment-intl',
              ),
    ));


    return html($file);
}

function news ()
{
    if ( !$file = page('news') ) halt(NOT_FOUND);

    set('page', 'news');
    set('name', message('news'));
    set('oficial', articles( params('language'), array('limit' => 15) ));
    set('planet', planet(15));

    return html($file);
}

function render_article ()
{
    if ( !$file = page('article') ) halt(NOT_FOUND);

    set('page', 'article');
    set('name', message('article'));
    set('article', article(params('id'), params('language')) );

    return html($file);
}



# Set the language for the session cookie.
function set_language ( )
{
    session_language(params('language'));
    redirect_to(params(0));
}

function not_found($errno, $errstr, $errfile=null, $errline=null)
{
    before();
    if ( !$file = page('not_found') ) halt("Even fallback language '".option('fallback_language')."' missing page 'not_found.html.php'.");

    set('page', 'not_found');
    set('name', message('not_found'));
    set('request', $errstr);

    return html($file);
}


function render_user_error($errno, $errstr, $errfile=null, $errline=null)
{
    before();
    if ( !$file = page('fatal_error') ) halt("Even fallback language '".option('fallback_language')."' missing page 'fatal_error.html.php'.");

    set('page', 'fatal_error');
    set('name', message('fatal_error'));
    set('errno', $errno);
    set('errstr', $errstr);
    set('errfile', $errfile);
    set('errline', $errline);

    return html($file);
}


?>
