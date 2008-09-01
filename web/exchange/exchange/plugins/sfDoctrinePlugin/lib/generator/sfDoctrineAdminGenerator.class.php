<?php
/*
 * This file is part of the sfDoctrinePlugin package.
 * (c) 2006-2007 Jonathan H. Wage <jonwage@gmail.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/**
 * Class responsible for Doctrine based symfony admin generators
 *
 * @package    sfDoctrinePlugin
 * @author     Jonathan H. Wage <jonwage@gmail.com>
 * @version    SVN: $Id: sfDoctrineAdminGenerator.class.php 8744 2008-05-03 05:53:43Z Jonathan.Wage $
 */
class sfDoctrineAdminGenerator extends sfAdminGenerator
{
  /**
   * Doctrine_Table instance for this admin generator
   *
   * @var Doctrine_Table $table
   */
  protected $table;

  /**
   * Initialize a doctrine admin generator instance
   *
   * @param sfGeneratorManager $generatorManager
   * @return void
   */
  public function initialize(sfGeneratorManager $generatorManager)
  {
    parent::initialize($generatorManager);

    $this->setGeneratorClass('sfDoctrineAdmin');
  }

  /**
   * Load all Doctrine_Table class to generate instance
   *
   * @return void
   */
  protected function loadMapBuilderClasses()
  {
    $this->table = Doctrine::getTable($this->getClassName());
  }

  /**
   * Get Doctrine_Table instance for admin generator
   *
   * @return Doctrine_Table $table
   */
  protected function getTable()
  {
    return $this->table;
  }

  /**
   * Load primary key columns as array of sfDoctrineAdminColumn instances
   *
   * @return void
   * @throws sfException
   */
  protected function loadPrimaryKeys()
  {
    $identifier = $this->getTable()->getIdentifier();
    if (is_array($identifier))
    {
      foreach ($identifier as $_key)
      {
        $this->primaryKey[] = new sfDoctrineAdminColumn($_key);
      }
    } else {
      $this->primaryKey[] = new sfDoctrineAdminColumn($identifier);
    }

    if (!empty($this->primaryKeys))
    {
      throw new sfException('You cannot use the admin generator on a model which does not have any primary keys defined');
    }
  }

  /**
   * Get columns for admin generator instance
   *
   * @param string $paramName 
   * @param string $category 
   * @return array $columns
   */
  public function getColumns($paramName, $category = 'NONE')
  {
    $columns = parent::getColumns($paramName, $category);

    // set the foreign key indicator
    $relations = $this->getTable()->getRelations();

    $cols = $this->getTable()->getColumns();

    foreach ($columns as $index => $column)
    {
      if (isset($relations[$column->getName()]))
      {
        $fkcolumn = $relations[$column->getName()];
        $columnName = $relations[$column->getName()]->getLocal();
        if ($columnName != 'id') // i don't know why this is necessary
        {
          $column->setRelatedClassName($fkcolumn->getTable()->getComponentName());
          $column->setColumnName($columnName);

          // if it is not a many2many
          if (isset($cols[$columnName]))
          {
            $column->setColumnInfo($cols[$columnName]);
          }

          $columns[$index] = $column;
        }
      }
    }

    return $columns;
  }

  /**
   * Get array of all columns as sfDoctrineAdminColumn instances
   *
   * @return array $columns
   */
  function getAllColumns()
  {
    $cols = $this->getTable()->getColumns();
    $rels = $this->getTable()->getRelations();
    $columns = array();
    foreach ($cols as $name => $col)
    {
      // we set out to replace the foreign key to their corresponding aliases
      $found = null;
      foreach ($rels as $alias => $rel)
      {
        $relType = $rel->getType();
        if ($rel->getLocal() == $name && $relType != Doctrine_Relation::MANY_AGGREGATE && $relType != Doctrine_Relation::MANY_COMPOSITE)
        {
          $found = $alias;
        }
      }

      if ($found)
      {
        $name = $found;
      }

      $columns[] = new sfDoctrineAdminColumn($name, $col);
    }

    return $columns;
  }

  /**
   * Get an sfDoctrineAdminColumn instance for a field/column
   *
   * @param string $field 
   * @param string $flag 
   * @return sfDoctrineAdminColumn $column
   */
  function getAdminColumnForField($field, $flag = null)
  {
    $cols = $this->getTable()->getColumns(); // put this in an internal variable?
    return  new sfDoctrineAdminColumn($field, (isset($cols[$field]) ? $cols[$field] : null), $flag);
  }

  /**
   * Get symfony php object helper
   *
   * @param string $helperName 
   * @param string $column 
   * @param string $params 
   * @param string $localParams 
   * @return string $helperCode
   */
  function getPHPObjectHelper($helperName, $column, $params, $localParams = array())
  {
    $params = $this->getObjectTagParams($params, $localParams);

    // special treatment for object_select_tag:
    if ($helperName == 'select_tag')
    {
      $column = new sfDoctrineAdminColumn($column->getColumnName(), null, null);
    }

    return sprintf('object_%s($%s, %s, %s)', $helperName, $this->getSingularName(), var_export($this->getColumnGetter($column), true), $params);
  }

  /**
   * Get php code for column getter
   *
   * @param string $column 
   * @param string $developed 
   * @param string $prefix 
   * @return string $getterCode
   */
  function getColumnGetter($column, $developed = false, $prefix = '')
  {
    if ($developed)
    {
      return sprintf("$%s%s->get('%s')", $prefix, $this->getSingularName(), $column->getName());
    }

    // no parenthesis, we return a method+parameters array
    return array('get', array($column->getName()));
  }

  /**
   * Get php code for column setter
   *
   * @param string $column 
   * @param string $value 
   * @param string $singleQuotes 
   * @param string $prefix 
   * @return string $setterCode
   */
  function getColumnSetter($column, $value, $singleQuotes = false, $prefix = 'this->')
  {
    if ($singleQuotes)
    {
      $value = sprintf("'%s'", $value);
    }

    return sprintf('$%s%s->set(\'%s\', %s)', $prefix, $this->getSingularName(), $column->getName(), $value);
  }

  /**
   * Get related class name for a column
   *
   * @param string $column 
   * @return string $className
   */
  function getRelatedClassName($column)
  {
    return $column->getRelatedClassName();
  }

  /**
   * Get php code for column edit tag
   *
   * @param string $column 
   * @param string $params 
   * @return string $columnEditTag
   */
  public function getColumnEditTag($column, $params = array())
  {
    if ($column->getDoctrineType() == 'enum')
    {
      $params = array_merge(array('control_name' => $this->getSingularName().'['.$column->getName().']'), $params);

      $values = $this->getTable()->getEnumValues($column->getName());
      $params = array_merge(array('enumValues'=>$values), $params);
      return $this->getPHPObjectHelper('enum_tag', $column, $params);
    }

    return parent::getColumnEditTag($column, $params);
  }
}