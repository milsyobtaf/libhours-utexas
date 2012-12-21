<?php

/**
 * @file
 * Module functions to display library hours.
 *
 * @author Sean Watkins <slwatkins@uh.edu>
 * @copyright 2011 University of Houston Libraries (http://info.lib.uh.edu)
 */

/**
 * Display the hours page for a given location
 *
 * @param int $lid
 *   Location unique identifier
 * @param int $pid
 *   Period unique identifier
 */
function libhours_display($lid = 0, $pid = 0) {
    global $conf;
    /* turn off drupal cache for this page */
    $conf['cache'] = FALSE;

    return theme('hours_display', $lid, $pid);
}

function libhours_display_m($lid = 0, $pid = 0) {
    global $conf;
    /* turn off drupal cache for this page */
    $conf['cache'] = FALSE;

    return theme('hours_display_m', $lid, $pid);
}

function libhours_open_display() {
    global $conf;
    /* turn off drupal cache for this page */
    $conf['cache'] = FALSE;

    return theme('hours_open_display');
}

function libhours_open_display_m() {
    global $conf;
    /* turn off drupal cache for this page */
    $conf['cache'] = FALSE;

    return theme('hours_open_display_m');
}

/**
 * Redirects page for backwards computability
 */
function libhours_redirect($lid = 0) {
    drupal_goto('hours/' . $lid);
}
