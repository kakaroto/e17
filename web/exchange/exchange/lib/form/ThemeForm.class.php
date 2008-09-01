<?php

class ThemeForm extends sfForm
{
	public function configure()
	{
		$this->setWidgets(array(
			'id'         => new sfWidgetFormInputHidden(),
			'name'        => new sfWidgetFormInput(),
			'description' => new sfWidgetFormTextarea(array(), array('cols' => 48,  'rows' => 5)),
			'file'        => new sfWidgetFormInputFile(),
			'screenshot'  => new sfWidgetFormInputFile(),
		));
		$this->widgetSchema->setLabels(array(
			'name'        => 'Name',
			'description' => 'Description',
			'file'        => 'Theme',
			'screenshot'  => 'Screenshot'
		));
		$this->widgetSchema->setHelps(array(
			'description' => '<a href="/home/wikiSyntax" target=_blank>Syntax</a>',
			'file' => 'Please select an edje file. Consider <a href="http://wiki.enlightenment.org/index.php/Edje_theme_labelling" traget=_blank>embedding</a> information in your themes.'
		));
		$this->widgetSchema->setNameFormat('theme[%s]');
		$formatter = new exchangeWidgetFormSchemaFormatter($this->getWidgetSchema());
		$this->getWidgetSchema()->addFormFormatter('exchange', $formatter);
		$this->getWidgetSchema()->setFormFormatterName('exchange');
	    
		if ($this->defaults['id']) {
			$fileValidator = new sfValidatorFile(array('required' => false));
			$screenshotValidator = new sfValidatorFile(array('required' => false, 'mime_types' => array('image/jpeg', 'image/png', 'image/x-png', 'image/pjpeg'),
														'max_size' => '5242880'),
												array('max_size' => 'Maximum screenshot file size: 5 MB',
														'mime_types' => 'Only PNG and JPEG images are allowed',
														'required'   => 'Screenshot is required'));
		} else {
			$fileValidator = new sfValidatorFile(array(), array('required'   => 'Theme file is required'));
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
			'file' => $fileValidator,
			'screenshot' => $screenshotValidator,
			'id'   => new sfValidatorPass(),
		)));
		$this->validatorSchema->setPostValidator(new sfValidatorDoctrineUnique(array('model' => 'Theme', 'column' => 'name', 'primary_key' => 'id'),
														array('invalid' => 'This Name is already in use')
													));
	}
}