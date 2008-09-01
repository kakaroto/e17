<?php slot('title') ?><?php if ($user->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> User<?php end_slot() ?>
<?php if ($user->getId()): ?>
<form method="post" action="/user/edit/<?php echo $user->getId() ?>">
<?php else: ?>
<form method="post" action="/user/create">
<?php endif; ?>
<table>
<tbody>
<tr>
	<td colspan="2"><h1><?php if ($user->getId()): ?>Edit<?php else : ?>Create<?php endif; ?> User</h1></td>
</tr>
<?php echo $form['name']->renderRow() ?>
<?php echo $form['password']->renderRow() ?>
<?php echo $form['password2']->renderRow() ?>
<?php echo $form['email']->renderRow() ?>
<?php if ($sf_user->hasCredential('admin')): ?>
<?php echo $form['role']->renderRow() ?>
<?php endif; ?>
<tr>
	<th></th>
    <td><?php echo $form->renderGlobalErrors() ?></td>
</tr>

<tr>
	<th></th>
	<td><?php echo $form['id'] ?><input type="submit" value="Save" /></td>
</tr>
</tbody>
</table>
</form>