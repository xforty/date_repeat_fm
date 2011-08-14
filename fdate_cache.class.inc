<?php
// $Id$

/**
 * @file
 */

class fDate_cache {
  
  protected static $ids;

  public function fetch($nid, $field_name) {
    if (isset(self::$ids[$nid . ':' . $field_name])) {
      return self::$ids[$nid . ':' . $field_name];
    }
    return FALSE;
  }
  
  public function set($nid, $field_name, $obj) {
    self::$ids[$nid . ':' . $field_name] = $obj;
  }
}

