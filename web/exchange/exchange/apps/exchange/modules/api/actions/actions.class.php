<?php

class apiActions extends exchangeActions
{
	public function executeIndex()
	{
		$this->output = '';
	}
	
	public function executeList()
	{
		$rss = $this->getRequestParameter('rss');
		$object = $this->getRequestParameter('object');
		
		$output = '';
		
		$title = Tools::get('name');
		$link = Tools::get('url');
		$description = Tools::get('rss_desc');
		
		$offset = $this->getRequestParameter('offset');
		$limit = $this->getRequestParameter('limit');
		if ($object=='application') {
			$title .= ' - Applications';
			$user_id = $this->getRequestParameter('user_id');
			if ($this->getRequestParameter('user_name'))
				$user_id = User::getByName($this->getRequestParameter('user_name'))->getId();
			
			$q = new Doctrine_Query();
			$q = $q->select('a.*')
					->from('Application a');
			if ($user_id)
				$q = $q->addWhere('user_id = ?', array($user_id));
			$q = $q->addWhere('approved = ?', array(true));
			$q = $q->orderby('updated_at desc');
			if ($limit)
				$q->limit($limit);
			if ($offset)
				$q->offset($offset);
			$applications = $q->execute();
			if (!$rss)
				$output .= '<applications count="'.$applications->count().'">';
			foreach ($applications as $application)
				$output .= $application->getXML($rss);
			if (!$rss)
				$output .= '</applications>';
		} elseif ($object=='comment') {
			$title .= ' - Comments';
			$user_id = $this->getRequestParameter('user_id');
			if ($this->getRequestParameter('user_name'))
				$user_id = User::getByName($this->getRequestParameter('user_name'))->getId();
			$application_id = $this->getRequestParameter('application_id');
			$module_id = $this->getRequestParameter('module_id');
			$theme_id = $this->getRequestParameter('theme_id');
			
			$q = new Doctrine_Query();
			$q = $q->select('c.*')
					->from('Comment c');
			if ($user_id)
				$q = $q->addWhere('user_id = ?', array($user_id));
			if ($application_id)
				$q = $q->addWhere('application_id = ?', array($application_id));
			if ($module_id)
				$q = $q->addWhere('madule_id = ?', array($module_id));
			if ($theme_id)
				$q = $q->addWhere('theme_id = ?', array($theme_id));
			$q = $q->orderby('updated_at desc');
			if ($limit)
				$q->limit($limit);
			if ($offset)
				$q->offset($offset);
			$comments = $q->execute();
			if (!$rss)
				$output .= '<comments count="'.$comments->count().'">';
			foreach ($comments as $comment)
				$output .= $comment->getXML($rss);
			if (!$rss)
				$output .= '</comments>';
		} elseif ($object=='module') {
			$title .= ' - Modules';
			$application_id = $this->getRequestParameter('application_id');
			$user_id = $this->getRequestParameter('user_id');
			if ($this->getRequestParameter('user_name'))
				$user_id = User::getByName($this->getRequestParameter('user_name'))->getId();
			
			$q = new Doctrine_Query();
			$q = $q->select('m.*')
					->from('Madule m');
			if ($application_id)
				$q = $q->addWhere('application_id = ?', array($application_id));
			if ($user_id)
				$q = $q->addWhere('user_id = ?', array($user_id));
			$q = $q->addWhere('approved = ?', array(true));
			$q = $q->orderby('updated_at desc');
			if ($limit)
				$q->limit($limit);
			if ($offset)
				$q->offset($offset);
			$modules = $q->execute();
			if (!$rss)
				$output .= '<modules count="'.$modules->count().'">';
			foreach ($modules as $module)
				$output .= $module->getXML($rss);
			if (!$rss)
				$output .= '</modules>';
		} elseif ($object=='theme') {
			$title .= ' - Themes';
			$user_id = $this->getRequestParameter('user_id');
			if ($this->getRequestParameter('user_name'))
				$user_id = User::getByName($this->getRequestParameter('user_name'))->getId();
			$theme_group_id = $this->getRequestParameter('theme_group_id');
			if ($this->getRequestParameter('theme_group_title'))
				$theme_group_id = ThemeGroup::getByTitle($this->getRequestParameter('theme_group_title'))->getId();
			if ($this->getRequestParameter('theme_group_name'))
				$theme_group_id = ThemeGroup::getByName($this->getRequestParameter('theme_group_name'))->getId();
			$application_id = $this->getRequestParameter('application_id');
			$module_id = $this->getRequestParameter('module_id');
			
			$q = new Doctrine_Query();
			$q = $q->select('t.*')
					->from('Theme t');
			if ($application_id) {
				$q = $q->addWhere('t.id IN (SELECT ttg.theme_id from ThemeThemeGroup ttg where ttg.theme_group_id IN ' .
											'(SELECT ag.theme_group_id FROM ApplicationThemeGroup ag where ag.application_id = ?))', array($application_id));
			} elseif ($module_id) {
				$q = $q->addWhere('t.id IN (SELECT ttg.theme_id from ThemeThemeGroup ttg where ttg.theme_group_id IN ' .
											'(SELECT mg.theme_group_id FROM MaduleThemeGroup mg where mg.madule_id = ?))', array($module_id));
			} elseif ($theme_group_id)
				$q = $q->innerJoin('t.ThemeThemeGroups ttg')->addWhere('ttg.theme_group_id = ?', array($theme_group_id));
			if ($user_id)
				$q = $q->addWhere('user_id = ?', array($user_id));
			$q = $q->addWhere('approved = ?', array(true));
			$q = $q->orderby('updated_at desc');
			if ($limit)
				$q->limit($limit);
			if ($offset)
				$q->offset($offset);
			$themes = $q->execute();
			if (!$rss)
				$output .= '<themes count="'.$themes->count().'">';
			foreach ($themes as $theme)
				$output .= $theme->getXML($rss);
			if (!$rss)
				$output .= '</themes>';
		} elseif ($object=='theme_group') {
			$title .= ' - Theme Groups';
			$theme_id = $this->getRequestParameter('theme_id');
			
			$q = new Doctrine_Query(); 
			$q = $q->select('t.*')
						->from('ThemeGroup t');
			if ($theme_id)
				$q = $q->innerJoin('t.ThemeThemeGroups ttg')
						->addWhere('ttg.theme_id = ? and t.known = ?', array($theme_id, true));
			$q = $q->orderby('updated_at desc');
			if ($limit)
				$q->limit($limit);
			if ($offset)
				$q->offset($offset);
			$themeGroups = $q->execute();
			if (!$rss)
				$output .= '<theme_groups count="'.$themeGroups->count().'">';
			foreach ($themeGroups as $themeGroup)
				$output .= $themeGroup->getXML($rss);
			if (!$rss)
				$output .= '</theme_groups>';
		} elseif ($object=='user') {
			$title .= ' - Users';
			$q = new Doctrine_Query();
			$q = $q->select('u.*')
							->from('User u')
							->orderby('updated_at desc');
			if ($limit)
				$q->limit($limit);
			if ($offset)
				$q->offset($offset);
			$users = $q->execute();
			if (!$rss)
				$output .= '<users count="'.$users->count().'">';
			foreach ($users as $user)
				$output .= $user->getXML($rss);
			if (!$rss)
				$output .= '</users>';
		}
		if ($rss)
			$this->output = '<rss version="2.0">' .
							'<channel>' .
							'<title>'.$title.'</title>' .
							'<link>'.$link.'</link>' .
							'<description>'.$description.'</description>' .
							'<generator>'.$title.'</generator>' .
							$output.
							'</channel>' .
							'</rss>';
		else
			$this->output = '<rsp stat="ok">'.$output.'</rsp>';
		$this->setTemplate('index');
	}
	
	public function executeLogin($request)
	{
		$form = new LoginForm();
		$form->bind(array('name' => $request->getParameter('name'), 'password' => $request->getParameter('password')));
		if ($form->isValid())
		{
			$values = $form->getValues();
			$user = User::getByPassword($values['name'], $values['password']);
			if ($user) {
				$output = '<rsp stat="ok">'.$user->getXML(false, true).'</rsp>';
			}
		} else {
			$output = '<rsp stat="fail"><err code="1" msg="'.$form->getErrorSchema().'" /></rsp>';
		}
		$this->output = $output;
		$this->setTemplate('index');
	}
	
	public function executeRead()
	{
		$object = $this->getRequestParameter('object');
		$output = '';
		if ($object=='application') {
			$q = new Doctrine_Query();
			$application = $q->select('a.*')
							->from('Application a')
							->where('(id = ? or name = ?) and approved = ?', array($this->getRequestParameter('id'), $this->getRequestParameter('name'), true))
							->execute()
							->getFirst();
			if ($application)
				$output .= $application->getXML();
		} elseif ($object=='comment') {
			$q = new Doctrine_Query();
			$comment = $q->select('c.*')
							->from('Comment c')
							->where('id = ?', array($this->getRequestParameter('id')))
							->execute()
							->getFirst();
			if ($comment)
				$output .= $comment->getXML();
		} elseif ($object=='module') {
			$q = new Doctrine_Query();
			$module = $q->select('m.*')
							->from('Madule m')
							->where('(id = ? or name = ?) and approved = ?', array($this->getRequestParameter('id'), $this->getRequestParameter('name'), true))
							->execute()
							->getFirst();
			if ($module)
				$output .= $module->getXML();
		} elseif ($object=='theme') {
			$q = new Doctrine_Query();
			$theme = $q->select('t.*')
							->from('Theme t')
							->where('(id = ? or name = ?) and approved = ?', array($this->getRequestParameter('id'), $this->getRequestParameter('name'), true))
							->execute()
							->getFirst();
			if ($theme)
				$output .= $theme->getXML();
		} elseif ($object=='theme_group') { 
			$q = new Doctrine_Query();
			$themeGroup = $q->select('t.*')
							->from('ThemeGroup t')
							->where('id = ? or name = ?', array($this->getRequestParameter('id'), $this->getRequestParameter('name')))
							->execute()
							->getFirst();
			if ($themeGroup)
				$output .= $themeGroup->getXML();
		} elseif ($object=='user') { 
			$q = new Doctrine_Query();
			$user = $q->select('u.*')
							->from('User u')
							->where('id = ? or name = ?', array($this->getRequestParameter('id'), $this->getRequestParameter('name')))
							->execute()
							->getFirst();
			if ($user)
				$output .= $user->getXML();
		}
		$this->output = '<rsp stat="ok">'.$output.'</rsp>';
		$this->setTemplate('index');
	}
	
	public function executeUpdate($request)
	{
		$object = $this->getRequestParameter('object');
		$user = User::getByApiKey($request->getParameter('login_id'), $request->getParameter('api_key'));
		if (!$user) {
			$output = '<rsp stat="fail"><err code="2" msg="login_id and api_key do not match" /></rsp>';
		} elseif ($object=='application') {
			$form = new ApplicationForm();
			$form->bind(array('id' => $request->getParameter('id'), 
								'name' => $request->getParameter('name'), 
								'description' => $request->getParameter('description'), 
								'source_url' => $request->getParameter('source_url')));
			if ($form->isValid()) {
				$application = Application::update($form->getValues(), $user);
				if ($application)
					$output = '<rsp stat="ok">'.$application->getXML().'</rsp>';
				else
					$output = '<rsp stat="fail"><err code="4" msg="Unable to update application." /></rsp>';
			} else {
				$output = '<rsp stat="fail"><err code="4" msg="'.$form->getErrorSchema().'" /></rsp>';
			}
		} elseif ($object=='comment') {
			$form = new CommentForm();
			$application_id = $module_id = $theme_id = null;
			if ($request->getParameter('application_id'))
				$application_id = $request->getParameter('application_id');
			if ($request->getParameter('module_id'))
				$module_id = $request->getParameter('module_id');
			if ($request->getParameter('theme_id'))
				$theme_id = $request->getParameter('theme_id');
			$form->bind(array('comment' => $request->getParameter('comment'), 'application_id' => $application_id, 'module_id' => $module_id, 'theme_id' => $theme_id));
			if ($form->isValid()) {
				$comment = Comment::update($form->getValues(), $user);
				$output = '<rsp stat="ok">'.$comment->getXML().'</rsp>';
			} else {
				$output = '<rsp stat="fail"><err code="3" msg="'.$form->getErrorSchema().'" /></rsp>';
			}
		} elseif ($object=='module') {
			$form = new ModuleForm();
			$form->bind(array('id' => $request->getParameter('id'), 
								'name' => $request->getParameter('name'), 
								'description' => $request->getParameter('description'), 
								'source_url' => $request->getParameter('source_url'), 
								'application_id' => $request->getParameter('application_id')));
			if ($form->isValid()) {
				$module = Madule::update($form->getValues(), $user);
				if ($module)
					$output = '<rsp stat="ok">'.$module->getXML().'</rsp>';
				else
					$output = '<rsp stat="fail"><err code="4" msg="Unable to update module." /></rsp>';
			} else {
				$output = '<rsp stat="fail"><err code="4" msg="'.$form->getErrorSchema().'" /></rsp>';
			}
		} elseif ($object=='theme') {
			$form = new ThemeForm();
			$form->bind(array('id' => $request->getParameter('id'), 'name' => $request->getParameter('name'), 'description' => $request->getParameter('description')), $request->getFiles());
			if ($form->isValid()) {
				$theme = Theme::update($form->getValues(), $user);
				if ($theme)
					$output = '<rsp stat="ok">'.$theme->getXML().'</rsp>';
				else
					$output = '<rsp stat="fail"><err code="5" msg="Unable to update theme." /></rsp>';
			} else {
				$output = '<rsp stat="fail"><err code="5" msg="'.$form->getErrorSchema().'" /></rsp>';
			}
		} elseif ($object=='theme_group') {
			$output = '<rsp stat="fail"><err code="6" msg="This object is not supported for update" /></rsp>';
		} elseif ($object=='user') {
			$form = new UserForm();
			$form->bind(array('id' => $request->getParameter('id'), 
										'name' => $request->getParameter('name'), 
										'password' => $request->getParameter('password'),
										'password2' => $request->getParameter('password'),
										'email' => $request->getParameter('email'), 
										'role' => null));
			if ($form->isValid()) {
				$update_user = User::update($form->getValues(), $user);
				if ($update_user)
					$output = '<rsp stat="ok">'.$update_user->getXML().'</rsp>';
				else
					$output = '<rsp stat="fail"><err code="7" msg="Unable to update user." /></rsp>';
			} else {
				$output = '<rsp stat="fail"><err code="7" msg="'.$form->getErrorSchema().'" /></rsp>';
			}
		}
		$this->output = $output;
		$this->setTemplate('index');
	}
}
