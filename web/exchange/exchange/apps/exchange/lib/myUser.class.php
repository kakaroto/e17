<?php

class myUser extends sfBasicSecurityUser
{
	public function getId()
	{
		return $this->getAttribute('user_id', '', 'user');
	}
	
	public function getUser()
	{
		return Doctrine::getTable('User')->find($this->getId());
	}
	
	public function logIn($user)
	{
		$this->setAttribute('user_id', $user->getId(), 'user');

		$this->setAuthenticated(true);
	 
		if($user->getRole() == User::ADMIN)
			$this->addCredentials('admin', 'user');
		elseif($user->getRole() == User::USER)
			$this->addCredential('user');
	}
	
	public function logOut()
	{	
		$this->getAttributeHolder()->removeNamespace('user');
		$this->getAttributeHolder()->clear();

		$this->setAuthenticated(false);
		$this->clearCredentials();		
	}
}
