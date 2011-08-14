<?php
// $Id$

/**
 * @file
 */

class fDate {
  
  var $date = array();
  
  var $node;
  var $field_name;
  var $repeating = FALSE;
  var $frequency = FALSE;
  var $interval = FALSE;
  var $ended;
  
  var $display = 'both';
  
  public function __construct($node, $field_name) {

    $node = is_object($node) ? $node : node_load($node);
    
    // set_date() requires we have our structured array representing
    // the node's content. Let's not assume this node has been 
    // fully processed.
    if (!isset($node->content)) {
      $node = node_build_content($node);
    }
    
    if (!$node) {
      throw new Exception(t('Can\'t create an fDate object: invalid node object or id.'));
    }

    $this->node = $node;
    $this->field_name = $field_name;

    if (!isset($node->{$field_name}[0]['value']) || !$node->{$field_name}[0]['value']) {
      throw new Exception(t('Can\'t create an fDate object: invalid array format or missing date value.'));
    }
    
    if (!isset($node->{$field_name})) {
      throw new Exception(t('Can\'t create an fDate object: field doesn\'t exist!'));
    }
    
    $display = variable_get('date:' . $node->type . ':full:' . $this->field_name . '_fromto', '');
    if ($display && $display != 'both') {
      $this->display = $display == 'value'  ? 'from' : 'to';
    }
    
    $this->set_repeating();
    
    for ($i=0; $i < count($node->{$this->field_name}); $i++){
      $next = $this->set_date($i);
    }

    $this->ended = $next ? FALSE : TRUE;
  }
  
  private function set_repeating() {

    if (isset($this->node->{$this->field_name}[0]['rrule'])) {
      $this->repeating = TRUE;
      
      $a = explode(';', substr($this->node->{$this->field_name}[0]['rrule'], 6));
      
      $this->frequency = false;
      $this->interval = false;
      
      foreach ($a as $v) {
        $b = explode('=', $v);
        if ($b[0] == 'FREQ'){
          $this->frequency = strtolower($b[1]);
        }
        if ($b[0] == 'INTERVAL' && $b[1] != '1'){
          $this->interval = $b[1];
        }
      }
    }
  }
  
  private function set_date($weight, $next=FALSE) {
    
    static $next;

    $element = $this->node->content[$this->field_name]['field']['items'][$weight];

    if ($weight == 0) {
      $next = FALSE;
    }

    $this->date[$weight] = $this->date_repeat_fm_date_process($element);

    if (!$next) {
      if ( ( $this->date[$weight]['end'] ? $this->date[$weight]['end']['timestamp'] : $this->date[$weight]['start']['timestamp'] ) >= strtotime("now -1 day") ){
        $next = $this->date[$weight]['next'] = TRUE;
      }
    }

    return $next;
  }
  
  private function date_repeat_fm_date_process($element) {

    $fields = content_fields();
    $field = $fields[$element['#field_name']];

    $system_short = variable_get('date_format_short', 'm/d/Y - H:i');
    $system_medium = variable_get('date_format_medium',  'D, m/d/Y - H:i');
    $system_long = variable_get('date_format_long',  'l, F j, Y - H:i');

    $format = ${'system_' . $field['default_format']};

    $timezone = isset($element['#item']['timezone']) ? $element['#item']['timezone'] : '';
    $timezone = date_get_timezone($field['tz_handling'], $timezone);
    $timezone_db = date_get_timezone_db($field['tz_handling']);

    $process = date_process_values($field);
    
    $date = array('end' => FALSE);

    $n = 0;
    foreach ($process as $processed) {
      $value = $element['#item'][$processed];
      if ($field['type'] == DATE_ISO) {
        $value = str_replace(' ', 'T', date_fuzzy_datetime($value));
      }
      $d = date_make_date($value, $timezone_db, $field['type'], $field['granularity']);

      date_timezone_set($d, timezone_open($timezone));

      // Extract time format from format string.
      preg_match_all("/[aABgGhHisu]/", $format, $matches, PREG_OFFSET_CAPTURE);
      if (isset($matches[0][0][1])) {
        $s = $matches[0][0][1];
        $e = $matches[0][count($matches[0])-1][1];
        $time = substr($format, $s, $e-$s+1);
      }

      $key = $n == 0 ? 'start' : 'end';

      if ( $n == 0 || ( count($process) > 1 && $n == count($process)-1 ) ) {
        $date[$key] = array(
          'object' => $d,
          'timestamp' => date_format_date($d, 'custom', 'U'),
          'formatted' => date_format_date($d, 'custom', $format),
          'abbrev' => date_format_date($d, 'small'),
          'mdy' => date_format_date($d, 'custom', 'M d, Y'),
          'time' => $time ? date_format_date($d, 'custom', $time) : FALSE,
          'date_format' => $format,
          'timezone' => $timezone,
          'next' => FALSE,
        );
      }
      $n++;
    }

    $date['ended'] = $date['end'] 
      ? $date['end']['timestamp'] < strtotime("now -1 day") 
      : $date['start']['timestamp'] < strtotime("now -1 day");

    return $date;
  }
}
