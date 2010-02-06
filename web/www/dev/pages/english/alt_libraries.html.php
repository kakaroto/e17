<h2>
    The Enlightenment Foundation Libraries
</h2>
<h3>
    Create lean and beautiful software
</h3>
<div id="introduction">
    <p>
        Born in the midst of the bleak world of Unix GUIs in 1998, the
        Enlightenment project was set to bring forth a Window Manager sporting
        advanced graphical capabilities to the system. As time progressed the
        interests of developers diversified to other platforms including the
        challenge of bringing such capabilities to resource constrained
        embedded devices. Today we can appreciate the result of this process
        in the <strong>Enlightenment Foundation Libraries</strong> intended to
        enable the development of software with rich interfaces writing less
        code, giving the artists more freedom, while still fitting in
        virtually any platform from small mobile devices to powerful
        multi-core desktops.
    </p>
</div>
<div class="feature" id="platform-support">
    <h4>
        Multi-Platform
    </h4>
    <p>
        Enlightenment and EFL support several platforms, though Linux is the
        primary platform of choice for our developers, some make efforts to
        make things work on FreeBSD and other BSD's, Solaris, MacOS X, Windows
        (XP, Vista, 7 etc.), Windows CE and more. Compatibility will vary, but
        most of core EFL support all Linuxes, BSD's, Solaris and other
        UNIX-like OS's. Mac support should work mostly thanks to the X11
        support in OS X, and Windows support exists for most of the core
        libraries (XP, Vista, 7, CE).
    </p>
</div>

<div class="feature" id="adaptive-graphics">
    <h4>
        Adaptive Graphics
    </h4><!--    <p>
        Enlightenment is built by designers and programmers who want others to
        be able to do more with less. Some of Enlightenment's libraries do not
        do anything with graphics at all, but it is the ones that do that are
        the shining stars of the Enlightenment world.
    </p>-->
    <p>
        Screens from even less than QVGA (320x240 or 240x320) all the way up
        to and beyond full-HD (1920x1080) are covered by EFL. It has the
        ability to scale user interfaces to almost any sane resolution, as
        well as adapt to differing input device resolutions, from mouse and
        stylus to fat fingers. It can draw displays from e-paper through 8-bit
        paletted displays, 16bit beautifully dithered ones all the way to full
        24/32bit OLED beauties.
    </p>
</div>
<div class="feature" id="effectiveness">
    <h4>
        Proven Effectiveness
    </h4>
    <p>
        Enlightenment libraries already power millions of systems, from mobile
        phones to set top boxes, desktops, laptops, game systems and more. It
        is only now being recognized for its forward-thinking approaches, as
        products and designers want to do more than the boring functional user
        experiences of the past. This is where EFL excels.
    </p>
    <p>
        <a href="http://www.free.fr">Free.fr</a> is shipping millions of set
        top boxes in France, powered by EFL. The <a href=
        "http://wiki.openmoko.org/wiki/Neo_FreeRunner">Openmoko Freerunner</a>
        sold thousands of devices with EFL on them. <a href=
        "http://www.yellowdoglinux.com/">Yellow Dog Linux</a> for the Sony PS3
        ships with Enlightenment as the default. EFL has been used on
        printers, netbooks and more.
    </p>
</div>
<div class="information" id="advanced-graphics">
    <h3>
        Advanced Graphic Capabilities
    </h3>
    <div class="for-developers">
        <h4>
            For Developers
        </h4>
             <p>
            Enlightenment is built by designers and programmers who want
            others to be able to do more with less. Some of Enlightenment's
            libraries do not do anything with graphics at all, but it is the
            ones that do that are the shining stars of the Enlightenment
            world.
        </p>
        <div class="evas">
            <p>
                <strong>Evas</strong> is the canvas layer. It is not a drawing
                library. It is not like OpenGL, Cairo, XRender, GDI, DirectFB
                etc. It is a scene graph library that retains state of all
                objects in it. They are created then manipulated until they
                are no longer needed, at which point they are deleted. This
                allows the programmer to work in terms that a designer thinks
                of. It is direct mapping, as opposed to having to convert the
                concepts into drawing commands in the right order, calculate
                minimum drawing calls needed to get the job done etc.
            </p>
            <p>
                Evas also handles abstracting the rendering mechanism. With
                zero changes the same application can move from software to
                OpenGL rendering, as they all use an abstracted scene graph to
                describe the world (canvas) to Evas. Evas supports multiple
                targets, but the most useful are the high-speed software
                rendering engines and OpenGL (as well as OpenGL-ES 2.0).
            </p>
            <p>
                Evas not only does quality rendering and compositing, but also
                can scale, rotate and fully 3D transform objects, allowing for
                sought-after 3D effects in your interfaces. It supplies these
                abilities in both software and OpenGL rendering, so you are
                never caught with unexpected loss of features. The software
                rendering is even fast enough to provide the 3D without any
                acceleration on devices for simple uses.
            </p>
        </div>
        <div class="edje">
            <p>
                <strong>Edje</strong> is a meta-object design library that is
                somewhere between Flash, PSD, SVG and HTML+CSS. It separates
                design out from code and into a dynamically loaded data file.
                This file is compressed and loaded very quickly, along with
                being cached and shared betweeen instances.
            </p>
            <p>
                This allows design to be provided at runtime by different
                design (EDJ) files, leaving the programmer to worry about
                overall application implementation and coarse grained UI as
                opposed to needing to worry about all the little details that
                the artists may vary even until the day before shipping the
                product.
            </p>
        </div>
    </div>
    <div class="for-artists">
        <h4>For Artists</h4>

            <p>
        Edje is a simple declarative language such as CSS, but much more
        powerful, is not tied to markup or programming. Edje can be used to
        dictate the design of the interface from the top down, to simply style
        a given element, or both.
    </p>

<div class="edje-feature" id="abstracted-structure">
    <h5>
        Abstracted structure
    </h5>
    <p>
        For an application the result of including an Edje files is obtaining
        a collection of objects. These objects, from the designer point of
        views are groups composition of other, more primitive, objects like
        images, out of reach of the developer except on special cases.
    </p>
</div>

<div class="edje-feature" id="relative-sizes-and-layouts">
    <h5>
        Relative sizes and layouts
    </h5>
    <p>
        In Edje every object created by the designer has a container. The
        relationship of the container and the object varies depending on the
        type of object created. Still, we know that the method to manipulate
        this container, and the behaviour of the contained object as a result,
        is always consistent.
    </p>
    <p>
        We use containers to manage the size and position of object. But the
        feature that puts Edje over every other system of its type is that the
        size and location of the container can be setup relatively to another
        container. As a result, we can create complex interfaces that scale
        seamessly in different screen sizes.
    </p>
</div>
<div class="edje-feature" id="rich-transitions-and-animations">
    <h5>
        Rich transitions and animations
    </h5>
    <p>
        Normally, complex interface transitions and animations are in the
        realm of programmers. Thanks to Edje's description system coupled with
        pre-programmed transition mechanisms it is possible to change every
        property of an object and turn this difference of values into an
        animation without a line of programming.
    </p>
    <p>
        The transition system makes possible to animate any kind of change
        from simple ahestetic changes like altering background colors to
        sizes, position, visibility in various ways and speeds at the
        designer's discretion.
    </p>
</div>
<div class="more">

        <div class="learn">

            <h4>Learn</h4>

        </div>

        <div class="tools">

            <h4>Tools</h4>

            <p>For those who don't want to write the Edje source files from scratch. The Enlightenment project is commited to provide the best graphical tools possible for potential designers, although some are under developement, a varied ecosystem of applications and utilities exist.</p>

            <ul class="simple">
                <li><em>[LinkToExchange: Edje Editor or Editje, only one of them]</em></li>

                <li><em>[LinkToExchange: Edje viewer]</em></li>
                <li><em>[LinkToExchange: The SVG to Edje script (if it still exists)]</em></li>
                <li>Visit the <a href="http://trac.enlightenment.org/e/wiki/Artists#Tools">artists wiki page</a> for a complete list of tools and resources.</li>
            </ul>


        </div>

    </div>


    </div>
</div>
<div id="libraries">

    <h3>Introduction to the <abbr title="Enlightenment Foundation Libraries">EFL</abbr></h3>

    <div class="core">

        <h4>Core Libraries</h4>

Enlightenment, the window manager is built on top of building blocks
    known as EFL (the Enlightenment Foundation Libraries). There are more
    than can be sensibly put into the simple block diagram above, but this
    covers the essentials.
   </p>

   <center>
    <img src="http://enlightenment.org/p/about/d/diagram-efl-simple-small.png" style="width: 240px; height: 360px; border: 0pt none;" alt="Simple E stack">   </center>

   <p>
    Pretty much any application written using Core EFL libraries will use one
    or more of these depending on its needs. It may only need the lower level
    ones or use all of them to the top of the stack. Each library fulfills a
    purpose, so it may be skipped if not needed.
   </p>

    </div>

    <div class="bindings">

        <h4>Language Bindings</h4>
   <p>
    Binding support exists for several languages such as:
   </p>

   <p>
    </p><ul>

     <li><a href="http://trac.enlightenment.org/e/wiki/Python">Python</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Javascript">Javascript</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Perl">Perl</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/C++">C++</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Ruby">Ruby</a></li>
    </ul>

   <p></p>

    </div>

    <div class="examples">

        <h4>Example Applications</h4>


   <p>
    There are other libraries and applications that build on core EFL and
    other systems too, to provide more functionality, examples and utility:
   </p>

   <p>
    </p><ul>
     <li><a href="http://trac.enlightenment.org/e/wiki/Emotion">Emotion</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Ethumb">Ethumb</a></li>

     <li><a href="http://trac.enlightenment.org/e/wiki/Exquisite">Exquisite</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Expedite">Expedite</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Epdf">Epdf</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Eve">Eve</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Exchange">Exchange</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Eweather">Eweather</a></li>

     <li><a href="http://trac.enlightenment.org/e/wiki/Shellementary">Shellementary</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Rage">Rage</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Evil">Evil</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Exalt">Exalt</a></li>
     <li><a href="http://trac.enlightenment.org/e/wiki/Esmart">Esmart</a></li>
    </ul>

   <p></p>

    </div>

</div>