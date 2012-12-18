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
    $('#libhours-periods-tabs li').removeClass('selected').addClass('deselected');
    $('#libhours-periods-tabs #libhours-tab-' + id).addClass('selected').removeClass('deselected');

    $('#libhours-periods .libhours-view').removeClass('selected').addClass('deselected');
    $('#libhours-periods #libhours-period-' + id).addClass('selected').removeClass('deselected');
}

libhours.location = function(id){
     window.location = Drupal.settings.basePath + 'hours/' + id;
}
