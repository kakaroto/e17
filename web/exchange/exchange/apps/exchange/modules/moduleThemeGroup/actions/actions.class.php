<?php

class moduleThemeGroupActions extends sfActions
{
	public function executeIndex($request)
	{
		
	}
	
	public function executeRemove()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			$this->maduleThemeGroup = $q->select('a.*')
							->from('MaduleThemeGroup a')
							->where('id = ?', array($id))
							->execute()
							->getFirst();
			$this->forward404Unless($this->maduleThemeGroup);
			$this->maduleThemeGroup->delete();
		}
		$module_id = $this->getRequestParameter('module_id');
		$theme_group_id = $this->getRequestParameter('theme_group_id');
		if ($module_id && $theme_group_id) {
			$q = new Doctrine_Query();
			$this->maduleThemeGroup = $q->select('a.*')
							->from('MaduleThemeGroup a')
							->where('madule_id = ? and theme_group_id = ?', array($module_id, $theme_group_id))
							->execute()
							->getFirst();
			$this->forward404Unless($this->maduleThemeGroup);
			$this->maduleThemeGroup->delete();
		}
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeUpdate($request)
	{
		$this->prepareUpdate();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('moduleThemeGroup'));
			if ($this->form->isValid())
			{
				$values = $this->form->getValues();
				$maduleThemeGroup = MaduleThemeGroup::update($this->form->getValues(), $this->getUser()->getUser());
				if ($maduleThemeGroup)
					$this->redirect('/module/read?id='.$maduleThemeGroup->getMaduleId());
			}
			return $this->redirect($this->getRequest()->getReferer()); 
		}
	}
	
	public function prepareUpdate()
	{
		$this->maduleThemeGroup = new MaduleThemeGroup();
		$this->module_id = $this->getRequestParameter('module_id');
		$this->theme_group_id = $this->getRequestParameter('theme_group_id');
		$this->form = new ModuleThemeGroupForm(array('module_id' => $this->module_id, 'theme_group_id' => $this->theme_group_id));
	}
}
