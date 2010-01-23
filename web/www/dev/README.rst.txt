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
    /SITEMAP.rst.txt         Those who want to add content or translate it.
    /style/README.rst.txt    Those who want to change the design of the website.
    /news/README.rst.txt     Those who want to add news articles.

All these README files reveal some inner workings of each system that someone
hacking the php code might be interested, these explanations are really simple
and usually atop of the file.

All README files have prettier HTML versions generated with restructured text,
any standard python installation with docutils should contain this command::

    rst2html README.rst.txt > README.html


.. contents::


Directory Structure
===================

::

    images/
        home/                <- Images for the content (in home)
    style/
        css/                 <- CSS and grid generator.
        images/              <- Images for the design.
    pages/
        english/             <- Content files for the english language.
            home.html.php    <- Content with spiced-up-markup for the home
            messages.inc.php <- Localized page names and messages.
    news/
        english/             <- News Folder for the english language.
            YYYYMMDD-HHMMSS  <- News File for the english language
    materials/               <- (or dev/materials) Various design resources.
    lib/                     <- Our own and and 3rd party php libs.
    index.php                <- The website's spine.


TODO
====

Stage 1: Brochure pages
-----------------------

#.  *(DONE: awayting for feedback)* Design a complete
    sitemap with stub content that includes the relationship between the
    wiki and the brochures, most content is going to the wiki, I agree with
    that, the brochures are an adverstiment and a portal to the wiki.

#.  Write the markup with the stub content.

#.  Merge the content written by raster into the stub pages.

#.  Merge the content I wrote in other places into and write the remaining
    content (not much).

#.  Generate the layout for these pages using the semantic grid system I
    already made (currently based on blueprint), fix the vertical rythm of
    some of the objects and and cleanup the unnecessary objects.

#.  Add the (new) graphical elements to the design, change all colors to match
    the pallete, replace some of the blueprint's default design decisions with
    my own.

#.  Create the stubs for the wiki pages pointed from the brochures.

#.  **Release the brochure pages into the main site**


Stage 2: Wiki reorganization
----------------------------

#.  Write the rest of content of the perviously mentioned stub pages. These
    pages might point to other stub pages if there was not content available
    to be used.

#.  Merge existing content in subdomains like svn.enlightenment.org and
    packages into the wiki.

#.  Merge the content of the older E17 user guide into a new, **smaller** one
    as outlined by the sitemap.

#.  Add special CSS to trac.e.org so certiain elements from the content float
    next to eachother like the brochure pages in the main wiki pages, only
    that, no other design update for any other page.

#.  **Announce the wiki restructurization and explain the general idea.**


Stage 3: Design of External sites and Doxygen
---------------------------------------------

#.  http://trac.enlightenment.org/

    #.  Remove a lot of crap hanging around the interface (markup).
    #.  Implement the new design matching the brochure pages..

#.  http://exchange.enlightenment.org/

    #.  Implement the new design matching the brochure pages.
    #.  Ask the developer to implement sort by rating (before the &limit=n
        kicks in) in the feed generator. I tried but the amount
        of code overwelmed me. I will have to spend time learning sympony
        to do this and I don't have it right now.

#.  Subdomains that will be removed or redirected.

    #.  http://svn.enlightenment.org/ Move the content to a wiki page and
        redirect the visitors there.
    #.  http://packages.enlightenment.org/ Move the content to a wiki page and
        redirect the visitors there.
    #.  http://docs.enlightenment.org/ Should redirect visitors to a wiki page
        that will now be used as index for documentation instead of the docs
        page as it does now.
    #.  http://bugs.enlightenment.org/ Redirect to trac bug reporting page.
    #.  http://bugzilla.enlightenment.org/ Redirect to trac bug reporting page.
    #.  http://trac.enlightenment.org/ should automatically redirect to `/e`
        right now it displays a very unproffesional error page.
    #.  http://wiki.enlightenment.org/ Remove it already, all the non traspased
        content at this point will be lost, most of which was written by me :(
        ...but screw it.

#.  Subdomains or external pages that will be redesigned at this point.

    #.  http://www.enlightenment.org/donate/ will be added to the
        brochure page system and redesigned so it can be translated in the
        future.

#.  Redesign doxygen for the core libraries and any other generated content
    that I don't know off to match the new style of the site.

#.  **Announce something?**

Notes about stage 3
....................

*   Subdomains and external pages that will be redesigned during **Stage 6**

    *   http://planet.enlightenment.org/
    *   http://l10n.enlightenment.org/
    *   http://git.enlightenment.org/

*   Subdomains and external pages I don't now what the hell I should do.

    *   http://fr.enlightenment.org/ looks like a cool community page to be
        linked from the community brochure page, should I ask to redesign it?
    *   http://forum.enlightenment.org/ Seems to be ignored by developers yet
        used by the users. Should I redesign it and include it in the
        community brochure page or remove it?
    *   http://download.enlightenment.org/ What is this for? It should it be
        redesigned, ignored or removed?. I can make a simple redesign of the
        files apache uses for the directory listing if needed.


Stage 4: Polish the brochures
-----------------------------

#.  Cleanup code from index.php, remove unnecessary handlers. Use a
    controller folder?

#.  Add the new sorted by rating + limit to the exchange theme getter.

#.  Improve news system

    #.  Add missing date filters for the news system.
    #.  Match formats of the dates displayed for planet articles and local
        articles.

#.  Profile the code execution. How much is time is really lifiting content
    from planet and exchange? How much time does configure and before
    consume?. etc

#.  Make the cache_refresh system a daemon and remove the call from before();

#.  Add some spanish content

    #.  pages/español/messages.inc.php
    #.  pages/español/home.html.php
    #.  news/español/one-of-the-latter-news-file

    #.  Test the localization (I had already tested it while I developed it by
        adding pages that consisted on random function calls, useless menus
        and "I'm Jennifer Lopez, I like tacos and burritos" as content this is
        simply an "official testing").

#.  Search a way for the content writers and translators to tidy-up the
    `.html.php` files for indentation and wrapping without HTMLtidy, which
    fucks up the php tags. Shouldn't be that hard, actually, as I don't intend
    to validate the markup in any way, simply indent tags and wrap text.

#.  **The translation fest can start now**

Stage 5: **Polish the external sites**
--------------------------------------

#.  http://trac.enlightenment.org/

    #.  Determine an standard for translated wiki articles in trac.

    #.  *whisful thinking* I can program a module to create a web based
        front-end to the mailing list for registered users that reesmebles a
        forum with a trusted user system to prevent spam.

#.  http://exchange.enlightenment.org/

    #.  Some sort of forum or other community feature besides comments.
    #.  Merge login system with trac's.
    #.  Automatic, server-side screenshot generation.

#.  Redesign the rest of the external pages as outlined in **Stage: 4**.

#.  **Now we are a complete and consistent website and community that rivals not
    only other open source projects but corporations like redhat, etc.**


