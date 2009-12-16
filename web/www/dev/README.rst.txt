#################
Enlightenment.org
#################

Welcome to the new e.org website, this file outlines the underlying
concepts for the design and programming of this website. There are additional
README files sprinkled around in the correspondent folders that go into
additional details.

This is the reading list list::

    /README.rst.txt          This file is for everyone.
    /index.php               Those who want to hack the php code.
    /lib/*.php               Those who want to hack the php code.
    /pages/README.rst.txt    Those who want to add content or translate it.
    /style/README.rst.txt    Those who want to change the desing of the website.
    /news/README.rst.txt     Those who want to add news articles.

All these README files reveal some inner workings of each system that someone
hacking the php code might be interested, these explanations are really simple
and usually atop of the file.

All readme files have prettier html versions genrated with restructured text,
any standard pythong installation with docutils should contain this command::

    rst2html README.rst.txt > README.html


Directory Structure
===================

::

    .cache/                 <- Cache file, to force a reload `touch` it.
    lib/                    <- our own and and 3rd party libs
    images/                 <- Image folder for the content.
    images/home             <- Image folder for the content on the home, etc.
    style/
        css/                <- CSS and grid.php to generate the semantic grids.
        images/             <- Images for the design.
    pages/                  <- Website contents and main layout files.
        english/            <- Content files for the english language.
            home.html.php   <- Content with spiced-up-markup for the home
            messages.php    <- Localized page names and messages.
            ..
        ..
    materials/              <- (or dev/materials) Various design resources.
    index.php               <- The website's spine.


TODO
====

#.  Cleanup current design (mostly display:none), move around elements, resize
    some things, add some extra margins. For all pages. Vertical rhythm.

#.  Cleanup code from index.php, remove unnecessary handlers. CONTROLLERS!?

#.  Add some spanish content

    #. messages.php
    #. home.php
    #. news/español/ante-ultima

#.  Test the localization.

#.  Commit changes (perhaps the last commit?).

#.  Write the content of the "brochure" pages that still contain stubs (the
    content is outlined in `materials/README.site_map.txt`):

        #. desktop
        #. development
        #. introduction and resources for artists

#.  Insert the new semantic grid generator I developed apart from this site
    instead of the currently used one and update the template file.

#.  Improve news system

    #.  Add missing date filters for the news system.
    #.  Match formats of the dates displayed for planet articles and local
        articles.

#.  *¿Add a list of applications (similar to the list of themes) in exchange to
    the home page? ¿Or add it to the desktop shell page (after all named
    community)?*



#.  Merge the donation page and code.

#.  Add the wiki pages refered from the brochure pages that do not exist yet.
    Mostly merging existent pages and writting some additional content.

#.  Modify the enlightenment.org "official" palette to include greyscale with
    the same values.

#.  **Do the "decorative" part of the design for all pages.**

#.  Commit and ask for feedback on the mailing list

#.  Search a way for the content writers and translators to tidy-up the
    `.html.php` files for indentation and wrapping without HTMLtidy, which
    fucks up the php tags. Shouldn't be that hard, actually, as I don't intend
    to validate the markup in any way, simply indent tags and wrap text.

#.  **If everything is well is it time to reeplace the old website?**

#.  Redierct http://trac.enlightenment.org/ to http://trac.enlightenment.org/e
    right now it displays an error message.

#.  Do a throrough reestructuring and reorganization of the Trac wiki, most of
    these are written partially or totally, some of the content is still in the
    old wiki, specially the pages outlined in the `README.site_map.txt` file
    inside the materials folder:

    #. main page: Split between Developers, Users and Artists.
    #. Create an index for manuals & API references
    #. Create a minimized user guide without explaining every single dialog, if
       you need a manual to explain a configuration dialog **your dialog is
       broken**
    #. Create a COMPLETE contributing page with an invitation and explanations
       for coders, translators, etc (most of this was written already)

#.  Redesign doxygen, trac, exchange, themes etc.

    #.  wtf is http://l10n.enlightenment.org? are there other subdomains? we
        hould remove them most of them and move their content to the wiki. ONLY
        the subdomains that can't be removed will be redisgned.

#.  Things I would like to add to exchange.enlightenment.org

    #. Some sort of forum or other community feature besides comments.
    #. Merge login system with trac's.
    #. Automatic, server-side screenshot generation.

#. Things I would like to add to trac sometime.

    #. Remove a lot of crap hanging around the interface.
    #. A web based front-end to the mailing list for registered users that
       reesmebles a forum with a trusted user system to prevent spam.