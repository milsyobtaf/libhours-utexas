<?php


/**
 * @file
 * Administration functions for the lib hours module.
 *
 * @author Sean Watkins <slwatkins@uh.edu>
 * @copyright 2011 University of Houston Libraries (http://info.lib.uh.edu)
 */

 /**
  * Display locations and operations.
  */
function libhours_admin() {
  $rs = db_query("SELECT lid, name FROM {libhours_locations} WHERE parent=%d ORDER BY name", 0);

  $header = array(t('Location Name'), t('Operation'));
  $rows = array();

  while ($row = db_fetch_object($rs)) {
    $rows[] = array(check_plain($row->name), implode(' | ', array(
        l(t('Edit Hours'), 'admin/content/libhours/location/hours/' . $row->lid),
        l(t('Edit Location'), 'admin/content/libhours/location/edit/' . $row->lid),
        l(t('Delete Location'), 'admin/content/libhours/location/delete/' . $row->lid),
    )));

    /* display any sub-locations to this location */
    $srs = db_query("SELECT lid, name FROM {libhours_locations} WHERE parent=%d ORDER BY name", $row->lid);
    while ($subs = db_fetch_object($srs)) {
      $rows[] = array('&nbsp;&nbsp; - ' . check_plain($subs->name), implode(' | ', array(
            l(t('Edit Hours'), 'admin/content/libhours/location/hours/' . $subs->lid),
            l(t('Edit Location'), 'admin/content/libhours/location/edit/' . $subs->lid),
            l(t('Delete Location'), 'admin/content/libhours/location/delete/' . $subs->lid),
            )));
    }
  }

  $output = theme('table', $header, $rows) . '<p>' . l(t('Add Location'), 'admin/content/libhours/location/add') . '</p>';

  return $output;
}

/**
 * Callback for creating or editing a location
 *
 * @see libhours_admin_location_form()
 */
function libhours_admin_location_edit($lid = 0) {
  if ($lid) {
    $location = db_fetch_array(db_query("SELECT lid, name, description, parent FROM {libhours_locations} WHERE lid=%d", $lid));
    drupal_set_title(t('Edit Location'));
    $output = drupal_get_form('libhours_admin_location_form', $location);
  }
  else {
    $output = drupal_get_form('libhours_admin_location_form');
  }

  return $output;
}

/**
 * Form for creating or editing a location
 *
 * @see libhours_admin_location_form_submit()
 */
function libhours_admin_location_form(
  &$form_state,
  $location = array(
    'lid' => 0,
    'name' => '',
    'description' => '',
    'parent' => 0,
  )
) {

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Location Name'),
    '#default_value' => check_plain($location['name']),
    '#required' => TRUE,
    '#max_size' => 100,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => check_plain($location['description']),
    '#max_size' => 500,
  );

  $locs = array();
  $locs[0] = 'None';
  $rs = db_query("SELECT lid, name FROM {libhours_locations} WHERE parent = %d AND lid <> %d", 0, $location['lid']);
  while ($row = db_fetch_object($rs)) {
        $locs[$row->lid] = check_plain($row->name);
  }

  $form['parent'] = array(
    '#type' => 'select',
    '#title' => t('Parent Location'),
    '#description' => t('If this location is a sub-location please select the location it belongs to.'),
    '#options' => $locs,
    '#default_value' => $location['parent'],
  );

  if ($location['lid']) {
      $form['lid'] = array(
        '#type' => 'value',
        '#value' => $location['lid'],
      );

      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save Location'),
      );
  }
  else {
        $form['submit'] = array(
          '#type' => 'submit',
          '#value' => t('Add Location'),
        );
  }

  return $form;
}

/**
 * Save Location
 */
function libhours_admin_location_form_submit($form, &$form_state) {
   $lid = isset($form_state['values']['lid']) ? $form_state['values']['lid'] : 0;

  $data = array(
    $form_state['values']['name'],
    $form_state['values']['description'],
    $form_state['values']['parent'],
    $lid,
  );

  if ($lid) {
        db_query("UPDATE {libhours_locations} SET name='%s', description='%s', parent=%d WHERE lid=%d", $data);
  }
  else {
        db_query("INSERT INTO {libhours_locations} (name, description, parent) VALUES ('%s', '%s', %d)", $data);
  }

  $form_state['redirect'] = 'admin/content/libhours';
}

/**
 * confirm deletion of a location
 *
 * @see libhours_admin_location_delete_confirm_submit()
 */
function libhours_admin_location_delete_confirm(&$form_state, $lid = 0) {
  $location = db_fetch_object(db_query("SELECT * FROM {libhours_locations} WHERE lid=%d", $lid));

  $form['lid'] = array(
    '#type' => 'value',
    '#value' => $lid,
  );

  $output = confirm_form($form, t('Are you sure you want to delete %location?', array('%location' => $location->name)), 'admin/content/libhours');

  return $output;
}

/**
 * do location deletion
 */
function libhours_admin_location_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    /* all sub-locations to the deleted location will be made into parent locations */
    $rs = db_query("SELECT lid FROM {libhours_locations} WHERE parent=%d", $form_state['values']['lid']);
    while ($row = db_fetch_object($rs)) {
      db_query("UPDATE {libhours_locations} SET parent=%d WHERE lid=%d", 0, $row->lid);
    }

    db_query("DELETE FROM {libhours_locations} WHERE lid=%d", $form_state['values']['lid']);
    db_query("DELETE FROM {libhours_exceptions} WHERE lid=%d", $form_state['values']['lid']);

    $form_state['redirect'] = 'admin/content/libhours';
  }
}

/**
 * display location hours summery
 */
function libhours_admin_location_hours($lid = 0) {
  $location = db_fetch_object(db_query("SELECT * FROM {libhours_locations} WHERE lid=%d", $lid));

  $title = t('Location Information: @locationname', array('@locationname' => check_plain($location->name)));
  drupal_set_title(t($title));

  $output .= '<h2>' . t('Periods') . '</h2>';

  $header = array(t('Semester'), t('Date Rage'), t('Operations'));
  $rows = array();
  $rs = db_query("SELECT p.*, s.name FROM {libhours_periods} p INNER JOIN {libhours_semesters} s ON s.sid=p.sid WHERE p.lid=%d ORDER BY p.from_date ASC", $lid);
  while ($row = db_fetch_object($rs)) {
    $from_date = date("n/j/Y", $row->from_date);
    $to_date = date("n/j/Y", $row->to_date);

    $rows[] = array(check_plain($row->name), $from_date . ' - ' . $to_date, implode(' | ', array(
      l(t('View Hours'), 'admin/content/libhours/period/hours/' . $row->pid),
      l(t('Edit Date Range'), 'admin/content/libhours/period/edit/' . $lid . '/' . $row->pid),
      l(t('Delete Period'), 'admin/content/libhours/period/delete/' . $row->pid),
    )));
  }

  $output .= theme('table', $header, $rows) . '<p>' . l(t('Add Period'), 'admin/content/libhours/period/add/' . $lid) . '</p>';

  $output .= '<br /><br /><h2>Exceptions</h2>';
  $header = array(
    t('Label'),
    t('Type'),
    t('Date Rage'),
    t('Hours'),
    t('Operations'),
  );

  $rows = array();
  $rs = db_query("SELECT e.*, t.name FROM {libhours_exceptions} e INNER JOIN {libhours_type} t ON t.tid=e.tid WHERE e.lid=%d ORDER BY e.from_date", $lid);
  while ($row = db_fetch_object($rs)) {
    $hdisplay = _libhours_displayhours($row->open, $row->close);

    $from_date = date("n/j/Y", $row->from_date);
    $to_date = date("n/j/Y", $row->to_date);

    $rows[] = array(check_plain($row->label), check_plain($row->name), $from_date . ' - ' . $to_date, $hdisplay, implode(' | ', array(
      l(t('Edit'), 'admin/content/libhours/exceptions/edit/' . $lid . '/' . $row->eid),
      l(t('Delete'), 'admin/content/libhours/exceptions/delete/' . $row->eid),
    )));
  }

  $output .= theme('table', $header, $rows) . '<p>' . l(t('Add Exception'), 'admin/content/libhours/exceptions/add/' . $lid) . '</p>';

  return $output;
}

/**
 * callback to create or edit a period
 *
 * @see libhours_admin_period_form()
 */
function libhours_admin_period_edit($lid = 0, $pid = 0) {
  if ($pid) {
    drupal_set_title(t('Edit period'));
    $period = db_fetch_array(db_query("SELECT * FROM {libhours_periods} WHERE pid=%d", $pid));
    $output = drupal_get_form('libhours_admin_period_form', $period);
  }
  else {
    $period = array(
      'pid' => 0,
      'lid' => $lid,
      'sid' => 0,
      'from_date' => time(),
      'to_date' => time(),
    );
    $output = drupal_get_form('libhours_admin_period_form', $period);
  }

  return $output;
}

/**
 * form for creating or editing periods
 *
 * @see libhours_admin_period_form_submit()
 */
function libhours_admin_period_form(&$form_state, $period) {
  $form['lid'] = array(
    '#type' => 'value',
    '#value' => $period['lid'],
  );

  $rs = db_query("SELECT * FROM {libhours_semesters} WHERE TRUE ORDER BY name");
  $semesters = array();
  while ($row = db_fetch_object($rs)) {
    $semesters[$row->sid] = check_plain($row->name);
  }

  $form['sid'] = array(
    '#type' => 'select',
    '#title' => t('Period Semester'),
    '#options' => $semesters,
    '#default_value' => $period['sid'],
    '#required' => TRUE,
  );

  $form['daterange'] = array(
    '#type' => 'fieldset',
    '#title' => t('Date Rage'),
    '#collapsible' => FALSE,
  );

  $form['daterange']['from_date'] = array(
    '#type' => 'date',
    '#title' => t('From'),
    '#default_value' => array(
      'year'  => date('Y', $period['from_date']),
      'month' => date('n', $period['from_date']),
      'day'   => date('j', $period['from_date']),
    ),
  );

  $form['daterange']['to_date'] = array(
    '#type' => 'date',
    '#title' => t('To'),
    '#default_value' => array(
      'year'  => date('Y', $period['to_date']),
      'month' => date('n', $period['to_date']),
      'day'   => date('j', $period['to_date']),
    ),
  );

  if ($period['pid']) {
    $form['pid'] = array(
      '#type' => 'value',
      '#value' => $period['pid'],
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Add'),
    );
  }
  return $form;
}

/**
 * Validate Period date range
 */
function libhours_admin_period_form_validate($form, &$form_state) {
  $form_state['values']['from_date'] = mktime(0, 0, 0,
                              $form_state['values']['from_date']['month'],
                              $form_state['values']['from_date']['day'],
                              $form_state['values']['from_date']['year']);
  $form_state['values']['to_date'] = mktime(23, 59, 59,
                              $form_state['values']['to_date']['month'],
                              $form_state['values']['to_date']['day'],
                              $form_state['values']['to_date']['year']);

  if ($form_state['values']['to_date'] < $form_state['values']['from_date']) {
      form_set_error('to_date', t('To date is invalid and happens before the from date.'));
  }
}

/**
 * Save Periods
 */
function libhours_admin_period_form_submit($form, &$form_state) {
  $pid = isset($form_state['values']['pid']) ? $form_state['values']['pid'] : 0;

  $data = array(
    $form_state['values']['lid'],
    $form_state['values']['sid'],
    $form_state['values']['from_date'],
    $form_state['values']['to_date'],
    $pid,
  );
  if ($pid) {
    db_query("UPDATE {libhours_periods} SET lid=%d, sid=%d, from_date=%d, to_date=%d WHERE pid=%d", $data);
  }
  else {
    db_query("INSERT INTO {libhours_periods} (lid, sid, from_date, to_date) VALUES (%d, %d, %d, %d)", $data);
  }

  $form_state['redirect'] = 'admin/content/libhours/location/hours/' . $form_state['values']['lid'];
}

/**
 * confirm deletion of a period
 *
 * @see libhours_admin_period_delete_confirm_submit()
 */
function libhours_admin_period_delete_confirm(&$form_state, $pid = 0) {
  $period = db_fetch_object(db_query("SELECT * FROM {libhours_periods} WHERE pid=%d", $pid));

  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $period->pid,
  );

  $form['lid'] = array(
    '#type' => 'value',
    '#value' => $period->lid,
  );

  $from_date = date("n/j/Y", $period->from_date);
  $to_date = date("n/j/Y", $period->to_date);

  $output = confirm_form($form, t('Are you sure you want to delete this period from %from_date to %to_date?', array('%from_date' => $from_date, '%to_date' => $to_date)), 'admin/content/libhours/location/hours/' . $period->lid);

  return $output;
}

/**
 * do the deletion of period
 */
function libhours_admin_period_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_query("DELETE FROM {libhours_periods} WHERE pid=%d", $form_state['values']['pid']);
    db_query("DELETE FROM {libhours_hours} WHERE pid=%d", $form_state['values']['pid']);

    $form_state['redirect'] = 'admin/content/libhours/location/hours/' . $form_state['values']['lid'];
  }
}

/**
 * display period hours
 */
function libhours_admin_period_hours($pid = 0) {
  if (!$pid) {
    drupal_goto('admin/content/libhours');
  }

  $period = db_fetch_object(db_query("SELECT p.*, s.name FROM {libhours_periods} p INNER JOIN {libhours_semesters} s ON p.sid=s.sid WHERE p.pid=%d", $pid));
  $location_name = db_result(db_query("SELECT name FROM {libhours_locations} WHERE lid=%d", $period->lid));

  $output = '<h2>' . t('Location: @locationname', array('@locationname' => $location_name)) . '</h2>';
  $output .= '<br />';

  drupal_set_title(t(check_plain($period->name) . ' Hours: ' . date("n/j/Y", $period->from_date) . ' - ' . date("n/j/Y", $period->to_date)));
  $header = array(t('Day of Week'), t('Hours'), t('Operation'));
  $rows = array();

  $rs = db_query("SELECT * FROM {libhours_hours} WHERE pid=%d ORDER BY dow DESC", $pid);
  while ($row = db_fetch_object($rs)) {
    $dow = _libhours_dowstring($row->dow);

    $hdisplay = _libhours_displayhours($row->open, $row->close);

    $rows[] = array($dow, $hdisplay, implode(' | ', array(
        l(t('Edit'), 'admin/content/libhours/hours/edit/' . $row->hid),
        l(t('Delete'), 'admin/content/libhours/hours/delete/' . $row->hid),
    )));
  }

  $output .= theme('table', $header, $rows);
  $output .= '<br />';
  $output .= drupal_get_form('libhours_admin_hours_form', $pid);

  return $output;
}

/**
 * callback to create or edit period hours
 *
 * @see libhours_admin_hours_form()
 */
function libhours_admin_hours_edit($hid = 0) {
  if ($hid) {
    $hours = db_fetch_array(db_query("SELECT * FROM {libhours_hours} WHERE hid=%d", $hid));
    $output = drupal_get_form('libhours_admin_hours_form', $hours['pid'], $hours);
  }
  else {
    $output = drupal_get_form('libhours_admin_hours_form');
  }

  return $output;
}

/**
 * Form to create or edit period hours
 *
 * @see libhours_admin_hours_form_validate()
 * @see libhours_admin_hours_form_submit()
 */
function libhours_admin_hours_form(
  &$form_state,
  $pid = 0,
  $hours = array(
    'hid' => 0,
    'pid' => 0,
    'dow' => '0000000',
    'open' => 0,
    'close' => 0,
  )
) {

  if ($hours['hid']) {
    $form['general_hours'] = array(
      '#type' => 'fieldset',
      '#title' => t('Edit Hours'),
    );
  }
  else {
    $form['general_hours'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add Hours'),
    );
  }

  $dow = array();
  $dow_values = array();
  for ($i = 0; $i < 7; $i++) {
    $dow[$i + 1] = _libhours_dow($i);
    if ($hours['dow'][$i] == '1') {
      $dow_values[] = $i + 1;
    }
  }

  $form['general_hours']['dayofweek'] = array(
    '#type' => 'checkboxes',
    '#options' => $dow,
    '#default_value' => $dow_values,
    '#prefix' => '<div class="libhours-dow-checkboxes">',
    '#suffix' => '</div>',
  );

  $open = _libhours_parsetime($hours['open']);
  $open_arry = _libhours_splittime($open);

  $close = _libhours_parsetime($hours['close']);
  $close_arry = _libhours_splittime($close);

  $form['general_hours']['open'] = array(
    '#type' => 'time',
    '#title' => t('Open'),
    '#description' => t('Time the location opens. To close a location set Open and Close to 12:00am. To open 24hrs set Open to 12:00am and Close to 11:59pm'),
    '#required' => TRUE,
    '#default_value' => array(
      'hour' => $open_arry[0],
      'minute' => $open_arry[1],
      'm' => $open_arry[2],
    ),
  );

  $form['general_hours']['close'] = array(
    '#type' => 'time',
    '#title' => t('Close'),
    '#description' => t('Time the location closes. To close a location set Open and Close to 12:00am. To open 24hrs set Open to 12:00am and Close to 11:59pm'),
    '#required' => TRUE,
    '#default_value' => array(
      'hour' => $close_arry[0],
      'minute' => $close_arry[1],
      'm' => $close_arry[2],
    ),
  );

  if ($pid) {
    $form['pid'] = array(
      '#type' => 'value',
      '#value' => $pid,
    );
  }

  if ($hours['hid']) {
    $form['hid'] = array(
      '#type' => 'value',
      '#value' => $hours['hid'],
    );
    $form['general_hours']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Hours'),
    );
    $form['general_hours']['cancel'] = array(
      '#type' => 'button',
      '#value' => t('Cancel'),
      '#executes_submit_callback' => TRUE,
      '#submit' => array('libhours_admin_hours_form_cancel'),
    );
  }
  else {
    $form['general_hours']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Add Hours'),
    );
  }


  $lid = db_result(db_query("SELECT lid FROM {libhours_periods} WHERE pid=%d", $pid));
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Done'), 'admin/content/libhours/location/hours/' . $lid),
  );

  return $form;
}

/**
 * cancels editing the from and returns to the period
 */
function libhours_admin_hours_form_cancel($form, &$form_state) {
  $hid = (isset($form_state['values']['hid'])) ? $form_state['values']['hid'] : 0;

  if ($hid) {
    drupal_goto('admin/content/libhours/period/hours/' . $form_state['values']['pid']);
  }
  else {
    $lid = db_result(db_query("SELECT lid FROM {libhours_periods} WHERE pid=%d", $form_state['values']['pid']));
    drupal_goto('admin/content/libhours/location/hours/' . $lid);
  }
}

/**
 * Hours from validation
 *
 * @see libhours_admin_hours_form()
 */
function libhours_admin_hours_form_validate($form, &$form_state) {
  /*
    Check to make sure these hours are attached to a period. they should be but
    incase of coder error.
  */
  if (!isset($form_state['values']['pid'])) {
    form_set_error('', t('Internal error occurred. Unable to find pid identifier.'));
  }

  $hid = (isset($form_state['values']['hid']) ? $form_state['values']['hid'] : 0);

  /* sort out the dow binary */
  $dow = '';
  for ($i = 1; $i <= 7; $i++) {
    $dow .= ($form_state['values']['dayofweek'][$i] != 0) ? '1' : '0';
  }
  if ($dow == '0000000') {
    form_set_error('dayofweek', t('At least one day must be selected'));
  }
  $form_state['values']['dow'] = $dow;

  $open = date('Hi', strtotime($form_state['values']['open']['hour'] . ':' . $form_state['values']['open']['minute'] . ' ' . $form_state['values']['open']['m'])) * 1;
  $close = date('Hi', strtotime($form_state['values']['close']['hour'] . ':' . $form_state['values']['close']['minute'] . ' ' . $form_state['values']['close']['m'])) * 1;

  $form_state['values']['open'] = $open;
  $form_state['values']['close'] = $close;

  /* ok lets look at the other hours in this period and make sure we don't conflict with anything */
  $rs = db_query("SELECT * FROM {libhours_hours} WHERE pid=%d AND hid <> %d", $form_state['values']['pid'], $hid);
  while ($row = db_fetch_object($rs)) {
    $error_hours = _libhours_dowstring($row->dow) . ' / ' . _libhours_parsetime($row->open) . ' - ' . _libhours_parsetime($row->close);

    for ($i = 0; $i < drupal_strlen($row->dow); $i++) {
      if ($row->dow[$i] == '1' && $dow[$i] == '1') {
        $close_check = ($row->close < $row->open) ? $row->close + 2400 : $row->close;
        $error = FALSE;

        /* time to check the hours for any conflicts */
        if ($row->open == 0 && $row->close == 0) {
          /* location closed */
          $error = TRUE;
        }
        elseif ($row->open == 0 && $row->close == 2359) {
          /* location open 24hrs */
          $error = TRUE;
        }
        elseif ($open > $row->open || $close > $row->open) {
          if ($open < $close_check || $close < $close_check) {
            $error = TRUE;
          }
        }
        elseif ($row->close < $row->open) {
          if ($open < $row->close || $close < $row->close) {
            $error = TRUE;
          }
        }

        if ($error) {
          form_set_error('', t('Sorry, this time conflicts with %hours.', array('%hours' => $error_hours)));
        }
      }
    }
  }
}

/**
 * save the hours
 */
function libhours_admin_hours_form_submit($form, &$form_state) {
  $hid = (isset($form_state['values']['hid']) ? $form_state['values']['hid'] : 0);

  $data = array(
    $form_state['values']['pid'],
    $form_state['values']['dow'],
    $form_state['values']['open'],
    $form_state['values']['close'],
    $hid,
  );

  if ($hid) {
    db_query("UPDATE {libhours_hours} SET pid=%d, dow='%s', open=%d, close=%d WHERE hid=%d", $data);
  }
  else {
    db_query("INSERT INTO {libhours_hours} (pid, dow, open, close) VALUES (%d, '%s', %d, %d)", $data);
  }

  $form_state['redirect'] = 'admin/content/libhours/period/hours/' . $form_state['values']['pid'];
}

/**
 * confirm deletion of hours
 *
 * @see libhours_admin_hours_delete_confirm_submit()
 */
function libhours_admin_hours_delete_confirm(&$form_state, $hid = 0) {
  $hours = db_fetch_object(db_query("SELECT * FROM {libhours_hours} WHERE hid=%d", $hid));

  $form['hid'] = array(
    '#type' => 'value',
    '#value' => $hid,
  );

  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $hours->pid,
  );

  $hdisplay = _libhours_dowstring($hours->dow) . ' / ' . _libhours_displayhours($hours->open, $hours->close);

  $output = confirm_form($form, t('Are you sure you want to delete the hours for %hours?', array('%hours' => $hdisplay)), 'admin/content/libhours/period/hours/' . $hours->pid);

  return $output;
}

/**
 * do the deletion of hours
 */
function libhours_admin_hours_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_query("DELETE FROM {libhours_hours} WHERE hid=%d", $form_state['values']['hid']);

    $form_state['redirect'] = 'admin/content/libhours/period/hours/' . $form_state['values']['pid'];
  }
}

/**
 * Callback to create or edit exceptions
 */
function libhours_admin_exceptions_edit($lid = 0, $eid = 0) {
  if ($eid) {
    $exception = db_fetch_array(db_query("SELECT * FROM {libhours_exceptions} WHERE eid=%d", $eid));
    $output = drupal_get_form('libhours_admin_exceptions_form', $lid, $exception);
  }
  else {
    $output = drupal_get_form('libhours_admin_exceptions_form', $lid);
  }

  return $output;
}

/**
 * form for creating or editing exceptions
 *
 * @see libhours_admin_exceptions_form_validate()
 * @see libhours_admin_exceptions_form_submit()
 */
function libhours_admin_exceptions_form(
  &$form_state,
  $lid = 0,
  $exception = array(
    'eid' => 0,
    'lid' => 0,
    'tid' => 0,
    'label' => '',
    'from_date' => 0,
    'to_date' => 0,
    'open' => 0,
    'close' => 0,
  )
) {

  $form['lid'] = array(
    '#type' => 'value',
    '#value' => $lid,
  );

  $rs = db_query("SELECT * FROM {libhours_type} WHERE TRUE ORDER BY name");
  $types = array();
  while ($row = db_fetch_object($rs)) {
      $types[$row->tid] = $row->name;
  }

  $form['tid'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => $types,
    '#default_value' => $exception['tid'],
  );

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#max_size' => 500,
    '#required' => TRUE,
    '#default_value' => $exception['label'],
  );

  $form['daterange'] = array(
    '#type' => 'fieldset',
    '#title' => t('Date Range'),
    '#collapsable' => FALSE,
  );

  if ($exception['from_date'] == 0) {
    $exception['from_date'] = time();
  }
  if ($exception['to_date'] == 0) {
    $exception['to_date'] = time();
  }

  $form['daterange']['from_date'] = array(
    '#type' => 'date',
    '#title' => t('From'),
    '#default_value' => array(
      'year' => date('Y', $exception['from_date']),
      'month' => date('n', $exception['from_date']),
      'day' => date('j', $exception['from_date']),
    ),
  );

  $form['daterange']['to_date'] = array(
    '#type' => 'date',
    '#title' => t('To'),
    '#default_value' => array(
      'year' => date('Y', $exception['to_date']),
      'month' => date('n', $exception['to_date']),
      'day' => date('j', $exception['to_date']),
    ),
  );

  $open = _libhours_parsetime($exception['open']);
  $open_arry = _libhours_splittime($open);

  $close = _libhours_parsetime($exception['close']);
  $close_arry = _libhours_splittime($close);

  $form['general_hours'] = array(
    '#type' => 'fieldset',
    '#title' => t('Hours'),
    '#collapsable' => FALSE,
  );

  $form['general_hours']['open'] = array(
    '#type' => 'time',
    '#title' => t('Open'),
    '#description' => t('Time the location opens. For closed location set Open and Close to 12:00am. To open 24hrs set Open to 12:00am and Close to 11:59pm'),
    '#required' => TRUE,
    '#default_value' => array(
      'hour' => $open_arry[0],
      'minute' => $open_arry[1],
      'm' => $open_arry[2],
    ),
  );

  $form['general_hours']['close'] = array(
    '#type' => 'time',
    '#title' => t('Close'),
    '#description' => t('Time the location closes. For closed location set Open and Close to 12:00am. To open 24hrs set Open to 12:00am and Close to 11:59pm'),
    '#required' => TRUE,
    '#default_value' => array(
      'hour' => $close_arry[0],
      'minute' => $close_arry[1],
      'm' => $close_arry[2],
    ),
  );

  if ($exception['eid']) {
    $form['eid'] = array(
      '#type' => 'value',
      '#value' => $exception['eid'],
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Exception'),
    );
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Add Exception'),
    );
  }

  return $form;
}

/**
 * validate exception from
 *
 * @see libhours_admin_exceptions_form_submit()
 */
function libhours_admin_exceptions_form_validate($form, &$form_state) {
  $eid = (isset($form_state['values']['eid'])) ? $form_state['values']['eid'] : 0;

  $form_state['values']['from_date'] = mktime(0, 0, 0,
                                $form_state['values']['from_date']['month'],
                                $form_state['values']['from_date']['day'],
                                $form_state['values']['from_date']['year']);
  $form_state['values']['to_date'] = mktime(23, 59, 59,
                                $form_state['values']['to_date']['month'],
                                $form_state['values']['to_date']['day'],
                                $form_state['values']['to_date']['year']);

  if ($form_state['values']['to_date'] < $form_state['values']['from_date']) {
    form_set_error('to_date', t('Sorry, the dates are invalid. The To date falls before the From date.'));
  }

  $form_state['values']['open'] = date('Hi', strtotime($form_state['values']['open']['hour'] . ':' . $form_state['values']['open']['minute'] . ' ' . $form_state['values']['open']['m'])) * 1;
  $form_state['values']['close'] = date('Hi', strtotime($form_state['values']['close']['hour'] . ':' . $form_state['values']['close']['minute'] . ' ' . $form_state['values']['close']['m'])) * 1;

  $rs = db_query("SELECT * FROM {libhours_exceptions} WHERE lid=%d AND eid <> %d", $form_state['values']['lid'], $eid);
  while ($row = db_fetch_object($rs)) {
    /* check to see if the exception entered is during another exception date */
    if (($form_state['values']['from_date'] >= $row->from_date && $form_state['values']['from_date'] <= $row->to_date) || ($form_state['values']['to_date'] >= $row->from_date && $form_state['values']['to_date'] <= $row->to_date)) {
      /* they dates overlap so check to make sure the hours don't overlap */
      $close_check = ($row->close < $row->open) ? $row->close + 2400 : $row->close;
      $error = FALSE;

      /* time to check the hours for any conflicts */
      if ($row->open == 0 && $row->close == 0) {
        /* location closed */
        $error = TRUE;
      }
      elseif ($row->open == 0 && $row->close == 2359) {
        /* location open 24hrs */
        $error = TRUE;
      }
      elseif ($form_state['values']['open'] > $row->open || $form_state['values']['close'] > $row->open) {
        if ($form_state['values']['open'] < $close_check || $form_state['values']['close'] < $close_check) {
          $error = TRUE;
        }
      }
      elseif ($row->close < $row->open) {
        if ($form_state['values']['open'] < $row->close || $form_state['values']['close'] < $row->close) {
          $error = TRUE;
        }
      }

      $excstr = $row->label . ' From: ' . date("n/j/Y", $row->from_date) . ' To: ' . date("n/j/Y", $row->to_date) . ' @ ' . _libhours_displayhours($row->open, $row->close);

      if ($error) {
        form_set_error('', t('Sorry but the exception conflicts with other exception: %exc', array('%exc' => $excstr)));
      }
    }
  }
}

/**
 * do save of exceptions form
 */
function libhours_admin_exceptions_form_submit($form, &$form_state) {
  $eid = (isset($form_state['values']['eid'])) ? $form_state['values']['eid'] : 0;

  $data = array(
    $form_state['values']['lid'],
    $form_state['values']['tid'],
    $form_state['values']['label'],
    $form_state['values']['from_date'],
    $form_state['values']['to_date'],
    $form_state['values']['open'],
    $form_state['values']['close'],
    $eid,
  );

  if ($eid) {
    db_query("UPDATE {libhours_exceptions} SET lid=%d, tid=%d, label='%s', from_date=%d, to_date=%d, open=%d, close=%d WHERE eid=%d", $data);
  }
  else {
    db_query("INSERT INTO {libhours_exceptions} (lid, tid, label, from_date, to_date, open, close) VALUES (%d, %d, '%s', %d, %d, %d, %d)", $data);
  }

  $form_state['redirect'] = 'admin/content/libhours/location/hours/' . $form_state['values']['lid'];
}

/**
 * confirm deletion of exception
 *
 * @see libhours_admin_exceptions_delete_confirm_submit()
 */
function libhours_admin_exceptions_delete_confirm(&$form_state, $eid = 0) {
  $exception = db_fetch_object(db_query("SELECT * FROM {libhours_exceptions} WHERE eid=%d", $eid));

  $form['eid'] = array(
    '#type' => 'value',
    '#value' => $eid,
  );

  $form['lid'] = array(
    '#type' => 'value',
    '#value' => $exception->lid,
  );

  $excdisplay = $exception->label . ' on ' . date("n/j/Y", $exception->from_date) . ' - ' . date("n/j/Y", $exception->to_date) . ' @ ' . _libhours_displayhours($exception->open, $exception->close);

  $output = confirm_form($form, t('Are you sure you want to delete the exception %exc?', array('%exc' => $excdisplay)), 'admin/content/libhours/location/hours/' . $exception->lid);

  return $output;
}

/**
 * do the deletion of hours
 */
function libhours_admin_exceptions_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_query("DELETE FROM {libhours_exceptions} WHERE eid=%d", $form_state['values']['eid']);

    $form_state['redirect'] = 'admin/content/libhours/location/hours/' . $form_state['values']['lid'];
  }
}
