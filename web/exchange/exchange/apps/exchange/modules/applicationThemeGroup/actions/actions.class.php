<?php

class applicationThemeGroupActions extends sfActions
{
	public function executeIndex($request)
	{
		
	}
	
	public function executeRemove()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			$this->applicationThemeGroup = $q->select('a.*')
							->from('ApplicationThemeGroup a')
							->where('id = ?', array($id))
							->execute()
							->getFirst();
			$this->forward404Unless($this->applicationThemeGroup);
			$this->applicationThemeGroup->delete();
		}
		$application_id = $this->getRequestParameter('application_id');
		$theme_group_id = $this->getRequestParameter('theme_group_id');
		if ($application_id && $theme_group_id) {
			$q = new Doctrine_Query();
			$this->applicationThemeGroup = $q->select('a.*')
							->from('ApplicationThemeGroup a')
							->where('application_id = ? and theme_group_id = ?', array($application_id, $theme_group_id))
							->execute()
							->getFirst();
			$this->forward404Unless($this->applicationThemeGroup);
			$this->applicationThemeGroup->delete();
		}
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeUpdate($request)
	{
		$this->prepareUpdate();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('applicationThemeGroup'));
			if ($this->form->isValid())
			{
				$values = $this->form->getValues();
				$applicationThemeGroup = ApplicationThemeGroup::update($this->form->getValues(), $this->getUser()->getUser());
				if ($applicationThemeGroup)
					$this->redirect('/application/read?id='.$applicationThemeGroup->getApplicationId());
			}
			return $this->redirect($this->getRequest()->getReferer()); 
		}
	}
	
	public function prepareUpdate()
	{
		$this->applicationThemeGroup = new ApplicationThemeGroup();
		$this->application_id = $this->getRequestParameter('application_id');
		$this->theme_group_id = $this->getRequestParameter('theme_group_id');
		$this->form = new ApplicationThemeGroupForm(array('application_id' => $this->application_id, 'theme_group_id' => $this->theme_group_id));
	}
}
