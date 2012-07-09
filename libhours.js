/**
 * @file
 *
 * Javascript functions for libhours module
 *
 * @author Sean Watkins <slwatkins@uh.edu>
 * @copyright 2011 University of Houston Libraries (http://info.lib.uh.edu)
 */

libhours = {};

libhours.tab = function(id){
    $('#libhours-periods-tabs li').removeClass('selected');
    $('#libhours-periods-tabs #libhours-tab-' + id).addClass('selected');

    $('#libhours-periods .libhours-view').removeClass('selected');
    $('#libhours-periods #libhours-period-' + id).addClass('selected');
}

libhours.location = function(id){
     window.location = Drupal.settings.basePath + 'hours/' + id;
}
