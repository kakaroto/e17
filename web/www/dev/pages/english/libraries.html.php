<div id="introduction">
    <h2>
        <span>The Enlightenment Foundation Libraries</span>
    </h2>
    <h3>
        <span>Create lean and beautiful software</span>
    </h3>
    <div class="information">
        <p>
            Set forth in 1998 as the <strong>window manager</strong> that
            brought advanced graphics to the then bleak word of Linux GUIs, Enlightenment
            would eventually became a collection of supporting development libraries.
        </p>
<p>
            Developers' interest in <strong>embedded development</strong> has
            steadly increased since 2002, making these libraries available to
            resource-constrained devices and exotic architectures. Sealing
            their independence from the role of Window Manager support libraries.
        </p>
        <p>
            Today Enlightenment libraries already power millions of systems, from
            mobile phones to set top boxes, desktops, laptops, game systems
            and more. It is only now being recognized for its forward-thinking
            approaches, as products and designers want to do more than the
            boring functional user experiences of the past. This is where the
            <abbr title="Enlightenment Foundation Libraries">EFL</abbr>
            excels.
        </p>
    </div>
    <div class="promotion">
        <!-- I would like to make this list dynamic -->
        <ul>
            <li>
                <p>
                    French ISP <a href="http://www.free.fr">Free.fr</a> is
                    currently distributing millons of <a href=
                    "http://www.free.fr/adsl/pages/television/multimedia.html">
                    Freebox HD</a> set-top boxes. Shippend with the
                    <abbr title=
                    "Enlightenment Foundation Libraries">EFL</abbr>-based
                    <a href="http://code.google.com/p/freebox-elixir/">Elixr
                    SDK</a> preinstaled.
                </p>
            </li>
            <li>
                <p>
                    Thousands of <a href=
                    "http://wiki.openmoko.org/wiki/Neo_FreeRunner">Openmoko
                    Freerunner</a> phones have been sold which included the
                    <abbr title=
                    "Enlightenment Foundation Libraries">EFL</abbr> as part of
                    the <a href=
                    "http://wiki.openmoko.org/wiki/Openmoko_developer_guide">SDK</a>
                    and the Enlightement DR0.17 desktop adjusted to mobile
                    usage through the <a href=
                    "http://wiki.openmoko.org/wiki/Illume">illumine
                    module</a>.
                </p>
            </li>
            <li>
                <p>
                    <a href="http://www.fixstars.com/en/products/ydl/">Yellow
                    Dog Linux</a>, a distrubtion ported to the PlayStation 3
                    under contract by Sony, ships with Enlightenment DR0.17 as
                    the <a href=
                    "http://www.fixstars.com/en/products/ydl/e17.html">default
                    destktop</a>.
                </p>
            </li>
        </ul>
    </div>
</div>
<div id="features">
    <div class="platforms">
        <h3>
            Multi-Platform
        </h3>
        <p>
            While Linux is the primary platform for historical reasons, as the
            number of developers grew and diversified support for additional
            platforms has steadly improved. Starting from the more familiar
            BSDs and Solaris to Mac OS X, Windows (XP, Vista, 7 and CE) and
            others.
        </p>
        <p>
            Compatibility will vary, but most of core EFL support all Linuxes,
            BSD's, Solaris and other UNIX-like OS's. Mac support should work
            mostly thanks to the X11 support in OS X, and Windows support
            exists for most of the core libraries.
        </p>
        <p class="more">
            <a href=
            "http://trac.enlightenment.org/e/wiki/EFL#OperatingSystemSupport">platform
            support status</a>
        </p>
    </div>
    <div class="architectures">
        <h3>
            Extensive Device Support
        </h3>
        <p>
            We have tested on x86-32, x86-64, Atom, Power-PC, ARM (ARM9,
            ARM11, Cortex-A8 and more), MIPS, Sparc, and other processor
            architectures.
        </p>
        <p>
            While 16MB of RAM is the suggested minimum for a Linux + EFL
            environment, you may be able to get by on 8MB for some uses and
            upwards to 64MB depending on the weight of the graphics used.
        </p>
        <p>
            As little as a 200Mhz ARM core will provide sufficient processing
            power for most graphical effects.
        </p>
        <p class="more">
            <a href=
            "http://trac.enlightenment.org/e/wiki/EFL#ProcessorArchitectureSupport">architecture
            support status</a>
        </p>
    </div>
    <div class="interfaces">
        <h3>
            Adaptive Interfaces
        </h3>
        <p>
            Screens from even less than QVGA (320x240 or 240x320) all the way
            up to and beyond full-HD (1920x1080) are covered by the EFL.
        </p>
        <p>
            User interfaces scale to almost any sane resolution, as well as
            adapt to differing input device resolutions, from mouse and stylus
            to fat fingers.
        </p>
        <p>
            It can draw displays from e-paper through 8-bit paletted displays,
            16bit beautifully dithered ones all the way to full 24/32bit OLED
            beauties.
        </p>
        <p class="more">
            <a href=
            "http://trac.enlightenment.org/e/wiki/EFL#TestedDevices">tested
            devices</a>
        </p>
    </div>
    <div class="graphics">
        <h3>
            Rich and Vibrant Designs
        </h3>
        <p>
            Enlightenment is built by designers and programmers who want
            others to be able to do more with less by making the <a class=
            "local" href="#evas">canvas</a> a central part of the interface
            instead of another widget.
        </p>
        <p>
            Interfaces can be built directly on the canvas by the designer
            using a <a class="local" href="#edje">simple declarative
            language</a> either by hand or through <a class="local" href=
            "#tools">tools</a>.
        </p>
        <p>
            This <a class="local" href="#edje">language</a> allows the artist to
             mix image files and native elements intelligently using features
             like smart scaling based on image border detection.
        </p>
        <p class="more">
            <a class="local" href="#graphics">graphics system architecture</a>
        </p>
    </div>
</div>