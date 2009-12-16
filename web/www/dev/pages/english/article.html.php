 <div class="article">
    <h2><?php echo $article['title'] ?></h2>

    <p class="data">by <a href="mailto:<?php echo $article['mail'] ?>">
        <span class="author"><?php echo $article['author'] ?></span></a> -
        <span class="date"><?php echo $article['date'] ?></span>
    </p>

    <?php echo $article['content']; ?>

</div>
