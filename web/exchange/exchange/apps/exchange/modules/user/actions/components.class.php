<?php

class userComponents extends exchangeComponents
{
	public function executeTable()
	{
		$limit = $this->getRequest()->getCookie('user_limit');
		if (!$limit)
			$limit = '5';
		$limit = $this->getRequest()->getParameter('limit', $limit);
		$page = $this->getRequest()->getParameter('page', 1);
		$this->getResponse()->setCookie('user_limit', $limit, time()+60*60*24*30, '/');
		$this->form = new SortForm(array('limit' => $limit));
		$this->role = $this->getRequest()->getParameter('role', $this->role);
		$q = new Doctrine_Query();
		$q = $q->select('u.*')
						->from('User u')
						->orderby('u.name ASC');
		if ($this->search)
			$q = $q->addWhere('(name LIKE ? or email LIKE ?)', array('%'.$this->search.'%', '%'.$this->search.'%'));
		if ($this->role)
			$q = $q->addWhere('(role = ?)', array($this->role));
		$count = $q->count();
		if ($count <= (($page - 1) * $limit))
			$page = ceil($count / $limit);
		if ($page < 1)
			$page = 1;
		$q->limit($limit);
		$q->offset(($page - 1) * $limit);
		$this->users = $q->execute();
		
		$args = array();
		$this->prevPage = $this->nextPage = false;
		if ($page > 1) {
			$args['page'] = $page - 1;
			$this->prevPage = '/user/index?'.http_build_query($args);
		}
		if (($page * $limit) < $count) {
			$args['page'] = $page + 1;
			$this->nextPage = '/user/index?'.http_build_query($args);
		}
		$this->range = (($page - 1) * $limit + 1).' to '.(($page - 1) * $limit + $this->users->count()).' of '.$count;
	}
}
