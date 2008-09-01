<?php

/**
 * exchangeActions for all the controllers.
 *
 * This class extends sfActions and is used by all actions classes
 */
class exchangeActions extends sfActions
{
	public function redirectRaw($url, $statusCode = 302)
	{
		if (sfConfig::get('sf_logging_enabled'))
		{
		  $this->dispatcher->notify(new sfEvent($this, 'application.log', array(sprintf('Redirect to "%s"', $url))));
		}
		
		// redirect
		$response = $this->context->getResponse();
		$response->clearHttpHeaders();
		$response->setStatusCode($statusCode);
		$response->setHttpHeader('Location', $url);
		$response->setContent(sprintf('<html><head><meta http-equiv="refresh" content="%d;url=%s"/></head></html>', 0, htmlspecialchars($url, ENT_QUOTES, sfConfig::get('sf_charset'))));
		$response->send();

		throw new sfStopException();
	}

	public function forwardUnsecure($message = '')
	{
		throw new forwardUnsecureException($message);
	}
	
	public function forwardUnsecureUnless($condition, $message = '')
	{
		if (!$condition)
			$this->forwardUnsecure($message);
	}
}