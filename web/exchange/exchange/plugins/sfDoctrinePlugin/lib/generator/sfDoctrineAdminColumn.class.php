<?php
/*
 * This file is part of the sfDoctrinePlugin package.
 * (c) 2006-2007 Jonathan H. Wage <jonwage@gmail.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * sfDoctrineAdminColumn
 *
 * Represents a doctrine column
 *
 * @package    sfDoctrinePlugin
 * @author     Jonathan H. Wage <jonwage@gmail.com>
 * @version    SVN: $Id: sfDoctrineAdminColumn.class.php 8743 2008-05-03 05:02:39Z Jonathan.Wage $
 */
class sfDoctrineAdminColumn extends sfAdminColumn
{
  /**
   * docToCreole
   */
  static $docToCreole = array(
    'boolean'   => 1,
    'string'    => 17,
    'integer'   => 5,
    'date'      => 10,
    'timestamp' => 12,
    'time'      => 11,
    'enum'      => 4,
    'float'     => 8,
    'double'    => 8,
    'clob'      => 16,
    'blob'      => 15,
    'object'    => 7,
    'array'     => 7,
    'decimal'   => 18,
  );

  /**
   * relatedClassName
   *
   * @var string
   */
  protected $relatedClassName = null;

  /**
   * name
   *
   * @var string
   */
  protected $name = null;

  /**
   * columnName
   *
   * stores the real foreign id column
   *
   * @var string
   */
  protected $columnName;

  /**
   * getDoctrineType
   *
   * @return void
   */
  function getDoctrineType()
  {
    return isset($this->column['type']) ? $this->column['type'] : null;
  }

  /**
   * getCreoleTypes
   *
   * @return void
   */
  function getCreoleType()
  {
    $dType = $this->getDoctrineType();

    // we simulate the CHAR/VARCHAR types to generate input_tags
    if(($dType == 'string') and ($this->getSize() < 256))
    {
      return 17;
    }

    return $dType ? self::$docToCreole[$dType] : -1;
  }

  /**
   * getSize
   *
   * @return void
   */
  function getSize()
  {
    return $this->column['length'];
  }

  /**
   * isNotNull
   *
   * @return void
   */
  function isNotNull()
  {
    //FIXME THIS NEEDS TO BE UPDATE-but I don't know the format for the column array
    if (isset($this->column[2]['notnull']))
      return $this->column[2]['notnull'];
    return false;
  }

  /**
   * isPrimaryKey
   *
   * @return void
   */
  function isPrimaryKey()
  {
    if (isset($this->column['primary']))
      return $this->column['primary'];
    return false;
  }

  /**
   * setRelatedClassName
   *
   * @param string $newName
   * @return void
   */
  function setRelatedClassName($newName)
  {
    $this->relatedClassName = $newName;
  }

  /**
   * getRelatedClassName
   *
   * @return void
   */
  function getRelatedClassName()
  {
    return $this->relatedClassName;
  }

  /**
   * setColumnName
   *
   * @param string $newName
   * @return void
   */
  function setColumnName($newName)
  {
    $this->columnName = $newName;
  }

  /**
   * getColumnName
   *
   * @return void
   */
  function getColumnName()
  {
    return $this->columnName;
  }

  /**
   * setColumnInfo
   *
   * @param string $col
   * @return void
   */
  function setColumnInfo($col)
  {
    $this->column = $col;
  }

  /**
   * setName
   *
   * // FIXME: Should this be removed? it is not used anywhere
   *
   * @param string $newName
   * @return void
   */
  function setName($newName)
  {
    $this->name = $newName;
  }

  /**
   * getName
   *
   * @return void
   */
  function getName()
  {
    if (isset($this->name))
    {
      return $this->name;
    }
    // a bit kludgy: the field name is actually in $this->phpName
    return parent::getPhpName();
  }

  /**
   * isForeignKey
   *
   * @return void
   */
  function isForeignKey()
  {
    return isset($this->relatedClassName);
  }

  /**
   * __call
   *
   * all the calls that were forwarded to the table object with propel
   * have to be dealt with explicitly here, otherwise:
   *
   * @param string $name
   * @param string $arguments
   * @return void
   */
  public function __call($name, $arguments)
  {
    throw new Exception(sprintf('Unhandled call: "%s"', $name));
  }
}