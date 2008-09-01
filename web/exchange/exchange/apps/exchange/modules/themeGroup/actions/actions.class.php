<?php

class themeGroupActions extends exchangeActions
{
	public function executeIndex()
	{
		$q = new Doctrine_Query();
		$this->theme = $q->select('t.*')
						->from('Theme t')
						->where('id = ?', array($this->getRequestParameter('theme_id')))
						->execute()
						->getFirst();
	}
	
	public function executeRead()
	{
		$q = new Doctrine_Query();
		$this->themeGroup = $q->select('t.*')
						->from('ThemeGroup t')
						->where('id = ?', array($this->getRequestParameter('id')))
						->execute()
						->getFirst();
		$this->forward404Unless($this->themeGroup);
	}
	
	public function executeUpdate($request)
	{
		$this->prepareUpdate();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('themeGroup'));
			if ($this->form->isValid())
			{
				$values = $this->form->getValues();
				$this->themeGroup->setTitle($values['title']);
				$this->themeGroup->setKnown($values['known']);
				$this->themeGroup->save();
				$this->redirect('/themeGroup/index');
			}
		}
	}
	
	public function prepareUpdate()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			$this->themeGroup = $q->select('t.*')
							->from('ThemeGroup t')
							->where('id = ?', array($id))
							->execute()
							->getFirst();
			$this->forward404Unless($this->themeGroup);
		} else
			$this->themeGroup = new ThemeGroup();
		$this->form = new ThemeGroupForm(array('title' => $this->themeGroup->getTitle(), 'known' => $this->themeGroup->getKnown()));
	}

}
