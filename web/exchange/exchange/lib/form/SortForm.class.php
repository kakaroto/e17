<?php

class SortForm extends sfForm
{
	protected static $sort_options = array('created' => 'Created Date',
											'updated' => 'Last Updated',
											'alphabetical' => 'Name',
											'owner' => 'User\'s Name',
											'downloads' => 'Downloads',
											'rating' => 'Rating');
	protected static $order_options = array('ASC' => 'Ascending',
									 		'DESC' => 'Descending');
	protected static $limit_options = array(5 => '5', 10 => '10', 20 => '20', 50 => '50');
	public function configure()
	{
		$this->setWidgets(array(
			'sort'    => new sfWidgetFormSelect(array('choices' => self::$sort_options), array('onchange' => 'this.form.submit();')),
			'order'   => new sfWidgetFormSelect(array('choices' => self::$order_options), array('onchange' => 'this.form.submit();')),
			'limit'    => new sfWidgetFormSelect(array('choices' => self::$limit_options), array('onchange' => 'this.form.submit();')),
		));
	}
}
