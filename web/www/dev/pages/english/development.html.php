<h2>Software Development with the Enlightenment Foundation Libraries</h2>

<div id="introduction">
    <div class="introduction">
        <p>A welcome text that resumes all the features outlined below</p>
    </div>

    <div class="ad">
        <p style="color:red">Basic stack diagram of the development libraries</p>
    </div>
</div>

<div id="features">
    <h3>Features</h3>

    <div class="platforms">
        <h4>Platform Support</h4>

        <p>Enlightenment and EFL support several platforms, though Linux is
        the primary platform of choice for our developers, some make efforts
        to make things work on FreeBSD and other BSD's, Solaris, MacOS X,
        Windows (XP, Vista, 7 etc.), Windows CE and more. Compatibility will
        vary, but most of core EFL support all Linuxes, BSD's, Solaris and
        other UNIX-like OS's. Mac support should work mostly thanks to the X11
        support in OS X, and Windows support exists for most of the core
        libraries (XP, Vista, 7, CE).</p>

        <p class="button platforms"><a href=
        "http://trac.enlightenment.org/e/wiki/Platforms">All platforms</a></p>
    </div>

    <div class="effectiveness">
        <h4>Proven effectiveness</h4>

        <p>Enlightenment libraries already power millions of systems, from
        mobile phones to set top boxes, desktops, laptops, game systems and
        more. It is only now being recognised for its forward-thinking
        approaches, as products and designers want to do more than the boring
        functional user experiences of the past. This is where EFL excels.</p>

        <p><a href="http://www.free.fr">Free.fr</a> is shipping millions of
        set top boxes in France, powered by EFL. The <a href=
        "http://wiki.openmoko.org/wiki/Neo_FreeRunner">Openmoko Freerunner</a>
        sold thousands of devices with EFL on them. <a href=
        "http://www.yellowdoglinux.com/">Yellow Dog Linux</a> for the Sony PS3
        ships with Enlightenment as the default. EFL has been used on
        printers, netbooks and more.</p>

        <!-- YES its the same link -->
        <p class="button devices"><a href=
        "http://trac.enlightenment.org/e/wiki/Platforms">All devices</a></p>
    </div>

    <div class="architectures">
        <h4>Architecture Support</h4>

        <p>
         Enlightenment and EFL use desktop Linux systems as a primary method of
         development because it is fast and simple to do so, but all of it is
         written with the express goal in mind of also working on devices from
         Mobile Phones, to Televisions, Netbooks and more.
        </p>

        <p>
         We have run and tested on x86-32, x86-64, Atom, Power-PC, ARM (ARM9, ARM11,
         Cortex-A8 and more), MIPS, Sparc, and many other architectures. The
         suggested minimum RAM required for a full Linux system + EFL application
         is 16MB, but you may be able to get by on 8MB. For full functionality
         64MB or more is suggested. As little as a 200Mhz ARM core will provide
         sufficient processing power (depending on needs).
        </p>

        <p>
         Screens from even less than QVGA (320x240 or 240x320) screens all the
         way up to and beyond full-HD (1920x1080) are covered by EFL. It has the
         ability to scale user interfaces to almost any sane resolution, as well
         as adapt to differing input device resolutions, from mouse and stylus
         to fat fingers. It can draw displays from e-paper through 8-bit
         paletted displays, 16bit beautifully dithered ones all the way to full
         24/32bit OLED beauties.
        </p>
    </div>

    <div class="graphics">
        <h4>Graphics</h4>

       <p>
         Enlightenment is built by designers and programmers who want others to
         be able to do more with less. Some of Enlightenment's libraries do not
         do anything with graphics at all, but it is the ones that do that are
         the shining stars of the Enlightenment world.
        </p>

        <p>
         <b>Evas</b> is the canvas layer. It is not a drawing library. It is not
         like OpenGL, Cairo, XRender, GDI, DirectFB etc. It is a scene graph
         library that retains state of all objects in it. They are created then
         manipulated until they are no longer needed, at which point they are
         deleted. This allows the programmer to work in terms that a designer
         thinks of. It is a direct mapping, as opposed to having to convert the
         concepts into drawing commands in the right order, calculate minimum
         drawing calls needed to get the job done etc.
        </p>

        <p>
         Evas also handles abstracting the rendering mechanism. With zero changes
         the same application can move from software to OpenGL rendering, as they
         all use an abstracted scene graph to describe the world (canvas) to Evas.
         Evas supports multiple targets, but the most useful are the high-speed
         software rendering engines and OpenGL (as well as OpenGL-ES 2.0).
        </p>

        <p>
         Evas not only does quality rendering and compositing, but also can
         scale, rotate and fully 3D transform objects, allowing for sought-after
         3D effects in your interfaces. It supplies these abilities in both
         software and OpenGL rendering, so you are never caught with unexpected
         loss of features. The software rendering is even fast enough to provide
         the 3D without any acceleration on devices for simple uses.
        </p>

        <p>
         <b>Edje</b> is a meta-object design library that is somewhere between
         Flash, PSD, SVG and HTML+CSS. It separates design out from code and into
         a dynamically loaded data file. This file is compressed and loaded very
         quickly, along with being cached and shared betweeen instances.
        </p>


        <p>
         This allows design to be provided at runtime by different design (EDJ)
         files, leaving the programmer to worry about overall application
         implementation and coarse grained UI as opposed to needing to worry about
         all the little details that the artists may vary even until the day
         before shipping the product.
         </p>
    </div>

</div>

<div id="libraries">
    <h3>Introduction to the Enlightenment Foundation Libraries</h3>

    <p>Short introduction to the Enlightenment Foundation Libraries (EFL) that
        I wrote somewhere, either the wiki or the book, I just have to find it
        out and paste it here.</p>

    <div class="outline">
        <h4>Libraries Outline</h4>

        <p>Most applications will only need this and can even ignore some of
           them like Efreet and Edbus for the linux desktop or devices that use
           them.</p>

        <ul>
            <li><a href="http://trac.enlightenment.org/e/wiki/Evas">Evas</a>
            - One line Intro</li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Eina">Eina</a>
            - One line Intro</li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Edje">Edje</a>
            - One line Intro</li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Eet">Eet</a>
            - One line Intro</li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Ecore">Ecore</a>
            - One line Intro</li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Efreet">Efreet</a>
            - One line Intro</li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Edbus">Edbus</a>
            - One line Intro</li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Elementary">
            Elementary</a> - One line Intro</li>
        </ul>
        <p class="button libraries"><a href="http://trac.enlightenment.org/e/wiki/Developers">
            All the EFL</a></p>
    </div>

     <div class="ad">
        <p style="color:red">A more complete stack diagram that includes
        third party libraries like lua, xlib and xcb and how they relate
        to eachother</p>
     </div>

    </div>

<div id="community">
    <h3>Community</h3>

    <div class="invitation">
        <p>Invitatory text for the developers, include link to wiki article
        explaining <a href="http://trac.enlightenment.org/e/wiki/Developers">SVN Access</a></p>
    </div>

    <div class="project-tracking">
        <h4>Project Tracking</h4>
        <p>Introduce trac and link the trac registration link.</p>
        <ul>
            <li><a href="http://trac.enlightenment.org/e/report">
                Report Bugs</a></li>
            <li><a href="http://trac.enlightenment.org/e/browser">
                Source Browser</a></li>
            <li><a href="http://trac.enlightenment.org/e/timeline?from=12%2F15%2F09&amp;daysback=15&amp;author=&amp;ticket=on&amp;milestone=on&amp;update=Update">
                Development Activity</a></li>
            <li><a href="http://trac.enlightenment.org/e/roadmap">
                Roadmap for all Components</a></li>
        </ul>
    </div>

    <div class="wiki">
        <h4>Wiki pages of interest</h4>
        <p>Introductory text to the wiki, explain that uses the same account as
           the bug tracker</p>
        <ul>
            <li><a href="http://trac.enlightenment.org/e/wiki">Main Page</a></li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Developers">
                Developer APIs and Manuals</a></li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Debugging">
                Debugging</a></li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Guidelines">
                Contribution Guidelines</a></li>
            <li><a href="http://trac.enlightenment.org/e/wiki/ECoding">
                Coding Style</a></li>
        </ul>
    </div>

    <div class="irc">
        <h4>IRC</h4>
        <p>To get in touch with other developers using the EFL simply
           log into our <a href="http://en.wikipedia.org/wiki/Internet_Relay_Chat">IRC</a>
         channels in <a href="http://freenode.net/">FreeNode</a>. To participate simply download a <a href="http://en.wikipedia.org/wiki/Comparison_of_Internet_Relay_Chat_clients">IRC client</a>,
        connect to one of the <a href="http://freenode.net/irc_servers.shtml">FreeNode Servers</a>
        and <code>/join #edevelop</code>.</p>
    </div>

    <div class="mailing-lists">
        <table cellspacing="0">
            <caption>
                Mailing Lists and archives
            </caption>
            <thead>
                <tr>
                    <th class="list">List Name</th>
                    <th class="archive">Archives</th>
                    <th class="description">Description</th>
                </tr>
            <tbody>
            <?php foreach ( $lists as $list): ?>
                <tr <?php tr_odd() ?> >
                    <td class="list"><a href="<?php echo $list['link'] ?>"><?php echo $list['name'] ?></a></td>
                    <td class="archive"><a href="<?php echo $list['archive link'] ?>"><?php echo $list['archive'] ?></a></td>
                    <td class="description"><?php echo message($list['name']) ?></td>
                </tr>
            <?php endforeach ?>
            </tbody>
        </table>
    </div>

</div>
