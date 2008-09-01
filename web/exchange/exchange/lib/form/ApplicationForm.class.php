<?php

class ApplicationForm extends sfForm
{
	public function configure()
	{
		$this->setWidgets(array(
			'id'          => new sfWidgetFormInputHidden(),
			'name'        => new sfWidgetFormInput(),
			'description' => new sfWidgetFormTextarea(array(), array('cols' => 48,  'rows' => 5)),
			'source_url'  => new sfWidgetFormInput(),
			'screenshot'  => new sfWidgetFormInputFile(),
		));
		$this->widgetSchema->setLabels(array(
			'name'        => 'Name',
			'description' => 'Description',
			'source_url'  => 'Source URL',
			'screenshot'  => 'Screenshot'
		));
		$this->widgetSchema->setHelps(array(
			'description' => '<a href="/home/wikiSyntax" target=_blank>Syntax</a>',
			'source_url' => 'If this application is in CVS, just enter the path to the CVS module',
		));
		$this->widgetSchema->setNameFormat('application[%s]');
		
		$formatter = new exchangeWidgetFormSchemaFormatter($this->getWidgetSchema());
		$this->getWidgetSchema()->addFormFormatter('exchange', $formatter);
		$this->getWidgetSchema()->setFormFormatterName('exchange');
	    
		if ($this->defaults['id']) {
			$screenshotValidator = new sfValidatorFile(array('required' => false, 'mime_types' => array('image/jpeg', 'image/png', 'image/x-png', 'image/pjpeg'),
														'max_size' => '5242880'),
												array('max_size' => 'Maximum screenshot file size: 5 MB',
														'mime_types' => 'Only PNG and JPEG images are allowed',
														'required'   => 'Screenshot is required'));
		} else {
			$screenshotValidator = new sfValidatorFile(array('mime_types' => array('image/jpeg', 'image/png', 'image/x-png', 'image/pjpeg'),
														'max_size' => '5242880'),
												array('max_size' => 'Maximum screenshot file size: 5 MB',
														'mime_types' => 'Only PNG and JPEG images are allowed',
														'required'   => 'Screenshot is required'));
		}
		$this->setValidatorSchema(new sfValidatorSchema(array(
			'name' => new sfValidatorAnd(array(
							new sfValidatorString(array('required' => true, 'min_length' => 3),
									array('min_length' => 'Name must be at least 3 characters long')
								)
							),
							array('required'   => true),
							array('required'   => 'Name is required')
						),
			'description' => new sfValidatorAnd(array(
							new sfValidatorString(array('required' => true, 'min_length' => 3),
									array('min_length' => 'Description must be at least 3 characters long')
								)
							),
							array('required'   => true),
							array('required'   => 'Description is required')
						),
			'source_url' => new sfValidatorAnd(array(
							new sfValidatorString(array('required' => true, 'min_length' => 3),
									array('min_length' => 'Source URL must be at least 3 characters long')
								)
							),
							array('required'   => true),
							array('required'   => 'Source URL is required')
						),
			'screenshot' => $screenshotValidator,
			'id'   => new sfValidatorPass(),
		)));
		$this->validatorSchema->setPostValidator(new sfValidatorDoctrineUnique(array('model' => 'Application', 'column' => 'name', 'primary_key' => 'id'),
														array('invalid' => 'This Name is already in use')
													));
	}
}