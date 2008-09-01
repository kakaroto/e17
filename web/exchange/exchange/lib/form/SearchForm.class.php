<?php

class SearchForm extends sfForm
{
	public function configure()
	{
		$this->setWidgets(array(
			'term'    => new sfWidgetFormInput(),
		));
	}
}
