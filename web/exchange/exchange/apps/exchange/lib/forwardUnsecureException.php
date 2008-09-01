<?php

class forwardUnsecureException extends sfException
{
  /**
   * Forwards to the Unsecire action.
   *
   * @param Exception An Exception implementation instance
   */
  public function printStackTrace($exception = null)
  {
    sfContext::getInstance()->getController()->forward(sfConfig::get('sf_secure_module'), sfConfig::get('sf_secure_action'));
  }
}
