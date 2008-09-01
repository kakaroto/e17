<?php

/*
 * This file is part of the symfony package.
 * (c) Fabien Potencier <fabien.potencier@symfony-project.com>
 * 
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * sfValidatorDoctrineUnique validates that the uniqueness of a column.
 *
 * Warning: sfValidatorDoctrineUnique is susceptible to race conditions.
 * To avoid this issue, wrap the validation process and the model saving
 * inside a transaction.
 *
 * @package    symfony
 * @subpackage validator
 * @author     Fabien Potencier <fabien.potencier@symfony-project.com>
 * @version    SVN: $Id: sfValidatorDoctrineUnique.class.php 8807 2008-05-06 14:12:28Z fabien $
 */
class validatorLogin extends sfValidatorSchema
{
  /**
   * Constructor.
   *
   * @param array  An array of options
   * @param array  An array of error messages
   *
   * @see sfValidatorSchema
   */
  public function __construct($options = array(), $messages = array())
  {
    parent::__construct(null, $options, $messages);
  }

  /**
   * Configures the current validator.
   *
   * Available options:
   *
   *  * username:    The username field
   *  * password:    The password field
   *  * connection:  The Doctrine connection to use (null by default)
   *
   * @see sfValidatorBase
   */
  protected function configure($options = array(), $messages = array())
  {
    $this->addRequiredOption('username');
    $this->addRequiredOption('password');
    $this->addOption('connection', null);

    $this->setMessage('invalid', 'Invalid Username or Password.');
  }

  /**
   * @see sfValidatorBase
   */
  protected function doClean($values)
  {
    $table = Doctrine::getTable('User');

    $q = Doctrine_Query::create()
          ->from('User u');
      $q->addWhere('u.name = ?', array($values[$this->getOption('username')]));
      $q->addWhere('u.password = ?', array(md5($values[$this->getOption('password')])));
      $q->addWhere('u.active = ?', array(true));
    $object = $q->fetchOne();

    // if no object or if we're updating the object, it's ok
    if ($object)
    {
      return $values;
    }

    $error = new sfValidatorError($this, 'invalid', array('column' => $this->getOption('username')));
    throw new sfValidatorErrorSchema($this, array($this->getOption('username') => $error));
  }
}