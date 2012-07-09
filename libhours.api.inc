<?php

/**
 * @file
 * API function callbacks for libhours module
 *
 * @author Sean Watkins <slwatkins@uh.edu>
 * @copyright 2011 University of Houston Libraries (http://info.lib.uh.edu)
 */

/**
 * Pre processes the REST API call
 */
function libhours_api() {
  $format = isset($_GET['format']) ? $_GET['format'] : '';
  $callback = isset($_GET['callback']) ? $_GET['callback'] : '';
  $action = isset($_GET['action']) ? $_GET['action'] : '';
  $library = isset($_GET['library']) ? $_GET['library'] : '';

  libhours_api_process($format, $action, $library, $callback);
}

/**
 * Process the API call
 *
 * @param string $format
 *   The return format to use
 * @param string $action
 *   The action being done to return the results
 * @param int $library
 *   The unique identification id for the library location
 * @param string $callback
 *   The callback function used in JSONP
 */
function libhours_api_process($format = '', $action = '', $library = 0, $callback = '') {
  global $conf;
  /* Turn off drupal cache */
  $conf['cache'] = FALSE;

  $format = drupal_strtolower($format);

  if (empty($action)) {
      libhours_api_error($format, "Missing 'action'.");
      return;
  }

  switch ($action) {
    case 'locations':
      $rs = db_query("SELECT * FROM {libhours_locations} WHERE parent=0 ORDER BY name");
      while ($row = db_fetch_object($rs)) {
        $locations[] = array('id' => $row->lid, 'location' => $row->name);
        $srs = db_query("SELECT * FROM {libhours_locations} WHERE parent=%d ORDER BY name", $row->lid);
        while ($child = db_fetch_object($srs)) {
            $locations[] = array('id' => $child->lid, 'location' => ' -- ' . $child->name);
        }
      }
      $return = array('status' => 'ok', 'locations' => $locations);
      break;

    case 'hours':
      if (empty($library)) {
        libhours_api_error($format, "Missing 'library' location id");
        return;
      }

      $location = db_result(db_query("SELECT name FROM {libhours_locations} WHERE lid=%d", $library));

      $today = time();
      $period = db_fetch_object(db_query("SELECT p.pid, s.name, p.from_date, p.to_date FROM {libhours_periods} p 
                                          INNER JOIN {libhours_semesters} s ON p.sid = s.sid
                                       WHERE %d >= from_date AND %d <= to_date AND lid=%d ORDER BY from_date DESC LIMIT 1", $today, $today, $library));

      $stdhours = _libhours_api_gethours($library, $period->pid);
      $exhours = _libhours_api_getexceptions($library, $period->pid, $period->from_date, $period->to_date);

      $return = array(
        'status' => 'ok',
        'location' => $location,
        'period' => $period->name,
        'hours' => $stdhours,
        'exceptions' => $exhours,
      );
      break;

    case 'open':
    case 'opennow':
      $locations = _libhours_api_getopennow();

      $return = array('status' => 'ok', 'locations' => $locations);
      break;

    default:
      libhours_api_error($format, 'action not supported');
      break;
  }

  $output = '';
  switch ($format) {
    case 'json':
      $output = json_encode($return);
      $output = (empty($callback) ? $output : ($callback . '(' . $output . ')'));
      drupal_set_header('Content-Type: application/json');
      break;

    default:
        libhours_api_error('html', 'Invalid format provided. Only JSON is supported.');
        return;
  }

  print $output;
}

/**
 * Process any error messages
 *
 * @param string $format
 *   The format to return the error
 * @param string $errormsg
 *   The error message
 */
function libhours_api_error(
  $format = 'html',
  $errormsg = 'Unknown error occurred.'
) {

  switch ($format) {
    case 'json':
      $return['status'] = $errormsg;
      $output = json_encode($return);
      drupal_set_header('Content-Type: application/json');
      break;

    default:
      $output = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">'
              . '<html><head><title>Error</title></head><body><strong>Error: </strong> ' . $errormsg . '</body></html>';
      break;
  }

  print $output;
  die();
}

/**
 * Returns the standard hours for a location
 *
 * @param int $lid
 *   Location unique identifier
 * @param int $pid
 *   Period unique identifier
 */
function _libhours_api_gethours($lid = 0, $pid = 0) {
  $rh = db_query("SELECT * FROM {libhours_hours} WHERE pid=%d ORDER BY dow DESC", $pid);
  while ($hour = db_fetch_object($rh)) {
    for ($i = 0; $i < drupal_strlen($hour->dow); $i++) {
      if ($hour->dow[$i] == '1') {
        $hours[$i] = array('open' => $hour->open, 'close' => $hour->close);
      }
    }
  }
  $return = array();
  for ($i = 0; $i < count($hours); $i++) {
    $previdx = ($i == 0) ? (count($hours) - 1) : ($i - 1);
    $nextidx = (($i + 1) == count($hours)) ? 0 : ($i + 1);

    $return[] = array(
      'dayofweek' => _libhours_dow($i),
      'hour' => _libhours_displayhours(
        $hours[$i]['open'], $hours[$i]['close'],
        $hours[$previdx],
        $hours[$nextidx]),
    );
  }

  return $return;
}

/**
 * Gets the exceptions
 *
 * @param int $lid
 *   Location unique identifier
 * @param int $pid
 *   Period unique identifier
 * @param int $from_date
 *   Period from date
 * @param int $to_date
 *   Period to date
 */
function _libhours_api_getexceptions(
  $lid = 0,
  $pid = 0,
  $from_date = 0,
  $to_date = 0
) {

  $today = time();
  $hours = array();

  $ers = db_query("SELECT * FROM {libhours_exceptions} WHERE lid=%d AND from_date >= %d AND to_date <= %d AND %d <= to_date ORDER BY from_date ASC", $lid, $from_date, $to_date, $today);
  while ($exception = db_fetch_object($ers)) {
    if (date("n/d/Y", $exception->from_date) == date("n/d/Y", $exception->to_date)) {
      $date = date("n/d l", $exception->from_date);
    }
    else {
      $date = date("n/d", $exception->from_date) . ' - ' . date("n/d", $exception->to_date);
    }

    $hours[] = array(
      'date' => $date,
      'hour' => _libhours_displayhours($exception->open, $exception->close),
    );
  }

  return $hours;
}

/**
 * Get list of locations currently open
 */
function _libhours_api_getopennow() {
  $today = time();
  $dow = date("w", $today);
  $time = date("Hi", $today);

  $query = "SELECT l.lid AS id, l.name AS location, l.parent, h.open, h.close FROM {libhours_locations} l 
                      INNER JOIN {libhours_periods} p ON l.lid=p.lid
                      INNER JOIN {libhours_hours} h ON p.pid=h.pid
                  WHERE SUBSTRING(h.dow, %d, 1) = '1' AND %d >= p.from_date AND %d <= p.to_date";

  /* Get the regular hours */
  $rs = db_query($query . " AND parent=0 ORDER BY location", ($dow + 1), $today, $today);
  while ($row = db_fetch_object($rs)) {
    $checktime = ($row->close < $row->open) ? ($row->close + 2400) : $row->close;
    if ($time >= $row->open && $time < $checktime) {
      $return[$row->id] = array(
        'id' => $row->id,
        'location' => $row->location,
        'hours' => _libhours_displayhours($row->open, $row->close),
      );
    }
    $crs = db_query($query . " AND parent=%d ORDER BY location", ($dow + 1), $today, $today, $row->id);
    while ($child = db_fetch_object($crs)) {
      $checktime = ($child->close < $child->open) ? ($child->close + 2400) : $child->close;
      if ($time >= $child->open && $time < $checktime) {
        $return[$child->id] = array(
          'id' => $child->id,
          'location' => ' -- ' . $child->location,
          'hours' => _libhours_displayhours($child->open, $child->close),
        );
      }
    }
  }

  /* Find any exceptions for today */
  $rs = db_query("SELECT l.lid AS id, l.name AS location, e.open, e.close FROM {libhours_locations} l
                      INNER JOIN {libhours_exceptions} e ON e.lid=l.lid
                  WHERE %d >= e.from_date AND %d <= e.to_date", $today, $today);
  /* Going to only removed locations that might be currently closed because of a exception, or see if a new location is open */
  while ($row = db_fetch_object($rs)) {
    $checktime = ($row->close < $row->open) ? ($row->close + 2400) : $row->close;
    /* We have a new location that is open because of a exception */
    if ($time >= $row->open && $time < $checktime && !isset($return[$row->id])) {
      $return[$row->id] = array(
        'id' => $row->id,
        'location' => $row->location,
        'hours' => _libhours_displayhours($row->open, $row->close),
      );
    }
    elseif ($row->open == 0 && $row->close == 0 && isset($return[$row->id])) {
      /* the location is closed */
      unset($return[$row->id]);
    }
    elseif ($row->open == 0 && $row->close == 2359 && !isset($return[$row->id])) {
      /* the location is open 24hrs */
      $return[$row->id] = array(
        'id' => $row->id,
        'location' => $row->location,
        'hours' => _libhours_displayhours($row->open, $row->close),
      );
    }
    elseif ($time <= $row->open || $time >= $checktime) {
      /* the location is closed */
      unset($return[$row->id]);
    }
  }

  foreach ($return as $index => $location) {
    $returns[] = $location;
  }

  return $returns;
}
