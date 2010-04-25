<h2>About the Enlightenment Project</h2>

<div class="introduction" id="e-project">
    <div class="text">
        <p>We are a group of people commited to the creation of programming
        libraries, tools and enviroments that facilitate the creation of
        applications with advanced graphical interfaces. Initally created to
        support development of the the next generation version of the
        Enlightenment Desktop Shell (E17) they outgrew that scope to become a
        portable and efficient foundation for embedded and desktop
        applications: The Enlightenment Foundation Libraries.</p>
        <p class="button learn-more"><a href="<?php url_page('desktop') ?>"><span>The Desktop Shell</span></a> | <a href="<?php url_page('libraires') ?>"><span>The Development Libraries</span></a></p>
    </div>

    <div class="ad">
        <p style="color: red">A pretty image about the diversity of deployment
        of the Enlightenment proyect that includes E17 and some other
        (preferably embedded) apps</p>
    </div>
</div>

<div id="project-history">
    <h3>The history of the Enlightenment Project</h3>

    <div class="first-column">
        <p>How did E start? As is often the case with open source projects,
        scratching a developer's itch. In this particular case it was Carsten
        (rasterman) Haitzler's. The year was 1996 and he felt dissatisfied
        with the current state of window managers, there was no reason for
        powerful workstation and a cheap Amiga to be indistinguishable from
        each other. Does function necessarily impairs form? Perhaps an
        environment that also felt "right" would make you more productive. He
        began hacking FVWM to experiment with this idea but architectural
        issues got in the way. By the end of the year it became clear that
        writing a window manager from scratch would be the safest path.</p>

        <blockquote>
            <p><a href=
            "http://web.archive.org/web/19981203013912/www.enlightenment.org/about.html">
            if you're like me and live on your PC, and CRAVE for your desktop
            to do the things YOU want it to do and LOOK how YOU want it to
            look, I don't know how you can go to the extreme more than with
            Enlightenment. By the time we've reached 1.0, you'll never be able
            to look at another window manager or GUI the same way.</a></p>
        </blockquote>

        <p>An ambitious goal statement for the website of a free software
        project at the beginnings of 1998. While the team had been working on
        its window manager only for about a year, the project's goals and
        design philosophy were already taking the shape we see today. The
        premise seemed simple enough: <q>"the user should maintain control of
        his desktop at all times"</q>. But making such capabilities available
        to the end-user was not immediately obvious 4 years before the now
        famous Firefox browser was released and extensions became commonplace
        for the average desktop user.</p>

        <blockquote>
            <p>Why should you have to change window managers every few weeks
            when you need some new feature? You should be able to embed the
            feature into it or be able to find someone else who can or has
            already done so. [...] Everything from simple backgrounds to the
            multiple border decoration styles surrounding the various windows
            on your desktop can be defined and modified until you are
            satisfied with its graphical layout and behavior.</p>
        </blockquote>

        <p>The seemingly overwhelming nature of such environment didn't
        discourage the developers who envisioned a <q>"highly customizable
        definition language"</q> to deal with it. By the time the project's
        first complete rewrite (Enlightenment DR0.14) was released, said
        language was customizable to the point it could be used successfully
        for both configuration and themeing. The syntax itself was
        customizable and made possible for themers to use their native
        languages as keywords. Background could become Hintergrund, Fond or
        Fondo depending on the designer's nationality. Beyond keywords the
        syntax was quite simple and complemented only by blocks delimited with
        {curly} {brackets}.</p>

        <p>At run-time, the window manager would load the selected
        configuration or theme file and parse it line by line into the
        necessary data structures. The aesthetic features of the theme were
        built from a combination of standard image files and rendering
        parameters specific to each file defined in the theme's configuration.
        Making possible advanced features such as identifying the areas of the
        image that should to be stretched as a border or corner without
        additional programming. To abstract the manipulation of these image
        files and X-Windows draw-ables, the first version of Imlib was
        created. An advanced graphic library capable of using either Xlib or
        GDK at the back-end.</p>

        <h4>Steady progress to DR0.16</h4>

        <blockquote>
            <p><a href=
            "http://web.archive.org/web/19990428135920/www.enlightenment.org/news.html">
            For those of you who partake of the virtues of the GNOME CVS
            repository, or of the latest Enlightenment snapshots, the filesize
            has jumped to an incredible whopping 5MB! The reason? A bundled
            assortment of TrueType fonts that you can use from within
            Enlightenment for spicing up those windows.</a></p>
        </blockquote>

        <p>By August 1998 CVS snapshots of DR0.15 were released regularly as
        tarballs weighting five outrageous megabytes. While the blame could be
        placed on the image files and TrueType fonts, in time, the source code
        did inevitably grow as a result of new features like eesh, a separate
        console application that exposed the window manager's IPC commands to
        the end user. Work on a front-end for the configuration files had also
        started since: <q><a href=
        "http://web.archive.org/web/19990826131641/www.linuxpower.org/display_item.phtml?id=69">
        "You shouldn't HAVE to ever touch the config file if you don't want
        to"</a></q>.</p>

        <p>The format of the theme configuration files had changed to a
        collection of C-like macros. These files were later parsed through a
        wrapper of the GCC preprocessor called epp, which was also a separate
        application distributed along the WM. The parsed result was compressed
        along with the images and fonts into a gzip file to be passed around
        using ".etheme" as the extension. This process sped up theme loading
        considerably and hinted at the system that would deal with theme files
        in the future.</p>

        <p>Even while DR0.15 was groomed for its March 1999 release, work on
        DR0.16 was already underway. The initial versions of DR0.16 maintained
        the code-base and file formats used by its predecessor. The focus had
        shifted to new features and easier better interfaces, after all,
        DR0.14 was expected to be the project's last rewrite. By September
        1999 development of new features for DR0.16 was halted, the new
        version would be released a month later presenting a large number of
        new features with only a modest increment of the code-base.</p>

        <h4>Envisioning DR0.17</h4>

        <blockquote>
            <p><a href=
            "http://web.archive.org/web/20000815082102/www.us.rasterman.com/imlib.html">
            My current project to make sure Enlightenment 0.17 is a hip and
            happening thing, is to make sure Imlib 2 gets written and works
            and does everything it needs to and I want it to.</a></p>
        </blockquote>

        <blockquote>
            <p>[Raster, what are your goals for imlib 2?] <a href=
            "http://web.archive.org/web/19990826131641/www.linuxpower.org/display_item.phtml?id=69">
            Layer-based graphics engine with image loading and rendering
            abilities, pixmaps and image sharing between clients (so server
            based), anti-aliased scaling, arbitrary mapping routines to allow
            arbitrary rotation and scaling of layers and images, alpha channel
            compositing, speedups here and there.. and more. Also possible
            threading of rendering routines for SMP machines (ie spawn 4
            rendering threads for a 4CPU machine to max out its processing
            power). This is a big project - ambitious but doable - it will
            take a long time to see it come to completion though.</a></p>
        </blockquote>

        <p>Raster's ambitions for DR0.17 eventually outgrew the features
        provided by the first version of Imlib. Now common graphical
        capabilities like alpha blending or anti-aliasing for shapes as well
        as text were quite new at the time. Keeping up with the renewal of
        image formats also required a new design that abstracted image loaders
        from the rest of the library. Working these features (among others)
        into the current version of Imlib would provide more complications
        than benefits in contrast to simply creating a new library. Imlib2 was
        born as a separate library to cover these features and could be used
        in conjunction with original Imlib.</p>

        <p>The year 2000 saw new libraries come to life to support new
        features and to modularize existing code in the window manager. As the
        intended feature list for DR0.17 became more ambitious the number and
        size of libraries escalated. The new ecosystem would inspire a second
        rewrite 3 years later.</p>
    </div>

    <div class="second-column">
        <p><span class="metatext">[cont'd]</span> This process was not really
        planned, the project simply shifted its focus from a Window Manager
        Project to a Software Development Libraries Project that used the
        window manager as a testing ground and inspiration for new interface
        ideas and library features.</p>

        <blockquote>
            <p><a href=
            "http://web.archive.org/web/20031205043216/enlightenment.org/pages/news.html">
            Imlib is still in heavy usage, Imlib2 has become a critical part
            of many projects. EVAS is going to be a significant force in the
            future, and will be as big a hit as Imlib. Not only that, our
            library collection includes a full widget library (EWL) based on
            EVAS, several convenience libs (Ecore, Eprog, EWD), libraries for
            text rendering in EVAS (Etox, Estyle), a packaging and
            distribution format (EET), a database (EDB), and a revolutionary
            themeing scheme (Ebits, Ebg). Not to mention tools for EDB
            (e_gtk_ed, Ebindings, etc), and Ebits/Ebg (Ebony, Etcher). We've
            got the file manager backend completed (EFSD). We've got a widget
            daemon (Ewidgetd), one of the nicest image viewers ever written
            (Entice), and two file managers (Essence, Evidence). And there is
            more than that! We're running out of words that start with
            E.</a></p>
        </blockquote>

        <p>In the following 8 years many libraries were created, rewritten and
        deprecated, each built around the original ideal of making
        customization and extension for both aesthetics and functionality
        accessible. To support such ideal a good number of convenience
        wrappers for the underlying system and emerging desktop standards were
        created. In the end, the majority of the libraries born during the
        three year period between 2000 and 2003 would be either deprecated or
        rewritten to the point they became unrecognizable. The most notable of
        them being Evas, <q>"a hardware-accelerated canvas API for
        X-Windows"</q>. Initially created for themeing the Enlightenment File
        Manager, Evas would eventually shape the architecture and aesthetics
        of the whole project to its current state.</p>

        <h4>Hitting the DR0.17 wall, Evas is born</h4>

        <blockquote>
            <p>Evas is a solution to a problem, that at the time had no other
            solution. The problem was that we needed a rendering abstraction
            layer for X11 that allows for alpha blending, anti-aliasing and
            image manipulation on a structured, rather than immediate-mode
            level. One that would also optimise the display of such structured
            objects and also allow for hardware on existing and future
            machines to accelerate this rendering without requiring the CPU to
            do all the work. <a href=
            "http://enlightenment.org/historical_files/pages/evas_programmers_guide-July-2001.pdf">Evas Programmers Guide -
            July 2001 [PDF]</a></p>
        </blockquote>

        <p>While in 2001 the graphical capabilities of both versions of the
        window manager, DR0.16 and DR0.17, were still way ahead of its
        counterparts in the open source world they were not issue-free. The
        performance in bottom line hardware was less-than-stellar when it came
        to rendering the most complex themes, which often featured large
        bitmaps and extravagantly shaped window borders. New methods to create
        better-performing yet complex themes were needed and Imlib2 simply did
        not cut it. A canvas library was planned that would complement Imlib2
        and provide the required advanced functionality at a faster speed.
        Evas became the way every graphic element would be displayed in DR0.17
        and was not only more advanced but required even less code to work its
        magic.</p>

        <blockquote>
            <p>[Where do you think the future lies for desktop Linux?]
            <a href="http://web.archive.org/web/20021216233956/www.linuxandmain.com/modules.php?name=News&amp;file=article&amp;sid=141">
            Not on the desktop. Not on the PC. Not on anything that resembles
            what you call the desktop. Windows has won. Face it. The market is
            not driven by a technically superior kernel, or an OS that avoids
            its crashes a few times a day. Users don't (mostly) care. They
            just reboot and get on with it. They want apps. If the apps they
            want and like aren't there, it's a lose-lose. Windows has the
            apps. Linux does not. Its life on the desktop is limited to nice
            areas (video production, though Mac is very strong and with a UNIX
            core now will probably end up ruling the roost). The only place
            you are likely to see Linux is the embedded space. Purpose-built
            devices to do a few things well. There is no encumbent app space
            to catch up with as a lot of the apps are custom written. It's
            still a mostly level playing field. This is where the strengths of
            Linux can help make it shine.</a></p>
        </blockquote>

        <p>By 2002 Raster had been porting his favorite libraries to various
        embedded devices while he worked for a small Sydney contracting firm.
        While this process boosted the libraries' performance, the window
        manager itself was pushed further away from the spotlight. The
        original branch of the project became pretty much leaderless, the
        number of libraries and legacy code continued to creep. Eventually the
        development of DR0.17 was grind down to a halt by the at least 12
        different libraries and utilities that competed to serve it, often
        duplicating features found in each other. At this point the inevitable
        process of merging and deprecating libraries and utilities began. It
        was decided that the development of DR0.17 would be halted until these
        issues were sorted out. Eventually the window manager itself wold be
        completely rewritten.</p>

        <h4>A new beginning for DR0.17</h4>

        <p>Evas became independent of Imlib2 and gained support for multiple
        rendering engines including Xlib, Frame-buffer and OpenGL. Since Evas
        was the base of DR0.17, the window manager became independent of
        Imlib2 as well. The previous themeing system known as Ebits which used
        Edb to store its data was replaced with the more capable Edje, a
        proper declarative language reminiscent of the DR0.14 days that used a
        simpler data serialization library known as Eet. Configuration storage
        was moved to Eet as well removing Edb from the dependency list.At this
        point many utilities that were built around Edb met their demise at
        this point as well.</p>

        <p>In parallel to this process DR0.16 underwent a massive move from
        Imlib to Imlib2. Since 2004 development of this branch had continued
        steadily under the direction of Kim Woelders and was not only kept up
        to date but gained new features. Moving to Imlib2 deprecated the
        original library to the point that the name "Imlib" became synonymous
        with Imlib2 instead.</p>

        <p>Other convenience libraries and utilities as Edb, Efsd and Ebony
        proved to be not convenient enough to warrant their maintenance and
        slowly deprecated. Some libraries that strayed too far from their
        window manager scope such as Etox shared the same fate. The majority
        of the remaining libraries were revised and merged into Ecore. This
        process continued well into 2007 until DR0.17 consolidated around Eet,
        Evas, Ecore, Embryo and Edje. In the following years, functionality
        duplicated by Evas and Ecore consolidated in Eina while the rise of
        two modern freedesktop.org standards, DBus and Desktop Menus, was
        addressed with two new libraries called E_Dbus and Efreet
        respectively.</p>

        <p>Today we find the code and architecture of DR0.17 to be radically
        different to those of DR0.14 yet the philosophy was consistently
        maintained. Many past statements are truer today than when they were
        made. While its advanced features put DR0.17 far above above the rest
        of the window managers, it remains light enough to be successfully
        used in embedded devices. Made possible through extreme modularity,
        this middle point between window manager and the desktop environment
        is the reason DR0.17 is referred to as a "desktop shell".</p>
    </div>
</div>


<div class="sponsors">
    <h3>Sponsors</h3>
    <p style="color: red">TODO: Add a dynamic sponsor listing along with link
    to a list of all sponsors of all time.</p>
</div>

<div id="people">
    <h3>People</h3>

    <table class="active" cellspacing="0">
        <caption>
            Active developers
        </caption>
        <thead>
            <tr>
                <th class="name">Name</th>
                <th class="manage">Managing/Contributing</th>
                <th class="irc">IRC Nick</th>
                <th class="location">Location</th>
                <th class="website">Website</th>
            </tr>
        </thead>
        <tbody>
        <?php foreach ( $developers as $login => $data ): ?>
             <tr <?php tr_odd() ?> >
                <td class="name"><?php td($data['name']) ?></a></td>
                <td class="manage"><?php td($data['managing']) ?></td>
                <td class="irc"><?php td($data['irc']) ?></td>
                <td class="location"><?php td($data['location']) ?></td>
                <td class="website"><?php if ( $data['website']): ?>
                                    <a href="<?php echo $data['website'] ?>"><?php echo str_replace(array('http://', 'www.'),'', $data['website']) ?></a>
                                    <?php else: ?>
                                    &nbsp;
                                    <?php endif ?></td>
            </tr>
        <?php endforeach ?>
        </tbody>
     </table>

    <table class="inactive" cellspacing="0">
        <caption>
            Inactive developers
        </caption>
        <thead>
            <tr>
                <th class="name">Name</th>
                <th class="manage">Managed / Contributed</th>
                <th class="irc">IRC Nick</th>
                <th class="website">Website</th>
            </tr>
        <tbody>
        <?php foreach ( $inactive_devs as $name => $data): ?>
            <tr <?php tr_odd() ?> >
                <td class="name"><?php td($name) ?></a></td>
                <td class="manage"><?php td($data['managed']) ?></td>
                <td class="irc"><?php td($data['irc']) ?></td>
                <td class="website"><?php if ( $data['website']): ?>
                                    <a href="<?php echo $data['website'] ?>"><?php echo str_replace(array('http://', 'www.'),'', $data['website']) ?></a>
                                    <?php else: ?>
                                    &nbsp;
                                    <?php endif ?></td>
            </tr>
        <?php endforeach ?>
        </tbody>
    </table>
</div>