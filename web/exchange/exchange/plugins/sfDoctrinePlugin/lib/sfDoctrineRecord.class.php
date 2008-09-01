<?php
/*
 * This file is part of the sfDoctrinePlugin package.
 * (c) 2006-2007 Jonathan H. Wage <jonwage@gmail.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * Base sfDoctrineRecord extends the base Doctrine_Record in Doctrine to provide some
 * symfony specific functionality to Doctrine_Records
 *
 * @package    sfDoctrinePlugin
 * @author     Jonathan H. Wage <jonwage@gmail.com>
 * @version    SVN: $Id: sfDoctrineRecord.class.php 8743 2008-05-03 05:02:39Z Jonathan.Wage $
 */
abstract class sfDoctrineRecord extends Doctrine_Record
{
  /**
   * __toString
   *
   * @return string $string
   */
  public function __toString()
  {
    // if the current object doesn't exist we return nothing
    if (!$this->exists())
    {
      return '-';
    }

    $guesses = array('name',
                     'title',
                     'description',
                     'subject',
                     'keywords',
                     'id');

    // we try to guess a column which would give a good description of the object
    foreach ($guesses as $descriptionColumn)
    {
      if ($this->getTable()->hasColumn($descriptionColumn))
      {
        return $this->get($descriptionColumn);
      }
    }

    return sprintf('No description for object of class "%s"', $this->getTable()->getComponentName());
  }

  /**
   * Get the primary key of a Doctrine_Record.
   * This a proxy method to Doctrine_Record::identifier() for Propel BC
   *
   * @return mixed $identifier Array for composite primary keys and string for single primary key
   */
  public function getPrimaryKey()
  {
    return $this->identifier();
  }

  /**
   * Get a record attribute. Allows overriding Doctrine record accessors with Propel style functions
   *
   * @param string $name 
   * @param string $load 
   * @return void
   */
  public function get($name, $load = true)
  {
    $getter = 'get' . Doctrine_Inflector::classify($name);

    if (method_exists($this, $getter))
    {
      return $this->$getter($load);
    }

    return parent::get($name, $load);
  }

  /**
   * Use inside of overriden Doctrine accessors
   *
   * @param string $name 
   * @param string $load 
   * @return void
   */
  public function rawGet($name)
  {
    return parent::rawGet($name);
  }

  /**
   * Set a record attribute. Allows overriding Doctrine record accessors with Propel style functions
   *
   * @param string $name 
   * @param string $value 
   * @param string $load 
   * @return void
   */
  public function set($name, $value, $load = true)
  {
    $setter = 'set' . Doctrine_Inflector::classify($name);

    if (method_exists($this, $setter))
    {
      return $this->$setter($value, $load);
    }

    return parent::set($name, $value, $load);
  }

  /**
   * Use inside of overriden Doctrine accessors
   *
   * @param string $name 
   * @param string $value 
   * @return void
   */
  public function rawSet($name, $value)
  {
    parent::set($name, $value);
  }

  /**
   * This magic __call is used to provide propel style accessors to Doctrine models
   *
   * @param string $m 
   * @param string $a 
   * @return void
   */
  public function __call($m, $a)
  {
    try {
      $verb = substr($m, 0, 3);

      if ($verb == 'set' || $verb == 'get')
      {
        $camelColumn = substr($m, 3);

        // If is a relation
        if (in_array($camelColumn, array_keys($this->getTable()->getRelations())))
        {
          $column = $camelColumn;
        } else {
          $column = sfInflector::underscore($camelColumn);
        }

        if ($verb == 'get')
        {
          return $this->get($column);
        } else {
          return $this->set($column, $a[0]);
        }
      } else {
        return parent::__call($m, $a);
      }
    } catch(Exception $e) {
      return parent::__call($m, $a);
    }
  }
}