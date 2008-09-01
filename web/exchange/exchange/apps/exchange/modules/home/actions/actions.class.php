<?php

class homeActions extends exchangeActions
{
	public function executeAbout()
	{
		
	}
	
	public function executeCvs()
	{
		$this->path = str_replace('|', '/', $this->getRequestParameter('path'));
	}
	
	public function executeError404()
	{
		
	}
	
	public function executeIndex()
	{
		
	}
	
	public function executeSearch()
	{
		$this->term = $this->getRequestParameter('term');
	}
	
	public function executeSecure()
	{
		
	}
	
	public function executeSitemap()
	{
		$urls = array();
		$home = array();
		$home['url'] = Tools::get('url');
		$home['priority'] = 1.0;
		$home['changefreq'] = 'daily';
		$urls[] = $home;
		$latest = 0;
		$q = new Doctrine_Query();
		$themes = $q->select('t.*')
				->from('Theme t')
				->addWhere('t.approved = ?', array(true))
				->orderby('t.updated_at desc')
				->limit(50)
				->execute();
		$q->free();
		$latest_theme = 0;
		
		$theme_home = array();
		$theme_home['url'] = Tools::get('url').'/theme';
		$theme_home['priority'] = 1.0;
		$theme_home['changefreq'] = 'daily';
		$urls[] = $theme_home;
		
		$application_home = array();
		$application_home['url'] = Tools::get('url').'/application';
		$application_home['priority'] = 1.0;
		$application_home['changefreq'] = 'daily';
		$urls[] = $application_home;
		
		$module_home = array();
		$module_home['url'] = Tools::get('url').'/module';
		$module_home['priority'] = 1.0;
		$module_home['changefreq'] = 'daily';
		$urls[] = $module_home;
		
		$user_home = array();
		$user_home['url'] = Tools::get('url').'/user';
		$user_home['priority'] = 1.0;
		$user_home['changefreq'] = 'daily';
		$urls[] = $user_home;
		
		foreach ($themes as $theme) {
			$url = array();
			$url['url'] = Tools::get('url').'/theme/show/'.$theme->getId();
			$url['priority'] = 0.5;
			$url['changefreq'] = 'daily';
			$url['lastmod'] = $theme->getUpdatedAt();
			$urls[] = $url;
			$current = strtotime($theme->getUpdatedAt());
			if ($current > $latest_theme)
				$latest_theme = $current;
			if ($current > $latest)
				$latest = $current;
		}
		$themes->free();
		$urls[1]['lastmod'] = date('r', $latest_theme);
		
		$q = new Doctrine_Query();
		$applications = $q->select('a.*')
				->from('Application a')
				->addWhere('a.approved = ?', array(true))
				->orderby('a.updated_at desc')
				->limit(50)
				->execute();
		$q->free();
		$latest_application = 0;
		foreach ($applications as $application) {
			$url = array();
			$url['url'] = Tools::get('url').'/application/show/'.$application->getId();
			$url['priority'] = 0.5;
			$url['changefreq'] = 'daily';
			$url['lastmod'] = $application->getUpdatedAt();
			$urls[] = $url;
			$current = strtotime($application->getUpdatedAt());
			if ($current > $latest_application)
				$latest_application = $current;
			if ($current > $latest)
				$latest = $current;
		}
		$applications->free();
		$urls[2]['lastmod'] = date('r', $latest_application);
		
		$q = new Doctrine_Query();
		$modules = $q->select('m.*')
				->from('Madule m')
				->addWhere('m.approved = ?', array(true))
				->orderby('m.updated_at desc')
				->limit(50)
				->execute();
		$q->free();
		$latest_module = 0;
		foreach ($modules as $module) {
			$url = array();
			$url['url'] = Tools::get('url').'/module/show/'.$module->getId();
			$url['priority'] = 0.5;
			$url['changefreq'] = 'daily';
			$url['lastmod'] = $module->getUpdatedAt();
			$urls[] = $url;
			$current = strtotime($module->getUpdatedAt());
			if ($current > $latest_module)
				$latest_module = $current;
			if ($current > $latest)
				$latest = $current;
		}
		$modules->free();
		$urls[3]['lastmod'] = date('r', $latest_module);
		
		$q = new Doctrine_Query();
		$users = $q->select('u.*')
				->from('User u')
				->addWhere('u.active = ?', array(true))
				->orderby('u.updated_at desc')
				->limit(50)
				->execute();
		$q->free();
		$latest_user = 0;
		foreach ($users as $user) {
			$url = array();
			$url['url'] = Tools::get('url').'/user/show/'.$user->getId();
			$url['priority'] = 0.5;
			$url['changefreq'] = 'daily';
			$url['lastmod'] = $user->getUpdatedAt();
			$urls[] = $url;
			$current = strtotime($user->getUpdatedAt());
			if ($current > $latest_user)
				$latest_user = $current;
		}
		$users->free();
		$urls[4]['lastmod'] = date('r', $latest_user);
		
		$urls[0]['lastmod'] = date('r', $latest);
		$this->urls = $urls;
	}
	
	public function executeWikiSyntax()
	{
		$this->wikiLines = array(	'\'\'\'Bold text\'\'\'', 
									'\'\'Italic text\'\'',
									'= Level 1 =',
									'== Level 2 ==',
									'=== Level 3 ===',
									'==== Level 4 ====',
									'===== Level 5 =====',
									'[theme:Fireball]',
									'[app:Enlightenment]',
									'[module:EFM Navigator]',
									"http://exchange.enlightenment.org\r\n[http://exchange.enlightenment.org The Exchange]\r\n[http://exchange.enlightenment.org]\r\n[http://exchange.enlightenment.org/theme]\r\n",
									"Ordered list:\r\n# item 1\r\n# item 2\r\n# item 3\r\n",
									"Unordered list:\r\n* item 1\r\n* item 2\r\n* item 3\r\n*^ next line\r\n*^  a preformatted text\r\n* item 4\r\n",
									"Nested list:\r\n* item 1\r\n** item 11\r\n** item 12\r\n* item 2\r\n*# item 21\r\n*# item 22\r\n",
									"{| border=\"1\" summary=\"A simple table\"\r\n! Col1\r\n! Col2\r\n|-\r\n|| Item 1\r\n|| Item 2\r\n|-\r\n|colspan=\"2\"| Item 3\r\n|}\r\n",
									//"[[#Bold|Bold]]\r\n[[#Italic|Italic]]\r\n[[#Lists]]\r\n[[#Syntax_5|Syntax of lists]]\r\n[[#Syntax_11|Syntax of raw text]]\r\n",
									//"----",
									"<nowiki>\r\n= Heading =\r\n'''bold'''r\n</nowiki>\r\n");
		$this->wpwiki = new WikiText();
	}
}
