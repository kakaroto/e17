<?php slot('title') ?>Forgot Password<?php end_slot() ?>
<form method="post" action="/user/forgot">
<table>
<tbody>
<tr>
	<td colspan="2"><h1>Forgot Password</h1></td>
</tr>
<?php echo $form ?>
<tr>
	<th></th>
	<td><input type="submit" value="Submit" /></td>
</tr>
<tr>
	<th></th>
	<td>
	A new password will be emailed to you.
	</td>
</tr>
<tr>
	<td colspan="2">
	<?php echo link_to('Login', '/user/login') ?> | <?php echo link_to('Register', '/user/update') ?>
	</td>
</tr>
</tbody>
</table>
</form>