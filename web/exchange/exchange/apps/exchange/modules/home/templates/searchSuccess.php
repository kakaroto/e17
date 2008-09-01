<?php slot('title') ?>Search Results: <?php echo $term ?><?php end_slot() ?>
<div class="read">
<h1>Search Results: <?php echo $term ?></h1>
<h3>Themes</h3>
<?php include_component('theme', 'table', array('search' => $term)) ?>
</div>
<h3>Applications</h3>
<?php include_component('application', 'table', array('search' => $term)) ?>
</div>
<h3>Modules</h3>
<?php include_component('module', 'table', array('search' => $term)) ?>
</div>
<h3>Theme Groups</h3>
<?php include_component('themeGroup', 'table', array('search' => $term)) ?>
</div>
<h3>Users</h3>
<?php include_component('user', 'table', array('search' => $term)) ?>
</div>