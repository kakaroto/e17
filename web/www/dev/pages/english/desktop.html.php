<h2>The Enlightenment Desktop Shell</h2>

<div id="introduction">

    <div class="introduction">
        <p>Flagship of the enlightenment project features a lightweight yet
         visually stunning window manager that provides a most of the amenities
         of full blown desktop enviroments while remaining lean enough to run
        on older hardware and embedded devices with little modifications.
    </div>

    <div class="ad">
       <p style="color:red"> Screnshot that links to the full image this time</p>
    </div>

    <div class="features">
        <h4>Standards compilant</h4>
        <p>
        Talk about platform independence of the base libraries due to their
        programming therefore making E17 easy to port to any system supporting
        X.
        </p>
        <p>
        Talk about the freedesktop icons and menus. Talk about hal and hardware
        autodetection and automounting
        </p>

        <h4>Easy on resources</h4>
        <p>
        Talk about the emphasis on performance put into the base libraries since
        their inception.
        </p>
        <p>
        Talk about modularity that even alows unloading the configuration dialogs
        and how this allows it to work on small devices like phones with LINKTO:illumine
        </p>
    </div>

    <div class="install">
        <h4>Install the Desktop Shell</h4>
        <p>Some short introductory text</p>
        <ul>
            <li><a href="http://trac.enlightenment.org/e/wiki/EnlightenmentDesktopShell#Distributions">From Package Managers</a></li>
            <li><a href="http://trac.enlightenment.org/e/wiki/EnlightenmentDesktopShell#Snapshots">From Snapshots</a></li>
            <li><a href="http://trac.enlightenment.org/e/wiki/EnlightenmentDesktopShell#SVN">From SVN</a></li>
        </ul>
    </div>

</div>

<div id="community">
    <h3>Community</h3>

    <div class="invitation">
    <p>Invitatory text for the community</p>
    </div>

    <div class="wiki">
        <h4>Participating in the Wiki</h4>
        <!-- Add brief registration instructions for trac or link to it -->
        <!-- Add brief ettiquete mention or link to it -->
        <h5>Pages of interest</h5>
        <ul>
            <li><a href="http://trac.enlightenment.org/e/wiki">Main Page</a></li>
            <li><a href="http://trac.enlightenment.org/e/wiki/Users">Users Page</a></li>
            <li><a href="http://trac.enlightenment.org/e/wiki/EnlightenmentDesktopShell">The Desktop Shell Page</a></li>
        <!-- Add links to the main page, the main user page and the desktop shell page -->
    </div>

    <div class="irc">
        <h4>IRC</h4>
        <p>Get help right away using our <a href="http://en.wikipedia.org/wiki/Internet_Relay_Chat">IRC</a>
         channels in <a href="http://freenode.net/">FreeNode</a>. To participate simply download a <a href="http://en.wikipedia.org/wiki/Comparison_of_Internet_Relay_Chat_clients">IRC client</a>,
        connect to one of the <a href="http://freenode.net/irc_servers.shtml">FreeNode Servers</a>
        and <code>/join #e</code>.</p>
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


    <div class="themes">
        <h4>Themes</h4>
        <p>Small intro to installing themes</p>

        <div class="gallery">
        <?php
            foreach ( $themes as $i => $id )
                echo "<a href='http://exchange.enlightenment.org/theme/show/$id'>
                      <img id='theme-shot-$i' class='theme-shot' src=
                      'http://exchange.enlightenment.org/files/theme/$id/smallthumb.png'
                       alt='Theme Screenshot - $i'></a>";
        ?>
        </div>

        <p class="button themes"><a href="http://exchange.enlightenment.org/themeGroup/show/1674">All Themes</a></p>
    </div>

    <div class="modules">
        <h4>Modules</h4>
        <p>Small intro to installing modules</p>

        <div class="gallery">
        <?php $i = 0; foreach ( $modules as $id => $name ): ?>
            <div <?php echo "class='module' id='module-i'" ?> >
                 <p>
                    <?php echo "<a href='http://exchange.enlightenment.org/module/show/$id'>" ?>
                    <?php echo $name ?></a>
                 </p>
                <?php echo "<img class='module-shot' alt='Module $i' src='http://exchange.enlightenment.org/files/module/$id/smallthumb.png'>" ?>
            </div>
        <?php $i++ ?>
        <?php endforeach ?>
        </div>

        <p class="button modules"><a href="http://exchange.enlightenment.org/module/index/application_id/4">
        All Modules</a></p>
    </div>
</div>

<div id="contribute">
    <h3>Contribute</h4>

    <div class="bugs">
        <h4>Reporting Bugs</h4>
<p>If you happen to come across a bug, missing feature or lack of
            documentation feel free to report it in the
            <a href="http://trac.enlightenment.org/e/report">Enlightenment
            Trac</a>.</p>
     <p>If you feel even more adventurous why not giving an extra hand and providing
        the call stack <a href="http://trac.enlightenment.org/e/wiki/Debugging">using a debugger</a>.
    </div>

    <div class="translating">
        <h4>Translating</h4>
        <p>links to translating in the wiki</p>
        <p>Links to the translating subdomain if its exists</p>

        <h4>Translating the website</h4>
        <p>Briefly explain the method with links to the wik to explain
            fetching from SVN and to the htmlized readme files</p>
    </div>
</div>