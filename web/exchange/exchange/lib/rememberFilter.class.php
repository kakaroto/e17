<?php

class rememberFilter extends sfFilter
{
	public function execute ($filterChain)
	{
		if ($this->isFirstCall() and !$this->context->getUser()->isAuthenticated())
		{
			if ($cookie = $this->getContext()->getRequest()->getCookie('remember'))
			{
				$q = new Doctrine_Query();
				$user = $q->select('u.*')
							->from('User u')
							->where('hash = ? and active = ?', 
								array($cookie, true))
							->execute()
							->getFirst();
				if ($user)
					$this->context->getUser()->logIn($user);
			}
		}

		$filterChain->execute();
	}
}
