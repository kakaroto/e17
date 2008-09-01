<?php

class userActions extends exchangeActions
{
	public function executeActivate()
	{
		$this->forward404Unless($this->getRequestParameter('hash'));
		$q = new Doctrine_Query();
		$this->user = $q->select('u.*')
						->from('User u')
						->where('hash = ?', array($this->getRequestParameter('hash')))
						->execute()
						->getFirst();
		$this->forward404Unless($this->user);
		$this->user->setActive(true);
		$this->user->setHash('');
		$this->user->save();
		$this->getUser()->logIn($this->user);
	}
	
	public function executeForgot($request)
	{
		$this->form = new ForgotForm();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('user'));
			if ($this->form->isValid())
			{
				$values = $this->form->getValues();
				$user = User::getByEmail($values['name'], $values['email']);
				if ($user) {
					$user->sendForgotPasswordEmail();
				}
			}
			$this->redirect('/user/login');
		}
	}

	public function executeIndex()
	{
	}
	
	public function executeLogin($request)
	{
		$this->form = new LoginForm();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('user'));
			if ($this->form->isValid())
			{
				$values = $this->form->getValues();
				$user = User::getByPassword($values['name'], $values['password']);
				if ($user) {
					if ($values['remember']) // Remeber for 3 months
						$this->getResponse()->setCookie('remember', $user->getHash(), time()+60*60*24*90, '/');
					$this->getUser()->logIn($user);
					return $this->redirect('/home/index');
				}
			}
		}
	}
	
	public function executeLogout()
	{
		$this->getUser()->logout();
		$this->getResponse()->setCookie('remember', '', time()+60*60*24*90, '/');
		$this->redirect('/user/login');
	}
	
	public function executeRead()
	{
		$q = new Doctrine_Query();
		$this->user = $q->select('u.*')
						->from('User u')
						->where('id = ?', array($this->getRequestParameter('id')))
						->execute()
						->getFirst();
		$this->forward404Unless($this->user);
	}
	
	public function executeRemove()
	{
		$id = $this->getRequestParameter('id');
		if ($id) {
			$q = new Doctrine_Query();
			if ($this->getUser()->hasCredential('admin')) {
				$this->user = $q->select('u.*')
								->from('User u')
								->where('id = ?', array($id))
								->execute()
								->getFirst();
			}
			$this->forward404Unless($this->user);
			$this->user->delete();
		}
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeSort()
	{
		if ($this->getRequestParameter('limit'))
			$this->getResponse()->setCookie('user_limit', $this->getRequestParameter('limit'), time()+60*60*24*30, '/');
		return $this->redirect($this->getRequest()->getReferer()); 
	}
	
	public function executeUpdate($request)
	{
		$this->prepareUpdate();
		if($this->getRequest()->getMethod() == sfRequest::POST) {
			$this->form->bind($request->getParameter('user'));
			if ($this->form->isValid())
			{
				$values = $this->form->getValues();
				$user = User::update($this->form->getValues(), $this->getUser()->getUser());
				if ($this->new_user)
					$this->setTemplate('checkEmail');
				else
					$this->redirect('/user/read?id='.$user->getId());
			}
		}
	}
	
	public function prepareUpdate()
	{
		$id = $this->getRequestParameter('id');
		$this->new_user = false;
		if ($id) {
			$q = new Doctrine_Query();
			if ($this->getUser()->hasCredential('admin') || ($this->getUser()->getId() == $id)) {
				$this->user = $q->select('u.*')
								->from('User u')
								->where('id = ?', array($id))
								->execute()
								->getFirst();
			}
			$this->forwardUnsecureUnless($this->user);
		} else {
			$this->new_user = true;
			$this->user = new User();
			$this->user->setRole(User::USER); // Not an Admin
		}
		$this->form = new UserForm(array('id' => $this->user->getId(), 'name' => $this->user->getName(), 'email' => $this->user->getEmail(), 'role' => $this->user->getRole()));
	}

}
