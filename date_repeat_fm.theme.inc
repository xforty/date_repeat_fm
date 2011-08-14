<?php
// $Id$

/**
 * @file
 */

function theme_date_repeat_fm_formatter_format_fdate($element) {
  $output = '';

  $fDate = date_repeat_fm_fetch($element['#node']->nid, $element['#field_name']);

  if ($element['#weight'] == 0) {
    $output .= theme('date_repeat_fm_dateline', $fDate);
  }

  return $output;
}

function theme_date_repeat_fm_formatter_format_fdate_expanded($element) {
  $output = '';

  drupal_add_css(drupal_get_path('module', 'date_repeat_fm') . '/date_repeat_fm.css');

  $fDate = date_repeat_fm_fetch($element['#node']->nid, $element['#field_name']);
  
  if ($element['#weight'] == 0) {
    $output = theme('date_repeat_fm_dateline', $fDate);
    $output .= theme('date_repeat_fm_view_all', $fDate);
    $output .= theme('date_repeat_fm_listing', $fDate);
  }
  
  return $output;
}

/**
 * Theme conditional dateline output.
 *
 * Variable output depending on the following conditions:
 *   - Where the output should be abbreviated;
 *   - whether it is a repeating date;
 *   - whether the date has passed.
 *
 * @param $fDate
 *   An fDate object.
 * @param $abbrev
 *   If TRUE, will output an abbreviated formatted date.
 *
 * @ingroup themeable
 */
function theme_date_repeat_fm_dateline($fDate, $abbrev=FALSE) {
  $output = '';

  If ($fDate) {
    if ($abbrev) {
      if (!$fDate->repeating) {
        $output .= theme('date_repeat_fm_duration', $fDate, 0, TRUE);
        if ($fDate->ended) {
          $output .= theme('date_repeat_fm_ended');
        }
      }
      else {
        $output .= theme('date_repeat_fm_repeat_rule', $fDate);
        if ($fDate->ended) {
          $output .= theme('date_repeat_fm_ended');
        }
        else {
          for ($i=0; $i < count($fDate->date); $i++) {
            if ($fDate->date[$i]['next']) {
              $output .= ' ' .  theme('date_repeat_fm_duration', $fDate, $i, TRUE);
              break;
            }
          }
        }
      }
    }
    else {
      if (!$fDate->repeating) {
        $output .= theme('date_repeat_fm_duration', $fDate);
        if ($fDate->ended) {
          $output .= theme('date_repeat_fm_ended');
        }
      }
      else {
        if ($fDate->ended) {
          $output .= theme('date_repeat_fm_period', $fDate);
          $output .= theme('date_repeat_fm_ended');
        }
        else {
          foreach ($fDate->date as $v) {
            if ($v['next']) {
              $output .= theme('date_repeat_fm_next', $fDate);
              break;
            }
          }
        }
      }
    }
  }

  return $output;
}

/**
 * Theme a standard date string, taking into account 'from' and 'to' dates.
 *
 * @param $fDate
 *   An fDate object.
 * @param $index
 *   The index in the fDate object date array to format. Default is 0.
 * @param $abbrev
 *   If TRUE, will output an abbreviated formatted date.
 *
 * @ingroup themeable
 */
function theme_date_repeat_fm_duration($fDate, $index=0, $abbrev=FALSE) {
  $output = '';

  global $language;

  $format = $abbrev ? 'abbrev' : 'formatted';
  
  $date = $fDate->date[$index];

  if ($fDate) {
    switch ($fDate->display) {
      case 'both': 
        $output = $date['start'][$format];
        if ($date['end'] && !$abbrev) {
          // If 'start' and 'end' dates are the same day, but the time is different,
          // display time span rather than repeating both strings.
          if ( date('M-d-Y', $date['start']['timestamp']) == date('M-d-Y', $date['end']['timestamp']) ) {
            if ( $date['start']['timestamp'] < $date['end']['timestamp'] ) {
              if ($date['end']['time']) {
                $output .= '-' . $date['end']['time'];
              }
            }
          }
          else {
            $output = t('%date-from to %date-to', array('%date-from' => $output, '%date-to' => $date['end'][$format]));
          }
        } 
        break;
      case 'from': 
        $output = $date['start'][$format];
        break;
      case 'to': 
        $output = $date['end'] ? $date['end'][$format] : '';
        break;
    }
    $output = '<span class="date_repeat_fm-duration' . ( $date['ended'] ? ' past-date' : '' ) . '">' . $output . '</span>&nbsp;';
  }
  return $output;
}

/**
 * Theme a string showing frequency/interval when we have a repeating date.
 *
 * @param $fDate
 *   An fDate object.
 *
 * @ingroup themeable
 */
function theme_date_repeat_fm_repeat_rule($fDate) {
  $output = '';

  if ($fDate && $fDate->repeating) {
    $freq_arr = array(
      'daily' => t('days'),
      'weekly' => t('weeks'),
      'monthly' => t('months'),
      'yearly' => t('years'),
      );

    $output = $fDate->interval
      ? t('Every %interval', array('%interval' => $fDate->interval . ' ' .  $freq_arr[$fDate->frequency]))
      : t('%frequency', array('%frequency' => ucfirst($fDate->frequency)));
    
    $output = '<span class="date_repeat_fm-frequency">' . $output . '</span>';
  }
  
  return $output;
}

/**
 * Repeating dates only; theme a string showing when the period begins and ends.
 *
 * @param $fDate
 *   An fDate object.
 *
 * @ingroup themeable
 */
function theme_date_repeat_fm_period($fDate) {
  $output = '';
  
  global $language;

  $output .= t('(!frequency event)', array('!frequency' => theme('date_repeat_fm_repeat_rule', $fDate))) . '&nbsp;';

  $start = $fDate->date[0]['start'];
  $end = $fDate->date[count($fDate->date)-1]['end'];
  
  if ($start && $end) {
    $output .= '<span class="date_repeat_fm-period">' . t('%start-date to %end-date', array('%start-date' => $start['mdy'], '%end-date' => $end['mdy'])) . '</span>&nbsp;';
  }
  return $output;
}

/**
 * Repeating dates only; theme a string showing the upcoming date.
 *
 * @param $fDate
 *   An fDate object.
 *
 * @ingroup themeable
 */
function theme_date_repeat_fm_next($fDate) {
  $output = '';
  
  foreach ($fDate->date as $k => $v) {
    if ($v['next']) {
      $output .= '<span class="date_repeat_fm-next">' . t('Next event (!frequency): ', array('!frequency' => theme('date_repeat_fm_repeat_rule', $fDate))) . theme('date_repeat_fm_duration', $fDate, $k) . '</span>';
      break;
    }
  }
  
  return $output;
}

/**
 * All dates; a string conveying that the date has passed.
 *
 * @ingroup themeable
 */
function theme_date_repeat_fm_ended() {
  return '<span class="date_repeat_fm-ended">' . t('(ended)') . '</span>';
}

/**
 * Repeating dates only; display a link to a full list of dates.
 *
 * @param $fDate
 *   An fDate object.
 *
 * @ingroup themeable
 */
function theme_date_repeat_fm_view_all($fDate) {
  $output = '';
  
  if ($fDate && $fDate->repeating) {
    $url = url('node/' . $nid, array('fragment' => 'all_dates'));
    $output .= '<span class="date_repeat_fm-view-all"><a href="#">' . t('View all dates') . '</a></span>';
  }
  
  return $output;
}

/**
 * Format a list of all dates.
 *
 * @param $fDate
 *   An fDate object.
 *
 * @ingroup themeable
 */
function theme_date_repeat_fm_listing($fDate) {
  $output = '';

  if ($fDate && count($fDate->date) > 1) {
    $str = '';
    foreach ($fDate->date as $k => $v) {
      $str .= '<li>' . theme('date_repeat_fm_duration', $fDate, $k);
      if ($v['ended']) {
        $str .= theme('date_repeat_fm_ended');
      }
      $str .= '</li>';
    }
    $output = '<div class="date_repeat_fm-listing"><p>' . t('All scheduled dates:') . '</p><ul>' . $str . '</ul></div>';
  }
  return $output;
}
