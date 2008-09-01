<?php

class CommentForm extends sfForm
{
	public function configure()
	{
		$this->setWidgets(array(
			'comment' => new sfWidgetFormTextarea(array(), array('cols' => 48,  'rows' => 5)),
			'application_id' => new sfWidgetFormInputHidden(),
			'module_id' => new sfWidgetFormInputHidden(),
			'theme_id' => new sfWidgetFormInputHidden(),
		));
		$this->widgetSchema->setNameFormat('comment[%s]');
	
		$formatter = new exchangeWidgetFormSchemaFormatter($this->getWidgetSchema());
		$this->getWidgetSchema()->addFormFormatter('exchange', $formatter);
		$this->getWidgetSchema()->setFormFormatterName('exchange');
		
		$this->setValidatorSchema(new sfValidatorSchema(array(
			'comment'   => new sfValidatorString(array('required' => true, 'min_length' => 3)),
			'application_id'  => new sfValidatorPass(),
			'module_id'  => new sfValidatorPass(),
			'theme_id'  => new sfValidatorPass(),
		)));
	}
}