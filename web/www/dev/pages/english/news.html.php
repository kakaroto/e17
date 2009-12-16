<div id="official">
<h2>Official News</h2>

    <?php foreach ( $oficial as $id => $article): ?>
    <div class="summary">
        <h3>
            <a href="<?php echo url_for("/article/$id") ?>">
                <?php echo $article['title'] ?>
            </a>
        </h3>

        <p class="data">by
            <a href="mailto:<?php echo $article['mail'] ?>">
            <span class="author"><?php echo $article['author'] ?></span></a> -
            <span class="date"><?php echo $article['date'] ?></span>
        </p>

        <p>
            <?php echo $article['summary']; ?>
        </p>
    </div>
    <?php endforeach; ?>

</div>

<div id="planet">
<h2>Planet News</h2>

    <?php foreach ( $planet as $article): ?>
    <div class="summary">
        <h4>
            <a href="<?php echo $article['link'] ?>">
                <?php echo $article['title'] ?>
            </a>
        </h4>

        <p class="data"> <small> by
            <span class="author"><?php echo $article['author'] ?></span> -
            <span class="date"><?php echo $article['date'] ?></span>
        </small> </p>

        <p class="summary"> <small>
            <?php echo $article['summary']; ?>
        </small> </p>
    </div>
    <?php endforeach; ?>

    <p class="button more"><a href="http://planet.enlightenment.org/">Go to planet E</a></p>
</div>