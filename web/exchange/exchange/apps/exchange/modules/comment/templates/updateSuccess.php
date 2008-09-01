<?php slot('title') ?>Add Comment<?php end_slot() ?>
<form method="post" action="/comment/update" >
<table>
	<tbody>
<tr>
	<td colspan="2"><h1>Add Comment</h1></td>
</tr>
	<tr>
	    <td>
	    	<?php echo $form['comment'] ?>
	    </td>
	</tr>
	<tr>
	    <td>
	    	<?php echo $form['application_id'] ?><?php echo $form['module_id'] ?><?php echo $form['theme_id'] ?>
	    	<input type="submit" value="Add Comment" />
	    </td>
	</tr>
	</tbody>
</table>
</form>