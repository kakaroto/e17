<?php

class commentActions extends exchangeActions
{
	public function executeIndex()
	{
		$this->user = $this->object = null;
		if ($application_id = $this->getRequestParameter('application_id'))
			$this->object = Doctrine::getTable('Application')->find($application_id);
		if ($module_id = $this->getRequestParameter('module_id'))
			$this->object = Doctrine::getTable('Madule')->find($module_id);
		if ($theme_id = $this->getRequestParameter('theme_id'))
			$this->object = Doctrine::getTable('Theme')->find($theme_id);
		if ($user_id = $this->getRequestParameter('user_id'))
			$this->user = Doctrine::getTable('User')->find($user_id);
	}

	public function executeRead()
	{
		$q = new Doctrine_Query();
		$q = $q->select('c.*')
						->from('Comment c')
						->addWhere('id = ?', array($this->getRequestParameter('id')));
		$this->comment = $q->execute()
						->getFirst();
		$this->forward404Unless($this->comment);
	}
	
	public function executeRemove()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			if ($this->getUser()->hasCredential('admin')) {
				$this->comment = $q->select('c.*')
								->from('Comment c')
								->where('id = ?', array($id))
								->execute()
								->getFirst();
			} else {
				$this->comment = $q->select('c.*')
								->from('Comment c')
								->where('id = ? and user_id = ?', array($id, $this->getUser()->getId()))
								->execute()
								->getFirst();
			}
			$this->forward404Unless($this->comment);
			$this->comment->delete();
		}
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeSort()
	{
		if ($this->getRequestParameter('limit'))
			$this->getResponse()->setCookie('comment_limit', $this->getRequestParameter('limit'), time()+60*60*24*30, '/');
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeUpdate($request)
	{
		$this->prepareUpdate();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('comment'));
			if ($this->form->isValid())
			{
				$values = $this->form->getValues();
				Comment::update($this->form->getValues(), $this->getUser()->getUser());
			}
		}
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function prepareUpdate()
	{
		$this->comment = new Comment();
		$this->theme_id = $this->getRequestParameter('application_id');
		$this->module_id = $this->getRequestParameter('module_id');
		$this->theme_id = $this->getRequestParameter('theme_id');
		$this->form = new CommentForm(array('application_id' => $this->application_id, 'module_id' => $this->module_id, 'theme_id' => $this->theme_id));
	}

}
