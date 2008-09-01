<?php slot('title') ?>Wiki Syntax<?php end_slot() ?>
<h1>Wiki Syntax</h1>
<table class="table">
	<tbody>
	<?php $count = 0; ?>
	<?php foreach ($wikiLines as $wikiLine): ?>
		<?php $count++ ?>
		<tr class="<?php if($count%2): ?>even<?php else: ?>odd<?php endif; ?>">
			<td><pre><?php echo htmlentities($wikiLine) ?></pre></td>
			<td><?php echo $wpwiki->transform($wikiLine) ?></td>
		</tr>
	<?php endforeach; ?>
	</tbody>
</table>