<h2><span><span class="e">Enlightenment</span>, create lean and beautiful
software</span></h2>

<div id="developers">
    <h3><span>Developers empowered</span></h3>

    <ul>
        <li>Libraries for multiple pruposes with a consistent an intuitive
        API. From interface design to advanced data types and system
        interaction.</li>

        <li>The approach to interface development uncouples design and
        development at every level making your applications themeable without
        extra effort.</li>

        <li>Written in ¿ANSI C or C99? with an emphasis on portability and
        performance. Even the high-level language bidings are suitable for
        both desktop and embedded development.</li>
    </ul>

    <p class="button learn-more"><a href="<?php url_page('development') ?>"><span>learn
    more</span></a></p>
</div>

<div id="artists">
    <h3><span>Artists unleashed</span></h3>

    <ul>
        <li>Every level of interface design can be covered using a simple
        declarative language, Edje. Syntactically similar to CSS but cable of
        creating arbitrary elements at the designer's whim.</li>

        <li>Transitions can be defined and tirggered using Edje to improve
        usability or create aesthetically pleasing animations without touching
        a line of alanguagepplication code.</li>

        <li>Applications and utilities to aid designers are either already
        available or under development, from simple viewers to WYSIWYG
        editors.</li>
    </ul>

    <p class="button learn-more"><a href="<?php url_page('artists') ?>"><span>learn
    more</span></a></p>
</div>

<div id="featured-software">
    <h3><span>Featured Software</span></h3>

    <div class="screenshots">
        <a href="<?php url_page('desktop') ?>">
            <img id="main-theme-shot" class="theme-shot"
                 src="<?php url_image('main_theme_shot.png') ?>"
                 name="main-theme-shot">
        </a>
        <?php
            foreach ( $themes as $i => $id )
                echo "<a href='http://exchange.enlightenment.org/theme/show/$id'>
                      <img id='theme-shot-$i' class='theme-shot' src=
                      'http://exchange.enlightenment.org/files/theme/$id/smallthumb.png'
                       alt='Theme Screenshot - $i'></a>";
          ?>
    </div>

    <div class="ad">
        <p>Featuring the <span class="featured">E17 desktop shell</span>.
        Flagship application of the Enlightenment Project, features
        flexibility comparable with the most advanced window managers yet
        remains easy enough to be setup and customized by a novice user.
        Despite the emphasis put on making it run acceptably in legacy
        hardware its theme creation capabilities remain unmatched.</p>

        <p class="button get-e17"><a href="<?php url_page('desktop') ?>">get E17</a></p>
    </div>
</div>

<div id="news">
    <h3><span>Official News</span></h3>

    <?php foreach ( $articles as $id => $article): ?>
    <div class="summary">
        <h4>
            <a href="<?php echo url_for('/article/'.$id) ?>">
                <?php echo $article['title'] ?>
            </a>
        </h4>

        <p class="data">by
            <?php echo "<a href='mailto:{$article['mail']}'>" ?>
                <span class="author"><?php echo $article['author'] ?></span>
            </a> - <span class="date"><?php echo $article['date'] ?></span>
        </p>

        <p class="summary">
            <?php echo $article['summary']; ?>
        </p>
    </div>
    <?php endforeach; ?>

    <p class="button more"><a href="<?php echo url_page('news') ?>">more</a></p>
</div>

<div id="planet">
    <h3><span>Planet Enlightenment</span></h3>

    <?php foreach ( $planet as $article): ?>
    <div class="summary">
        <h4>
            <a href="<?php echo $article['link'] ?>">
                <?php echo $article['title'] ?>
            </a>
        </h4>

        <p class="data">by
            <span class="author"><?php echo $article['author'] ?></span> -
            <span class="date"><?php echo $article['date'] ?></span>
        </p>

        <p class="summary">
            <?php echo $article['summary']; ?>
        </p>
    </div>
    <?php endforeach; ?>

    <p class="button more"><a href="http://planet.enlightenment.org/">more</a></p>

</div>

